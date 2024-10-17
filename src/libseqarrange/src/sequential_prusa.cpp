/*================================================================*/
/*
 * Author:  Pavel Surynek, 2023 - 2024
 * Company: Prusa Research
 *
 * File:    sequential_prusa.cpp
 *
 * SEQUENTIAL 3D Print Scheduler|Arranger.
 */
/*================================================================*/


#include "libslic3r/Polygon.hpp"
#include "libslic3r/Geometry/ConvexHull.hpp"
#include "libslic3r/SVG.hpp"

#include "seq_version.hpp"
#include "seq_preprocess.hpp"
#include "seq_utilities.hpp"

#include "sequential_prusa.hpp"

/*----------------------------------------------------------------*/

using namespace Slic3r;
using namespace Sequential;


/*----------------------------------------------------------------*/


void print_IntroductoryMessage(void)
{
    printf("----------------------------------------------------------------\n");
    printf("SEQUENTIAL 3D Print Scheduler|Arranger - build %s\n", SEQ_SEQUENTIAL_BUILD);
    printf("(C) 2024 Prusa Research \n");
    printf("================================================================\n");	
}


void print_ConcludingMessage(void)
{
    printf("----------------------------------------------------------------\n");
}


void print_Help(void)
{
    printf("Usage:\n");
    printf("sequential_prusa [--input-file=<string>]\n");
    printf("                 [--output-file=<string>]\n");
    printf("                 [--printer-file=<string>]\n");    
    printf("                 [--decimation={yes|no}]\n");
    printf("                 [--precision={low|high}]\n");    
    printf("                 [--assumptions={yes|no}]\n");
    printf("                 [--interactive={yes|no}]\n");        
    printf("                 [--object-group-size=<int>]\n");
    printf("                 [--help]\n");		
    printf("\n");
    printf("\n");
    printf("Defaults: --input-file=arrange_data_export.txt\n");
    printf("          --output-file=arrange_data_import.txt\n");
    printf("          --printer-file=../printers/printer_geometry.mk4.compatibility.txt\n");
    printf("          --object-group-size=4 \n");    
    printf("          --decimation=yes\n");
    printf("          --precision=high\n");    
    printf("          --assumptions=yes\n");
    printf("          --interactive=no\n");    
    printf("\n");
}


int parse_CommandLineParameter(const string &parameter, CommandParameters &command_parameters)
{
    if (parameter.find("--input-file=") == 0)
    {
	command_parameters.input_filename = parameter.substr(13, parameter.size());
    }
    else if (parameter.find("--output-file=") == 0)
    {
	command_parameters.output_filename = parameter.substr(14, parameter.size());
    }
    else if (parameter.find("--printer-file=") == 0)
    {
	command_parameters.printer_filename = parameter.substr(15, parameter.size());
    }    
    else if (parameter.find("--object-group-size=") == 0)
    {
	command_parameters.object_group_size = std::atoi(parameter.substr(20, parameter.size()).c_str());
    }
    else if (parameter.find("--decimation=") == 0)
    {
	string decimation_str = parameter.substr(13, parameter.size());
	
	if (decimation_str == "yes")
	{
	    command_parameters.decimation = true;
	}
	else if (decimation_str == "no")
	{
	    command_parameters.decimation = false;
	}
	else
	{
	    return -2;
	}
    }
    else if (parameter.find("--precision=") == 0)
    {
	string decimation_str = parameter.substr(12, parameter.size());
	
	if (decimation_str == "high")
	{
	    command_parameters.precision = true;
	}
	else if (decimation_str == "low")
	{
	    command_parameters.precision = false;
	}
	else
	{
	    return -2;
	}
    }    
    else if (parameter.find("--assumptions=") == 0)
    {
	string assumptions_str = parameter.substr(14, parameter.size());
	
	if (assumptions_str == "yes")
	{
	    command_parameters.assumptions = true;
	}
	else if (assumptions_str == "no")
	{
	    command_parameters.assumptions = false;
	}
	else
	{
	    return -2;
	}
    }
    else if (parameter.find("--interactive=") == 0)
    {
	string interactive_str = parameter.substr(14, parameter.size());
	
	if (interactive_str == "yes")
	{
	    command_parameters.interactive = true;
	}
	else if (interactive_str == "no")
	{
	    command_parameters.interactive = false;
	}
	else
	{
	    return -2;
	}
    }        
    else if (parameter.find("--help") == 0)
    {
	command_parameters.help = true;
    }    
    else
    {
	return -1;
    }
    return 0;
}


string convert_Index2Suffix(int index)
{
    string buffer = "000";

    string index_str = std::to_string(index);
    buffer.replace(buffer.length() - index_str.length(), index_str.length(), index_str);
    
    return buffer;
}


int solve_SequentialPrint(const CommandParameters &command_parameters)
{
    clock_t start, finish;
    
    printf("Sequential scheduling/arranging ...\n");

    start = clock();

    SolverConfiguration solver_configuration;
    solver_configuration.object_group_size = command_parameters.object_group_size;

    PrinterGeometry printer_geometry;

    if (!command_parameters.printer_filename.empty())
    {
	printf("  Loading printer geometry ...\n");
	int result = load_printer_geometry(command_parameters.printer_filename, printer_geometry);

	if (result != 0)
	{
	    printf("Cannot load printer geometry (code: %d).\n", result);
	    return result;
	}
	solver_configuration.setup(printer_geometry);
	printf("  Loading printer geometry ... finished\n");
    }

    std::vector<ObjectToPrint> objects_to_print = load_exported_data(command_parameters.input_filename);    

    std::vector<Slic3r::Polygon> polygons;
    std::vector<std::vector<Slic3r::Polygon> > unreachable_polygons;
    std::vector<bool> lepox_to_next;

    printf("  Preparing objects ...\n");

    map<int, int> original_index_map;    

    for (unsigned int i = 0; i < objects_to_print.size(); ++i)
    {
	Polygon nozzle_polygon;
	Polygon extruder_polygon;
	Polygon hose_polygon;
	Polygon gantry_polygon;

	std::vector<Slic3r::Polygon> convex_level_polygons;	    
	std::vector<Slic3r::Polygon> box_level_polygons;

	std::vector<std::vector<Slic3r::Polygon> > extruder_convex_level_polygons;	    
	std::vector<std::vector<Slic3r::Polygon> > extruder_box_level_polygons;	
	
	std::vector<Slic3r::Polygon> scale_down_unreachable_polygons;	

	original_index_map[i] = objects_to_print[i].id;

	if (command_parameters.printer_filename.empty())
	{	    
	    for (unsigned int j = 0; j < objects_to_print[i].pgns_at_height.size(); ++j)
	    {
		coord_t height = objects_to_print[i].pgns_at_height[j].first;
		
		if (!objects_to_print[i].pgns_at_height[j].second.points.empty())
		{
		    Polygon decimated_polygon;
		    //ground_PolygonByFirstPoint(objects_to_print[i].pgns_at_height[j].second);

		    if (command_parameters.decimation)
		    {
			if (command_parameters.precision)
			{
			    solver_configuration.decimation_precision = SEQ_DECIMATION_PRECISION_HIGH;
			}
			else
			{
			    solver_configuration.decimation_precision = SEQ_DECIMATION_PRECISION_LOW;			
			}
			decimate_PolygonForSequentialSolver(solver_configuration,
							    objects_to_print[i].pgns_at_height[j].second,
							    decimated_polygon,
							    true);
		    }
		    else
		    {
			decimated_polygon = objects_to_print[i].pgns_at_height[j].second;
			decimated_polygon.make_counter_clockwise();
		    }
		    if (!check_PolygonSize(solver_configuration, SEQ_SLICER_SCALE_FACTOR, decimated_polygon))
		    {
			printf("Object too large to fit onto plate [ID:%d RID:%d].\n", original_index_map[i], i);
			return -1;
		    }
		    
		    switch (height)
		    {
		    case 0:        // nozzle
		    {
			nozzle_polygon = decimated_polygon;		    
			break;
		    }
		    case 2000000:  // extruder
		    {
			extruder_polygon = decimated_polygon;		    		    
			break;
		    }
		    case 18000000: // hose
		    {
			hose_polygon = decimated_polygon;		    		    		    
			break;
		    }
		    case 26000000: // gantry
		    {
			gantry_polygon = decimated_polygon;
			break;
		    }
		    default:
		    {
			throw std::runtime_error("UNSUPPORTED POLYGON HEIGHT");		    
			break;
		    }
		    }
		}
	    }

	    Polygon scale_down_polygon;
	    scaleDown_PolygonForSequentialSolver(nozzle_polygon, scale_down_polygon);
	    polygons.push_back(scale_down_polygon);
	    
	    convex_level_polygons.push_back(nozzle_polygon);
	    convex_level_polygons.push_back(extruder_polygon);
	    box_level_polygons.push_back(hose_polygon);
	    box_level_polygons.push_back(gantry_polygon);

	    prepare_UnreachableZonePolygons(solver_configuration,
					    convex_level_polygons,
					    box_level_polygons,
					    SEQ_UNREACHABLE_POLYGON_CONVEX_LEVELS_MK4,
					    SEQ_UNREACHABLE_POLYGON_BOX_LEVELS_MK4,
					    scale_down_unreachable_polygons);
	    
	    unreachable_polygons.push_back(scale_down_unreachable_polygons);
	    lepox_to_next.push_back(objects_to_print[i].glued_to_next);		    
	}
	else
	{
	    Polygon scale_down_object_polygon;
	    
	    prepare_ExtruderPolygons(solver_configuration,
				     printer_geometry,
				     objects_to_print[i],
				     convex_level_polygons,
				     box_level_polygons,
				     extruder_convex_level_polygons,
				     extruder_box_level_polygons,
				     true);

	    prepare_ObjectPolygons(solver_configuration,
				   convex_level_polygons,
				   box_level_polygons,
				   extruder_convex_level_polygons,
				   extruder_box_level_polygons,
				   scale_down_object_polygon,
				   scale_down_unreachable_polygons);
	    
	    unreachable_polygons.push_back(scale_down_unreachable_polygons);
	    polygons.push_back(scale_down_object_polygon);

	    lepox_to_next.push_back(objects_to_print[i].glued_to_next);
	}
		
	SVG preview_svg("sequential_prusa.svg");	    	
	for (unsigned int k = 0; k < unreachable_polygons.back().size(); ++k)
	{
	    Polygon display_unreachable_polygon = transform_UpsideDown(solver_configuration, SEQ_SVG_SCALE_FACTOR, scaleUp_PolygonForSlicer(SEQ_SVG_SCALE_FACTOR, unreachable_polygons.back()[k], 0, 0));
	    preview_svg.draw(display_unreachable_polygon, "lightgrey");   
	}
	/*
	Polygon display_unreachable_polygon = scale_UP(polygons.back(), 0, 0);
	preview_svg.draw(display_unreachable_polygon, "blue");
	*/
	preview_svg.Close();
    }

    vector<int> remaining_polygons;
    vector<int> polygon_index_map;
    //vector<int> original_index_map;
    vector<int> decided_polygons;

    for (unsigned int index = 0; index < polygons.size(); ++index)
    {
	polygon_index_map.push_back(index);
    }
    
    vector<Rational> poly_positions_X;
    vector<Rational> poly_positions_Y;
    vector<Rational> times_T;

    printf("  Preparing objects ... finished\n");

    int plate_index = 0;

    int progress_objects_done = 0;
    int progress_objects_total = objects_to_print.size();            
    
    do
    {
	decided_polygons.clear();
	remaining_polygons.clear();

	printf("  Object scheduling/arranging ...\n");
	bool optimized;
	
	if (command_parameters.assumptions)
	{
	    optimized = optimize_SubglobalConsequentialPolygonNonoverlappingBinaryCentered(solver_configuration,
											   poly_positions_X,
											   poly_positions_Y,
											   times_T,
											   polygons,
											   unreachable_polygons,
											   lepox_to_next,
											   polygon_index_map,
											   decided_polygons,
											   remaining_polygons,
											   progress_objects_done,
											   progress_objects_total);

	}
	else
	{
	    optimized = optimize_SubglobalSequentialPolygonNonoverlappingBinaryCentered(solver_configuration,
											poly_positions_X,
											poly_positions_Y,
											times_T,
											polygons,
											unreachable_polygons,
											polygon_index_map,
											decided_polygons,
											remaining_polygons);

	}

	printf("  Object scheduling/arranging ... finished\n");	
	
	if (optimized)
	{
	    printf("Polygon positions:\n");
	    for (unsigned int i = 0; i < decided_polygons.size(); ++i)
	    {
		printf("  [ID:%d,RID:%d] x:%.3f, y:%.3f (t:%.3f)\n",
		       original_index_map[decided_polygons[i]],
		       decided_polygons[i],
		       poly_positions_X[decided_polygons[i]].as_double(),
		       poly_positions_Y[decided_polygons[i]].as_double(),
		       times_T[decided_polygons[i]].as_double());
	    }
	    printf("Remaining polygons: %ld\n", remaining_polygons.size());
	    for (unsigned int i = 0; i < remaining_polygons.size(); ++i)
	    {
		printf("  ID:%d\n", original_index_map[remaining_polygons[i]]);
	    }

	    std::map<double, int> scheduled_polygons;
	    for (unsigned int i = 0; i < decided_polygons.size(); ++i)
	    {
		scheduled_polygons.insert(std::pair<double, int>(times_T[decided_polygons[i]].as_double(), decided_polygons[i]));
	    }
	    progress_objects_done += decided_polygons.size();	    

	    string output_filename;
	    
	    if (command_parameters.interactive)
	    {
		output_filename = command_parameters.output_filename;
	    }
	    else
	    {
		int suffix_position = command_parameters.output_filename.find(".");

		output_filename =   command_parameters.output_filename.substr(0, suffix_position) + "_"
 		                  + convert_Index2Suffix(plate_index)		    
		                  + command_parameters.output_filename.substr(suffix_position, command_parameters.output_filename.length());
	    }

	    save_import_data(output_filename,
			     scheduled_polygons,
			     original_index_map,
			     poly_positions_X,
			     poly_positions_Y);

	    string svg_filename;
	    
	    if (command_parameters.interactive)
	    {
 		svg_filename = "sequential_prusa.svg";
	    }
	    else
	    {
 		svg_filename = "sequential_prusa_" + convert_Index2Suffix(plate_index) +  ".svg";		
	    }
	    
	    SVG preview_svg(svg_filename);

	    if (!unreachable_polygons.empty())
	    {
		for (unsigned int i = 0; i < decided_polygons.size(); ++i)
		{
		    for (unsigned int j = 0; j < unreachable_polygons[decided_polygons[i]].size(); ++j)
		    {
			Polygon display_unreachable_polygon = transform_UpsideDown(solver_configuration, SEQ_SVG_SCALE_FACTOR, scaleUp_PolygonForSlicer(SEQ_SVG_SCALE_FACTOR,
																			unreachable_polygons[decided_polygons[i]][j],
																			poly_positions_X[decided_polygons[i]].as_double(),
																			poly_positions_Y[decided_polygons[i]].as_double()));
			
			string unreachable_color;
		
			switch(j % 8)
			{
			case 0:
			{
			    unreachable_color = "lightgray";
			    break;
			}
			case 1:
			{
			    unreachable_color = "darkgray";
			    break;
			}
			case 2:
			{
			    unreachable_color = "dimgrey";
			    break;
			}
			case 3:
			{
			    unreachable_color = "silver";
			    break;			    
			}
			case 4:
			{
			    unreachable_color = "gainsboro";
			    break;			    
			}
			case 5:
			{
			    unreachable_color = "lavender";
			    break;			    
			}
			case 6:
			{
			    unreachable_color = "lavenderblush";
			    break;			    
			}
			case 7:
			{
			    unreachable_color = "beige";
			    break;			    
			}			
			default:
			{
			    break;
			}
			}			
			preview_svg.draw(display_unreachable_polygon, unreachable_color);   
		    }
		}
	    }

	    for (unsigned int i = 0; i < decided_polygons.size(); ++i)
	    {
		Polygon display_polygon = transform_UpsideDown(solver_configuration, SEQ_SVG_SCALE_FACTOR, scaleUp_PolygonForSlicer(SEQ_SVG_SCALE_FACTOR,
																    polygons[decided_polygons[i]],
																    poly_positions_X[decided_polygons[i]].as_double(),
																    poly_positions_Y[decided_polygons[i]].as_double()));
		
		string color;
		
		switch(i % 12)
		{
		case 0:
		{
		    color = "green";
		    break;
		}
		case 1:
		{
		    color = "blue";
		    break;
		}
		case 2:
		{
		    color = "red";	    
		    break;
		}
		case 3:
		{
		    color = "grey";	    
		    break;
		}
		case 4:
		{
		    color = "cyan";
		    break;
		}
		case 5:
		{
		    color = "magenta";
		    break;
		}
		case 6:
		{
		    color = "yellow";
		    break;
		}

		case 7:
		{
		    color = "rosybrown";
		    break;
		}		
		
		case 8:
		{
		    color = "indigo";
		    break;
		}
		case 9:
		{
		    color = "olive";
		    break;
		}
		case 10:
		{
		    color = "firebrick";	    
		    break;
		}
		case 11:
		{
		    color = "violet";
		    break;
		}	
		case 12:
		{
		    color = "midnightblue";
		    break;
		}
		case 13:
		{
		    color = "khaki";
		    break;
		}
		case 14:
		{
		    color = "darkslategrey";
		    break;
		}
		case 15:
		{
		    color = "hotpink";
		    break;
		}			    	    	    
				
		default:
		{
		    break;
		}
		}
		
		preview_svg.draw(display_polygon, color);
	    }
	    std::map<double, int>::const_iterator scheduled_polygon = scheduled_polygons.begin();
	    for (unsigned int i = 0; i < decided_polygons.size(); ++i, ++scheduled_polygon)
	    {
		coord_t sx, sy, x, y;
		
		scaleUp_PositionForSlicer(SEQ_SVG_SCALE_FACTOR,
					  poly_positions_X[decided_polygons[i]].as_double(),
					  poly_positions_Y[decided_polygons[i]].as_double(),
					  sx, sy);
		transform_UpsideDown(solver_configuration, SEQ_SVG_SCALE_FACTOR, sx, sy, x, y);

		string text_color;		

		switch(i % 12)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		{
		    text_color = "black";
		    break;
		}
		case 7:
		{
		    text_color = "grey";
		    break;
		}
		case 8:
		case 9:
		case 10:
		case 11:
		{
		    text_color = "black";
		    break;
		}			    	    	    
		default:
		{
		    break;
		}
		}		
		
		preview_svg.draw_text(Point(x, y), ("ID:" + std::to_string(original_index_map[decided_polygons[i]]) + " T:" + std::to_string(times_T[decided_polygons[i]].as_int64())).c_str(), text_color.c_str());
	    }
	    Polygon plate_polygon({ { 0, 0},
				  { solver_configuration.x_plate_bounding_box_size, 0 },
				  { solver_configuration.x_plate_bounding_box_size, solver_configuration.y_plate_bounding_box_size},
				  { 0, solver_configuration.y_plate_bounding_box_size} });		
	    Polygon display_plate_polygon = scaleUp_PolygonForSlicer(SEQ_SVG_SCALE_FACTOR,
								     plate_polygon,
								     0,
								     0);
	    preview_svg.draw_outline(display_plate_polygon, "black");
	    
	    preview_svg.Close();
	}
	else
	{
	    printf("Polygon optimization FAILED.\n");
	}	
	finish = clock();	
	printf("Intermediate CPU time: %.3f\n", (finish - start) / (double)CLOCKS_PER_SEC);

	if (!remaining_polygons.empty())
	{
	    printf("Some object did not fit into plate.\n");

	    if (command_parameters.interactive)
	    {
		printf("Press ENTER to continue to the next plate ...\n");
		getchar();
	    }
	    else
	    {
		++plate_index;
		printf("Continuing to the next plate number %d ...\n", plate_index);
	    }
	}
	else
	{
	    printf("All objects fit onto plate.\n");
	}
	
	std::vector<Polygon> next_polygons;
	std::vector<vector<Polygon> > next_unreachable_polygons;
	std::vector<bool> next_lepox_to_next;

	#ifdef DEBUG
	{
	    for (unsigned int i = 0; i < polygon_index_map.size(); ++i)
	    {
		printf("  %d\n", polygon_index_map[i]);
	    }
	}
	#endif
	for (unsigned int i = 0; i < remaining_polygons.size(); ++i)
	{
	    next_polygons.push_back(polygons[remaining_polygons[i]]);	    	    
	    next_unreachable_polygons.push_back(unreachable_polygons[remaining_polygons[i]]);
	    next_lepox_to_next.push_back(lepox_to_next[remaining_polygons[i]]);
	}

	/* TODO: remove */
	polygons.clear();
	unreachable_polygons.clear();
	lepox_to_next.clear();
	
	polygon_index_map.clear();	
	
	polygons = next_polygons;
	unreachable_polygons = next_unreachable_polygons;
	lepox_to_next = next_lepox_to_next;

	std::vector<int> next_polygon_index_map;
	std::map<int, int> next_original_index_map;

	for (unsigned int index = 0; index < polygons.size(); ++index)
	{
	    next_polygon_index_map.push_back(index);
	    next_original_index_map[index] = original_index_map[remaining_polygons[index]];
	}
	polygon_index_map = next_polygon_index_map;
	original_index_map = next_original_index_map;
    }
    while (!remaining_polygons.empty());    

    finish = clock();

    printf("Sequential scheduling/arranging ... finished\n");
    printf("Total CPU time: %.3f\n", (finish - start) / (double)CLOCKS_PER_SEC);

    return 0;
}


/*----------------------------------------------------------------------------*/
// main program

int main(int argc, char **argv)
{
    int result;
    CommandParameters command_parameters;

    print_IntroductoryMessage();
   
    if (argc >= 1 && argc <= 10)
    {		
	for (int i = 1; i < argc; ++i)
	{
	    result = parse_CommandLineParameter(argv[i], command_parameters);
	    if (result < 0)
	    {
		printf("Error: Cannot parse command line parameters (code = %d).\n", result);
		print_Help();
		
		return result;
	    }
	}
	if (command_parameters.help)
	{
	    print_Help();	    
	}
	else
	{	    
	    result = solve_SequentialPrint(command_parameters);
	    if (result < 0)
	    {
		return result;
	    }
	}
    }
    else
    {
	print_Help();
    }
    print_ConcludingMessage();
    
    return 0;
}
