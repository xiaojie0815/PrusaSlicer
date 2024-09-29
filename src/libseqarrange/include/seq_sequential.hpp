/*================================================================*/
/*
 * Author:  Pavel Surynek, 2023 - 2024
 * Company: Prusa Research
 *
 * File:    seq_sequential.hpp
 *
 * SMT models for sequential printing.
 */
/*================================================================*/

#ifndef __SEQ_SEQUENTIAL_HPP__
#define __SEQ_SEQUENTIAL_HPP__


/*----------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <vector>

#include <unordered_map>

#include "libslic3r/Geometry.hpp"
#include "libslic3r/ExPolygon.hpp"
#include "libslic3r/Geometry/ConvexHull.hpp"

#include <z3++.h>

#include "seq_defs.hpp"


/*----------------------------------------------------------------*/

using namespace Slic3r;


/*----------------------------------------------------------------*/

namespace Sequential
{


    
/*----------------------------------------------------------------*/

const coord_t SEQ_SLICER_SCALE_FACTOR = 100000;
const coord_t SEQ_SVG_SCALE_FACTOR    =  50000;  

    
#define SEQ_INTERSECTION_REPULSION_MIN    "-0.01"
#define SEQ_INTERSECTION_REPULSION_MAX    "1.01"
#define SEQ_TEMPORAL_ABSENCE_THRESHOLD    "-16"
#define SEQ_TEMPORAL_PRESENCE_THRESHOLD    "16"

#define SEQ_Z3_SOLVER_TIMEOUT              "8000"
    
const int SEQ_GROUND_PRESENCE_TIME    = 32;
const int64_t SEQ_RATIONAL_PRECISION  = 1000;
const double SEQ_DECIMATION_TOLERANCE = 400000.0;

const double SEQ_DECIMATION_TOLERANCE_VALUE_UNDEFINED = 0.0;    
const double SEQ_DECIMATION_TOLERANCE_VALUE_LOW       = 150000.0;
const double SEQ_DECIMATION_TOLERANCE_VALUE_HIGH      = 450000.0;    

    
/*----------------------------------------------------------------*/

typedef std::basic_string<char> string;
typedef std::unordered_map<string, int> string_map;

    
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
    
enum PrinterType
{
    SEQ_PRINTER_TYPE_UNDEFINED,
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

const int SEQ_PRUSA_MK3S_X_SIZE = 2500;
const int SEQ_PRUSA_MK3S_Y_SIZE = 2100;    
    
const coord_t SEQ_PRUSA_MK3S_NOZZLE_LEVEL   = 0;
const coord_t SEQ_PRUSA_MK3S_EXTRUDER_LEVEL = 2000000;
const coord_t SEQ_PRUSA_MK3S_HOSE_LEVEL     = 18000000;
const coord_t SEQ_PRUSA_MK3S_GANTRY_LEVEL   = 26000000;

    
const int SEQ_PRUSA_MK4_X_SIZE = 2500;
const int SEQ_PRUSA_MK4_Y_SIZE = 2100;    

// TODO: measure for true values    
const coord_t SEQ_PRUSA_MK4_NOZZLE_LEVEL   = 0;
const coord_t SEQ_PRUSA_MK4_EXTRUDER_LEVEL = 2000000;
const coord_t SEQ_PRUSA_MK4_HOSE_LEVEL     = 18000000;
const coord_t SEQ_PRUSA_MK4_GANTRY_LEVEL   = 26000000;

const int SEQ_PRUSA_XL_X_SIZE = 3600;
const int SEQ_PRUSA_XL_Y_SIZE = 3600;    
    
// TODO: measure for true values    
const coord_t SEQ_PRUSA_XL_NOZZLE_LEVEL   = 0;
const coord_t SEQ_PRUSA_XL_EXTRUDER_LEVEL = 2000000;
const coord_t SEQ_PRUSA_XL_HOSE_LEVEL     = 18000000;
const coord_t SEQ_PRUSA_XL_GANTRY_LEVEL   = 26000000;        
    

/*----------------------------------------------------------------*/    
    
struct SolverConfiguration
{
    SolverConfiguration()
	: bounding_box_size_optimization_step(4)
	, minimum_X_bounding_box_size(10)
	, minimum_Y_bounding_box_size(10)
	, maximum_X_bounding_box_size(SEQ_PRUSA_MK3S_X_SIZE)
	, maximum_Y_bounding_box_size(SEQ_PRUSA_MK3S_Y_SIZE)
	, minimum_bounding_box_size(MIN(minimum_X_bounding_box_size, minimum_Y_bounding_box_size))
	, maximum_bounding_box_size(MAX(maximum_X_bounding_box_size, maximum_Y_bounding_box_size))
	, object_group_size(4)
	, temporal_spread(16)
	, decimation_precision(SEQ_DECIMATION_PRECISION_UNDEFINED)
	, printer_type(SEQ_PRINTER_TYPE_PRUSA_MK3S)
	, optimization_timeout(SEQ_Z3_SOLVER_TIMEOUT)
    {
	/* nothing */
    }

    SolverConfiguration(const PrinterGeometry &printer_geometry)
	: bounding_box_size_optimization_step(4)
	, minimum_X_bounding_box_size(10)
	, minimum_Y_bounding_box_size(10)
	, maximum_X_bounding_box_size(printer_geometry.x_size / SEQ_SLICER_SCALE_FACTOR)
	, maximum_Y_bounding_box_size(printer_geometry.y_size / SEQ_SLICER_SCALE_FACTOR)
	, minimum_bounding_box_size(MIN(minimum_X_bounding_box_size, minimum_Y_bounding_box_size))
	, maximum_bounding_box_size(MAX(maximum_X_bounding_box_size, maximum_Y_bounding_box_size))
	, object_group_size(4)
	, temporal_spread(16)
	, decimation_precision(SEQ_DECIMATION_PRECISION_UNDEFINED)
	, printer_type(SEQ_PRINTER_TYPE_PRUSA_MK3S)
	, optimization_timeout(SEQ_Z3_SOLVER_TIMEOUT)
    {
	/* nothing */
    }

    static double convert_DecimationPrecision2Tolerance(DecimationPrecision decimation_precision)
    {
	switch (decimation_precision)
	{
	case SEQ_DECIMATION_PRECISION_UNDEFINED:
	{
	    return SEQ_DECIMATION_TOLERANCE_VALUE_UNDEFINED;	    
	    break;
	}
	case SEQ_DECIMATION_PRECISION_LOW:
	{
	    return SEQ_DECIMATION_TOLERANCE_VALUE_HIGH;
	    break;
	}	
	case SEQ_DECIMATION_PRECISION_HIGH:
	{
	    return SEQ_DECIMATION_TOLERANCE_VALUE_LOW;
	    break;
	}
	default:
	{
	    break;
	}
	}
	return SEQ_DECIMATION_TOLERANCE_VALUE_UNDEFINED;	
    }

    void setup(const PrinterGeometry &printer_geometry)
    {
	maximum_X_bounding_box_size = printer_geometry.x_size / SEQ_SLICER_SCALE_FACTOR;
	maximum_Y_bounding_box_size = printer_geometry.y_size / SEQ_SLICER_SCALE_FACTOR;
	minimum_bounding_box_size = MIN(minimum_X_bounding_box_size, minimum_Y_bounding_box_size);
	maximum_bounding_box_size = MAX(maximum_X_bounding_box_size, maximum_Y_bounding_box_size);
    }
    
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
    PrinterType printer_type;
    
    string optimization_timeout;    
};


/*----------------------------------------------------------------*/

struct Rational
{
    Rational()
	: numerator(0)
	, denominator(1)
    {
	/* nothing */
    }

    Rational(int64_t n)
	: numerator(n)
	, denominator(1)	
    {
	/* nothing */	
    }
    
    Rational(int64_t n, int64_t d)
	: numerator(n)
	, denominator(d)
    {
	/* nothing */
    }

    Rational(const z3::expr &expr)
    {
	if (expr.denominator().as_int64() != 0)
	{
	    if (expr.numerator().as_int64() != 0)
	    {
		numerator = expr.numerator().as_int64();
		denominator = expr.denominator().as_int64();
	    }
	    else
	    {
		double expr_val = expr.as_double();
		if (fabs(expr_val) > EPSILON)
		{
		    numerator = expr_val * SEQ_RATIONAL_PRECISION;
		    denominator = SEQ_RATIONAL_PRECISION;
		}
		else
		{
		    numerator = 0;
		    denominator = 1; 
		}
	    }
	}
	else
	{
	    numerator = expr.as_double() * SEQ_RATIONAL_PRECISION;
	    denominator = SEQ_RATIONAL_PRECISION;
	}
    }

    bool is_Positive(void) const
    {
	return ((numerator > 0 && denominator > 0) || (numerator < 0 && denominator < 0));
    }

    bool is_Negative(void) const
    {	
	return ((numerator > 0 && denominator < 0) || (numerator < 0 && denominator > 0));	
    }

    double as_double() const
    {
	return (double)numerator / denominator;
    }

    int64_t as_int64() const
    {
	return numerator / denominator;
    }    

    Rational operator+(int64_t val) const
    {
	return Rational(numerator + val * denominator, denominator);
    }

    Rational operator*(int64_t val) const
    {
	return Rational(numerator * val, denominator);
    }

    Rational normalize(void) const
    {
	return Rational(as_double() * SEQ_RATIONAL_PRECISION, SEQ_RATIONAL_PRECISION);
    }

    bool operator<(const Rational &rational) const
    {
	return (as_double() < rational.as_double());
    }
    
    bool operator>(const Rational &rational) const
    {
	return (as_double() > rational.as_double());
    }
    
    int64_t numerator;
    int64_t denominator;
};


/*----------------------------------------------------------------*/

bool lines_intersect_(coord_t ax, coord_t ay, coord_t ux, coord_t uy, coord_t bx, coord_t by, coord_t vx, coord_t vy);
bool lines_intersect(double ax, double ay, double ux, double uy, double bx, double by, double vx, double vy);
bool lines_intersect_closed(double ax, double ay, double ux, double uy, double bx, double by, double vx, double vy);
bool lines_intersect_open(double ax, double ay, double ux, double uy, double bx, double by, double vx, double vy);


/*----------------------------------------------------------------*/

void introduce_DecisionBox(z3::solver     &Solver,
			   const z3::expr &dec_var_X,
			   const z3::expr &dec_var_Y,
			   int             box_size_x,
			   int             box_size_y);

void assume_DecisionBox(const z3::expr  &dec_var_X,
			const z3::expr  &dec_var_Y,
			int              box_size_x,
			int              box_size_y,
			z3::expr_vector &box_constraints);


void introduce_BedBoundingBox(z3::solver            &Solver,
			      const z3::expr        &dec_var_X,
			      const z3::expr        &dec_var_Y,			      
			      const Slic3r::Polygon &polygon,
			      int                    box_size_x,
			      int                    box_size_y);

void assume_BedBoundingBox(const z3::expr        &dec_var_X,
			   const z3::expr        &dec_var_Y,			      
			   const Slic3r::Polygon &polygon,
			   int                    box_size_x,
			   int                    box_size_y,
			   z3::expr_vector       &bounding_constraints);

void introduce_BedBoundingBox(z3::solver            &Solver,
			      const z3::expr        &dec_var_X,
			      const z3::expr        &dec_var_Y,			      
			      const Slic3r::Polygon &polygon,
			      int                    box_min_x,
			      int                    box_min_y,
			      int                    box_max_x,
			      int                    box_max_y);
			      

void assume_BedBoundingBox(const z3::expr        &dec_var_X,
			   const z3::expr        &dec_var_Y,			      
			   const Slic3r::Polygon &polygon,
			   int                    box_min_x,
			   int                    box_min_y,
			   int                    box_max_x,
			   int                    box_max_y,			   
			   z3::expr_vector       &bounding_constraints);

void introduce_BedBoundingBox(z3::solver                         &Solver,
			      const z3::expr_vector              &dec_vars_X,
			      const z3::expr_vector              &dec_vars_Y,
			      const std::vector<Slic3r::Polygon> &polygons,
			      int                                 box_size_x,
			      int                                 box_size_y);

void assume_BedBoundingBox(const z3::expr_vector              &dec_vars_X,
			   const z3::expr_vector              &dec_vars_Y,
			   const std::vector<Slic3r::Polygon> &polygons,
			   int                                 box_size_x,
			   int                                 box_size_y,
			   z3::expr_vector                    &bounding_constraints);

void introduce_BedBoundingBox(z3::solver                         &Solver,
			      const z3::expr_vector              &dec_vars_X,
			      const z3::expr_vector              &dec_vars_Y,
			      const std::vector<Slic3r::Polygon> &polygons,
			      int                                 box_min_x,
			      int                                 box_min_y,			   
			      int                                 box_max_x,
			      int                                 box_max_y);

void assume_BedBoundingBox(const z3::expr_vector              &dec_vars_X,
			   const z3::expr_vector              &dec_vars_Y,
			   const std::vector<Slic3r::Polygon> &polygons,
			   int                                 box_min_x,
			   int                                 box_min_y,
			   int                                 box_max_x,
			   int                                 box_max_y,			   
			   z3::expr_vector                    &bounding_constraints);

void assume_ConsequentialObjectPresence(z3::context            &Context,
					const z3::expr_vector  &dec_vars_T,
					const std::vector<int> &present,
					const std::vector<int> &missing,
					z3::expr_vector        &presence_constraints);


/*----------------------------------------------------------------*/

void introduce_TemporalOrdering(z3::solver                         &Solver,
				z3::context                        &Context,
				const z3::expr_vector              &dec_vars_T,
				int                                temporal_spread,
				const std::vector<Slic3r::Polygon> &polygons);

void introduce_SequentialTemporalOrderingAgainstFixed(z3::solver                         &Solver,
						      z3::context                        &Context,
						      const z3::expr_vector              &dec_vars_T,
						      std::vector<Rational>              &dec_values_T,
						      const std::vector<int>             &fixed,
						      const std::vector<int>             &undecided,				
						      int                                temporal_spread,
						      const std::vector<Slic3r::Polygon> &polygons);

void introduce_ConsequentialTemporalOrderingAgainstFixed(z3::solver                         &Solver,
							 z3::context                        &Context,
							 const z3::expr_vector              &dec_vars_T,
							 std::vector<Rational>              &dec_values_T,
							 const std::vector<int>             &fixed,
							 const std::vector<int>             &undecided,				
							 int                                temporal_spread,
							 const std::vector<Slic3r::Polygon> &polygons);

/*----------------------------------------------------------------*/

void introduce_LineNonIntersection(z3::solver         &Solver,
				   z3::context        &Context,				   
				   const z3::expr     &dec_var_X1,
				   const z3::expr     &dec_var_Y1,
				   const z3::expr     &dec_var_T1,
				   const Slic3r::Line &line1,
				   const z3::expr     &dec_var_X2,
				   const z3::expr     &dec_var_Y2,
				   const z3::expr     &dec_var_T2,				   
				   const Slic3r::Line &line2);

void introduce_SequentialLineNonIntersection(z3::solver         &Solver,
					     z3::context        &Context,					     
					     const z3::expr     &dec_var_X1,
					     const z3::expr     &dec_var_Y1,
					     const z3::expr     &dec_var_T1,
					     const z3::expr     &dec_var_t1,
					     const Slic3r::Line &line1,
					     const z3::expr     &dec_var_X2,
					     const z3::expr     &dec_var_Y2,
					     const z3::expr     &dec_var_T2,
					     const z3::expr     &dec_var_t2,
					     const Slic3r::Line &line2);

void introduce_ConsequentialLineNonIntersection(z3::solver         &Solver,
						z3::context        &Context,					     
						const z3::expr     &dec_var_X1,
						const z3::expr     &dec_var_Y1,
						const z3::expr     &dec_var_T1,
						const z3::expr     &dec_var_t1,
						const Slic3r::Line &line1,
						const z3::expr     &dec_var_X2,
						const z3::expr     &dec_var_Y2,
						const z3::expr     &dec_var_T2,
						const z3::expr     &dec_var_t2,
						const Slic3r::Line &line2);

void introduce_LineNonIntersection_implicit(z3::solver         &Solver,
					    z3::context        &Context,					    
					    const z3::expr     &dec_var_X1,
					    const z3::expr     &dec_var_Y1,
					    const z3::expr     &dec_var_T1,
					    const Slic3r::Line &line1,
					    const z3::expr     &dec_var_X2,
					    const z3::expr     &dec_var_Y2,
					    const z3::expr     &dec_var_T2,				   
					    const Slic3r::Line &line2);

void introduce_SequentialLineNonIntersection_implicit(z3::solver         &Solver,
						      z3::context        &Context,						      
						      const z3::expr     &dec_var_X1,
						      const z3::expr     &dec_var_Y1,
						      const z3::expr     &dec_var_T1,
						      const z3::expr     &dec_var_t1,
						      const Slic3r::Line &line1,
						      const z3::expr     &dec_var_X2,
						      const z3::expr     &dec_var_Y2,
						      const z3::expr     &dec_var_T2,
						      const z3::expr     &dec_var_t2,
						      const Slic3r::Line &line2);

void introduce_ConsequentialLineNonIntersection_implicit(z3::solver         &Solver,
							 z3::context        &Context,						      
							 const z3::expr     &dec_var_X1,
							 const z3::expr     &dec_var_Y1,
							 const z3::expr     &dec_var_T1,
							 const z3::expr     &dec_var_t1,
							 const Slic3r::Line &line1,
							 const z3::expr     &dec_var_X2,
							 const z3::expr     &dec_var_Y2,
							 const z3::expr     &dec_var_T2,
							 const z3::expr     &dec_var_t2,
							 const Slic3r::Line &line2);

void introduce_LineNonIntersection_explicit(z3::solver         &Solver,
					    z3::context        &Context,					    
					    const z3::expr     &dec_var_X1,
					    const z3::expr     &dec_var_Y1,
					    const z3::expr     &dec_var_T1,				   
					    const Slic3r::Line &line1,
					    const z3::expr     &dec_var_X2,
					    const z3::expr     &dec_var_Y2,
					    const z3::expr     &dec_var_T2,				   
					    const Slic3r::Line &line2);

void introduce_LineNonIntersectionAgainstFixedLine(z3::solver         &Solver,
						   z3::context        &Context,
						   const z3::expr     &dec_var_X1,
						   const z3::expr     &dec_var_Y1,
						   const z3::expr     &dec_var_T1,
						   const Slic3r::Line &line1,
						   const Rational     &dec_value_X2,
						   const Rational     &dec_value_Y2,
						   const z3::expr     &dec_var_T2,				   
						   const Slic3r::Line &line2);

void introduce_SequentialLineNonIntersectionAgainstFixedLine(z3::solver         &Solver,
							     z3::context        &Context,
							     const z3::expr     &dec_var_X1,
							     const z3::expr     &dec_var_Y1,
							     const z3::expr     &dec_var_T1,
							     const z3::expr     &dec_var_t1,
							     const Slic3r::Line &line1,
							     const Rational     &dec_value_X2,
							     const Rational     &dec_value_Y2,
							     const Rational     &dec_value_T2,
							     const z3::expr     &dec_var_t2,
							     const Slic3r::Line &line2);

void introduce_SequentialFixedLineNonIntersectionAgainstLine(z3::solver         &Solver,
							     z3::context        &Context,
							     const Rational     &dec_value_X1,
							     const Rational     &dec_value_Y1,
							     const Rational     &dec_value_T1,
							     const z3::expr     &dec_var_t1,
							     const Slic3r::Line &line1,
							     const z3::expr     &dec_var_X2,
							     const z3::expr     &dec_var_Y2,
							     const z3::expr     &dec_var_T2,
							     const z3::expr     &dec_var_t2,
							     const Slic3r::Line &line2);

void introduce_ConsequentialLineNonIntersectionAgainstFixedLine(z3::solver         &Solver,
								z3::context        &Context,
								const z3::expr     &dec_var_X1,
								const z3::expr     &dec_var_Y1,
								const z3::expr     &dec_var_T1,
								const z3::expr     &dec_var_t1,
								const Slic3r::Line &line1,
								const Rational     &dec_value_X2,
								const Rational     &dec_value_Y2,
								const Rational     &dec_value_T2,
								const z3::expr     &dec_var_t2,
								const Slic3r::Line &line2);

void introduce_ConsequentialFixedLineNonIntersectionAgainstLine(z3::solver         &Solver,
								z3::context        &Context,
								const Rational     &dec_value_X1,
								const Rational     &dec_value_Y1,
								const Rational     &dec_value_T1,
								const z3::expr     &dec_var_t1,
								const Slic3r::Line &line1,
								const z3::expr     &dec_var_X2,
								const z3::expr     &dec_var_Y2,
								const z3::expr     &dec_var_T2,
								const z3::expr     &dec_var_t2,
							     const Slic3r::Line &line2);

void introduce_LineNonIntersectionAgainstFixedLine_implicit(z3::solver         &Solver,
							    z3::context        &Context,
							    const z3::expr     &dec_var_X1,
							    const z3::expr     &dec_var_Y1,
							    const z3::expr     &dec_var_T1,
							    const Slic3r::Line &line1,
							    const Rational     &dec_value_X2,
							    const Rational     &dec_value_Y2,
							    const z3::expr     &dec_var_T2,
							    const Slic3r::Line &line2);

void introduce_LineNonIntersectionAgainstFixedLine_explicit(z3::solver         &Solver,
							    z3::context        &Context,
							    const z3::expr     &dec_var_X1,
							    const z3::expr     &dec_var_Y1,
							    const z3::expr     &dec_var_T1,
							    const Slic3r::Line &line1,
							    const Rational     &dec_value_X2,
							    const Rational     &dec_value_Y2,
							    const z3::expr     &dec_var_T2,
							    const Slic3r::Line &line2);

void introduce_SequentialLineNonIntersectionAgainstFixedLine_implicit(z3::solver         &Solver,
								      z3::context        &Context,
								      const z3::expr     &dec_var_X1,
								      const z3::expr     &dec_var_Y1,
								      const z3::expr     &dec_var_T1,
								      const z3::expr     &dec_var_t1,
								      const Slic3r::Line &line1,
								      const Rational     &dec_value_X2,
								      const Rational     &dec_value_Y2,
								      const Rational     &dec_value_T2,
								      const z3::expr     &dec_var_t2,
								      const Slic3r::Line &line2);

void introduce_SequentialFixedLineNonIntersectionAgainstLine_implicit(z3::solver         &Solver,
								      z3::context        &Context,
								      const Rational     &dec_value_X1,
								      const Rational     &dec_value_Y1,
								      const Rational     &dec_value_T1,
								      const z3::expr     &dec_var_t1,
								      const Slic3r::Line &line1,
								      const z3::expr     &dec_var_X2,
								      const z3::expr     &dec_var_Y2,
								      const z3::expr     &dec_var_T2,
								      const z3::expr     &dec_var_t2,
								      const Slic3r::Line &line2);

void introduce_ConsequentialLineNonIntersectionAgainstFixedLine_implicit(z3::solver         &Solver,
									 z3::context        &Context,
									 const z3::expr     &dec_var_X1,
									 const z3::expr     &dec_var_Y1,
									 const z3::expr     &dec_var_T1,
									 const z3::expr     &dec_var_t1,
									 const Slic3r::Line &line1,
									 const Rational     &dec_value_X2,
									 const Rational     &dec_value_Y2,
									 const Rational     &dec_value_T2,
									 const z3::expr     &dec_var_t2,
									 const Slic3r::Line &line2);

void introduce_ConsequentialFixedLineNonIntersectionAgainstLine_implicit(z3::solver         &Solver,
									 z3::context        &Context,
									 const Rational     &dec_value_X1,
									 const Rational     &dec_value_Y1,
									 const Rational     &dec_value_T1,
									 const z3::expr     &dec_var_t1,
									 const Slic3r::Line &line1,
									 const z3::expr     &dec_var_X2,
									 const z3::expr     &dec_var_Y2,
									 const z3::expr     &dec_var_T2,
									 const z3::expr     &dec_var_t2,
									 const Slic3r::Line &line2);


/*----------------------------------------------------------------*/

void introduce_PointInsideHalfPlane(z3::solver         &Solver,
				    const z3::expr     &dec_var_X1,
				    const z3::expr     &dec_var_Y1,
				    const z3::expr     &dec_var_X2,
				    const z3::expr     &dec_var_Y2,
				    const Slic3r::Line &halving_line);

void introduce_PointOutsideHalfPlane(z3::solver         &Solver,
				     const z3::expr     &dec_var_X1,
				     const z3::expr     &dec_var_Y1,
				     const z3::expr     &dec_var_X2,
				     const z3::expr     &dec_var_Y2,
				     const Slic3r::Line &halving_line);

void introduce_PointInsidePolygon(z3::solver            &Solver,
				  z3::context           &Context,			      
				  const z3::expr        &dec_var_X1,
				  const z3::expr        &dec_var_Y1,
				  const z3::expr        &dec_var_X2,
				  const z3::expr        &dec_var_Y2,
				  const Slic3r::Polygon &polygon);

void introduce_PointOutsidePolygon(z3::solver            &Solver,
				   z3::context           &Context,			      
				   const z3::expr        &dec_var_X1,
				   const z3::expr        &dec_var_Y1,
				   const z3::expr        &dec_var_X2,
				   const z3::expr        &dec_var_Y2,
				   const Slic3r::Polygon &polygon);

void introduce_SequentialPointOutsidePolygon(z3::solver            &Solver,
					     z3::context           &Context,			      
					     const z3::expr        &dec_var_X1,
					     const z3::expr        &dec_var_Y1,
					     const z3::expr        &dec_var_T1,					     
					     const z3::expr        &dec_var_X2,
					     const z3::expr        &dec_var_Y2,
					     const z3::expr        &dec_var_T2,
					     const Slic3r::Polygon &polygon);

void introduce_ConsequentialPointOutsidePolygon(z3::solver            &Solver,
						z3::context           &Context,			      
						const z3::expr        &dec_var_X1,
						const z3::expr        &dec_var_Y1,
						const z3::expr        &dec_var_T1,					     
						const z3::expr        &dec_var_X2,
						const z3::expr        &dec_var_Y2,
						const z3::expr        &dec_var_T2,
						const Slic3r::Polygon &polygon);

void introduce_FixedPointOutsidePolygon(z3::solver            &Solver,
					z3::context           &Context,
					double                 dec_value_X1,
					double                 dec_value_Y1,					
					const z3::expr        &dec_var_X2,
					const z3::expr        &dec_var_Y2,
					const Slic3r::Polygon &polygon);

void introduce_FixedPointOutsidePolygon(z3::solver            &Solver,
					z3::context           &Context,
					const Rational        &dec_value_X2,
					const Rational        &dec_value_Y2,					
					const z3::expr        &dec_var_X2,
					const z3::expr        &dec_var_Y2,
					const Slic3r::Polygon &polygon);

void introduce_SequentialFixedPointOutsidePolygon(z3::solver            &Solver,
						  z3::context           &Context,
						  const Rational        &dec_value_X1,
						  const Rational        &dec_value_Y1,
						  const Rational        &dec_value_T1,
						  const z3::expr        &dec_var_X2,
						  const z3::expr        &dec_var_Y2,
						  const z3::expr        &dec_var_T2,
						  const Slic3r::Polygon &polygon);

void introduce_SequentialFixedPointOutsidePolygon(z3::solver            &Solver,
						  z3::context           &Context,
						  const Rational        &dec_value_X1,
						  const Rational        &dec_value_Y1,
						  const z3::expr        &dec_var_T1,
						  const z3::expr        &dec_var_X2,
						  const z3::expr        &dec_var_Y2,
						  const Rational        &dec_value_T2,
						  const Slic3r::Polygon &polygon);

void introduce_ConsequentialFixedPointOutsidePolygon(z3::solver            &Solver,
						     z3::context           &Context,
						     const Rational        &dec_value_X1,
						     const Rational        &dec_value_Y1,
						     const Rational        &dec_value_T1,
						     const z3::expr        &dec_var_X2,
						     const z3::expr        &dec_var_Y2,
						     const z3::expr        &dec_var_T2,
						     const Slic3r::Polygon &polygon);

void introduce_ConsequentialFixedPointOutsidePolygon(z3::solver            &Solver,
						     z3::context           &Context,
						     const Rational        &dec_value_X1,
						     const Rational        &dec_value_Y1,
						     const z3::expr        &dec_var_T1,
						     const z3::expr        &dec_var_X2,
						     const z3::expr        &dec_var_Y2,
						     const Rational        &dec_value_T2,
						     const Slic3r::Polygon &polygon);

void introduce_PointOutsideFixedPolygon(z3::solver            &Solver,
					z3::context           &Context,			      
					const z3::expr        &dec_var_X1,
					const z3::expr        &dec_var_Y1,
					double                 dec_value_X2,
					double                 dec_value_Y2,
					const Slic3r::Polygon &polygon);

void introduce_PointOutsideFixedPolygon(z3::solver            &Solver,
					z3::context           &Context,
					const z3::expr        &dec_var_X1,
					const z3::expr        &dec_var_Y1,
					const Rational        &dec_value_X2,
					const Rational        &dec_value_Y2,
					const Slic3r::Polygon &polygon);

void introduce_SequentialPointOutsideFixedPolygon(z3::solver            &Solver,
						  z3::context           &Context,
						  const z3::expr        &dec_var_X1,
						  const z3::expr        &dec_var_Y1,
						  const z3::expr        &dec_var_T1,
						  const Rational        &dec_value_X2,
						  const Rational        &dec_value_Y2,
						  const Rational        &dec_value_T2,
						  const Slic3r::Polygon &polygon);

void introduce_SequentialPointOutsideFixedPolygon(z3::solver            &Solver,
						  z3::context           &Context,
						  const z3::expr        &dec_var_X1,
						  const z3::expr        &dec_var_Y1,
						  const Rational        &dec_value_T1,
						  const Rational        &dec_value_X2,
						  const Rational        &dec_value_Y2,
						  const z3::expr        &dec_var_T2,
						  const Slic3r::Polygon &polygon);

void introduce_ConsequentialPointOutsideFixedPolygon(z3::solver            &Solver,
						     z3::context           &Context,
						     const z3::expr        &dec_var_X1,
						     const z3::expr        &dec_var_Y1,
						     const z3::expr        &dec_var_T1,
						     const Rational        &dec_value_X2,
						     const Rational        &dec_value_Y2,
						     const Rational        &dec_value_T2,
						     const Slic3r::Polygon &polygon);

void introduce_ConsequentialPointOutsideFixedPolygon(z3::solver            &Solver,
						     z3::context           &Context,
						     const z3::expr        &dec_var_X1,
						     const z3::expr        &dec_var_Y1,
						     const Rational        &dec_value_T1,
						     const Rational        &dec_value_X2,
						     const Rational        &dec_value_Y2,
						     const z3::expr        &dec_var_T2,
						     const Slic3r::Polygon &polygon);

void introduce_PolygonOutsidePolygon(z3::solver            &Solver,
				     z3::context           &Context,
				     const z3::expr        &dec_var_X1,
				     const z3::expr        &dec_var_Y1,
				     const Slic3r::Polygon &polygon1,
				     const z3::expr        &dec_var_X2,
				     const z3::expr        &dec_var_Y2,
				     const Slic3r::Polygon &polygon2);

void introduce_PolygonOutsideFixedPolygon(z3::solver            &Solver,
					  z3::context           &Context,
					  const z3::expr        &dec_var_X1,
					  const z3::expr        &dec_var_Y1,
					  const Slic3r::Polygon &polygon1,
					  double                 dec_value_X2,
					  double                 dec_value_Y2,
					  const Slic3r::Polygon &polygon2);

void introduce_PolygonOutsideFixedPolygon(z3::solver            &Solver,
					  z3::context           &Context,
					  const z3::expr        &dec_var_X1,
					  const z3::expr        &dec_var_Y1,
					  const Slic3r::Polygon &polygon1,
					  const Rational        &dec_value_X2,
					  const Rational        &dec_value_Y2,
					  const Slic3r::Polygon &polygon2);

void introduce_SequentialPolygonOutsidePolygon(z3::solver            &Solver,
					       z3::context           &Context,
					       const z3::expr        &dec_var_X1,
					       const z3::expr        &dec_var_Y1,
					       const z3::expr        &dec_var_T1,
					       const Slic3r::Polygon &polygon1,
					       const Slic3r::Polygon &unreachable_polygon1,
					       const z3::expr        &dec_var_X2,
					       const z3::expr        &dec_var_Y2,
					       const z3::expr        &dec_var_T2,
					       const Slic3r::Polygon &polygon2,
					       const Slic3r::Polygon &unreachable_polygon2);

void introduce_SequentialPolygonOutsidePolygon(z3::solver                         &Solver,
					       z3::context                        &Context,
					       const z3::expr                     &dec_var_X1,
					       const z3::expr                     &dec_var_Y1,
					       const z3::expr                     &dec_var_T1,
					       const Slic3r::Polygon              &polygon1,
					       const std::vector<Slic3r::Polygon> &unreachable_polygons1,
					       const z3::expr                     &dec_var_X2,
					       const z3::expr                     &dec_var_Y2,
					       const z3::expr                     &dec_var_T2,
					       const Slic3r::Polygon              &polygon2,
					       const std::vector<Slic3r::Polygon> &unreachable_polygons2);

void introduce_SequentialPolygonOutsideFixedPolygon(z3::solver            &Solver,
						    z3::context           &Context,
						    const z3::expr        &dec_var_X1,
						    const z3::expr        &dec_var_Y1,
						    const z3::expr        &dec_var_T1,
						    const Slic3r::Polygon &polygon1,
						    const Slic3r::Polygon &unreachable_polygon1,
						    const Rational        &dec_value_X2,
						    const Rational        &dec_value_Y2,
						    const Rational        &dec_value_T2,
						    const Slic3r::Polygon &polygon2,
						    const Slic3r::Polygon &unreachable_polygon2);

void introduce_SequentialPolygonOutsideFixedPolygon(z3::solver                         &Solver,
						    z3::context                        &Context,
						    const z3::expr                     &dec_var_X1,
						    const z3::expr                     &dec_var_Y1,
						    const z3::expr                     &dec_var_T1,
						    const Slic3r::Polygon              &polygon1,
						    const std::vector<Slic3r::Polygon> &unreachable_polygons1,
						    const Rational                     &dec_value_X2,
						    const Rational                     &dec_value_Y2,
						    const Rational                     &dec_value_T2,
						    const Slic3r::Polygon              &polygon2,
						    const std::vector<Slic3r::Polygon> &unreachable_polygons2);

void introduce_ConsequentialPolygonOutsidePolygon(z3::solver            &Solver,
						  z3::context           &Context,
						  const z3::expr        &dec_var_X1,
						  const z3::expr        &dec_var_Y1,
						  const z3::expr        &dec_var_T1,
						  const Slic3r::Polygon &polygon1,
						  const Slic3r::Polygon &unreachable_polygon1,
						  const z3::expr        &dec_var_X2,
						  const z3::expr        &dec_var_Y2,
						  const z3::expr        &dec_var_T2,
						  const Slic3r::Polygon &polygon2,
						  const Slic3r::Polygon &unreachable_polygon2);

void introduce_ConsequentialPolygonOutsidePolygon(z3::solver                         &Solver,
						  z3::context                        &Context,
						  const z3::expr                     &dec_var_X1,
						  const z3::expr                     &dec_var_Y1,
						  const z3::expr                     &dec_var_T1,
						  const Slic3r::Polygon              &polygon1,
						  const std::vector<Slic3r::Polygon> &unreachable_polygons1,
						  const z3::expr                     &dec_var_X2,
						  const z3::expr                     &dec_var_Y2,
						  const z3::expr                     &dec_var_T2,
						  const Slic3r::Polygon              &polygon2,
						  const std::vector<Slic3r::Polygon> &unreachable_polygons2);

void introduce_ConsequentialPolygonExternalPolygon(z3::solver            &Solver,
						   z3::context           &Context,
						   const z3::expr        &dec_var_X1,
						   const z3::expr        &dec_var_Y1,
						   const z3::expr        &dec_var_T1,
						   const Slic3r::Polygon &polygon1,
						   const Slic3r::Polygon &unreachable_polygon1,
						   const z3::expr        &dec_var_X2,
						   const z3::expr        &dec_var_Y2,
						   const z3::expr        &dec_var_T2,
						   const Slic3r::Polygon &polygon2,
						   const Slic3r::Polygon &unreachable_polygon2);

void introduce_ConsequentialPolygonExternalPolygon(z3::solver                         &Solver,
						   z3::context                        &Context,
						   const z3::expr                     &dec_var_X1,
						   const z3::expr                     &dec_var_Y1,
						   const z3::expr                     &dec_var_T1,
						   const Slic3r::Polygon              &polygon1,
						   const std::vector<Slic3r::Polygon> &unreachable_polygons1,
						   const z3::expr                     &dec_var_X2,
						   const z3::expr                     &dec_var_Y2,
						   const z3::expr                     &dec_var_T2,
						   const Slic3r::Polygon              &polygon2,
						   const std::vector<Slic3r::Polygon> &unreachable_polygons2);

void introduce_ConsequentialPolygonOutsideFixedPolygon(z3::solver            &Solver,
						       z3::context           &Context,
						       const z3::expr        &dec_var_X1,
						       const z3::expr        &dec_var_Y1,
						       const z3::expr        &dec_var_T1,
						       const Slic3r::Polygon &polygon1,
						       const Slic3r::Polygon &unreachable_polygon1,
						       const Rational        &dec_value_X2,
						       const Rational        &dec_value_Y2,
						       const Rational        &dec_value_T2,
						       const Slic3r::Polygon &polygon2,
						       const Slic3r::Polygon &unreachable_polygon2);

void introduce_ConsequentialPolygonOutsideFixedPolygon(z3::solver                         &Solver,
						       z3::context                        &Context,
						       const z3::expr                     &dec_var_X1,
						       const z3::expr                     &dec_var_Y1,
						       const z3::expr                     &dec_var_T1,
						       const Slic3r::Polygon              &polygon1,
						       const std::vector<Slic3r::Polygon> &unreachable_polygons1,
						       const Rational                     &dec_value_X2,
						       const Rational                     &dec_value_Y2,
						       const Rational                     &dec_value_T2,
						       const Slic3r::Polygon              &polygon2,
						       const std::vector<Slic3r::Polygon> &unreachable_polygons2);

void introduce_ConsequentialPolygonExternalFixedPolygon(z3::solver            &Solver,
							z3::context           &Context,
							const z3::expr        &dec_var_X1,
							const z3::expr        &dec_var_Y1,
							const z3::expr        &dec_var_T1,
							const Slic3r::Polygon &polygon1,
							const Slic3r::Polygon &unreachable_polygon1,
							const Rational        &dec_value_X2,
							const Rational        &dec_value_Y2,
							const Rational        &dec_value_T2,
							const Slic3r::Polygon &polygon2,
							const Slic3r::Polygon &unreachable_polygon2);

void introduce_ConsequentialPolygonExternalFixedPolygon(z3::solver                         &Solver,
							z3::context                        &Context,
							const z3::expr                     &dec_var_X1,
							const z3::expr                     &dec_var_Y1,
							const z3::expr                     &dec_var_T1,
							const Slic3r::Polygon              &polygon1,
							const std::vector<Slic3r::Polygon> &unreachable_polygons1,
							const Rational                     &dec_value_X2,
							const Rational                     &dec_value_Y2,
							const Rational                     &dec_value_T2,
							const Slic3r::Polygon              &polygon2,
							const std::vector<Slic3r::Polygon> &unreachable_polygons2);

void introduce_PolygonLineNonIntersection(z3::solver            &Solver,
					  z3::context           &Context,
					  const z3::expr        &dec_var_X1,
					  const z3::expr        &dec_var_Y1,
					  const Slic3r::Polygon &polygon1,
					  const z3::expr        &dec_var_X2,
					  const z3::expr        &dec_var_Y2,
					  const Slic3r::Polygon &polygon2);


/*----------------------------------------------------------------*/

void introduce_PolygonWeakNonoverlapping(z3::solver                         &Solver,
					 z3::context                        &Context,
					 const z3::expr_vector              &dec_vars_X,
					 const z3::expr_vector              &dec_vars_Y,
					 const std::vector<Slic3r::Polygon> &polygons);

void introduce_SequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						   z3::context                        &Context,
						   const z3::expr_vector              &dec_vars_X,
						   const z3::expr_vector              &dec_vars_Y,
						   const z3::expr_vector              &dec_vars_T,
						   const std::vector<Slic3r::Polygon> &polygons,
						   const std::vector<Slic3r::Polygon> &unreachable_polygons);

void introduce_SequentialPolygonWeakNonoverlapping(z3::solver                                       &Solver,
						   z3::context                                      &Context,
						   const z3::expr_vector                            &dec_vars_X,
						   const z3::expr_vector                            &dec_vars_Y,
						   const z3::expr_vector                            &dec_vars_T,
						   const std::vector<Slic3r::Polygon>               &polygons,
						   const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);

void introduce_ConsequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						      z3::context                        &Context,
						      const z3::expr_vector              &dec_vars_X,
						      const z3::expr_vector              &dec_vars_Y,
						      const z3::expr_vector              &dec_vars_T,
						      const std::vector<Slic3r::Polygon> &polygons,
						      const std::vector<Slic3r::Polygon> &unreachable_polygons);

void introduce_ConsequentialPolygonWeakNonoverlapping(z3::solver                                       &Solver,
						      z3::context                                      &Context,
						      const z3::expr_vector                            &dec_vars_X,
						      const z3::expr_vector                            &dec_vars_Y,
						      const z3::expr_vector                            &dec_vars_T,
						      const std::vector<Slic3r::Polygon>               &polygons,
						      const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);

void introduce_PolygonWeakNonoverlapping(z3::solver                         &Solver,
					 z3::context                        &Context,
					 const z3::expr_vector              &dec_vars_X,
					 const z3::expr_vector              &dec_vars_Y,
					 std::vector<Rational>              &dec_values_X,
					 std::vector<Rational>              &dec_values_Y,
					 const std::vector<int>             &fixed,
					 const std::vector<int>             &undecided,
					 const std::vector<Slic3r::Polygon> &polygons);

void introduce_SequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						   z3::context                        &Context,
						   const z3::expr_vector              &dec_vars_X,
						   const z3::expr_vector              &dec_vars_Y,
						   const z3::expr_vector              &dec_vars_T,
						   std::vector<Rational>              &dec_values_X,
						   std::vector<Rational>              &dec_values_Y,
						   std::vector<Rational>              &dec_values_T,
						   const std::vector<int>             &fixed,
						   const std::vector<int>             &undecided,
						   const std::vector<Slic3r::Polygon> &polygons,
						   const std::vector<Slic3r::Polygon> &unreachable_polygons);

void introduce_SequentialPolygonWeakNonoverlapping(z3::solver                                       &Solver,
						   z3::context                                      &Context,
						   const z3::expr_vector                            &dec_vars_X,
						   const z3::expr_vector                            &dec_vars_Y,
						   const z3::expr_vector                            &dec_vars_T,
						   std::vector<Rational>                            &dec_values_X,
						   std::vector<Rational>                            &dec_values_Y,
						   std::vector<Rational>                            &dec_values_T,
						   const std::vector<int>                           &fixed,
						   const std::vector<int>                           &undecided,
						   const std::vector<Slic3r::Polygon>               &polygons,
						   const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);

void introduce_ConsequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						      z3::context                        &Context,
						      const z3::expr_vector              &dec_vars_X,
						      const z3::expr_vector              &dec_vars_Y,
						      const z3::expr_vector              &dec_vars_T,
						      std::vector<Rational>              &dec_values_X,
						      std::vector<Rational>              &dec_values_Y,
						      std::vector<Rational>              &dec_values_T,
						      const std::vector<int>             &fixed,
						      const std::vector<int>             &undecided,
						      const std::vector<Slic3r::Polygon> &polygons,
						      const std::vector<Slic3r::Polygon> &unreachable_polygons);

void introduce_ConsequentialPolygonWeakNonoverlapping(z3::solver                                       &Solver,
						      z3::context                                      &Context,
						      const z3::expr_vector                            &dec_vars_X,
						      const z3::expr_vector                            &dec_vars_Y,
						      const z3::expr_vector                            &dec_vars_T,
						      std::vector<Rational>                            &dec_values_X,
						      std::vector<Rational>                            &dec_values_Y,
						      std::vector<Rational>                            &dec_values_T,
						      const std::vector<int>                           &fixed,
						      const std::vector<int>                           &undecided,
						      const std::vector<Slic3r::Polygon>               &polygons,
						      const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);

void introduce_PolygonStrongNonoverlapping(z3::solver                         &Solver,
					   z3::context                        &Context,
					   const z3::expr_vector              &dec_vars_X,
					   const z3::expr_vector              &dec_vars_Y,
					   const std::vector<Slic3r::Polygon> &polygons);

bool refine_PolygonWeakNonoverlapping(z3::solver                         &Solver,
				      z3::context                        &Context,
				      const z3::expr_vector              &dec_vars_X,
				      const z3::expr_vector              &dec_vars_Y,
				      const std::vector<double>          &dec_values_X,
				      const std::vector<double>          &dec_values_Y,
				      const std::vector<Slic3r::Polygon> &polygons);

bool refine_PolygonWeakNonoverlapping(z3::solver                         &Solver,
				      z3::context                        &Context,
				      const z3::expr_vector              &dec_vars_X,
				      const z3::expr_vector              &dec_vars_Y,
				      const z3::expr_vector              &dec_values_X,
				      const z3::expr_vector              &dec_values_Y,
				      const std::vector<Slic3r::Polygon> &polygons);

bool refine_PolygonWeakNonoverlapping(z3::solver                         &Solver,
				      z3::context                        &Context,
				      const z3::expr_vector              &dec_vars_X,
				      const z3::expr_vector              &dec_vars_Y,
				      const std::vector<Rational>        &dec_values_X,
				      const std::vector<Rational>        &dec_values_Y,
				      const std::vector<Slic3r::Polygon> &polygons);

bool refine_SequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						z3::context                        &Context,
						const z3::expr_vector              &dec_vars_X,
						const z3::expr_vector              &dec_vars_Y,
						const z3::expr_vector              &dec_vars_T,
						const std::vector<double>          &dec_values_X,
						const std::vector<double>          &dec_values_Y,
						const std::vector<double>          &dec_values_T,
						const std::vector<Slic3r::Polygon> &polygons,
						const std::vector<Slic3r::Polygon> &unreachable_polygons);

bool refine_SequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						z3::context                        &Context,
						const z3::expr_vector              &dec_vars_X,
						const z3::expr_vector              &dec_vars_Y,
						const z3::expr_vector              &dec_vars_T,
						const std::vector<Rational>        &dec_values_X,
						const std::vector<Rational>        &dec_values_Y,
						const std::vector<Rational>        &dec_values_T,
						const std::vector<Slic3r::Polygon> &polygons,
						const std::vector<Slic3r::Polygon> &unreachable_polygons);

bool refine_SequentialPolygonWeakNonoverlapping(z3::solver                                       &Solver,
						z3::context                                      &Context,
						const z3::expr_vector                            &dec_vars_X,
						const z3::expr_vector                            &dec_vars_Y,
						const z3::expr_vector                            &dec_vars_T,
						const std::vector<Rational>                      &dec_values_X,
						const std::vector<Rational>                      &dec_values_Y,
						const std::vector<Rational>                      &dec_values_T,
						const std::vector<Slic3r::Polygon>               &polygons,
						const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);

bool refine_ConsequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						   z3::context                        &Context,
						   const z3::expr_vector              &dec_vars_X,
						   const z3::expr_vector              &dec_vars_Y,
						   const z3::expr_vector              &dec_vars_T,
						   const std::vector<double>          &dec_values_X,
						   const std::vector<double>          &dec_values_Y,
						   const std::vector<double>          &dec_values_T,
						   const std::vector<Slic3r::Polygon> &polygons,
						   const std::vector<Slic3r::Polygon> &unreachable_polygons);

bool refine_ConsequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						   z3::context                        &Context,
						   const z3::expr_vector              &dec_vars_X,
						   const z3::expr_vector              &dec_vars_Y,
						   const z3::expr_vector              &dec_vars_T,
						   const std::vector<Rational>        &dec_values_X,
						   const std::vector<Rational>        &dec_values_Y,
						   const std::vector<Rational>        &dec_values_T,
						   const std::vector<Slic3r::Polygon> &polygons,
						   const std::vector<Slic3r::Polygon> &unreachable_polygons);

bool refine_ConsequentialPolygonWeakNonoverlapping(z3::solver                                       &Solver,
						   z3::context                                      &Context,
						   const z3::expr_vector                            &dec_vars_X,
						   const z3::expr_vector                            &dec_vars_Y,
						   const z3::expr_vector                            &dec_vars_T,
						   const std::vector<Rational>                      &dec_values_X,
						   const std::vector<Rational>                      &dec_values_Y,
						   const std::vector<Rational>                      &dec_values_T,
						   const std::vector<Slic3r::Polygon>               &polygons,
						   const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);


/*----------------------------------------------------------------*/

void introduce_PolygonWeakNonoverlappingAgainstFixed(z3::solver                         &Solver,
						     z3::context                        &Context,
						     const z3::expr_vector              &dec_vars_X,
						     const z3::expr_vector              &dec_vars_Y,
						     const z3::expr_vector              &dec_values_X,
						     const z3::expr_vector              &dec_values_Y,
						     const std::vector<int>             &fixed,
						     const std::vector<int>             &undecided,
						     const std::vector<Slic3r::Polygon> &polygons);

bool refine_PolygonWeakNonoverlapping(z3::solver                         &Solver,
				      z3::context                        &Context,
				      const z3::expr_vector              &dec_vars_X,
				      const z3::expr_vector              &dec_vars_Y,
				      const z3::expr_vector              &dec_values_X,
				      const z3::expr_vector              &dec_values_Y,
				      const std::vector<int>             &fixed,
				      const std::vector<int>             &undecided,
				      const std::vector<Slic3r::Polygon> &polygons);

bool refine_PolygonWeakNonoverlapping(z3::solver                         &Solver,
				      z3::context                        &Context,
				      const z3::expr_vector              &dec_vars_X,
				      const z3::expr_vector              &dec_vars_Y,
				      const std::vector<Rational>        &dec_values_X,
				      const std::vector<Rational>        &dec_values_Y,
				      const std::vector<int>             &fixed,
				      const std::vector<int>             &undecided,
				      const std::vector<Slic3r::Polygon> &polygons);

bool refine_SequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						z3::context                        &Context,
						const z3::expr_vector              &dec_vars_X,
						const z3::expr_vector              &dec_vars_Y,
						const z3::expr_vector              &dec_vars_T,
						const std::vector<Rational>        &dec_values_X,
						const std::vector<Rational>        &dec_values_Y,
						const std::vector<Rational>        &dec_values_T,
						const std::vector<int>             &fixed,
						const std::vector<int>             &undecided,
						const std::vector<Slic3r::Polygon> &polygons,
						const std::vector<Slic3r::Polygon> &unreachable_polygons);

bool refine_SequentialPolygonWeakNonoverlapping(z3::solver                                       &Solver,
						z3::context                                      &Context,
						const z3::expr_vector                            &dec_vars_X,
						const z3::expr_vector                            &dec_vars_Y,
						const z3::expr_vector                            &dec_vars_T,
						const std::vector<Rational>                      &dec_values_X,
						const std::vector<Rational>                      &dec_values_Y,
						const std::vector<Rational>                      &dec_values_T,
						const std::vector<int>                           &fixed,
						const std::vector<int>                           &undecided,
						const std::vector<Slic3r::Polygon>               &polygons,
						const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);

bool refine_ConsequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						   z3::context                        &Context,
						   const z3::expr_vector              &dec_vars_X,
						   const z3::expr_vector              &dec_vars_Y,
						   const z3::expr_vector              &dec_vars_T,
						   const std::vector<Rational>        &dec_values_X,
						   const std::vector<Rational>        &dec_values_Y,
						   const std::vector<Rational>        &dec_values_T,
						   const std::vector<int>             &fixed,
						   const std::vector<int>             &undecided,
						   const std::vector<Slic3r::Polygon> &polygons,
						   const std::vector<Slic3r::Polygon> &unreachable_polygons);

bool refine_ConsequentialPolygonWeakNonoverlapping(z3::solver                                       &Solver,
						   z3::context                                      &Context,
						   const z3::expr_vector                            &dec_vars_X,
						   const z3::expr_vector                            &dec_vars_Y,
						   const z3::expr_vector                            &dec_vars_T,
						   const std::vector<Rational>                      &dec_values_X,
						   const std::vector<Rational>                      &dec_values_Y,
						   const std::vector<Rational>                      &dec_values_T,
						   const std::vector<int>                           &fixed,
						   const std::vector<int>                           &undecided,
						   const std::vector<Slic3r::Polygon>               &polygons,
						   const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);


/*----------------------------------------------------------------*/

bool check_PointsOutsidePolygons(const std::vector<Rational>                      &dec_values_X,
				 const std::vector<Rational>                      &dec_values_Y,	
				 const std::vector<Rational>                      &dec_values_T,
				 const std::vector<Slic3r::Polygon>               &polygons,
				 const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);

bool check_PolygonLineIntersections(const std::vector<Rational>                      &dec_values_X,
				    const std::vector<Rational>                      &dec_values_Y,	
				    const std::vector<Rational>                      &dec_values_T,
				    const std::vector<Slic3r::Polygon>               &polygons,
				    const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);    


/*----------------------------------------------------------------*/

void extract_DecisionValuesFromModel(const z3::model     &Model,
				     const string_map    &dec_var_names_map,
				     std::vector<double> &dec_values_X,
				     std::vector<double> &dec_values_Y);

void extract_DecisionValuesFromModel(const z3::model     &Model,
				     z3::context         &Context,				     
				     const string_map    &dec_var_names_map,
				     z3::expr_vector     &dec_values_X,
				     z3::expr_vector     &dec_values_Y);

void extract_DecisionValuesFromModel(const z3::model       &Model,
				     const string_map      &dec_var_names_map,
				     std::vector<Rational> &dec_values_X,
				     std::vector<Rational> &dec_values_Y);

void extract_DecisionValuesFromModel(const z3::model       &Model,
				     const string_map      &dec_var_names_map,
				     std::vector<Rational> &dec_values_X,
				     std::vector<Rational> &dec_values_Y,
				     std::vector<Rational> &dec_values_T);

void build_WeakPolygonNonoverlapping(z3::solver                         &Solver,
				     z3::context                        &Context,
				     const std::vector<Slic3r::Polygon> &polygons,
				     z3::expr_vector                    &dec_vars_X,
				     z3::expr_vector                    &dec_vars_Y,
				     std::vector<double>                &dec_values_X,
				     std::vector<double>                &dec_values_Y,
				     string_map                         &dec_var_names_map);

void build_WeakPolygonNonoverlapping(z3::solver                         &Solver,
				     z3::context                        &Context,
				     const std::vector<Slic3r::Polygon> &polygons,
				     z3::expr_vector                    &dec_vars_X,
				     z3::expr_vector                    &dec_vars_Y,
				     z3::expr_vector                    &dec_values_X,
				     z3::expr_vector                    &dec_values_Y,
				     string_map                         &dec_var_names_map);

void build_WeakPolygonNonoverlapping(z3::solver                         &Solver,
				     z3::context                        &Context,
				     const std::vector<Slic3r::Polygon> &polygons,
				     z3::expr_vector                    &dec_vars_X,
				     z3::expr_vector                    &dec_vars_Y,
				     std::vector<Rational>              &dec_values_X,
				     std::vector<Rational>              &dec_values_Y,
				     string_map                         &dec_var_names_map);

bool optimize_WeakPolygonNonoverlapping(z3::solver                         &Solver,
					z3::context                        &Context,
					const SolverConfiguration          &solver_configuration,
					const z3::expr_vector              &dec_vars_X,
					const z3::expr_vector              &dec_vars_Y,
					std::vector<double>                &dec_values_X,
					std::vector<double>                &dec_values_Y,
					const string_map                   &dec_var_names_map,
					const std::vector<Slic3r::Polygon> &polygons);

bool optimize_WeakPolygonNonoverlapping(z3::solver                         &Solver,
					z3::context                        &Context,
					const SolverConfiguration          &solver_configuration,
					const z3::expr_vector              &dec_vars_X,
					const z3::expr_vector              &dec_vars_Y,
					z3::expr_vector                    &dec_values_X,
					z3::expr_vector                    &dec_values_Y,
					const string_map                   &dec_var_names_map,
					const std::vector<Slic3r::Polygon> &polygons);

bool optimize_WeakPolygonNonoverlapping(z3::solver                         &Solver,
					z3::context                        &Context,
					const SolverConfiguration          &solver_configuration,
					const z3::expr_vector              &dec_vars_X,
					const z3::expr_vector              &dec_vars_Y,
					std::vector<Rational>              &dec_values_X,
					std::vector<Rational>              &dec_values_Y,
					const string_map                   &dec_var_names_map,
					const std::vector<Slic3r::Polygon> &polygons);

/*----------------------------------------------------------------*/

void build_WeakPolygonNonoverlapping(z3::solver                         &Solver,
				     z3::context                        &Context,
				     const std::vector<Slic3r::Polygon> &polygons,
				     z3::expr_vector                    &dec_vars_X,
				     z3::expr_vector                    &dec_vars_Y,
				     std::vector<Rational>              &dec_values_X,
				     std::vector<Rational>              &dec_values_Y,
				     const std::vector<int>             &fixed,
				     const std::vector<int>             &undecided,
				     string_map                         &dec_var_names_map);

void build_SequentialWeakPolygonNonoverlapping(z3::solver                         &Solver,
					       z3::context                        &Context,
					       const std::vector<Slic3r::Polygon> &polygons,
					       const std::vector<Slic3r::Polygon> &unreachable_polygons,
					       z3::expr_vector                    &dec_vars_X,
					       z3::expr_vector                    &dec_vars_Y,
					       z3::expr_vector                    &dec_vars_T,
					       std::vector<Rational>              &dec_values_X,
					       std::vector<Rational>              &dec_values_Y,
					       std::vector<Rational>              &dec_values_T,
					       const std::vector<int>             &fixed,
					       const std::vector<int>             &undecided,
					       string_map                         &dec_var_names_map);

void build_SequentialWeakPolygonNonoverlapping(z3::solver                                       &Solver,
					       z3::context                                      &Context,
					       const std::vector<Slic3r::Polygon>               &polygons,
					       const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons,
					       z3::expr_vector                                  &dec_vars_X,
					       z3::expr_vector                                  &dec_vars_Y,
					       z3::expr_vector                                  &dec_vars_T,
					       std::vector<Rational>                            &dec_values_X,
					       std::vector<Rational>                            &dec_values_Y,
					       std::vector<Rational>                            &dec_values_T,
					       const std::vector<int>                           &fixed,
					       const std::vector<int>                           &undecided,
					       string_map                                       &dec_var_names_map);

void build_ConsequentialWeakPolygonNonoverlapping(z3::solver                         &Solver,
						  z3::context                        &Context,
						  const std::vector<Slic3r::Polygon> &polygons,
						  const std::vector<Slic3r::Polygon> &unreachable_polygons,
						  z3::expr_vector                    &dec_vars_X,
						  z3::expr_vector                    &dec_vars_Y,
						  z3::expr_vector                    &dec_vars_T,
						  std::vector<Rational>              &dec_values_X,
						  std::vector<Rational>              &dec_values_Y,
						  std::vector<Rational>              &dec_values_T,
						  const std::vector<int>             &fixed,
						  const std::vector<int>             &undecided,
						  string_map                         &dec_var_names_map);

void build_ConsequentialWeakPolygonNonoverlapping(z3::solver                                       &Solver,
						  z3::context                                      &Context,
						  const std::vector<Slic3r::Polygon>               &polygons,
						  const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons,
						  z3::expr_vector                                  &dec_vars_X,
						  z3::expr_vector                                  &dec_vars_Y,
						  z3::expr_vector                                  &dec_vars_T,
						  std::vector<Rational>                            &dec_values_X,
						  std::vector<Rational>                            &dec_values_Y,
						  std::vector<Rational>                            &dec_values_T,
						  const std::vector<int>                           &fixed,
						  const std::vector<int>                           &undecided,
						  string_map                                       &dec_var_names_map);

bool optimize_WeakPolygonNonoverlapping(z3::solver                         &Solver,
					z3::context                        &Context,
					const SolverConfiguration          &solver_configuration,
					const z3::expr_vector              &dec_vars_X,
					const z3::expr_vector              &dec_vars_Y,
					z3::expr_vector                    &dec_values_X,
					z3::expr_vector                    &dec_values_Y,
					const std::vector<int>             &fixed,
					const std::vector<int>             &undecided,
					const string_map                   &dec_var_names_map,
					const std::vector<Slic3r::Polygon> &polygons);

bool optimize_WeakPolygonNonoverlapping(z3::solver                         &Solver,
					z3::context                        &Context,
					const SolverConfiguration          &solver_configuration,
					const z3::expr_vector              &dec_vars_X,
					const z3::expr_vector              &dec_vars_Y,
					std::vector<Rational>              &dec_values_X,
					std::vector<Rational>              &dec_values_Y,
					const std::vector<int>             &fixed,
					const std::vector<int>             &undecided,
					const string_map                   &dec_var_names_map,
					const std::vector<Slic3r::Polygon> &polygons);

bool optimize_SequentialWeakPolygonNonoverlapping(z3::solver                         &Solver,
						  z3::context                        &Context,
						  const SolverConfiguration          &solver_configuration,
						  const z3::expr_vector              &dec_vars_X,
						  const z3::expr_vector              &dec_vars_Y,
						  const z3::expr_vector              &dec_vars_T,
						  std::vector<Rational>              &dec_values_X,
						  std::vector<Rational>              &dec_values_Y,
						  std::vector<Rational>              &dec_values_T,
						  const std::vector<int>             &fixed,
						  const std::vector<int>             &undecided,
						  const string_map                   &dec_var_names_map,
						  const std::vector<Slic3r::Polygon> &polygons,
						  const std::vector<Slic3r::Polygon> &unreachable_polygons);

bool optimize_SequentialWeakPolygonNonoverlapping(z3::solver                                       &Solver,
						  z3::context                                      &Context,
						  const SolverConfiguration                        &solver_configuration,
						  const z3::expr_vector                            &dec_vars_X,
						  const z3::expr_vector                            &dec_vars_Y,
						  const z3::expr_vector                            &dec_vars_T,
						  std::vector<Rational>                            &dec_values_X,
						  std::vector<Rational>                            &dec_values_Y,
						  std::vector<Rational>                            &dec_values_T,
						  const std::vector<int>                           &fixed,
						  const std::vector<int>                           &undecided,
						  const string_map                                 &dec_var_names_map,
						  const std::vector<Slic3r::Polygon>               &polygons,
						  const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);

bool optimize_SequentialWeakPolygonNonoverlappingCentered(z3::solver                                       &Solver,
							  z3::context                                      &Context,
							  const SolverConfiguration                        &solver_configuration,
							  const z3::expr_vector                            &dec_vars_X,
							  const z3::expr_vector                            &dec_vars_Y,
							  const z3::expr_vector                            &dec_vars_T,
							  std::vector<Rational>                            &dec_values_X,
							  std::vector<Rational>                            &dec_values_Y,
							  std::vector<Rational>                            &dec_values_T,
							  const std::vector<int>                           &fixed,
							  const std::vector<int>                           &undecided,
							  const string_map                                 &dec_var_names_map,
							  const std::vector<Slic3r::Polygon>               &polygons,
							  const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);

bool checkArea_SequentialWeakPolygonNonoverlapping(coord_t                                           box_min_x,
						   coord_t                                           box_min_y,
						   coord_t                                           box_max_x,
						   coord_t                                           box_max_y,
						   const std::vector<int>                           &fixed,
						   const std::vector<int>                           &undecided,
						   const std::vector<Slic3r::Polygon>               &polygons,
						   const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);

bool checkExtens_SequentialWeakPolygonNonoverlapping(coord_t                                           box_min_x,
						     coord_t                                           box_min_y,
						     coord_t                                           box_max_x,
						     coord_t                                           box_max_y,
						     std::vector<Rational>                            &dec_values_X,
						     std::vector<Rational>                            &dec_values_Y,
						     const std::vector<int>                           &fixed,
						     const std::vector<int>                           &undecided,
						     const std::vector<Slic3r::Polygon>               &polygons,
						     const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);

bool optimize_SequentialWeakPolygonNonoverlappingBinaryCentered(z3::solver                                       &Solver,
								z3::context                                      &Context,
								const SolverConfiguration                        &solver_configuration,
								int                                              &box_half_x_max,
								int                                              &box_half_y_max,
								const z3::expr_vector                            &dec_vars_X,
								const z3::expr_vector                            &dec_vars_Y,
								const z3::expr_vector                            &dec_vars_T,
								std::vector<Rational>                            &dec_values_X,
								std::vector<Rational>                            &dec_values_Y,
								std::vector<Rational>                            &dec_values_T,
								const std::vector<int>                           &fixed,
								const std::vector<int>                           &undecided,
								const string_map                                 &dec_var_names_map,
								const std::vector<Slic3r::Polygon>               &polygons,
								const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);

bool optimize_ConsequentialWeakPolygonNonoverlappingBinaryCentered(z3::solver                                       &Solver,
								   z3::context                                      &Context,
								   const SolverConfiguration                        &solver_configuration,
								   int                                              &box_half_x_max,
								   int                                              &box_half_y_max,
								   const z3::expr_vector                            &dec_vars_X,
								   const z3::expr_vector                            &dec_vars_Y,
								   const z3::expr_vector                            &dec_vars_T,
								   std::vector<Rational>                            &dec_values_X,
								   std::vector<Rational>                            &dec_values_Y,
								   std::vector<Rational>                            &dec_values_T,
								   const std::vector<int>                           &fixed,
								   const std::vector<int>                           &undecided,
								   const string_map                                 &dec_var_names_map,
								   const std::vector<Slic3r::Polygon>               &polygons,
								   const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons);


/*----------------------------------------------------------------*/

void augment_TemporalSpread(const SolverConfiguration &solver_configuration,
			    std::vector<Rational>     &dec_values_T,
			    const std::vector<int>    &decided_polygons);


bool optimize_SubglobalPolygonNonoverlapping(const SolverConfiguration          &solver_configuration,
					     std::vector<Rational>              &dec_values_X,
					     std::vector<Rational>              &dec_values_Y,
					     const std::vector<Slic3r::Polygon> &polygons,
					     const std::vector<int>             &undecided_polygons,
					     std::vector<int>                   &decided_polygons,
					     std::vector<int>                   &remaining_polygons);

bool optimize_SubglobalSequentialPolygonNonoverlapping(const SolverConfiguration          &solver_configuration,
						       std::vector<Rational>              &dec_values_X,
						       std::vector<Rational>              &dec_values_Y,
						       std::vector<Rational>              &dec_values_T,
						       const std::vector<Slic3r::Polygon> &polygons,
						       const std::vector<Slic3r::Polygon> &unreachable_polygons,
						       const std::vector<int>             &undecided_polygons,
						       std::vector<int>                   &decided_polygons,
						       std::vector<int>                   &remaining_polygons);

bool optimize_SubglobalSequentialPolygonNonoverlapping(const SolverConfiguration                        &solver_configuration,
						       std::vector<Rational>                            &dec_values_X,
						       std::vector<Rational>                            &dec_values_Y,
						       std::vector<Rational>                            &dec_values_T,
						       const std::vector<Slic3r::Polygon>               &polygons,
						       const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons,
						       const std::vector<int>                           &undecided_polygons,
						       std::vector<int>                                 &decided_polygons,
						       std::vector<int>                                 &remaining_polygons);

bool optimize_SubglobalSequentialPolygonNonoverlappingCentered(const SolverConfiguration          &solver_configuration,
							       std::vector<Rational>              &dec_values_X,
							       std::vector<Rational>              &dec_values_Y,
							       std::vector<Rational>              &dec_values_T,
							       const std::vector<Slic3r::Polygon> &polygons,
							       const std::vector<Slic3r::Polygon> &unreachable_polygons,
							       const std::vector<int>             &undecided_polygons,
							       std::vector<int>                   &decided_polygons,
							       std::vector<int>                   &remaining_polygons);

bool optimize_SubglobalSequentialPolygonNonoverlappingCentered(const SolverConfiguration                        &solver_configuration,
							       std::vector<Rational>                            &dec_values_X,
							       std::vector<Rational>                            &dec_values_Y,
							       std::vector<Rational>                            &dec_values_T,
							       const std::vector<Slic3r::Polygon>               &polygons,
							       const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons,
							       const std::vector<int>                           &undecided_polygons,
							       std::vector<int>                                 &decided_polygons,
							       std::vector<int>                                 &remaining_polygons);

bool optimize_SubglobalSequentialPolygonNonoverlappingBinaryCentered(const SolverConfiguration          &solver_configuration,
								     std::vector<Rational>              &dec_values_X,
								     std::vector<Rational>              &dec_values_Y,
								     std::vector<Rational>              &dec_values_T,
								     const std::vector<Slic3r::Polygon> &polygons,
								     const std::vector<Slic3r::Polygon> &unreachable_polygons,
								     const std::vector<int>             &undecided_polygons,
								     std::vector<int>                   &decided_polygons,
								     std::vector<int>                   &remaining_polygons);

bool optimize_SubglobalSequentialPolygonNonoverlappingBinaryCentered(const SolverConfiguration                        &solver_configuration,
								     std::vector<Rational>                            &dec_values_X,
								     std::vector<Rational>                            &dec_values_Y,
								     std::vector<Rational>                            &dec_values_T,
								     const std::vector<Slic3r::Polygon>               &polygons,
								     const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons,
								     const std::vector<int>                           &undecided_polygons,
								     std::vector<int>                                 &decided_polygons,
								     std::vector<int>                                 &remaining_polygons);


bool optimize_SubglobalConsequentialPolygonNonoverlappingBinaryCentered(const SolverConfiguration          &solver_configuration,
									std::vector<Rational>              &dec_values_X,
									std::vector<Rational>              &dec_values_Y,
									std::vector<Rational>              &dec_values_T,
									const std::vector<Slic3r::Polygon> &polygons,
									const std::vector<Slic3r::Polygon> &unreachable_polygons,
									const std::vector<int>             &undecided_polygons,
									std::vector<int>                   &decided_polygons,
									std::vector<int>                   &remaining_polygons);

bool optimize_SubglobalConsequentialPolygonNonoverlappingBinaryCentered(const SolverConfiguration                        &solver_configuration,
									std::vector<Rational>                            &dec_values_X,
									std::vector<Rational>                            &dec_values_Y,
									std::vector<Rational>                            &dec_values_T,
									const std::vector<Slic3r::Polygon>               &polygons,
									const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons,
									const std::vector<int>                           &undecided_polygons,
									std::vector<int>                                 &decided_polygons,
									std::vector<int>                                 &remaining_polygons);

/*----------------------------------------------------------------*/

} // namespace Sequential


/*----------------------------------------------------------------*/

#endif /* __SEQ_SEQUENTIAL_HPP__ */
