#include "ArrangeHelper.hpp"

#include "libslic3r/Model.hpp"
#include "libslic3r/TriangleMesh.hpp"

#include "boost/algorithm/string/split.hpp"
#include "boost/filesystem/path.hpp"

#include <string>


namespace Slic3r {



static bool find_and_remove(std::string& src, const std::string& key)
{
	size_t pos = src.find(key);
	if (pos != std::string::npos) {
		src.erase(pos, key.length());
		return true;
	}
	return false;
}

struct ObjectToPrint {
	int id = 0;
	coord_t total_height = 0;
	std::vector<std::pair<coord_t, Polygon>> pgns_at_height;
};

std::vector<ObjectToPrint> load_exported_data(const std::string& filename)
{
	std::vector<ObjectToPrint> objects_to_print;

	std::ifstream in(filename);
	if (! in)
		throw std::runtime_error("NO EXPORTED FILE WAS FOUND");
	std::string line;

	while (in) {		
		std::getline(in, line);
		if (find_and_remove(line, "OBJECT_ID")) {
			objects_to_print.push_back(ObjectToPrint());
		    objects_to_print.back().id = std::stoi(line);
		}
		if (find_and_remove(line, "TOTAL_HEIGHT"))
			objects_to_print.back().total_height = std::stoi(line);
		if (find_and_remove(line, "POLYGON_AT_HEIGHT"))
			objects_to_print.back().pgns_at_height.emplace_back(std::make_pair(std::stoi(line), Polygon()));
		if (find_and_remove(line, "POINT")) {
			std::stringstream ss(line);
			std::string val;
			ss >> val;
			Point pt(std::stoi(val), 0);
			ss >> val;
			pt.y() = std::stoi(val);
			objects_to_print.back().pgns_at_height.back().second.append(pt);
		}
	}
	return objects_to_print;
}

std::vector<double> read_required_heights()
{
	std::vector<double> heights;
	std::ifstream out("printer_geometry.mk4.txt");
	std::string line;
	std::array<std::string, 2> keys = {"CONVEX_HEIGHT", "BOX_HEIGHT"};
	while (out) {
		std::getline(out, line);
		for (const std::string& key : keys) {
			if (size_t pos = line.find(key); pos != std::string::npos) {
				line = line.substr(pos + key.size());
				heights.push_back(double(std::stoi(line)) * SCALING_FACTOR);
				break;
			}
		}
	}
	return heights;
}



void export_arrange_data(const Model& model, const std::vector<double>& heights)
{
	std::ofstream out("arrange_data_export.txt");
	for (const ModelObject* mo : model.objects) {
		// Calculate polygon describing convex hull of everything above given heights.
		const ModelInstance* mi = mo->instances.front();
		out << "OBJECT_ID" << mo->id().id << std::endl;
		out << "TOTAL_HEIGHT" << scaled(mo->instance_bounding_box(0).size().z()) << std::endl;;
		for (double height : heights) {
			auto tr = Transform3d::Identity();
			Vec3d offset = mi->get_offset();
			tr.translate(Vec3d(-offset.x(), -offset.y(), 0.));
			Polygon pgn = its_convex_hull_2d_above(mo->mesh().its, tr.cast<float>(), height);
			out << "POLYGON_AT_HEIGHT" << scaled(height) << std::endl;
			for (const Point& pt : pgn)
				out << "POINT" << pt.x() << " " << pt.y() << std::endl;
		}
	}
}




void import_arrange_data(Model& model, bool delete_files)
{
	// First go through all files in the current directory
	// and remember all which match the pattern.
	namespace fs = boost::filesystem;
	fs::path p(".");
    fs::directory_iterator end_itr;
	std::vector<std::string> filenames;
    for (fs::directory_iterator itr(p); itr != end_itr; ++itr)
    {
        if (fs::is_regular_file(itr->path())) {
			std::string name = itr->path().filename().string();
            if (boost::starts_with(name, "arrange_data_import") && boost::ends_with(name, ".txt"))
				filenames.emplace_back(name);
        }
    }
	// Sort the files alphabetically.
	std::sort(filenames.begin(), filenames.end());


	struct MoveData {
		size_t id;
		coord_t x;
		coord_t y;
		size_t bed_idx;
	};

	// A vector to collect move data for all the files.
	std::vector<MoveData> move_data_all;


	// Now iterate through all the files, read the data and move the objects accordingly.
	// Save the move data from this file to move_data_all.
	size_t bed_idx = 0;
	for (const std::string& name : filenames) {
		std::cout << " - loading file " << name << "..." << std::endl;
		std::ifstream in(name);

		std::string line;		
		std::vector<MoveData> move_data;
		while (in) {
			std::getline(in, line);
			std::vector<std::string> values;
			boost::split(values, line, boost::is_any_of(" "));
			if (values.size() > 2)
				move_data.emplace_back(MoveData{size_t(std::stoi(values[0])), std::stoi(values[1]), std::stoi(values[2]), bed_idx});
		}

		// Iterate the same way as when exporting.
		for (ModelObject* mo : model.objects) {
			ModelInstance* mi = mo->instances.front();
			const ObjectID& oid = mo->id();
			auto it = std::find_if(move_data.begin(), move_data.end(), [&oid](const auto& md) { return md.id == oid.id; });
			if (it != move_data.end()) {
				mi->set_offset(Vec3d(unscaled(it->x) + bed_idx * 300, unscaled(it->y), mi->get_offset().z()));
			}
		}
		move_data_all.insert(move_data_all.end(), move_data.begin(), move_data.end());
		++bed_idx;
	}

	if (delete_files) {
		std::cout << " - removing all the files...";
		for (const std::string& name : filenames)
			fs::remove(fs::path(name));
		std::cout << "done" << std::endl;
	}

	// Now reorder the objects in the model so they are in the same order as requested.
	auto comp = [&move_data_all](ModelObject* mo1, ModelObject* mo2) {
		auto it1 = std::find_if(move_data_all.begin(), move_data_all.end(), [&mo1](const auto& md) { return md.id == mo1->id().id; });
		auto it2 = std::find_if(move_data_all.begin(), move_data_all.end(), [&mo2](const auto& md) { return md.id == mo2->id().id; });
		return it1->bed_idx == it2->bed_idx ? it1 < it2 : it1->bed_idx < it2->bed_idx;
	};
	std::sort(model.objects.begin(), model.objects.end(), comp);
}



void export_run_and_import_arrange_data(Model& model, const std::string& cmd, bool delete_files)
{
	std::cout << "Reading height from printer_geometry.mk4.txt" << std::endl;
	std::vector<double> heights = read_required_heights();
	if (heights.empty()) {
	    std::cout << "unable" << std::endl;
		return;
	}

	std::cout << "Exporting the arrange data...";
	export_arrange_data(model, heights);
	std::cout << "done" << std::endl;

	std::cout << "Running " << cmd << std::endl;
	int out = wxExecute(wxString::FromUTF8(cmd), wxEXEC_SYNC);
	if (out == -1) {
	    std::cout << "unable" << std::endl;
		return;
	}
	std::cout << "sequential_prusa returned " << out << (out == 0 ? ": ok" : ": appears to be an error") << std::endl;

	if (out ==0 ) {
		std::cout << "Importing the arrange data..." << std::endl;
		import_arrange_data(model, delete_files);
		std::cout << "Import done" << std::endl;
	}
}




}
