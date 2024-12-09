#ifndef slic3r_Arrange_Helper_hpp
#define slic3r_Arrange_Helper_hpp

#include "libseqarrange/seq_interface.hpp"



namespace Slic3r {

	class Model;

	void arrange_model_sequential(Model& model);
	bool check_seq_printability(const Model& model);


	// This is just a helper class to collect data for seq. arrangement, running the arrangement
	// and applying the results to model. It is here so the processing itself can be offloaded
	// into a separate thread without copying the Model or sharing it with UI thread.
	class SeqArrange {
	public:
		explicit SeqArrange(const Model& model);
		void process_seq_arrange(std::function<void(int)> progress_fn);
		void apply_seq_arrange(Model& model) const;

	private:
		// Following three are inputs, filled in by the constructor.
		Sequential::PrinterGeometry m_printer_geometry;
		Sequential::SolverConfiguration m_solver_configuration;
		std::vector<Sequential::ObjectToPrint> m_objects;

		// This is the output, filled in by process_seq_arrange.
		std::vector<Sequential::ScheduledPlate> m_plates;
	};
	
}

#endif // slic3r_Arrange_Helper_hpp