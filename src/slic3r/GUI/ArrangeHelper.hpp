#ifndef slic3r_Arrange_Helper_hpp
#define slic3r_Arrange_Helper_hpp

class Model;

namespace Slic3r {
	void export_run_and_import_arrange_data(Model&, const std::string&, bool);
	void export_arrange_data(const Model& model, const std::vector<double>& heights);
	void import_arrange_data(Model& model, bool delete_files);
}

#endif // slic3r_Arrange_Helper_hpp