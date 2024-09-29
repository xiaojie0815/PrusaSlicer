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
#include "seq_sequential.hpp"
#include "seq_preprocess.hpp"


/*----------------------------------------------------------------*/

using namespace Slic3r;


/*----------------------------------------------------------------*/

namespace Sequential
{

    
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
  This is the recommended interface for testing sequential printability.

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
