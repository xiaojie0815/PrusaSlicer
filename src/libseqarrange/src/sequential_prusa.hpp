/*================================================================*/
/*
 * Author:  Pavel Surynek, 2023 - 2025
 * Company: Prusa Research
 *
 * File:    sequential_prusa.hpp
 *
 * SEQUENTIAL 3D Print Scheduler|Arranger.
 */
/*================================================================*/

#ifndef __SEQUENTIAL_PRUSA_HPP__
#define __SEQUENTIAL_PRUSA_HPP__

/*----------------------------------------------------------------*/

#include "seq_sequential.hpp"
#include "seq_preprocess.hpp"
#include "seq_interface.hpp"


/*----------------------------------------------------------------*/

struct CommandParameters
{	
    CommandParameters()
	: decimation(true)
	, precision(true)
	, assumptions(true)
	, interactive(false)	  
	, object_group_size(4)
	, input_filename("arrange_data_export.txt")
	, output_filename("arrange_data_import.txt")
	, printer_filename("../printers/printer_geometry.mk4.compatibility.txt")
	, help(false)
    {
	/* nothing */
    }
   
    bool decimation;
    bool precision;
    bool assumptions;
    bool interactive;    
    int object_group_size;

    string input_filename;
    string output_filename;
    string printer_filename;    

    bool help;
};


/*----------------------------------------------------------------------------*/

    void print_IntroductoryMessage(void);
    void print_ConcludingMessage(void);
    void print_Help(void);
    
    int parse_CommandLineParameter(const string &parameter, CommandParameters &parameters);
    int solve_SequentialPrint(const CommandParameters &command_parameters);

/*----------------------------------------------------------------*/

#endif /* __SEQUENTIAL_PRUSA_HPP__ */
