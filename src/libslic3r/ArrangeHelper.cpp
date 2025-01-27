#include "ArrangeHelper.hpp"

#include "libslic3r/Model.hpp"
#include "libslic3r/TriangleMesh.hpp"
#include "libslic3r/MultipleBeds.hpp"
#include "libslic3r/PresetBundle.hpp"

#include <string>

#include "boost/regex.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/algorithm/string/replace.hpp""



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

	std::vector<std::vector<ExtruderSlice>> printers_geometries;
	std::vector<std::array<std::string, 2>> printers_regexps;
	std::vector<ExtruderSlice> slices;
	
	// Just hardcode geometry (simplified head model) for the Original Prusa MK4.
	slices.push_back(ExtruderSlice{ 0,        CONVEX, { { {  -5000000,   -5000000 }, {   5000000,   -5000000 }, {   5000000,   5000000 }, {  -5000000,   5000000 } } } });
	slices.push_back(ExtruderSlice{ 3000000,  CONVEX, { { { -10000000,  -21000000 }, {  37000000,  -21000000 }, {  37000000,  44000000 }, { -10000000,  44000000 } },
							    { { -40000000,  -45000000 }, {  38000000,  -45000000 }, {  38000000,  20000000 }, { -40000000,  20000000 } } } });
	slices.push_back(ExtruderSlice{ 11000000, BOX,    { { {-350000000,  -23000000 }, { 350000000,  -23000000 }, { 350000000, -35000000 }, {-350000000, -35000000 } } } });
	slices.push_back(ExtruderSlice{ 13000000, BOX,    { { { -13000000,  -84000000 }, {  11000000,  -84000000 }, {  11000000, -38000000 }, { -13000000, -38000000 } },
							    { {  11000000, -300000000 }, { 300000000, -300000000 }, { 300000000, -84000000 }, {  11000000, -84000000 } } } });
	printers_geometries.emplace_back(slices);
	printers_regexps.push_back({ ".*PRINTER_MODEL_MK4.*", "prusa3d_mk4_gantry.stl" });
	slices = {};
	
	// Geometry (simplified head model) for the Original Prusa MK3S+ printer
    slices.push_back(ExtruderSlice{ 0,        CONVEX, { { {   -5000000,  -5000000 }, {   5000000,  -5000000 }, {   5000000,    5000000 }, {   -5000000,    5000000 } } ,
                                                        { {  -30000000, -12000000 }, { -14000000, -12000000 }, { -14000000,    2000000 }, {  -30000000,    2000000 } } } });
    slices.push_back(ExtruderSlice{ 2000000,  CONVEX, { { {  -20000000, -38000000 }, {  44000000, -38000000 }, {  44000000,   18000000 }, {  -20000000,   18000000 } } } });
    slices.push_back(ExtruderSlice{ 6000000,  CONVEX, { { {  -34000000, -43000000 }, {  37000000, -43000000 }, {  37000000,   16000000 }, {  -34000000,   16000000 } },
							    { {  -45000000,   9000000 }, {  37000000,   9000000 }, {  37000000,   69000000 }, {  -45000000,   69000000 } } } });
	slices.push_back(ExtruderSlice{11000000,  BOX,    { { {   -8000000, -82000000 }, {   8000000, -82000000 }, {   8000000,  -36000000 }, {   -8000000,  -36000000 } },
							    { {   -8000000, -82000000 }, { 250000000, -82000000 }, { 250000000, -300000000 }, {   -8000000, -300000000 } } } });
	slices.push_back(ExtruderSlice{17000000,  BOX,    { { { -300000000, -35000000 }, { 300000000, -35000000 }, { 300000000,  -21000000 }, { -300000000,  -21000000 } } } });
	printers_geometries.emplace_back(slices);
	printers_regexps.push_back({ ".*PRINTER_MODEL_MK3.*", "prusa3d_mk3s_gantry.stl" });
	slices = {};

	// Geometry (simplified head model) for the Original Prusa Mini+ printer
	slices.push_back(ExtruderSlice{ 0,        CONVEX, { { {  -5000000,   -5000000 }, {   5000000,   -5000000 }, {   5000000,   5000000 }, {  -5000000,   5000000 } },
						            { {  24000000,   -3000000 }, {  35000000,   -3000000 }, {  35000000,  10000000 }, {  24000000,  10000000 } },
						 	    { {  -5000000,    4000000 }, {   5000000,    4000000 }, {   5000000,  18000000 }, {  -5000000,  18000000 } } } });
	slices.push_back(ExtruderSlice{ 3000000,  CONVEX, { { { -16000000,  -44000000 }, {  37000000,  -44000000 }, {  37000000,  31000000 }, { -16000000,  31000000 } } } });
	slices.push_back(ExtruderSlice{ 10000000, CONVEX, { { { -10000000,  -88000000 }, {  10000000,  -88000000 }, {  10000000, -38000000 }, { -10000000, -38000000 } },
							    { { -17000000,  -44000000 }, {  43000000,  -44000000 }, {  43000000,  33000000 }, { -17000000,  33000000 } } } });
	slices.push_back(ExtruderSlice{ 22000000, BOX,	  { { {-200000000,  -28000000 }, { 200000000,  -28000000 }, { 200000000, -14000000 }, { -200000000, -14000000 } } } });
	slices.push_back(ExtruderSlice{100000000, BOX,    { { {-200000000, -200000000 }, {  10000000, -200000000 }, {  10000000,  10000000 }, { -200000000,  10000000 } } } });
	printers_geometries.emplace_back(slices);
	printers_regexps.push_back({ ".*PRINTER_MODEL_MINI.*", "prusa3d_mini_gantry.stl" });
	slices = {};


	// Geometry (simplified head model) for the Original Prusa XL printer
	slices.push_back(ExtruderSlice{0,  	  CONVEX, { { {   -5000000,   -5000000 }, {   5000000,   -5000000 }, {   5000000,   5000000 }, {   -5000000,    5000000 } } } });
	slices.push_back(ExtruderSlice{2000000,   CONVEX, { { {  -10000000,  -47000000 }, {  34000000,  -47000000 }, {  34000000,  16000000 }, {  -10000000,   16000000 } },
						 	    { {  -34000000,   13000000 }, {  32000000,   13000000 }, {  32000000,  67000000 }, {  -34000000,   67000000 } } } });
	slices.push_back(ExtruderSlice{23000000,  CONVEX, { { {  -42000000,   11000000 }, {  32000000,   11000000 }, {  32000000,  66000000 }, {  -42000000,   66000000 } },
						 	    { {  -33000000,  -37000000 }, {  43000000,  -37000000 }, {  43000000,  18000000 }, {  -33000000,   18000000 } },
							    { {  -13000000,  -68000000 }, {  47000000,  -68000000 }, {  47000000, -30000000 }, {  -13000000,  -30000000 } } } });
    slices.push_back(ExtruderSlice{19000000,  BOX,	  { { { -400000000,   24000000 }, { 400000000,   24000000 }, { 400000000,  50000000 }, { -400000000,   50000000 } } } });
    slices.push_back(ExtruderSlice{180000000, BOX,	  { { { -400000000, -400000000 }, { 400000000, -400000000 }, { 400000000,  10000000 }, { -400000000,   10000000 } } } });
    slices.push_back(ExtruderSlice{220000000, BOX,	  { { { -400000000, -400000000 }, { 400000000, -400000000 }, { 400000000, 400000000 }, { -400000000,  400000000 } } } });
	printers_geometries.emplace_back(slices);
	printers_regexps.push_back({ ".*PRINTER_MODEL_XL.*", "prusa3d_xl_gantry.stl" });
	slices = {};
	

	double bed_x = s_multiple_beds.get_bed_size().x();
	double bed_y = s_multiple_beds.get_bed_size().y();


	{
		// JUST FOR DEBUGGING: Dump slices into JSON.
		int printer_id = 0;
		boost::property_tree::ptree pt;
		boost::property_tree::ptree printers_array;
		for (const auto& printer : printers_geometries) {
			boost::property_tree::ptree printer_node;
			printer_node.put("printer_notes_regex", printers_regexps[printer_id][0]);
			printer_node.put("gantry_model_filename", printers_regexps[printer_id++][1]);
			boost::property_tree::ptree slices_array;
			for (const auto& slice : printer) {
				boost::property_tree::ptree slice_node;
				slice_node.put("height", unscaled(slice.height));
				slice_node.put("type", slice.shape_type == BOX ? "box" : "convex");

				boost::property_tree::ptree polygons_array;
				for (const auto& polygon : slice.polygons) {
					boost::property_tree::ptree polygon_node;
					std::string s;
					for (auto& pt : polygon.points)
						s += std::to_string(int(unscaled(pt.x()))) + "," + std::to_string(int(unscaled(pt.y()))) + ";";
					s.pop_back();
					polygon_node.put("", s); // "" for array elements
					polygons_array.push_back(std::make_pair("", polygon_node));
				}
				slice_node.add_child("polygons", polygons_array);
				slices_array.push_back(std::make_pair("", slice_node));
			}
			printer_node.add_child("slices", slices_array);
			printers_array.push_back(std::make_pair("", printer_node));
		}
		pt.add_child("printers", printers_array);
		boost::property_tree::write_json("out.txt", pt);
	}




	slices = {};
	const std::string printer_notes = config.opt_string("printer_notes");
	{
		if (! printer_notes.empty()) {
			try {
				std::ifstream in(resources_dir() + "/data/printer_gantries/geometries.txt");
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
			slices.push_back(ExtruderSlice{ 0, CONVEX, { { {  -5000000,   -5000000 }, {   5000000,   -5000000 }, {   5000000,   5000000 }, {  -5000000,   5000000 } } } });
            slices.push_back(ExtruderSlice{ 1000000, BOX, { { {  -r, -r }, { r, -r }, {   r,   r }, {  -r,  r } } } });
            slices.push_back(ExtruderSlice{ h, BOX, { { { -scaled(bed_x),  -r }, { scaled(bed_x),  -r }, { scaled(bed_x), r }, { -scaled(bed_x), r}}}});
		}
	}


	// Convert the read data so libseqarrange understands them.
	Sequential::PrinterGeometry out;
	out.plate = { { 0, 0 }, { scaled(bed_x), 0}, {scaled(bed_x), scaled(bed_y)}, {0, scaled(bed_y)}};
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
	std::vector<Sequential::ObjectToPrint> objects;
	for (const ModelObject* mo : model.objects) {
		size_t inst_id = 0;
		const TriangleMesh& raw_mesh = mo->raw_mesh();
		for (const ModelInstance* mi : mo->instances) {
			objects.emplace_back(Sequential::ObjectToPrint{int(inst_id == 0 ? mo->id().id  : mi->id().id), inst_id + 1 < mo->instances.size(),
				scaled(mo->instance_bounding_box(inst_id).size().z()), {}});
			for (double height : heights) {
			        // It seems that zero level in the object instance is mi->get_offset().z(), however need to have bed as zero level,
			        // hence substracting mi->get_offset().z() from height seems to be an easy hack
 			        Polygon pgn = its_convex_hull_2d_above(raw_mesh.its, mi->get_matrix_no_offset().cast<float>(), height - mi->get_offset().z());
				objects.back().pgns_at_height.emplace_back(std::make_pair(scaled(height), pgn));
			}
			++inst_id;
		}
	}
	return objects;
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

	// FIXME: This does not consider plates, non-printable objects and instances.
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
