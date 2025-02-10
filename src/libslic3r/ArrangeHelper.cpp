#include "ArrangeHelper.hpp"

#include "libslic3r/Model.hpp"
#include "libslic3r/TriangleMesh.hpp"
#include "libslic3r/MultipleBeds.hpp"
#include "libslic3r/PresetBundle.hpp"
#include "libslic3r/BuildVolume.hpp"

#include <string>

#include "boost/regex.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/algorithm/string/replace.hpp"
#include <boost/nowide/fstream.hpp>



namespace Slic3r {


static Sequential::PrinterGeometry get_printer_geometry(const ConfigBase& config)
{
	enum ShapeType {
		BOX,
		CONVEX
	};
	struct ExtruderSlice {
		coord_t height;
		ShapeType shape_type;
		std::vector<Polygon> polygons;
	};

	BuildVolume bv(config.opt<ConfigOptionPoints>("bed_shape")->values, 10.);
	const BoundingBox& bb = bv.bounding_box();
	Polygon bed_polygon;
	if (bv.type() == BuildVolume::Type::Circle) {
		// Generate an inscribed octagon.
		double r = bv.bounding_volume2d().size().x() / 2.;
		for (double a = 2*M_PI; a > 0.1; a -= M_PI/4.)
			bed_polygon.points.emplace_back(Point::new_scale(r * std::sin(a), r * std::cos(a)));
	} else {
		// Rectangle of Custom. Just use the bounding box.
		bed_polygon = bb.polygon();
	}

	std::vector<ExtruderSlice> slices;
	const std::string printer_notes = config.opt_string("printer_notes");
	{
		if (! printer_notes.empty()) {
			try {
				boost::nowide::ifstream in(resources_dir() + "/data/printer_gantries/geometries.txt");
				boost::property_tree::ptree pt;
				boost::property_tree::read_json(in, pt);
				for (const auto& printer : pt.get_child("printers")) {
					slices = {};
					std::string printer_notes_match = printer.second.get<std::string>("printer_notes_regex");
					boost::regex rgx(printer_notes_match);
					if (! boost::regex_match(printer_notes, rgx))
						continue;

					for (const auto& obj : printer.second.get_child("slices")) {
						ExtruderSlice slice;
						slice.height = scaled(obj.second.get<double>("height"));
						std::string type_str = obj.second.get<std::string>("type");
						slice.shape_type = type_str == "box" ? BOX : CONVEX;
						for (const auto& polygon : obj.second.get_child("polygons")) {
							Polygon pgn;
							std::string pgn_str = polygon.second.data();
							boost::replace_all(pgn_str, ";", " ");
							boost::replace_all(pgn_str, ",", " ");
							std::stringstream ss(pgn_str);
							while (ss) {
								double x = 0.;
								double y = 0.;
								ss >> x >> y;
								if (ss)
									pgn.points.emplace_back(Point::new_scale(x, y));
							}
							if (! pgn.points.empty())
								slice.polygons.emplace_back(std::move(pgn));
						}
						slices.emplace_back(std::move(slice));
					}
					break;
				}
			}
			catch (const boost::property_tree::json_parser_error&) {
				// Failed to parse JSON. slices are empty, fallback will be used.
			}
		}
		if (slices.empty()) {
			// Fallback to primitive model using radius and height.
			coord_t r = scaled(std::max(0.1, config.opt_float("extruder_clearance_radius")));
			coord_t h = scaled(std::max(0.1, config.opt_float("extruder_clearance_height")));
			double bed_x = bv.bounding_volume2d().size().x();
			double bed_y = bv.bounding_volume2d().size().y();
			slices.push_back(ExtruderSlice{ 0, CONVEX, { { {  -5000000,   -5000000 }, {   5000000,   -5000000 }, {   5000000,   5000000 }, {  -5000000,   5000000 } } } });
			slices.push_back(ExtruderSlice{ 1000000, BOX, { { {  -r, -r }, { r, -r }, {   r,   r }, {  -r,  r } } } });
			slices.push_back(ExtruderSlice{ h, BOX, { { { -scaled(bed_x),  -r }, { scaled(bed_x),  -r }, { scaled(bed_x), r }, { -scaled(bed_x), r}}} });
		}
	}

	// Convert the read data so libseqarrange understands them.
	Sequential::PrinterGeometry out;
	out.plate = bed_polygon;
	for (const ExtruderSlice& slice : slices) {
		(slice.shape_type == CONVEX ? out.convex_heights : out.box_heights).emplace(slice.height);
		out.extruder_slices.insert(std::make_pair(slice.height, slice.polygons));
	}
	return out;
}

static Sequential::SolverConfiguration get_solver_config(const Sequential::PrinterGeometry& printer_geometry)
{
	return Sequential::SolverConfiguration(printer_geometry);
}

static std::vector<Sequential::ObjectToPrint> get_objects_to_print(const Model& model, const Sequential::PrinterGeometry& printer_geometry)
{
	// First extract the heights of interest.
	std::vector<double> heights;
	for (const auto& [height, pgns] : printer_geometry.extruder_slices)
		heights.push_back(unscaled(height));
	Slic3r::sort_remove_duplicates(heights);

	// Now collect all objects and projections of convex hull above respective heights.
	std::vector<std::pair<Sequential::ObjectToPrint, std::vector<Sequential::ObjectToPrint>>> objects; // first = object id, the vector = ids of its instances
	for (const ModelObject* mo : model.objects) {
		size_t inst_id = 0;
		const TriangleMesh& raw_mesh = mo->raw_mesh();
		for (const ModelInstance* mi : mo->instances) {
			coord_t height = scaled(mo->instance_bounding_box(inst_id).size().z());
			Sequential::ObjectToPrint* new_object =
				(inst_id == 0 ? &objects.emplace_back(std::make_pair(Sequential::ObjectToPrint{int(mo->id().id), inst_id + 1 < mo->instances.size(), height, {}}, std::vector<Sequential::ObjectToPrint>())).first
				              : &objects.back().second.emplace_back(Sequential::ObjectToPrint{int(mi->id().id), inst_id + 1 < mo->instances.size(), height, {}}));

			for (double height : heights) {
			    // It seems that zero level in the object instance is mi->get_offset().z(), however need to have bed as zero level,
			    // hence substracting mi->get_offset().z() from height seems to be an easy hack
                Polygon pgn = its_convex_hull_2d_above(raw_mesh.its, mi->get_matrix_no_offset().cast<float>(), height - mi->get_offset().z());
				new_object->pgns_at_height.emplace_back(std::make_pair(scaled(height), pgn));
			}
			++inst_id;
		}
	}

	// Now order the objects so that the are always passed in the order of increasing id.
	// That way, the algorithm will give the same result when called repeatedly.
	// However, there is an exception: instances cannot be separated from their objects.
	std::sort(objects.begin(), objects.end(), [](const auto& a, const auto& b) { return a.first.id < b.first.id; });
	std::vector<Sequential::ObjectToPrint> objects_out;
	for (const auto& o : objects) {
		objects_out.emplace_back(o.first);
		for (const auto& i : o.second)
			objects_out.emplace_back(i);
	}

	return objects_out;
}




void arrange_model_sequential(Model& model, const ConfigBase& config)
{
	SeqArrange seq_arrange(model, config);
	seq_arrange.process_seq_arrange([](int) {});
	seq_arrange.apply_seq_arrange(model);
}



SeqArrange::SeqArrange(const Model& model, const ConfigBase& config)
{
    m_printer_geometry = get_printer_geometry(config);
	m_solver_configuration = get_solver_config(m_printer_geometry);
	m_objects = get_objects_to_print(model, m_printer_geometry);

}



void SeqArrange::process_seq_arrange(std::function<void(int)> progress_fn)
{
	m_plates =
		Sequential::schedule_ObjectsForSequentialPrint(
			m_solver_configuration,
			m_printer_geometry,
			m_objects, progress_fn);
}



void SeqArrange::apply_seq_arrange(Model& model) const
{
	// Extract the result and move the objects in Model accordingly.
	struct MoveData {
		Sequential::ScheduledObject scheduled_object;
		size_t bed_idx;
	};

	// A vector to collect move data for all the objects.
	std::vector<MoveData> move_data_all;

	// Now iterate through all the files, read the data and move the objects accordingly.
	// Save the move data from this file to move_data_all.
	size_t bed_idx = 0;
	for (const Sequential::ScheduledPlate& plate : m_plates) {
		Vec3d bed_offset = s_multiple_beds.get_bed_translation(bed_idx);
		// Iterate the same way as when exporting.
		for (ModelObject* mo : model.objects) {
			for (ModelInstance* mi : mo->instances) {
				const ObjectID& oid = (mi == mo->instances.front() ? mo->id() : mi->id());
				auto it = std::find_if(plate.scheduled_objects.begin(), plate.scheduled_objects.end(), [&oid](const auto& md) { return md.id == oid.id; });
				if (it != plate.scheduled_objects.end()) {
					mi->set_offset(Vec3d(unscaled(it->x) + bed_offset.x(), unscaled(it->y) + bed_offset.y(), mi->get_offset().z()));
				}
			}
		}
		for (const Sequential::ScheduledObject& object : plate.scheduled_objects)
			move_data_all.push_back({ object, bed_idx });
		++bed_idx;
	}

	// Now reorder the objects in the model so they are in the same order as requested.
	auto comp = [&move_data_all](ModelObject* mo1, ModelObject* mo2) {
		auto it1 = std::find_if(move_data_all.begin(), move_data_all.end(), [&mo1](const auto& md) { return md.scheduled_object.id == mo1->id().id; });
		auto it2 = std::find_if(move_data_all.begin(), move_data_all.end(), [&mo2](const auto& md) { return md.scheduled_object.id == mo2->id().id; });
		return it1->bed_idx == it2->bed_idx ? it1 < it2 : it1->bed_idx < it2->bed_idx;
	};
	std::sort(model.objects.begin(), model.objects.end(), comp);
}







bool check_seq_printability(const Model& model, const ConfigBase& config)
{
	Sequential::PrinterGeometry printer_geometry = get_printer_geometry(config);
	Sequential::SolverConfiguration solver_config = get_solver_config(printer_geometry);
	std::vector<Sequential::ObjectToPrint> objects = get_objects_to_print(model, printer_geometry);

	if (printer_geometry.extruder_slices.empty()) {
		// If there are no data for extruder (such as extruder_clearance_radius set to 0),
		// consider it printable.
		return true;
	}

	Sequential::ScheduledPlate plate;
	for (const ModelObject* mo : model.objects) {
		int inst_id = -1;
		for (const ModelInstance* mi : mo->instances) {
			++inst_id;

			auto it = s_multiple_beds.get_inst_map().find(mi->id());
			if (it == s_multiple_beds.get_inst_map().end() || it->second != s_multiple_beds.get_active_bed())
				continue;

			Vec3d offset = s_multiple_beds.get_bed_translation(s_multiple_beds.get_active_bed());
			plate.scheduled_objects.emplace_back(inst_id == 0 ? mo->id().id : mi->id().id, scaled(mi->get_offset().x() - offset.x()), scaled(mi->get_offset().y() - offset.y()));
		}
	}

	return Sequential::check_ScheduledObjectsForSequentialPrintability(solver_config, printer_geometry, objects, std::vector<Sequential::ScheduledPlate>(1, plate));
}


} // namespace Slic3r
