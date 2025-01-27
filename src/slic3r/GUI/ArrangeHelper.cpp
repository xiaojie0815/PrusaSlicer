#include "ArrangeHelper.hpp"

#include "libslic3r/Model.hpp"
#include "libslic3r/TriangleMesh.hpp"
#include "libslic3r/MultipleBeds.hpp"

#include <string>



namespace Slic3r {


static Sequential::PrinterGeometry get_printer_geometry() {
	enum ShapeType {
		BOX,
		CONVEX
	};
	struct ExtruderSlice {
		coord_t height;
		ShapeType shape_type;
		std::vector<Polygon> polygons;

	};

	// Just hardcode MK4 geometry for now.	
	std::vector<ExtruderSlice> slices;
	slices.push_back(ExtruderSlice{ 0,        CONVEX, { { {  -5000000,   -5000000 }, {   5000000,   -5000000 }, {   5000000,   5000000 }, {  -5000000,   5000000 } } } });
	slices.push_back(ExtruderSlice{ 3000000,  CONVEX, { { { -10000000,  -21000000 }, {  37000000,  -21000000 }, {  37000000,  44000000 }, { -10000000,  44000000 } },
							    { { -40000000,  -45000000 }, {  38000000,  -45000000 }, {  38000000,  20000000 }, { -40000000,  20000000 } } } });
	slices.push_back(ExtruderSlice{ 11000000, BOX,    { { {-350000000,  -23000000 }, { 350000000,  -23000000 }, { 350000000, -35000000 }, {-350000000, -35000000 } } } });
	slices.push_back(ExtruderSlice{ 13000000, BOX,    { { { -13000000,  -84000000 }, {  11000000,  -84000000 }, {  11000000, -38000000 }, { -13000000, -38000000 } },
							    { {  11000000, -300000000 }, { 300000000, -300000000 }, { 300000000, -84000000 }, {  11000000, -84000000 } } } });
	

	// Geometry (simplified head model) for the MK3S printer
	/*
	std::vector<ExtruderSlice> slices;
        slices.push_back(ExtruderSlice{ 0,        CONVEX, { { {   -5000000,  -5000000 }, {   5000000,  -5000000 }, {   5000000,    5000000 }, {   -5000000,    5000000 } } ,
                                                            { {  -30000000, -12000000 }, { -14000000, -12000000 }, { -14000000,    2000000 }, {  -30000000,    2000000 } } } });
        slices.push_back(ExtruderSlice{ 2000000,  CONVEX, { { {  -20000000, -38000000 }, {  44000000, -38000000 }, {  44000000,   18000000 }, {  -20000000,   18000000 } } } });
        slices.push_back(ExtruderSlice{ 6000000,  CONVEX, { { {  -34000000, -43000000 }, {  37000000, -43000000 }, {  37000000,   16000000 }, {  -34000000,   16000000 } },
							    { {  -45000000,   9000000 }, {  37000000,   9000000 }, {  37000000,   69000000 }, {  -45000000,   69000000 } } } });
	slices.push_back(ExtruderSlice{11000000,  BOX,    { { {   -8000000, -82000000 }, {   8000000, -82000000 }, {   8000000,  -36000000 }, {   -8000000,  -36000000 } },
							    { {   -8000000, -82000000 }, { 250000000, -82000000 }, { 250000000, -300000000 }, {   -8000000, -300000000 } } } });
	slices.push_back(ExtruderSlice{17000000,  BOX,    { { { -300000000, -35000000 }, { 300000000, -35000000 }, { 300000000,  -21000000 }, { -300000000,  -21000000 } } } });
	*/
	Sequential::PrinterGeometry out;

	/*
	out.x_size = scaled(s_multiple_beds.get_bed_size().x());
	out.y_size = scaled(s_multiple_beds.get_bed_size().y());
	*/
	coord_t plate_x_size = scaled(s_multiple_beds.get_bed_size().x());
	coord_t plate_y_size = scaled(s_multiple_beds.get_bed_size().y());
	out.plate = { { 0, 0 }, { plate_x_size, 0}, { plate_x_size, plate_y_size }, { 0, plate_y_size } };
	
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




void arrange_model_sequential(Model& model)
{
	SeqArrange seq_arrange(model);
	seq_arrange.process_seq_arrange([](int) {});
	seq_arrange.apply_seq_arrange(model);
}



SeqArrange::SeqArrange(const Model& model)
{
    m_printer_geometry = get_printer_geometry();
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







bool check_seq_printability(const Model& model)
{
	Sequential::PrinterGeometry printer_geometry = get_printer_geometry();
	Sequential::SolverConfiguration solver_config = get_solver_config(printer_geometry);
	std::vector<Sequential::ObjectToPrint> objects = get_objects_to_print(model, printer_geometry);

	// FIXME: This does not consider plates, non-printable objects and instances.
	Sequential::ScheduledPlate plate;
	for (ModelObject* mo : model.objects) {
		ModelInstance* mi = mo->instances.front();
		plate.scheduled_objects.emplace_back(mo->id().id, scaled(mi->get_offset().x()), scaled(mi->get_offset().y()));
	}

	return Sequential::check_ScheduledObjectsForSequentialPrintability(solver_config, printer_geometry, objects, std::vector<Sequential::ScheduledPlate>(1, plate));
}


} // namespace Slic3r
