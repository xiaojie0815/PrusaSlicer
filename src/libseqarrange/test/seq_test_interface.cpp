/*================================================================*/
/*
 * Author:  Pavel Surynek, 2023 - 2024
 * Company: Prusa Research
 *
 * File:    seq_test_interface.cpp
 *
 * Tests of the sequential printing interface for Prusa Slic3r
 */
/*================================================================*/

#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

#include "libslic3r/Polygon.hpp"
#include "libslic3r/ExPolygon.hpp"
#include "libslic3r/Geometry/ConvexHull.hpp"
#include "libslic3r/SVG.hpp"

#define CATCH_CONFIG_EXTERNAL_INTERFACES
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <z3++.h>

#include "seq_interface.hpp"
#include "seq_utilities.hpp"
#include "seq_preprocess.hpp"

#include "seq_test_interface.hpp"


/*----------------------------------------------------------------*/


using namespace Sequential;


/*----------------------------------------------------------------*/

/*
static bool find_and_remove(std::string& src, const std::string& key)
{
    size_t pos = src.find(key);
    if (pos != std::string::npos) {
        src.erase(pos, key.length());
        return true;
    }
    return false;
}
*/

/*
std::vector<ObjectToPrint> load_exported_data(const std::string& filename)
{
    std::vector<ObjectToPrint> objects_to_print;

    std::ifstream in(filename);
    if (!in)
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
*/


void save_import_data(const std::string           &filename,
		      const std::map<double, int> &scheduled_polygons,
		      const map<int, int>         &original_index_map,
		      const vector<Rational>      &poly_positions_X,
		      const vector<Rational>      &poly_positions_Y)
{
    std::ofstream out(filename);
    if (!out)
        throw std::runtime_error("CANNOT CREATE IMPORT FILE");

    for (const auto& scheduled_polygon: scheduled_polygons)
    {
	coord_t X, Y;

	scaleUp_PositionForSlicer(poly_positions_X[scheduled_polygon.second],
				  poly_positions_Y[scheduled_polygon.second],
				  X,
				  Y);
	const auto& original_index = original_index_map.find(scheduled_polygon.second);
	    
//	out << original_index_map[scheduled_polygon.second] << " " << X << " " << Y << endl;
	out << original_index->second << " " << X << " " << Y << endl;	    
    }
}


/*----------------------------------------------------------------*/
/*
TEST_CASE("Interface test 1", "[Sequential Arrangement Interface]")
{ 
    clock_t start, finish;
    
    printf("Testing interface 1 ...\n");

    start = clock();

    SolverConfiguration solver_configuration;
    solver_configuration.decimation_precision = SEQ_DECIMATION_PRECISION_HIGH;

    printf("Loading objects ...\n");
    std::vector<ObjectToPrint> objects_to_print = load_exported_data("arrange_data_export.txt");
    REQUIRE(objects_to_print.size() > 0);
    printf("Loading objects ... finished\n");    

    std::vector<ScheduledPlate> scheduled_plates;
    printf("Scheduling objects for sequential print ...\n");
		
    int result = schedule_ObjectsForSequentialPrint(solver_configuration,
						    objects_to_print,
						    scheduled_plates);

    REQUIRE(result == 0);
    if (result == 0)
    {
	printf("Object scheduling for sequential print SUCCESSFUL !\n");

	printf("Number of plates: %ld\n", scheduled_plates.size());
	REQUIRE(scheduled_plates.size() > 0);

	for (unsigned int plate = 0; plate < scheduled_plates.size(); ++plate)
	{
	    printf("  Number of objects on plate: %ld\n", scheduled_plates[plate].scheduled_objects.size());
	    REQUIRE(scheduled_plates[plate].scheduled_objects.size() > 0);

	    for (const auto& scheduled_object: scheduled_plates[plate].scheduled_objects)
	    {
		cout << "    ID: " << scheduled_object.id << "  X: " << scheduled_object.x << "  Y: " << scheduled_object.y << endl;
		REQUIRE(scheduled_object.x >= 0);
		REQUIRE(scheduled_object.x <= solver_configuration.x_plate_bounding_box_size * SEQ_SLICER_SCALE_FACTOR);
		REQUIRE(scheduled_object.y >= 0);
		REQUIRE(scheduled_object.y <= solver_configuration.y_plate_bounding_box_size * SEQ_SLICER_SCALE_FACTOR);
	    }
	}
    }
    else
    {
	printf("Something went WRONG during sequential scheduling (code: %d)\n", result);
    }
    
    finish = clock();
    
    printf("Time: %.3f\n", (finish - start) / (double)CLOCKS_PER_SEC);
    printf("Testing interface 1 ... finished\n");    
}


TEST_CASE("Interface test 2", "[Sequential Arrangement Interface]")
{ 
    clock_t start, finish;
    
    printf("Testing interface 2 ...\n");

    start = clock();

    SolverConfiguration solver_configuration;
    solver_configuration.decimation_precision = SEQ_DECIMATION_PRECISION_HIGH;

    printf("Loading objects ...\n");    
    std::vector<ObjectToPrint> objects_to_print = load_exported_data("arrange_data_export.txt");

    std::vector<std::vector<Slic3r::Polygon> > convex_unreachable_zones;
    std::vector<std::vector<Slic3r::Polygon> > box_unreachable_zones;    

    printf("Preparing extruder unreachable zones ...\n");
    setup_ExtruderUnreachableZones(solver_configuration, convex_unreachable_zones, box_unreachable_zones);

    std::vector<ScheduledPlate> scheduled_plates;
    printf("Scheduling objects for sequential print ...\n");

    int result = schedule_ObjectsForSequentialPrint(solver_configuration,
						    objects_to_print,
						    convex_unreachable_zones,
						    box_unreachable_zones,
						    scheduled_plates);

    REQUIRE(result == 0);    
    if (result == 0)
    {
	printf("Object scheduling for sequential print SUCCESSFUL !\n");

	printf("Number of plates: %ld\n", scheduled_plates.size());
	REQUIRE(scheduled_plates.size() > 0);	

	for (unsigned int plate = 0; plate < scheduled_plates.size(); ++plate)
	{
	    printf("  Number of objects on plate: %ld\n", scheduled_plates[plate].scheduled_objects.size());
	    REQUIRE(scheduled_plates[plate].scheduled_objects.size() > 0);	    

	    for (const auto& scheduled_object: scheduled_plates[plate].scheduled_objects)
	    {
		cout << "    ID: " << scheduled_object.id << "  X: " << scheduled_object.x << "  Y: " << scheduled_object.y << endl;
		REQUIRE(scheduled_object.x >= 0);
		REQUIRE(scheduled_object.x <= solver_configuration.x_plate_bounding_box_size * SEQ_SLICER_SCALE_FACTOR);
		REQUIRE(scheduled_object.y >= 0);
		REQUIRE(scheduled_object.y <= solver_configuration.y_plate_bounding_box_size * SEQ_SLICER_SCALE_FACTOR);		
	    }
	}
    }
    else
    {
	printf("Something went WRONG during sequential scheduling (code: %d)\n", result);
    }          
    
    finish = clock();
    
    printf("Time: %.3f\n", (finish - start) / (double)CLOCKS_PER_SEC);
    printf("Testing interface 2 ... finished\n");    
}


TEST_CASE("Interface test 3", "[Sequential Arrangement Interface]")
{ 
    clock_t start, finish;
    
    printf("Testing interface 3 ...\n");

    start = clock();
    
    PrinterGeometry printer_geometry;
    int result = load_printer_geometry("printer_geometry.mk4.txt", printer_geometry);
    REQUIRE(result == 0);
    
    if (result != 0)
    {
	printf("Printer geometry load error.\n");
	return;
    }

    printf("x_size: %d\n", printer_geometry.x_size);
    printf("y_size: %d\n", printer_geometry.y_size);
    REQUIRE(printer_geometry.x_size > 0);
    REQUIRE(printer_geometry.y_size > 0);

    for (const auto& convex_height: printer_geometry.convex_heights)
    {
	cout << "convex_height:" << convex_height << endl;
    }

    for (const auto& box_height: printer_geometry.box_heights)
    {
	cout << "box_height:" << box_height << endl;
    }
    printf("extruder slices:\n");
    REQUIRE(printer_geometry.extruder_slices.size() > 0);
    
    for (std::map<coord_t, std::vector<Polygon> >::const_iterator extruder_slice = printer_geometry.extruder_slices.begin(); extruder_slice != printer_geometry.extruder_slices.end(); ++extruder_slice)
    {
	for (const auto &polygon: extruder_slice->second)
	{
	    printf("  polygon height: %d\n", extruder_slice->first);
	    
	    for (const auto &point: polygon.points)
	    {
		cout << "    " << point.x() << "  " << point.y() << endl;
	    }
	}
    }

    finish = clock();
    
    printf("Time: %.3f\n", (finish - start) / (double)CLOCKS_PER_SEC);
    printf("Testing interface 3 ... finished\n");    
}    


TEST_CASE("Interface test 4", "[Sequential Arrangement Interface]")
{
    clock_t start, finish;
    
    printf("Testing interface 4 ...\n");

    start = clock();

    SolverConfiguration solver_configuration;
    solver_configuration.decimation_precision = SEQ_DECIMATION_PRECISION_HIGH;
    solver_configuration.object_group_size = 4;

    printf("Loading objects ...\n");    
    std::vector<ObjectToPrint> objects_to_print = load_exported_data("arrange_data_export.txt");
    printf("Loading objects ... finished\n");

    PrinterGeometry printer_geometry;

    printf("Loading printer geometry ...\n");
    int result = load_printer_geometry("../printers/printer_geometry.mk4.compatibility.txt", printer_geometry);
    
    REQUIRE(result == 0);    
    if (result != 0)
    {
	printf("Cannot load printer geometry (code: %d).\n", result);
	return;
    }
    solver_configuration.setup(printer_geometry);
    printf("Loading printer geometry ... finished\n");
    
    std::vector<ScheduledPlate> scheduled_plates;
    printf("Scheduling objects for sequential print ...\n");

    scheduled_plates = schedule_ObjectsForSequentialPrint(solver_configuration,
							  printer_geometry,
							  objects_to_print);    

    printf("Object scheduling for sequential print SUCCESSFUL !\n");
    
    printf("Number of plates: %ld\n", scheduled_plates.size());
    REQUIRE(scheduled_plates.size() > 0);    

    for (unsigned int plate = 0; plate < scheduled_plates.size(); ++plate)
    {
	printf("  Number of objects on plate: %ld\n", scheduled_plates[plate].scheduled_objects.size());
	REQUIRE(scheduled_plates[plate].scheduled_objects.size() > 0);	
	
	for (const auto& scheduled_object: scheduled_plates[plate].scheduled_objects)
	{
	    cout << "    ID: " << scheduled_object.id << "  X: " << scheduled_object.x << "  Y: " << scheduled_object.y << endl;
	    REQUIRE(scheduled_object.x >= 0);
	    REQUIRE(scheduled_object.x <= printer_geometry.x_size);
	    REQUIRE(scheduled_object.y >= 0);
	    REQUIRE(scheduled_object.y <= printer_geometry.y_size);
	}
    }
    
    finish = clock();
    
    printf("Time: %.3f\n", (finish - start) / (double)CLOCKS_PER_SEC);
    printf("Testing interface 4 ... finished\n");
}


TEST_CASE("Interface test 5", "[Sequential Arrangement Interface]")
{
    clock_t start, finish;
    
    printf("Testing interface 5 ...\n");

    start = clock();

    SolverConfiguration solver_configuration;
    solver_configuration.decimation_precision = SEQ_DECIMATION_PRECISION_LOW;
    solver_configuration.object_group_size = 4;    

    printf("Loading objects ...\n");    
    std::vector<ObjectToPrint> objects_to_print = load_exported_data("arrange_data_export.txt");
    printf("Loading objects ... finished\n");

    PrinterGeometry printer_geometry;

    printf("Loading printer geometry ...\n");
    int result = load_printer_geometry("../printers/printer_geometry.mk4.compatibility.txt", printer_geometry);

    REQUIRE(result == 0);    
    if (result != 0)
    {
	printf("Cannot load printer geometry (code: %d).\n", result);
	return;
    }
    solver_configuration.setup(printer_geometry);
    printf("Loading printer geometry ... finished\n");
    
    std::vector<ScheduledPlate> scheduled_plates;
    printf("Scheduling objects for sequential print ...\n");

    scheduled_plates = schedule_ObjectsForSequentialPrint(solver_configuration,
							  printer_geometry,
							  objects_to_print,
							  [](int progress) { printf("Progress: %d\n", progress);
							                     REQUIRE(progress >= 0);
									     REQUIRE(progress <= 100); });

    printf("Object scheduling for sequential print SUCCESSFUL !\n");
    
    printf("Number of plates: %ld\n", scheduled_plates.size());
    REQUIRE(scheduled_plates.size() > 0);    

    for (unsigned int plate = 0; plate < scheduled_plates.size(); ++plate)
    {
	printf("  Number of objects on plate: %ld\n", scheduled_plates[plate].scheduled_objects.size());
	REQUIRE(scheduled_plates[plate].scheduled_objects.size() > 0);	
	
	for (const auto& scheduled_object: scheduled_plates[plate].scheduled_objects)
	{
	    cout << "    ID: " << scheduled_object.id << "  X: " << scheduled_object.x << "  Y: " << scheduled_object.y << endl;
	    REQUIRE(scheduled_object.x >= 0);
	    REQUIRE(scheduled_object.x <= printer_geometry.x_size);
	    REQUIRE(scheduled_object.y >= 0);
	    REQUIRE(scheduled_object.y <= printer_geometry.y_size);	    
	}
    }
    
    finish = clock();    
    printf("Solving time: %.3f\n", (finish - start) / (double)CLOCKS_PER_SEC);

    start = clock();
    
    printf("Checking sequential printability ...\n");

    bool printable = check_ScheduledObjectsForSequentialPrintability(solver_configuration,
								     printer_geometry,
								     objects_to_print,
								     scheduled_plates);    
    printf("  Scheduled/arranged objects are sequentially printable: %s\n", (printable ? "YES" : "NO"));
    REQUIRE(printable);

    printf("Checking sequential printability ... finished\n");

    finish = clock();   
    printf("Checking time: %.3f\n", (finish - start) / (double)CLOCKS_PER_SEC);    
    
    printf("Testing interface 5 ... finished\n");
}
*/

TEST_CASE("Interface test 6", "[Sequential Arrangement Interface]")
{
    clock_t start, finish;
    
    printf("Testing interface 6 ...\n");

    start = clock();

    SolverConfiguration solver_configuration;
    solver_configuration.decimation_precision = SEQ_DECIMATION_PRECISION_LOW;
    solver_configuration.object_group_size = 4;    

    printf("Loading objects ...\n");    
    std::vector<ObjectToPrint> objects_to_print = load_exported_data("arrange_data_export.txt");
    REQUIRE(objects_to_print.size() > 0);    
    printf("Loading objects ... finished\n");

    for (auto& object_to_print: objects_to_print)
    {
	object_to_print.glued_to_next = true;
    }

    PrinterGeometry printer_geometry;

    printf("Loading printer geometry ...\n");
    int result = load_printer_geometry("../printers/printer_geometry.mk4.compatibility.txt", printer_geometry);
    REQUIRE(result == 0);    
    if (result != 0)
    {
	printf("Cannot load printer geometry (code: %d).\n", result);
	return;
    }
    solver_configuration.setup(printer_geometry);
    printf("Loading printer geometry ... finished\n");
    
    std::vector<ScheduledPlate> scheduled_plates;
    printf("Scheduling objects for sequential print ...\n");

    scheduled_plates = schedule_ObjectsForSequentialPrint(solver_configuration,
							  printer_geometry,
							  objects_to_print,
							  [](int progress) { printf("Progress: %d\n", progress);
							                     REQUIRE(progress >= 0);
									     REQUIRE(progress <= 100); });

    printf("Object scheduling for sequential print SUCCESSFUL !\n");
    
    printf("Number of plates: %ld\n", scheduled_plates.size());
    REQUIRE(scheduled_plates.size() > 0);    

    for (unsigned int plate = 0; plate < scheduled_plates.size(); ++plate)
    {
	printf("  Number of objects on plate: %ld\n", scheduled_plates[plate].scheduled_objects.size());
	REQUIRE(scheduled_plates[plate].scheduled_objects.size() > 0);	
	
	for (const auto& scheduled_object: scheduled_plates[plate].scheduled_objects)
	{
	    cout << "    ID: " << scheduled_object.id << "  X: " << scheduled_object.x << "  Y: " << scheduled_object.y << endl;
	    REQUIRE(scheduled_object.x >= 0);
	    REQUIRE(scheduled_object.x <= printer_geometry.x_size);
	    REQUIRE(scheduled_object.y >= 0);
	    REQUIRE(scheduled_object.y <= printer_geometry.y_size);		
	}
    }
    
    finish = clock();    
    printf("Solving time: %.3f\n", (finish - start) / (double)CLOCKS_PER_SEC);

    start = clock();
    
    printf("Checking sequential printability ...\n");

    bool printable = check_ScheduledObjectsForSequentialPrintability(solver_configuration,
								     printer_geometry,
								     objects_to_print,
								     scheduled_plates);
    
    printf("  Scheduled/arranged objects are sequentially printable: %s\n", (printable ? "YES" : "NO"));
    REQUIRE(printable);    

    printf("Checking sequential printability ... finished\n");

    finish = clock();   
    printf("Checking time: %.3f\n", (finish - start) / (double)CLOCKS_PER_SEC);    
    
    printf("Testing interface 6 ... finished\n");
}


/*----------------------------------------------------------------*/


