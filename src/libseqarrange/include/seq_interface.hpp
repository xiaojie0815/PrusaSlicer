/*================================================================*/
/*
 * Author:  Pavel Surynek, 2023 - 2024
 * Company: Prusa Research
 *
 * File:    seq_interface.hpp
 *
 * Interface of the sequential printing SMT model for Prusa Slic3r
 */
/*================================================================*/

#ifndef __SEQ_INTERFACE_HPP__
#define __SEQ_INTERFACE_HPP__


/*----------------------------------------------------------------*/

#include "libslic3r/Polygon.hpp"
#include "libslic3r/Geometry/ConvexHull.hpp"

#include "seq_defs.hpp"


/*----------------------------------------------------------------*/

using namespace Slic3r;


/*----------------------------------------------------------------*/

namespace Sequential
{


    
/*----------------------------------------------------------------*/
    
struct PrinterGeometry
{
    coord_t x_size;
    coord_t y_size;
    
    std::set<coord_t> convex_heights;
    std::set<coord_t> box_heights;
    
    std::map<coord_t, std::vector<Slic3r::Polygon> > extruder_slices;
};

    
/*----------------------------------------------------------------*/

// Setting printer type is obsolete, will be removed
enum PrinterType
{
    SEQ_PRINTER_TYPE_UNDEFINED,
    SEQ_PRINTER_TYPE_PRUSA_MINI,    
    SEQ_PRINTER_TYPE_PRUSA_MK3S,
    SEQ_PRINTER_TYPE_PRUSA_MK4,
    SEQ_PRINTER_TYPE_PRUSA_XL,    
};


enum DecimationPrecision
{
    SEQ_DECIMATION_PRECISION_UNDEFINED,
    SEQ_DECIMATION_PRECISION_LOW,
    SEQ_DECIMATION_PRECISION_HIGH    
};
    

/*----------------------------------------------------------------*/    
    
struct SolverConfiguration
{
    SolverConfiguration();
    SolverConfiguration(const PrinterGeometry &printer_geometry);
    
    static double convert_DecimationPrecision2Tolerance(DecimationPrecision decimation_precision);
    void setup(const PrinterGeometry &printer_geometry);

    void set_DecimationPrecision(DecimationPrecision decimation_precision);
    void set_ObjectGroupSize(int object_group_size);

    int bounding_box_size_optimization_step;
    int minimum_X_bounding_box_size;
    int minimum_Y_bounding_box_size;        
    int maximum_X_bounding_box_size;
    int maximum_Y_bounding_box_size;
    int minimum_bounding_box_size;
    int maximum_bounding_box_size;
    int object_group_size;
    int temporal_spread;

    DecimationPrecision decimation_precision;

    // Setting printer type is obsolete, will be removed    
    PrinterType printer_type;
    
    string optimization_timeout;    
};

    
/*----------------------------------------------------------------*/
    
struct ObjectToPrint
{
    int id = 0;
    coord_t total_height = 0;
    std::vector<std::pair<coord_t, Slic3r::Polygon>> pgns_at_height;
};


struct ScheduledObject {
    ScheduledObject(int _id, coord_t _x, coord_t _y)
	: id(_id)
	, x(_x)
	, y(_y) { /* */ }
    
    int id = 0;
    coord_t x, y;
};


struct ScheduledPlate {
    std::vector<ScheduledObject> scheduled_objects;
};


/*----------------------------------------------------------------*/
/*
  This is the recommended interface for checking sequential printability.

  Returns true if objects are sequentially printable according to their
  ordering in the input vector and the arrangement on the plate specified
  by the schedule. Printable means that the extruder never hits printed
  objects during printing. Otherwise returns false.

  Please see the corresponding example of usage (seq_test_interface.cpp)

  Note: The function always succeeds, does not throw any exception.
*/

bool check_ScheduledObjectsForSequentialPrintability(const SolverConfiguration         &solver_configuration,
						     const PrinterGeometry             &printer_geometry,
						     const std::vector<ObjectToPrint>  &objects_to_print,
						     const std::vector<ScheduledPlate> &scheduled_plates);    

    
/*----------------------------------------------------------------*/
/*
  This is the recommended interface for sequential scheduling/arranging.

  Please see the corresponding example of usage (seq_test_interface.cpp)

  Note: The function should succeed except the case when there is an
  object that does not fit on the plate and in the case when the solver
  is unable to scedule even single object on the plate. The latter case
  is detected by timeout and should not normally happen. These failures
  are reported via exceptions.
*/

std::vector<ScheduledPlate> schedule_ObjectsForSequentialPrint(const SolverConfiguration        &solver_configuration,
							       const PrinterGeometry            &printer_geometry,
							       const std::vector<ObjectToPrint> &objects_to_print);
    
void schedule_ObjectsForSequentialPrint(const SolverConfiguration        &solver_configuration,
					const PrinterGeometry            &printer_geometry,
					const std::vector<ObjectToPrint> &objects_to_print,
					std::vector<ScheduledPlate>      &scheduled_plates);
    
    
/*----------------------------------------------------------------*/
/*
  The following interface is for more internal use.
 */
           
int schedule_ObjectsForSequentialPrint(const SolverConfiguration        &solver_configuration,
				       const std::vector<ObjectToPrint> &objects_to_print,
				       std::vector<ScheduledPlate>      &scheduled_plates);

void setup_ExtruderUnreachableZones(const SolverConfiguration                  &solver_configuration,
				    std::vector<std::vector<Slic3r::Polygon> > &convex_unreachable_zones,
				    std::vector<std::vector<Slic3r::Polygon> > &box_unreachable_zones);

int schedule_ObjectsForSequentialPrint(const SolverConfiguration                        &solver_configuration,
				       const std::vector<ObjectToPrint>                 &objects_to_print,
				       const std::vector<std::vector<Slic3r::Polygon> > &convex_unreachable_zones,
				       const std::vector<std::vector<Slic3r::Polygon> > &box_unreachable_zones,     
				       std::vector<ScheduledPlate>                      &scheduled_plates);

    
/*----------------------------------------------------------------*/

} // namespace Sequential


/*----------------------------------------------------------------*/

#endif /* __SEQ_INTERFACE_HPP__ */
