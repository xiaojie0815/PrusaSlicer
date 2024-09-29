#ifndef slic3r_Arrange_Helper_hpp
#define slic3r_Arrange_Helper_hpp

class Model;

namespace Slic3r {
	void arrange_model_sequential(Model& model);
	bool check_seq_printability(const Model& model);
}

#endif // slic3r_Arrange_Helper_hpp