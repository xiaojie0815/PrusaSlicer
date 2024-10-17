/*================================================================*/
/*
 * Author:  Pavel Surynek, 2023 - 2024
 * Company: Prusa Research
 *
 * File:    seq_sequential.cpp
 *
 * SMT models for sequential printing.
 */
/*================================================================*/

#include "seq_defs.hpp"

#include "seq_sequential.hpp"
#include "seq_preprocess.hpp"


/*----------------------------------------------------------------*/

using namespace std;
using namespace Slic3r;


/*----------------------------------------------------------------*/

namespace Sequential
{

    
/*----------------------------------------------------------------*/

int hidden_var_cnt = 0;


/*----------------------------------------------------------------*/
 
void introduce_DecisionBox(z3::solver     &Solver,
			   const z3::expr &dec_var_X,
			   const z3::expr &dec_var_Y,
			   int             box_size_x,
			   int             box_size_y)
{
    Solver.add(dec_var_X >= 0);
    Solver.add(dec_var_X <= box_size_x);
    Solver.add(dec_var_Y >= 0);
    Solver.add(dec_var_Y <= box_size_y);
}


void assume_DecisionBox(const z3::expr  &dec_var_X,
			const z3::expr  &dec_var_Y,
			int              box_size_x,
			int              box_size_y,
			z3::expr_vector &box_constraints)
{
    box_constraints.push_back(dec_var_X >= 0);
    box_constraints.push_back(dec_var_X <= box_size_x);
    box_constraints.push_back(dec_var_Y >= 0);
    box_constraints.push_back(dec_var_Y <= box_size_y);
}


void introduce_BedBoundingBox(z3::solver             &Solver,
			      const z3::expr         &dec_var_X,
			      const z3::expr         &dec_var_Y,			      
			      const Slic3r::Polygon &polygon,
			      int                    box_size_x,
			      int                    box_size_y)
{
    BoundingBox box = get_extents(polygon);
    
    Solver.add(dec_var_X + box.min.x() >= 0);
    Solver.add(dec_var_X + box.max.x() <= box_size_x);
    
    Solver.add(dec_var_Y + box.min.y() >= 0);
    Solver.add(dec_var_Y + box.max.y() <= box_size_y);	    
}


void assume_BedBoundingBox(const z3::expr        &dec_var_X,
			   const z3::expr        &dec_var_Y,			      
			   const Slic3r::Polygon &polygon,
			   int                    box_size_x,
			   int                    box_size_y,
			   z3::expr_vector       &bounding_constraints)
{
    BoundingBox box = get_extents(polygon);
    
    bounding_constraints.push_back(dec_var_X + box.min.x() >= 0);
    bounding_constraints.push_back(dec_var_X + box.max.x() <= box_size_x);
    
    bounding_constraints.push_back(dec_var_Y + box.min.y() >= 0);
    bounding_constraints.push_back(dec_var_Y + box.max.y() <= box_size_y);    
}






void introduce_BedBoundingBox(z3::solver             &Solver,
			      const z3::expr         &dec_var_X,
			      const z3::expr         &dec_var_Y,			      
			      const Slic3r::Polygon &polygon,
			      int                    box_min_x,
			      int                    box_min_y,
			      int                    box_max_x,
			      int                    box_max_y)    
{
    BoundingBox box = get_extents(polygon);
    
    Solver.add(dec_var_X + box.min.x() >= box_min_x);
    Solver.add(dec_var_X + box.max.x() <= box_max_x);
    
    Solver.add(dec_var_Y + box.min.y() >= box_min_y);
    Solver.add(dec_var_Y + box.max.y() <= box_max_y);	    
}


void assume_BedBoundingBox(const z3::expr        &dec_var_X,
			   const z3::expr        &dec_var_Y,			      
			   const Slic3r::Polygon &polygon,
			   int                    box_min_x,
			   int                    box_min_y,
			   int                    box_max_x,
			   int                    box_max_y,			   
			   z3::expr_vector       &bounding_constraints)
{
    BoundingBox box = get_extents(polygon);
    
    bounding_constraints.push_back(dec_var_X + box.min.x() >= box_min_x);
    bounding_constraints.push_back(dec_var_X + box.max.x() <= box_max_x);
    
    bounding_constraints.push_back(dec_var_Y + box.min.y() >= box_min_y);
    bounding_constraints.push_back(dec_var_Y + box.max.y() <= box_max_y);    
}





void introduce_BedBoundingBox(z3::solver                         &Solver,
			      const z3::expr_vector              &dec_vars_X,
			      const z3::expr_vector              &dec_vars_Y,
			      const std::vector<Slic3r::Polygon> &polygons,
			      int                                 box_size_x,
			      int                                 box_size_y)
{
    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	BoundingBox box = get_extents(polygons[i]);

	Solver.add(dec_vars_X[i] + box.min.x() >= 0);
	Solver.add(dec_vars_X[i] + box.max.x() <= box_size_x);

	Solver.add(dec_vars_Y[i] + box.min.y() >= 0);
	Solver.add(dec_vars_Y[i] + box.max.y() <= box_size_y);	
    }    
}


void assume_BedBoundingBox(const z3::expr_vector              &dec_vars_X,
			   const z3::expr_vector              &dec_vars_Y,
			   const std::vector<Slic3r::Polygon> &polygons,
			   int                                 box_size_x,
			   int                                 box_size_y,
			   z3::expr_vector                    &bounding_constraints)
{
    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	BoundingBox box = get_extents(polygons[i]);

	bounding_constraints.push_back(dec_vars_X[i] + box.min.x() >= 0);
	bounding_constraints.push_back(dec_vars_X[i] + box.max.x() <= box_size_x);

	bounding_constraints.push_back(dec_vars_Y[i] + box.min.y() >= 0);
	bounding_constraints.push_back(dec_vars_Y[i] + box.max.y() <= box_size_y);
    }
}


void introduce_BedBoundingBox(z3::solver                         &Solver,
			      const z3::expr_vector              &dec_vars_X,
			      const z3::expr_vector              &dec_vars_Y,
			      const std::vector<Slic3r::Polygon> &polygons,
			      int                                 box_min_x,
			      int                                 box_min_y,			   
			      int                                 box_max_x,
			      int                                 box_max_y)			      
{
    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	BoundingBox box = get_extents(polygons[i]);

	Solver.add(dec_vars_X[i] + box.min.x() >= box_min_x);
	Solver.add(dec_vars_X[i] + box.max.x() <= box_max_x);

	Solver.add(dec_vars_Y[i] + box.min.y() >= box_min_y);
	Solver.add(dec_vars_Y[i] + box.max.y() <= box_max_y);	
    }    
}


void assume_BedBoundingBox_(const z3::expr_vector              &dec_vars_X,
			    const z3::expr_vector              &dec_vars_Y,
			    const std::vector<Slic3r::Polygon> &polygons,
			    int                                 box_min_x,
			    int                                 box_min_y,			   
			    int                                 box_max_x,
			    int                                 box_max_y,
			    z3::expr_vector                    &bounding_constraints)
{
    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	BoundingBox box = get_extents(polygons[i]);

	bounding_constraints.push_back(dec_vars_X[i] + box.min.x() >= box_min_x);
	bounding_constraints.push_back(dec_vars_X[i] + box.max.x() <= box_max_x);

	bounding_constraints.push_back(dec_vars_Y[i] + box.min.y() >= box_min_y);
	bounding_constraints.push_back(dec_vars_Y[i] + box.max.y() <= box_max_y);
    }
}


void assume_ConsequentialObjectPresence(z3::context            &Context,
					const z3::expr_vector  &dec_vars_T,
					const std::vector<int> &present,
					const std::vector<int> &missing,
					z3::expr_vector        &presence_constraints)
{
    for (unsigned int i = 0; i < present.size(); ++i)
    {
	presence_constraints.push_back(dec_vars_T[present[i]] > Context.real_val(SEQ_TEMPORAL_PRESENCE_THRESHOLD));
    }

    for (unsigned int i = 0; i < missing.size(); ++i)
    {
	presence_constraints.push_back(dec_vars_T[missing[i]] < Context.real_val(SEQ_TEMPORAL_ABSENCE_THRESHOLD));
    }    
}


void introduce_TemporalOrdering(z3::solver                         &Solver,
				z3::context                        &SEQ_UNUSED(Context),
				const z3::expr_vector              &dec_vars_T,
				int                                temporal_spread,				
				const std::vector<Slic3r::Polygon> &polygons)
{
    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    Solver.add(dec_vars_T[i] > dec_vars_T[j] + temporal_spread || dec_vars_T[i] + temporal_spread < dec_vars_T[j]);
	}
    }
}


void introduce_SequentialTemporalOrderingAgainstFixed(z3::solver                         &Solver,
						      z3::context                        &Context,
						      const z3::expr_vector              &dec_vars_T,
						      std::vector<Rational>              &dec_values_T,
						      const std::vector<int>             &fixed,
						      const std::vector<int>             &undecided,
						      int                                temporal_spread,
						      const std::vector<Slic3r::Polygon> &SEQ_UNUSED(polygons))
{
    for (unsigned int i = 0; i < undecided.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < undecided.size(); ++j)
	{
	    Solver.add(dec_vars_T[undecided[i]] > dec_vars_T[undecided[j]] + temporal_spread || dec_vars_T[undecided[i]] + temporal_spread < dec_vars_T[undecided[j]]);
	}
    }

    for (unsigned int i = 0; i < undecided.size(); ++i)
    {
	for (unsigned int j = 0; j < fixed.size(); ++j)
	{
	    Solver.add(   dec_vars_T[undecided[i]] > Context.real_val(dec_values_T[fixed[j]].numerator, dec_values_T[fixed[j]].denominator) + temporal_spread
		       || dec_vars_T[undecided[i]] + temporal_spread < Context.real_val(dec_values_T[fixed[j]].numerator, dec_values_T[fixed[j]].denominator));
	}	
    }

    #ifdef DEBUG
    {
	printf("Origo\n");
	for (unsigned int i = 0; i < fixed.size(); ++i)
	{
	    printf("%.3f\n", dec_values_T[fixed[i]].as_double());
	}
    }
    #endif
}


void introduce_ConsequentialTemporalOrderingAgainstFixed(z3::solver                         &Solver,
							 z3::context                        &Context,
							 const z3::expr_vector              &dec_vars_T,
							 std::vector<Rational>              &dec_values_T,
							 const std::vector<int>             &fixed,
							 const std::vector<int>             &undecided,
							 int                                 temporal_spread,
							 const std::vector<Slic3r::Polygon> &SEQ_UNUSED(polygons))
{
    for (unsigned int i = 0; i < undecided.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < undecided.size(); ++j)
	{
	    Solver.add(dec_vars_T[undecided[i]] > dec_vars_T[undecided[j]] + temporal_spread || dec_vars_T[undecided[i]] + temporal_spread < dec_vars_T[undecided[j]]);
	}
    }

    for (unsigned int i = 0; i < undecided.size(); ++i)
    {
	for (unsigned int j = 0; j < fixed.size(); ++j)
	{
	    Solver.add(   dec_vars_T[undecided[i]] > Context.real_val(dec_values_T[fixed[j]].numerator, dec_values_T[fixed[j]].denominator) + temporal_spread
		       || dec_vars_T[undecided[i]] + temporal_spread < Context.real_val(dec_values_T[fixed[j]].numerator, dec_values_T[fixed[j]].denominator));
	}	
    }

    #ifdef DEBUG
    {
	printf("Origo\n");
	for (unsigned int i = 0; i < fixed.size(); ++i)
	{
	    printf("%.3f\n", dec_values_T[fixed[i]].as_double());
	}
    }
    #endif
}


void introduce_ConsequentialTemporalLepoxAgainstFixed(z3::solver                         &Solver,
						      z3::context                        &Context,
						      const z3::expr_vector              &dec_vars_T,
						      std::vector<Rational>              &dec_values_T,
						      const std::vector<int>             &fixed,
						      const std::vector<int>             &undecided,
						      int                                 temporal_spread,
						      const std::vector<Slic3r::Polygon> &SEQ_UNUSED(polygons),
						      const std::vector<int>             &previous_polygons)
{
    std::set<int> fixed_(fixed.begin(), fixed.end());
    std::set<int> undecided_(undecided.begin(), undecided.end());
	
    for (unsigned int i = 0; i < undecided.size(); ++i)
    {
	if (previous_polygons[undecided[i]] >= 0)
	{
	    //Solver.add(dec_vars_T[previous_polygons[undecided[i]]] + temporal_spread < dec_vars_T[undecided[i]] && dec_vars_T[previous_polygons[undecided[i]]] + temporal_spread + temporal_spread / 2 > dec_vars_T[undecided[i]]);
	}
    }

    #ifdef DEBUG
    {
	printf("Origo\n");
	for (unsigned int i = 0; i < fixed.size(); ++i)
	{
	    printf("%.3f\n", dec_values_T[fixed[i]].as_double());
	}
    }
    #endif
}


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
				   const Slic3r::Line &line2)
{
    introduce_LineNonIntersection_implicit(Solver,
					   Context,
					   dec_var_X1,
					   dec_var_Y1,
					   dec_var_T1,
					   line1,
					   dec_var_X2,
					   dec_var_Y2,
					   dec_var_T2,				   
					   line2);    
}


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
					     const Slic3r::Line &line2)
{
    introduce_SequentialLineNonIntersection_implicit(Solver,
						     Context,
						     dec_var_X1,
						     dec_var_Y1,
						     dec_var_T1,
						     dec_var_t1,					   
						     line1,
						     dec_var_X2,
						     dec_var_Y2,
						     dec_var_T2,
						     dec_var_t2,
						     line2);    
}


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
						const Slic3r::Line &line2)
{
    introduce_ConsequentialLineNonIntersection_implicit(Solver,
							Context,
							dec_var_X1,
							dec_var_Y1,
							dec_var_T1,
							dec_var_t1,					   
							line1,
							dec_var_X2,
							dec_var_Y2,
							dec_var_T2,
							dec_var_t2,
							line2);    
}


void introduce_LineNonIntersection_implicit(z3::solver         &Solver,
					    z3::context        &Context,					    
					    const z3::expr     &dec_var_X1,
					    const z3::expr     &dec_var_Y1,
					    const z3::expr     &dec_var_T1,				   
					    const Slic3r::Line &line1,
					    const z3::expr     &dec_var_X2,
					    const z3::expr     &dec_var_Y2,
					    const z3::expr     &dec_var_T2,
					    const Slic3r::Line &line2)
{    
    Point point;
    
    if (line1.intersection_infinite(line2, &point))
    {
	int v1x = line1.b.x() - line1.a.x();
	int v1y = line1.b.y() - line1.a.y();
	
	int v2x = line2.b.x() - line2.a.x();
	int v2y = line2.b.y() - line2.a.y();

	#ifdef DEBUG
	{
	    printf("adding constraint iota: [%d, %d, %d, %d] [%d, %d, %d, %d]\n", line1.a.x(), line1.a.y(), line1.b.x(), line1.b.y(),
		   line2.a.x(), line2.a.y(), line2.b.x(), line2.b.y());
	}
	#endif
	
	Solver.add((dec_var_X1 + line1.a.x() + v1x * dec_var_T1) == (dec_var_X2 + line2.a.x() + v2x * dec_var_T2));
	Solver.add((dec_var_Y1 + line1.a.y() + v1y * dec_var_T1) == (dec_var_Y2 + line2.a.y() + v2y * dec_var_T2));
	
	Solver.add(   dec_var_T1 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		   || dec_var_T1 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX)
		   || dec_var_T2 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		   || dec_var_T2 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX));
    }
}


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
						      const Slic3r::Line &line2)
{    
    Point point;
    
    if (line1.intersection_infinite(line2, &point))
    {
	int v1x = line1.b.x() - line1.a.x();
	int v1y = line1.b.y() - line1.a.y();
	
	int v2x = line2.b.x() - line2.a.x();
	int v2y = line2.b.y() - line2.a.y();

        #ifdef DEBUG
	{
	    printf("adding constraint seq: [%d, %d, %d, %d] [%d, %d, %d, %d]\n", line1.a.x(), line1.a.y(), line1.b.x(), line1.b.y(),
		   line2.a.x(), line2.a.y(), line2.b.x(), line2.b.y());
	}
	#endif
	
	Solver.add((dec_var_X1 + line1.a.x() + v1x * dec_var_t1) == (dec_var_X2 + line2.a.x() + v2x * dec_var_t2));
	Solver.add((dec_var_Y1 + line1.a.y() + v1y * dec_var_t1) == (dec_var_Y2 + line2.a.y() + v2y * dec_var_t2));
	
//	Solver.add(dec_var_T1 < dec_var_T2 || dec_var_t1 < 0 || dec_var_t1 > 1 || dec_var_t2 < 0 || dec_var_t2 > 1);
	Solver.add(   dec_var_T1 < dec_var_T2
		   || dec_var_t1 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		   || dec_var_t1 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX)
		   || dec_var_t2 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		   || dec_var_t2 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX));
    }
}


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
							 const Slic3r::Line &line2)
{    
    Point point;
    
    if (line1.intersection_infinite(line2, &point))
    {
	int v1x = line1.b.x() - line1.a.x();
	int v1y = line1.b.y() - line1.a.y();
	
	int v2x = line2.b.x() - line2.a.x();
	int v2y = line2.b.y() - line2.a.y();

        #ifdef DEBUG
	{
	    printf("adding constraint seq: [%d, %d, %d, %d] [%d, %d, %d, %d]\n", line1.a.x(), line1.a.y(), line1.b.x(), line1.b.y(),
		   line2.a.x(), line2.a.y(), line2.b.x(), line2.b.y());
	}
	#endif
	
	Solver.add((dec_var_X1 + line1.a.x() + v1x * dec_var_t1) == (dec_var_X2 + line2.a.x() + v2x * dec_var_t2));
	Solver.add((dec_var_Y1 + line1.a.y() + v1y * dec_var_t1) == (dec_var_Y2 + line2.a.y() + v2y * dec_var_t2));
	
//	Solver.add(dec_var_T1 < dec_var_T2 || dec_var_t1 < 0 || dec_var_t1 > 1 || dec_var_t2 < 0 || dec_var_t2 > 1);
	Solver.add(   dec_var_T1 < 0
		   || dec_var_T2 < 0
		   || dec_var_T1 < dec_var_T2
		   || dec_var_t1 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		   || dec_var_t1 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX)
		   || dec_var_t2 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		   || dec_var_t2 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX));
    }
}


void introduce_LineNonIntersection_explicit(z3::solver         &Solver,
					    z3::context        &Context,					    
					    const z3::expr     &dec_var_X1,
					    const z3::expr     &dec_var_Y1,
					    const z3::expr     &dec_var_T1,				   
					    const Slic3r::Line &line1,
					    const z3::expr     &dec_var_X2,
					    const z3::expr     &dec_var_Y2,
					    const z3::expr     &dec_var_T2,
					    const Slic3r::Line &line2)
{
    Point point;
    if (line1.intersection_infinite(line2, &point))
    {    
	int v1x = line1.b.x() - line1.a.x();
	int v1y = line1.b.y() - line1.a.y();

	int v2x = line2.b.x() - line2.a.x();
	int v2y = line2.b.y() - line2.a.y();
	
	if (abs(v2x) > 0)
	{
	    int coef_T1 = v1y * v2x - v1x * v2y;
	    int d1 = v2x * line1.a.y() - v2x * line2.a.y() - v2y * line1.a.x() + v2y * line2.a.x();
	    
	    int coef_X1 = -v2y;
	    int coef_Y1 = v2x;
	    
	    int coef_X2 = v2y;
	    int coef_Y2 = -v2x;
	    
	    Solver.add( ((coef_X1 * dec_var_X1)
			 + (coef_Y1 * dec_var_Y1)
			 + (coef_X2 * dec_var_X2)
			 + (coef_Y2 * dec_var_Y2)
			 + (coef_T1 * dec_var_T1)
			 + d1) == 0);

	    int d2 = line1.a.x() - line2.a.x();
	    
	    Solver.add( (dec_var_X1
			 - dec_var_X2
			 + v1x * dec_var_T1
			 - v2x * dec_var_T2
			 + d2) == 0);
	    
	    Solver.add(   dec_var_T1 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		       || dec_var_T1 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX)
		       || dec_var_T2 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		       || dec_var_T2 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX));
	}
	else
	{
	    if (abs(v2y) > 0)
	    {
		int coef_T2 = v1y * v2x - v1x * v2y;
		int d1 = v2y * line1.a.x() - v2y * line2.a.x() - v2x * line1.a.y() + v2x * line2.a.y();
		
		int coef_X1 = v2y;
		int coef_Y1 = -v2x;
		
		int coef_X2 = -v2y;
		int coef_Y2 = v2x;
		
		Solver.add( ((coef_X1 * dec_var_X1)
			     + (coef_Y1 * dec_var_Y1)
			     + (coef_X2 * dec_var_X2)
			     + (coef_Y2 * dec_var_Y2)
			     + (coef_T2 * dec_var_T2)
			     + d1) == 0);	    
		
		int d2 = line1.a.y() - line2.a.y();
		
		Solver.add( (dec_var_Y1
			     - dec_var_Y2
			     + v1y * dec_var_T1
			     - v2y* dec_var_T2
			     + d2) == 0);
		
		Solver.add(   dec_var_T1 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
			   || dec_var_T1 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX)
			   || dec_var_T2 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
			   || dec_var_T2 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX));	    
	    }
	    else
	    {
		/* intersection not possible, the second line is empty */
		assert(false);
	    }
	}
    }
}



void introduce_LineNonIntersectionAgainstFixedLine(z3::solver         &Solver,
						   z3::context        &Context,
						   const z3::expr     &dec_var_X1,
						   const z3::expr     &dec_var_Y1,
						   const z3::expr     &dec_var_T1,
						   const Slic3r::Line &line1,
						   const Rational     &dec_value_X2,
						   const Rational     &dec_value_Y2,
						   const z3::expr     &dec_var_T2,				   
						   const Slic3r::Line &line2)
{
    introduce_LineNonIntersectionAgainstFixedLine_implicit(Solver,
							   Context,
							   dec_var_X1,
							   dec_var_Y1,
							   dec_var_T1,
							   line1,
							   dec_value_X2,
							   dec_value_Y2,
							   dec_var_T2,				   
							   line2);    
}


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
							     const Slic3r::Line &line2)
{
    introduce_SequentialLineNonIntersectionAgainstFixedLine_implicit(Solver,
								     Context,
								     dec_var_X1,
								     dec_var_Y1,
								     dec_var_T1,
								     dec_var_t1,
								     line1,
								     dec_value_X2,
								     dec_value_Y2,
								     dec_value_T2,
								     dec_var_t2,
								     line2);    
}


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
							     const Slic3r::Line &line2)
{
    introduce_SequentialFixedLineNonIntersectionAgainstLine_implicit(Solver,
								     Context,
								     dec_value_X1,
								     dec_value_Y1,
								     dec_value_T1,
								     dec_var_t1,
								     line1,
								     dec_var_X2,
								     dec_var_Y2,
								     dec_var_T2,
								     dec_var_t2,
								     line2);    
}


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
								const Slic3r::Line &line2)
{
    introduce_ConsequentialLineNonIntersectionAgainstFixedLine_implicit(Solver,
									Context,
									dec_var_X1,
									dec_var_Y1,
									dec_var_T1,
									dec_var_t1,
									line1,
									dec_value_X2,
									dec_value_Y2,
									dec_value_T2,
									dec_var_t2,
									line2);    
}


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
								const Slic3r::Line &line2)
{
    introduce_ConsequentialFixedLineNonIntersectionAgainstLine_implicit(Solver,
									Context,
									dec_value_X1,
									dec_value_Y1,
									dec_value_T1,
									dec_var_t1,
									line1,
									dec_var_X2,
									dec_var_Y2,
									dec_var_T2,
									dec_var_t2,
									line2);    
}


void introduce_LineNonIntersectionAgainstFixedLine_implicit(z3::solver         &Solver,
							    z3::context        &Context,
							    const z3::expr     &dec_var_X1,
							    const z3::expr     &dec_var_Y1,
							    const z3::expr     &dec_var_T1,
							    const Slic3r::Line &line1,
							    const Rational     &dec_value_X2,
							    const Rational     &dec_value_Y2,
							    const z3::expr     &dec_var_T2,
							    const Slic3r::Line &line2)
{    
    Point point;
    
    if (line1.intersection_infinite(line2, &point))
    {
	int v1x = line1.b.x() - line1.a.x();
	int v1y = line1.b.y() - line1.a.y();
	
	int v2x = line2.b.x() - line2.a.x();
	int v2y = line2.b.y() - line2.a.y();

	#ifdef DEBUG
	{
	    printf("adding constraint alpha [%d, %d, %d, %d] [%d, %d, %d, %d]\n", line1.a.x(), line1.a.y(), line1.b.x(), line1.b.y(),
		   line2.a.x(), line2.a.y(), line2.b.x(), line2.b.y());
	}
	#endif
	
	Solver.add((dec_var_X1 + line1.a.x() + v1x * dec_var_T1) == (Context.real_val(dec_value_X2.numerator, dec_value_X2.denominator) + line2.a.x() + v2x * dec_var_T2));
	Solver.add((dec_var_Y1 + line1.a.y() + v1y * dec_var_T1) == (Context.real_val(dec_value_Y2.numerator, dec_value_Y2.denominator) + line2.a.y() + v2y * dec_var_T2));
	
	Solver.add(   dec_var_T1 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		   || dec_var_T1 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX)
		   || dec_var_T2 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		   || dec_var_T2 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX));
    }
}


void introduce_LineNonIntersectionAgainstFixedLine_explicit(z3::solver         &Solver,
							    z3::context        &Context,
							    const z3::expr     &dec_var_X1,
							    const z3::expr     &dec_var_Y1,
							    const z3::expr     &dec_var_T1,
							    const Slic3r::Line &line1,
							    const Rational     &dec_value_X2,
							    const Rational     &dec_value_Y2,
							    const z3::expr     &dec_var_T2,
							    const Slic3r::Line &line2)
{
    Point point;
    if (line1.intersection_infinite(line2, &point))
    {    
	int v1x = line1.b.x() - line1.a.x();
	int v1y = line1.b.y() - line1.a.y();

	int v2x = line2.b.x() - line2.a.x();
	int v2y = line2.b.y() - line2.a.y();
	
	if (abs(v2x) > 0)
	{
	    int coef_T1 = v1y * v2x - v1x * v2y;
	    int d1 = v2x * line1.a.y() - v2x * line2.a.y() - v2y * line1.a.x() + v2y * line2.a.x();
	    
	    int coef_X1 = -v2y;
	    int coef_Y1 = v2x;
	    
	    int coef_X2 = v2y;
	    int coef_Y2 = -v2x;
	    
	    Solver.add( ((coef_X1 * dec_var_X1)
			 + (coef_Y1 * dec_var_Y1)
			 + (coef_X2 * Context.real_val(dec_value_X2.numerator, dec_value_X2.denominator))
			 + (coef_Y2 * Context.real_val(dec_value_Y2.numerator, dec_value_Y2.denominator))
			 + (coef_T1 * dec_var_T1)
			 + d1) == 0);

	    int d2 = line1.a.x() - line2.a.x();
	    
	    Solver.add( (dec_var_X1
			 - Context.real_val(dec_value_X2.numerator, dec_value_X2.denominator)
			 + v1x * dec_var_T1
			 - v2x * dec_var_T2
			 + d2) == 0);
	    
	    Solver.add(   dec_var_T1 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		       || dec_var_T1 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX)
		       || dec_var_T2 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		       || dec_var_T2 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX));
	}
	else
	{
	    if (abs(v2y) > 0)
	    {
		int coef_T2 = v1y * v2x - v1x * v2y;
		int d1 = v2y * line1.a.x() - v2y * line2.a.x() - v2x * line1.a.y() + v2x * line2.a.y();
		
		int coef_X1 = v2y;
		int coef_Y1 = -v2x;
		
		int coef_X2 = -v2y;
		int coef_Y2 = v2x;
		
		Solver.add( (  (coef_X1 * dec_var_X1)
			     + (coef_Y1 * dec_var_Y1)
			     + (coef_X2 * Context.real_val(dec_value_X2.numerator, dec_value_X2.denominator))
			     + (coef_Y2 * Context.real_val(dec_value_Y2.numerator, dec_value_Y2.denominator))
			     + (coef_T2 * dec_var_T2)
			     + d1) == 0);	    
		
		int d2 = line1.a.y() - line2.a.y();
		
		Solver.add( (  dec_var_Y1
			     - Context.real_val(dec_value_Y2.numerator, dec_value_Y2.denominator)
			     + v1y * dec_var_T1
			     - v2y* dec_var_T2
			     + d2) == 0);
		
		Solver.add(   dec_var_T1 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
			   || dec_var_T1 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX)
			   || dec_var_T2 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
			   || dec_var_T2 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX));	    
	    }
	    else
	    {
		/* intersection not possible, the second line is empty */
		assert(false);
	    }
	}
    }
}


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
								      const Slic3r::Line &line2)
{    
    Point point;
    
    if (line1.intersection_infinite(line2, &point))
    {
	int v1x = line1.b.x() - line1.a.x();
	int v1y = line1.b.y() - line1.a.y();
	
	int v2x = line2.b.x() - line2.a.x();
	int v2y = line2.b.y() - line2.a.y();

	#ifdef DEBUG
	{
	    printf("adding constraint beta: [%d, %d, %d, %d] [%d, %d, %d, %d] (%.3f,%.3f,%.3f)\n", line1.a.x(), line1.a.y(), line1.b.x(), line1.b.y(),
		   line2.a.x(), line2.a.y(), line2.b.x(), line2.b.y(), dec_value_X2.as_double(), dec_value_Y2.as_double(), dec_value_T2.as_double());
	    printf("v1: %d,%d v2:%d,%d\n", v1x, v1y, v2x, v2y);
	}
        #endif
	
	Solver.add((dec_var_X1 + line1.a.x() + v1x * dec_var_t1) == (Context.real_val(dec_value_X2.numerator, dec_value_X2.denominator) + line2.a.x() + v2x * dec_var_t2));
	Solver.add((dec_var_Y1 + line1.a.y() + v1y * dec_var_t1) == (Context.real_val(dec_value_Y2.numerator, dec_value_Y2.denominator) + line2.a.y() + v2y * dec_var_t2));

	Solver.add(   (dec_var_T1 < Context.real_val(dec_value_T2.numerator, dec_value_T2.denominator))
		   || (dec_var_t1 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN))
		   || (dec_var_t1 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX))
		   || (dec_var_t2 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN))
		   || (dec_var_t2 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX)));
    }
}


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
								      const Slic3r::Line &line2)
{    
    Point point;
    
    if (line1.intersection_infinite(line2, &point))
    {
	int v1x = line1.b.x() - line1.a.x();
	int v1y = line1.b.y() - line1.a.y();
	
	int v2x = line2.b.x() - line2.a.x();
	int v2y = line2.b.y() - line2.a.y();

	#ifdef DEBUG
	{
	    printf("adding constraint gamma: [%d, %d, %d, %d] [%d, %d, %d, %d]\n", line1.a.x(), line1.a.y(), line1.b.x(), line1.b.y(),
		   line2.a.x(), line2.a.y(), line2.b.x(), line2.b.y());
	}
	#endif
	
	Solver.add((Context.real_val(dec_value_X1.numerator, dec_value_X1.denominator) + line1.a.x() + v1x * dec_var_t1) == (dec_var_X2 + line2.a.x() + v2x * dec_var_t2));
	Solver.add((Context.real_val(dec_value_Y1.numerator, dec_value_Y1.denominator) + line1.a.y() + v1y * dec_var_t1) == (dec_var_Y2 + line2.a.y() + v2y * dec_var_t2));
	
	Solver.add(   Context.real_val(dec_value_T1.numerator, dec_value_T1.denominator) < dec_var_T2
	           || dec_var_t1 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		   || dec_var_t1 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX)
		   || dec_var_t2 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
		   || dec_var_t2 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX));
    }
}


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
									 const Slic3r::Line &line2)
{

    if (dec_value_T2.is_Positive())
    {
	Point point;
    
	if (line1.intersection_infinite(line2, &point))
	{
	    int v1x = line1.b.x() - line1.a.x();
	    int v1y = line1.b.y() - line1.a.y();
	    
	    int v2x = line2.b.x() - line2.a.x();
	    int v2y = line2.b.y() - line2.a.y();

	    #ifdef DEBUG
	    {
		printf("adding constraint beta: [%d, %d, %d, %d] [%d, %d, %d, %d] (%.3f,%.3f,%.3f)\n", line1.a.x(), line1.a.y(), line1.b.x(), line1.b.y(),
		       line2.a.x(), line2.a.y(), line2.b.x(), line2.b.y(), dec_value_X2.as_double(), dec_value_Y2.as_double(), dec_value_T2.as_double());
		printf("v1: %d,%d v2:%d,%d\n", v1x, v1y, v2x, v2y);
	    }
	    #endif
	
	    Solver.add((dec_var_X1 + line1.a.x() + v1x * dec_var_t1) == (Context.real_val(dec_value_X2.numerator, dec_value_X2.denominator) + line2.a.x() + v2x * dec_var_t2));
	    Solver.add((dec_var_Y1 + line1.a.y() + v1y * dec_var_t1) == (Context.real_val(dec_value_Y2.numerator, dec_value_Y2.denominator) + line2.a.y() + v2y * dec_var_t2));
	    
	    Solver.add(    dec_var_T1 < 0
		       || (dec_var_T1 < Context.real_val(dec_value_T2.numerator, dec_value_T2.denominator))
		       || (dec_var_t1 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN))
		       || (dec_var_t1 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX))
		       || (dec_var_t2 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN))
		       || (dec_var_t2 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX)));
	}
    }
}


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
									 const Slic3r::Line &line2)
{
    if (dec_value_T1.is_Positive())
    {
	Point point;
    
	if (line1.intersection_infinite(line2, &point))
	{
	    int v1x = line1.b.x() - line1.a.x();
	    int v1y = line1.b.y() - line1.a.y();
	    
	    int v2x = line2.b.x() - line2.a.x();
	    int v2y = line2.b.y() - line2.a.y();
	    
	    #ifdef DEBUG
	    {
		printf("adding constraint gamma: [%d, %d, %d, %d] [%d, %d, %d, %d]\n", line1.a.x(), line1.a.y(), line1.b.x(), line1.b.y(),
		       line2.a.x(), line2.a.y(), line2.b.x(), line2.b.y());
	    }
	    #endif
	
	    Solver.add((Context.real_val(dec_value_X1.numerator, dec_value_X1.denominator) + line1.a.x() + v1x * dec_var_t1) == (dec_var_X2 + line2.a.x() + v2x * dec_var_t2));
	    Solver.add((Context.real_val(dec_value_Y1.numerator, dec_value_Y1.denominator) + line1.a.y() + v1y * dec_var_t1) == (dec_var_Y2 + line2.a.y() + v2y * dec_var_t2));
	
	    Solver.add(    dec_var_T2 < 0
			|| Context.real_val(dec_value_T1.numerator, dec_value_T1.denominator) < dec_var_T2
			|| dec_var_t1 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
			|| dec_var_t1 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX)
			|| dec_var_t2 < Context.real_val(SEQ_INTERSECTION_REPULSION_MIN)
			|| dec_var_t2 > Context.real_val(SEQ_INTERSECTION_REPULSION_MAX));
	}
    }
}


/*----------------------------------------------------------------*/

void introduce_PointInsideHalfPlane(z3::solver         &Solver,
				    const z3::expr     &dec_var_X1,
				    const z3::expr     &dec_var_Y1,
				    const z3::expr     &dec_var_X2,
				    const z3::expr     &dec_var_Y2,
				    const Slic3r::Line &halving_line)
{
    Vector normal = halving_line.normal();

    Solver.add(  (normal.x() * dec_var_X1)
	       + (normal.y() * dec_var_Y1)		 
	       - (normal.x() * dec_var_X2 + normal.x() * halving_line.a.x())
	       - (normal.y() * dec_var_Y2 + normal.y() * halving_line.a.y()) < 0);
}


void introduce_PointOutsideHalfPlane(z3::solver         &Solver,
				     const z3::expr     &dec_var_X1,
				     const z3::expr     &dec_var_Y1,
				     const z3::expr     &dec_var_X2,
				     const z3::expr     &dec_var_Y2,
				     const Slic3r::Line &halving_line)
{
    Vector normal = halving_line.normal();

    Solver.add(  (normal.x() * dec_var_X1)
	       + (normal.y() * dec_var_Y1)		 
	       - (normal.x() * dec_var_X2 + normal.x() * halving_line.a.x())
	       - (normal.y() * dec_var_Y2 + normal.y() * halving_line.a.y()) > 0);
}


void introduce_PointInsidePolygon(z3::solver            &Solver,
				  z3::context           &Context,
				  const z3::expr        &dec_var_X1,
				  const z3::expr        &dec_var_Y1,
				  const z3::expr        &dec_var_X2,
				  const z3::expr        &dec_var_Y2,
				  const Slic3r::Polygon &polygon)
{
    if (polygon.points.size() >= 3)
    {   
	z3::expr in_conjunction(Context);

	for (Points::const_iterator point = polygon.points.begin(); point != polygon.points.end(); ++point)
	{
	    Points::const_iterator next_point = point + 1;
	    if (next_point == polygon.points.end())
	    {
		next_point = polygon.points.begin();
	    }
	    
	    Line line(*point, *next_point);
	    Vector normal = line.normal();
	    
	    z3::expr inside_half_plane(  (normal.x() * dec_var_X1)
				       + (normal.y() * dec_var_Y1)		 
				       - (normal.x() * dec_var_X2)
				       - (normal.x() * line.a.x())
				       - (normal.y() * dec_var_Y2)
				       - (normal.y() * line.a.y()) < 0);

	    if (point == polygon.points.begin())
	    {
		in_conjunction = inside_half_plane;
	    }
	    else
	    {
		in_conjunction = in_conjunction && inside_half_plane;
	    }
	}

	Solver.add(in_conjunction);
    }
}


/*----------------------------------------------------------------*/

void introduce_PointOutsidePolygon(z3::solver            &Solver,
				   z3::context           &Context,
				   const z3::expr        &dec_var_X1,
				   const z3::expr        &dec_var_Y1,
				   const z3::expr        &dec_var_X2,
				   const z3::expr        &dec_var_Y2,
				   const Slic3r::Polygon &polygon)
{
    if (polygon.points.size() >= 3)
    {  
	z3::expr out_disjunction(Context);
	
	for (unsigned int p = 0; p < polygon.points.size(); ++p)
	{
	    int np = (p + 1) % polygon.points.size();
	    
	    Line line(polygon.points[p], polygon.points[np]);
	    Vector normal = line.normal();
	    
	    z3::expr outside_half_plane(  (normal.x() * dec_var_X1)
				        + (normal.y() * dec_var_Y1)		 
					- (normal.x() * dec_var_X2)
					- (normal.x() * line.a.x())
					- (normal.y() * dec_var_Y2)
					- (normal.y() * line.a.y()) > 0);
	    
	    if (p == 0)
	    {
		out_disjunction = outside_half_plane;
	    }
	    else
	    {
		out_disjunction = out_disjunction || outside_half_plane;
	    }
	}

	Solver.add(out_disjunction);
    }
}


void introduce_SequentialPointOutsidePolygon(z3::solver            &Solver,
					     z3::context           &Context,
					     const z3::expr        &dec_var_X1,
					     const z3::expr        &dec_var_Y1,
					     const z3::expr        &dec_var_T1,
					     const z3::expr        &dec_var_X2,
					     const z3::expr        &dec_var_Y2,
					     const z3::expr        &dec_var_T2,
					     const Slic3r::Polygon &polygon2)
{
    if (polygon2.points.size() >= 3)
    {
	z3::expr out_disjunction(Context);
	
	for (unsigned int p = 0; p < polygon2.points.size(); ++p)
	{
	    int np = (p + 1) % polygon2.points.size();
	    
	    Line line(polygon2.points[p], polygon2.points[np]);
	    Vector normal = line.normal();
	    
	    z3::expr outside_half_plane(  (normal.x() * dec_var_X1)
					+ (normal.y() * dec_var_Y1)		 
					- (normal.x() * dec_var_X2)
					- (normal.x() * line.a.x())
					- (normal.y() * dec_var_Y2)
					- (normal.y() * line.a.y()) > 0);
	    
	    if (p == 0)
	    {
		out_disjunction = (dec_var_T1 < dec_var_T2) || outside_half_plane;
	    }
	    else
	    {
		out_disjunction = out_disjunction || outside_half_plane;
	    }
	}
	
	Solver.add(out_disjunction);
    }
}


void introduce_ConsequentialPointOutsidePolygon(z3::solver            &Solver,
						z3::context           &Context,
						const z3::expr        &dec_var_X1,
						const z3::expr        &dec_var_Y1,
						const z3::expr        &dec_var_T1,
						const z3::expr        &dec_var_X2,
						const z3::expr        &dec_var_Y2,
						const z3::expr        &dec_var_T2,
						const Slic3r::Polygon &polygon2)
{
    if (polygon2.points.size() >= 3)
    {
	z3::expr out_disjunction(Context);
	
	for (unsigned int p = 0; p < polygon2.points.size(); ++p)
	{
	    int np = (p + 1) % polygon2.points.size();
	    
	    Line line(polygon2.points[p], polygon2.points[np]);
	    Vector normal = line.normal();
	    
	    z3::expr outside_half_plane(  (normal.x() * dec_var_X1)
					+ (normal.y() * dec_var_Y1)		 
					- (normal.x() * dec_var_X2)
					- (normal.x() * line.a.x())
					- (normal.y() * dec_var_Y2)
					- (normal.y() * line.a.y()) > 0);
	    
	    if (p == 0)
	    {
		out_disjunction = (dec_var_T1 < 0) || (dec_var_T2 < 0) || (dec_var_T1 < dec_var_T2) || outside_half_plane;
	    }
	    else
	    {
		out_disjunction = out_disjunction || outside_half_plane;
	    }
	}

	Solver.add(out_disjunction);
    }
}


void introduce_ShiftSequentialPointOutsidePolygon(z3::solver            &Solver,
						  z3::context           &Context,
						  int                    x,
						  int                    y,
						  const z3::expr        &dec_var_X1,
						  const z3::expr        &dec_var_Y1,
						  const z3::expr        &dec_var_T1,
						  const z3::expr        &dec_var_X2,
						  const z3::expr        &dec_var_Y2,
						  const z3::expr        &dec_var_T2,
						  const Slic3r::Polygon &polygon2)
{
    if (polygon2.points.size() >= 3)
    {    
	z3::expr out_disjunction(Context);
	
	for (unsigned int p = 0; p < polygon2.points.size(); ++p)
	{
	    int np = (p + 1) % polygon2.points.size();
	    
	    Line line(polygon2.points[p], polygon2.points[np]);
	    Vector normal = line.normal();
	    
	    z3::expr outside_half_plane(  (normal.x() * dec_var_X1) + (normal.x() * x)
					+ (normal.y() * dec_var_Y1) + (normal.y() * y)
					- (normal.x() * dec_var_X2)
					- (normal.x() * line.a.x())
					- (normal.y() * dec_var_Y2)
					- (normal.y() * line.a.y()) > 0);
	    
	    if (p == 0)
	    {
		out_disjunction = (dec_var_T1 < dec_var_T2) || outside_half_plane;
	    }
	    else
	    {
		out_disjunction = out_disjunction || outside_half_plane;
	    }
	}
	
	Solver.add(out_disjunction);
    }
}


void introduce_ShiftConsequentialPointOutsidePolygon(z3::solver            &Solver,
						     z3::context           &Context,
						     int                    x,
						     int                    y,
						     const z3::expr        &dec_var_X1,
						     const z3::expr        &dec_var_Y1,
						     const z3::expr        &dec_var_T1,
						     const z3::expr        &dec_var_X2,
						     const z3::expr        &dec_var_Y2,
						     const z3::expr        &dec_var_T2,
						     const Slic3r::Polygon &polygon2)
{
    if (polygon2.points.size() >= 3)
    {    
	z3::expr out_disjunction(Context);
	
	for (unsigned int p = 0; p < polygon2.points.size(); ++p)
	{
	    int np = (p + 1) % polygon2.points.size();
	    
	    Line line(polygon2.points[p], polygon2.points[np]);
	    Vector normal = line.normal();
	    
	    z3::expr outside_half_plane(  (normal.x() * dec_var_X1) + (normal.x() * x)
					+ (normal.y() * dec_var_Y1) + (normal.y() * y)
					- (normal.x() * dec_var_X2)
					- (normal.x() * line.a.x())
					- (normal.y() * dec_var_Y2)
					- (normal.y() * line.a.y()) > 0);
	    
	    if (p == 0)
	    {
		out_disjunction = (dec_var_T1 < 0) || (dec_var_T2 < 0) || (dec_var_T1 < dec_var_T2) || outside_half_plane;
	    }
	    else
	    {
		out_disjunction = out_disjunction || outside_half_plane;
	    }
	}
	
	Solver.add(out_disjunction);
    }
}


void introduce_FixedPointOutsidePolygon(z3::solver            &Solver,
					z3::context           &Context,
					const Rational        &dec_value_X1,
					const Rational        &dec_value_Y1,					
					const z3::expr        &dec_var_X2,
					const z3::expr        &dec_var_Y2,
					const Slic3r::Polygon &polygon)
{
    if (polygon.points.size() >= 3)
    {   
	z3::expr out_disjunction(Context);
	
	for (unsigned int p = 0; p < polygon.points.size(); ++p)
	{
	    int np = (p + 1) % polygon.points.size();
	    
	    Line line(polygon.points[p], polygon.points[np]);
	    Vector normal = line.normal();
	    
	    z3::expr outside_half_plane(  (normal.x() * Context.real_val(dec_value_X1.numerator, dec_value_X1.denominator))
					+ (normal.y() * Context.real_val(dec_value_Y1.numerator, dec_value_Y1.denominator))
					- (normal.x() * dec_var_X2)
					- (normal.x() * line.a.x())
					- (normal.y() * dec_var_Y2)
					- (normal.y() * line.a.y()) > 0);
	    
	    if (p == 0)
	    {
		out_disjunction = outside_half_plane;
	    }
	    else
	    {
		out_disjunction = out_disjunction || outside_half_plane;
	    }
	}

	Solver.add(out_disjunction);
    }
}


void introduce_SequentialFixedPointOutsidePolygon(z3::solver            &Solver,
						  z3::context           &Context,
						  const Rational        &dec_value_X1,
						  const Rational        &dec_value_Y1,
						  const Rational        &dec_value_T1,
						  const z3::expr        &dec_var_X2,
						  const z3::expr        &dec_var_Y2,
						  const z3::expr        &dec_var_T2,
						  const Slic3r::Polygon &polygon)
{
    if (polygon.points.size() >= 3)
    {
	z3::expr out_disjunction(Context);
	
	for (unsigned int p = 0; p < polygon.points.size(); ++p)
	{
	    int np = (p + 1) % polygon.points.size();
	    
	    Line line(polygon.points[p], polygon.points[np]);
	    Vector normal = line.normal();
	    
	    z3::expr outside_half_plane(  (normal.x() * Context.real_val(dec_value_X1.numerator, dec_value_X1.denominator))
					+ (normal.y() * Context.real_val(dec_value_Y1.numerator, dec_value_Y1.denominator))
					- (normal.x() * dec_var_X2)
					- (normal.x() * line.a.x())
					- (normal.y() * dec_var_Y2)
					- (normal.y() * line.a.y()) > 0);
	    
	    if (p == 0)
	    {
		out_disjunction = (Context.real_val(dec_value_T1.numerator, dec_value_T1.denominator) <  dec_var_T2) || outside_half_plane;
	    }
	    else
	    {
		out_disjunction = out_disjunction || outside_half_plane;
	    }
	}
	
	Solver.add(out_disjunction);
    }
}


void introduce_SequentialFixedPointOutsidePolygon(z3::solver            &Solver,
						  z3::context           &Context,
						  const Rational        &dec_value_X1,
						  const Rational        &dec_value_Y1,
						  const z3::expr        &dec_var_T1,
						  const z3::expr        &dec_var_X2,
						  const z3::expr        &dec_var_Y2,
						  const Rational        &dec_value_T2,
						  const Slic3r::Polygon &polygon)
{
    if (polygon.points.size() >= 3)
    {   
	z3::expr out_disjunction(Context);
	
	for (unsigned int p = 0; p < polygon.points.size(); ++p)
	{
	    int np = (p + 1) % polygon.points.size();    
	    Line line(polygon.points[p], polygon.points[np]);
	    Vector normal = line.normal();
	    
	    z3::expr outside_half_plane(  (normal.x() * Context.real_val(dec_value_X1.numerator, dec_value_X1.denominator))
					+ (normal.y() * Context.real_val(dec_value_Y1.numerator, dec_value_Y1.denominator))
					- (normal.x() * dec_var_X2)
					- (normal.x() * line.a.x())
					- (normal.y() * dec_var_Y2)
					- (normal.y() * line.a.y()) > 0);
	    
	    if (p == 0)
	    {
		out_disjunction = (dec_var_T1 < Context.real_val(dec_value_T2.numerator, dec_value_T2.denominator)) || outside_half_plane;
	    }
	    else
	    {
		out_disjunction = out_disjunction || outside_half_plane;
	    }
	}
	
	Solver.add(out_disjunction);
    }
}


void introduce_ConsequentialFixedPointOutsidePolygon(z3::solver            &Solver,
						     z3::context           &Context,
						     const Rational        &dec_value_X1,
						     const Rational        &dec_value_Y1,
						     const Rational        &dec_value_T1,
						     const z3::expr        &dec_var_X2,
						     const z3::expr        &dec_var_Y2,
						     const z3::expr        &dec_var_T2,
						     const Slic3r::Polygon &polygon)
{
    if (dec_value_T1.is_Positive())
    {
	if (polygon.points.size() >= 3)
	{
	    z3::expr out_disjunction(Context);
	    
	    for (unsigned int p = 0; p < polygon.points.size(); ++p)
	    {
		int np = (p + 1) % polygon.points.size();
		
		Line line(polygon.points[p], polygon.points[np]);
		Vector normal = line.normal();
		
		z3::expr outside_half_plane(  (normal.x() * Context.real_val(dec_value_X1.numerator, dec_value_X1.denominator))
					    + (normal.y() * Context.real_val(dec_value_Y1.numerator, dec_value_Y1.denominator))
					    - (normal.x() * dec_var_X2)
					    - (normal.x() * line.a.x())
					    - (normal.y() * dec_var_Y2)
					    - (normal.y() * line.a.y()) > 0);
	    
		if (p == 0)
		{
		    out_disjunction = (dec_var_T2 < 0) || (Context.real_val(dec_value_T1.numerator, dec_value_T1.denominator) <  dec_var_T2) || outside_half_plane;
		}
		else
		{
		    out_disjunction = out_disjunction || outside_half_plane;
		}
	    }
	    
	    Solver.add(out_disjunction);
	}
    }
}


void introduce_ConsequentialFixedPointOutsidePolygon(z3::solver            &Solver,
						     z3::context           &Context,
						     const Rational        &dec_value_X1,
						     const Rational        &dec_value_Y1,
						     const z3::expr        &dec_var_T1,
						     const z3::expr        &dec_var_X2,
						     const z3::expr        &dec_var_Y2,
						     const Rational        &dec_value_T2,
						     const Slic3r::Polygon &polygon)
{
    if (dec_value_T2.is_Positive())
    {
	if (polygon.points.size() >= 3)
	{   
	    z3::expr out_disjunction(Context);
	    
	    for (unsigned int p = 0; p < polygon.points.size(); ++p)
	    {
		int np = (p + 1) % polygon.points.size();    
		Line line(polygon.points[p], polygon.points[np]);
		Vector normal = line.normal();
		
		z3::expr outside_half_plane(  (normal.x() * Context.real_val(dec_value_X1.numerator, dec_value_X1.denominator))
					    + (normal.y() * Context.real_val(dec_value_Y1.numerator, dec_value_Y1.denominator))
					    - (normal.x() * dec_var_X2)
					    - (normal.x() * line.a.x())
					    - (normal.y() * dec_var_Y2)
					    - (normal.y() * line.a.y()) > 0);
	    
		if (p == 0)
		{
		    out_disjunction = (dec_var_T1 < 0) || (dec_var_T1 < Context.real_val(dec_value_T2.numerator, dec_value_T2.denominator)) || outside_half_plane;
		}
		else
		{
		    out_disjunction = out_disjunction || outside_half_plane;
		}
	    }
	
	    Solver.add(out_disjunction);
	}
    }
}


void introduce_PointOutsideFixedPolygon(z3::solver            &Solver,
					z3::context           &Context,
					const z3::expr        &dec_var_X1,
					const z3::expr        &dec_var_Y1,
					const Rational        &dec_value_X2,
					const Rational        &dec_value_Y2,
					const Slic3r::Polygon &polygon)
{
    if (polygon.points.size() >= 3)
    {    
	z3::expr out_disjunction(Context);

	for (unsigned int p = 0; p < polygon.points.size(); ++p)
	{
	    int np = (p + 1) % polygon.points.size();
	    
	    Line line(polygon.points[p], polygon.points[np]);
	    Vector normal = line.normal();
	    
	    z3::expr outside_half_plane(  (normal.x() * dec_var_X1)
					+ (normal.y() * dec_var_Y1)		 
					- (normal.x() * Context.real_val(dec_value_X2.numerator, dec_value_X2.denominator))
					- (normal.x() * line.a.x())
					- (normal.y() * Context.real_val(dec_value_Y2.numerator, dec_value_Y2.denominator))
					- (normal.y() * line.a.y()) > 0);
	    
	    if (p == 0)
	    {
		out_disjunction = outside_half_plane;
	    }
	    else
	    {
		out_disjunction = out_disjunction || outside_half_plane;
	    }
	}
	
	Solver.add(out_disjunction);
    }
}


void introduce_SequentialPointOutsideFixedPolygon(z3::solver            &Solver,
						  z3::context           &Context,
						  const z3::expr        &dec_var_X1,
						  const z3::expr        &dec_var_Y1,
						  const z3::expr        &dec_var_T1,
						  const Rational        &dec_value_X2,
						  const Rational        &dec_value_Y2,
						  const Rational        &dec_value_T2,
						  const Slic3r::Polygon &polygon)
{
    if (polygon.points.size() >= 3)
    {   
	z3::expr out_disjunction(Context);
	
	for (unsigned int p = 0; p < polygon.points.size(); ++p)
	{
	    int np = (p + 1) % polygon.points.size();
	    
	    Line line(polygon.points[p], polygon.points[np]);
	    Vector normal = line.normal();
	    
	    z3::expr outside_half_plane(  (normal.x() * dec_var_X1)
					+ (normal.y() * dec_var_Y1)		 
					- (normal.x() * Context.real_val(dec_value_X2.numerator, dec_value_X2.denominator))
					- (normal.x() * line.a.x())
					- (normal.y() * Context.real_val(dec_value_Y2.numerator, dec_value_Y2.denominator))
					- (normal.y() * line.a.y()) > 0);
	    
	    if (p == 0)
	    {
		out_disjunction = (dec_var_T1 < Context.real_val(dec_value_T2.numerator, dec_value_T2.denominator)) || outside_half_plane;
	    }
	    else
	    {
		out_disjunction = out_disjunction || outside_half_plane;
	    }
	}
	
	Solver.add(out_disjunction);
    }
}


void introduce_SequentialPointOutsideFixedPolygon(z3::solver            &Solver,
						  z3::context           &Context,
						  const z3::expr        &dec_var_X1,
						  const z3::expr        &dec_var_Y1,
						  const Rational        &dec_value_T1,
						  const Rational        &dec_value_X2,
						  const Rational        &dec_value_Y2,
						  const z3::expr        &dec_var_T2,
						  const Slic3r::Polygon &polygon)
{
    if (polygon.points.size() >= 3)
    {    
	z3::expr out_disjunction(Context);
	
	for (unsigned int p = 0; p < polygon.points.size(); ++p)
	{
	    int np = (p + 1) % polygon.points.size();
	    
	    Line line(polygon.points[p], polygon.points[np]);
	    Vector normal = line.normal();
	    
	    z3::expr outside_half_plane(  (normal.x() * dec_var_X1)
					+ (normal.y() * dec_var_Y1)		 
					- (normal.x() * Context.real_val(dec_value_X2.numerator, dec_value_X2.denominator))
					- (normal.x() * line.a.x())
					- (normal.y() * Context.real_val(dec_value_Y2.numerator, dec_value_Y2.denominator))
					- (normal.y() * line.a.y()) > 0);
	    
	    if (p == 0)
	    {
		out_disjunction = (Context.real_val(dec_value_T1.numerator, dec_value_T1.denominator) <  dec_var_T2) || outside_half_plane;
	    }
	    else
	    {
		out_disjunction = out_disjunction || outside_half_plane;
	    }
	}

	Solver.add(out_disjunction);
    }
}


void introduce_ConsequentialPointOutsideFixedPolygon(z3::solver            &Solver,
						     z3::context           &Context,
						     const z3::expr        &dec_var_X1,
						     const z3::expr        &dec_var_Y1,
						     const z3::expr        &dec_var_T1,
						     const Rational        &dec_value_X2,
						     const Rational        &dec_value_Y2,
						     const Rational        &dec_value_T2,
						     const Slic3r::Polygon &polygon)
{
    if (dec_value_T2.is_Positive())
    {
	if (polygon.points.size() >= 3)
	{   
	    z3::expr out_disjunction(Context);
	    
	    for (unsigned int p = 0; p < polygon.points.size(); ++p)
	    {
		int np = (p + 1) % polygon.points.size();
		
		Line line(polygon.points[p], polygon.points[np]);
		Vector normal = line.normal();
		
		z3::expr outside_half_plane(  (normal.x() * dec_var_X1)
					    + (normal.y() * dec_var_Y1)		 
					    - (normal.x() * Context.real_val(dec_value_X2.numerator, dec_value_X2.denominator))
					    - (normal.x() * line.a.x())
					    - (normal.y() * Context.real_val(dec_value_Y2.numerator, dec_value_Y2.denominator))
					    - (normal.y() * line.a.y()) > 0);
	    
		if (p == 0)
		{
		    out_disjunction = (dec_var_T1 < 0) || (dec_var_T1 < Context.real_val(dec_value_T2.numerator, dec_value_T2.denominator)) || outside_half_plane;
		}
		else
		{
		    out_disjunction = out_disjunction || outside_half_plane;
		}
	    }
	    
	    Solver.add(out_disjunction);
	}
    }
}


void introduce_ConsequentialPointOutsideFixedPolygon(z3::solver            &Solver,
						     z3::context           &Context,
						     const z3::expr        &dec_var_X1,
						     const z3::expr        &dec_var_Y1,
						     const Rational        &dec_value_T1,
						     const Rational        &dec_value_X2,
						     const Rational        &dec_value_Y2,
						     const z3::expr        &dec_var_T2,
						     const Slic3r::Polygon &polygon)
{
    if (dec_value_T1.is_Positive())
    {
	if (polygon.points.size() >= 3)
	{    
	    z3::expr out_disjunction(Context);
	    
	    for (unsigned int p = 0; p < polygon.points.size(); ++p)
	    {
		int np = (p + 1) % polygon.points.size();
		
		Line line(polygon.points[p], polygon.points[np]);
		Vector normal = line.normal();
		
		z3::expr outside_half_plane(  (normal.x() * dec_var_X1)
					    + (normal.y() * dec_var_Y1)		 
					    - (normal.x() * Context.real_val(dec_value_X2.numerator, dec_value_X2.denominator))
					    - (normal.x() * line.a.x())
					    - (normal.y() * Context.real_val(dec_value_Y2.numerator, dec_value_Y2.denominator))
					    - (normal.y() * line.a.y()) > 0);
	    
		if (p == 0)
		{
		    out_disjunction = (dec_var_T2 < 0) || (Context.real_val(dec_value_T1.numerator, dec_value_T1.denominator) <  dec_var_T2) || outside_half_plane;
		}
		else
		{
		    out_disjunction = out_disjunction || outside_half_plane;
		}
	    }
	    
	    Solver.add(out_disjunction);
	}
    }
}

void introduce_PolygonLineNonIntersection(z3::solver            &Solver,
					  z3::context           &Context,
					  const z3::expr        &dec_var_X1,
					  const z3::expr        &dec_var_Y1,
					  const Slic3r::Polygon &polygon1,
					  const z3::expr        &dec_var_X2,
					  const z3::expr        &dec_var_Y2,
					  const Slic3r::Polygon &polygon2)
{
    for (unsigned int p1 = 0; p1 < polygon1.points.size(); ++p1)
    {
	const Point &point1 = polygon1.points[p1];
	const Point &next_point1 = polygon1.points[(p1 + 1) % polygon1.points.size()];

	for (unsigned int p2 = 0; p2 < polygon2.points.size(); ++p2)
	{
	    const Point &point2 = polygon2.points[p2];
	    const Point &next_point2 = polygon2.points[(p2 + 1) % polygon2.points.size()];

	    introduce_LineNonIntersection(Solver,
					  Context,
					  dec_var_X1,
					  dec_var_Y1,
					  z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
					  Line(point1, next_point1),
					  dec_var_X2,
					  dec_var_Y2,
					  z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
					  Line(point2, next_point2));
	    hidden_var_cnt += 2;
	}
    }
}


void introduce_PolygonOutsidePolygon(z3::solver            &Solver,
				     z3::context           &Context,				     
				     const z3::expr        &dec_var_X1,
				     const z3::expr        &dec_var_Y1,
				     const Slic3r::Polygon &polygon1,
				     const z3::expr        &dec_var_X2,
				     const z3::expr        &dec_var_Y2,
				     const Slic3r::Polygon &polygon2)
{
    for (unsigned int p1 = 0; p1 < polygon1.points.size(); ++p1)
    {
	const Point &point1 = polygon1.points[p1];

	introduce_PointOutsidePolygon(Solver,
				      Context,
				      dec_var_X1 + point1.x(),
				      dec_var_Y1 + point1.y(),
				      dec_var_X2,
				      dec_var_Y2,
				      polygon2);
    }

    for (unsigned int p2 = 0; p2 < polygon2.points.size(); ++p2)
    {
	const Point &point2 = polygon2.points[p2];

	introduce_PointOutsidePolygon(Solver,
				      Context,
				      dec_var_X2 + point2.x(),
				      dec_var_Y2 + point2.y(),
				      dec_var_X1,
				      dec_var_Y1,
				      polygon1);
    }
}


void introduce_PolygonOutsideFixedPolygon(z3::solver            &Solver,
					  z3::context           &Context,				     
					  const z3::expr        &dec_var_X1,
					  const z3::expr        &dec_var_Y1,
					  const Slic3r::Polygon &polygon1,
					  const Rational        &dec_value_X2,
					  const Rational        &dec_value_Y2,				     
					  const Slic3r::Polygon &polygon2)
{
    for (unsigned int p1 = 0; p1 < polygon1.points.size(); ++p1)
    {
	const Point &point1 = polygon1.points[p1];

	introduce_PointOutsideFixedPolygon(Solver,
					   Context,
					   dec_var_X1 + point1.x(),
					   dec_var_Y1 + point1.y(),
					   dec_value_X2,
					   dec_value_Y2,
					   polygon2);
    }

    for (unsigned int p2 = 0; p2 < polygon2.points.size(); ++p2)
    {
	const Point &point2 = polygon2.points[p2];

	introduce_FixedPointOutsidePolygon(Solver,
					   Context,
					   dec_value_X2 + point2.x(),
					   dec_value_Y2 + point2.y(),
					   dec_var_X1,
					   dec_var_Y1,
					   polygon1);
    }
}


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
					       const Slic3r::Polygon &unreachable_polygon2)
{
    std::vector<Slic3r::Polygon> _unreachable_polygons1;
    _unreachable_polygons1.push_back(unreachable_polygon1);

    std::vector<Slic3r::Polygon> _unreachable_polygons2;
    _unreachable_polygons2.push_back(unreachable_polygon2);

    introduce_SequentialPolygonOutsidePolygon(Solver,
					      Context,
					      dec_var_X1,
					      dec_var_Y1,
					      dec_var_T1,
					      polygon1,
					      _unreachable_polygons1,
					      dec_var_X2,
					      dec_var_Y2,
					      dec_var_T2,
					      polygon2,
					      _unreachable_polygons2);
}


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
					       const std::vector<Slic3r::Polygon> &unreachable_polygons2)
{
//    Solver.add(dec_var_T1 < dec_var_T2);

    #ifdef DEBUG
    {
	printf("polygon1:\n");
	for (unsigned int p1 = 0; p1 < polygon1.points.size(); ++p1)
	{
	    printf("[%d,%d] ", polygon1.points[p1].x(), polygon1.points[p1].y());
	}
	printf("\n");

	for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
	{
	    printf("pro_polygon1 %d:\n", poly1);
	    for (unsigned int p1 = 0; p1 < unreachable_polygons1[poly1].points.size(); ++p1)
	    {
		printf("[%d,%d] ", unreachable_polygons1[poly1].points[p1].x(), unreachable_polygons1[poly1].points[p1].y());
	    }
	    printf("\n");
	}
	printf("\n");    

	printf("polygon2:\n");
	for (unsigned int p2 = 0; p2 < polygon2.points.size(); ++p2)
	{
	    printf("[%d,%d] ", polygon2.points[p2].x(), polygon2.points[p2].y());
	}
	printf("\n");
    
	for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
	{
	    printf("pro_polygon2 %d:\n", poly2);
	    for (unsigned int p2 = 0; p2 < unreachable_polygons2[poly2].points.size(); ++p2)
	    {
		printf("[%d,%d] ", unreachable_polygons2[poly2].points[p2].x(), unreachable_polygons2[poly2].points[p2].y());
	    }
	    printf("\n");
	}    
	printf("\n");
    }
    #endif

    
    for (unsigned int p1 = 0; p1 < polygon1.points.size(); ++p1)
    {
	const Point &point1 = polygon1.points[p1];

	for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
	{
	    introduce_SequentialPointOutsidePolygon(Solver,
						    Context,
						    dec_var_X1 + point1.x(),
						    dec_var_Y1 + point1.y(),
						    dec_var_T1,
						    dec_var_X2,
						    dec_var_Y2,
					  	    dec_var_T2,
						    unreachable_polygons2[poly2]);
	}
    }
    
    for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
    {
	for (unsigned int p2 = 0; p2 < unreachable_polygons2[poly2].points.size(); ++p2)
	{
	    const Point &pro_point2 = unreachable_polygons2[poly2].points[p2];

	    introduce_SequentialPointOutsidePolygon(Solver,
						    Context,
						    dec_var_X2 + pro_point2.x(),
						    dec_var_Y2 + pro_point2.y(),
						    dec_var_T1,
						    dec_var_X1,
						    dec_var_Y1,
						    dec_var_T2,
						    polygon1);	
	}
    }

    for (unsigned int p2 = 0; p2 < polygon2.points.size(); ++p2)
    {
	const Point &point2 = polygon2.points[p2];

	for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
	{
	    /*
	    introduce_ShiftSequentialPointOutsidePolygon(Solver,
							 Context,
							 point2.x(),
							 point2.y(),
							 dec_var_X2,
							 dec_var_Y2,
							 dec_var_T2,				      
							 dec_var_X1,
							 dec_var_Y1,
							 dec_var_T1,
							 polygon1);						    
							 unreachable_polygons1[poly1]);	    
	    */ 
	    introduce_SequentialPointOutsidePolygon(Solver,
						    Context,
						    dec_var_X2 + point2.x(),
						    dec_var_Y2 + point2.y(),
						    dec_var_T2,				      
						    dec_var_X1,
						    dec_var_Y1,
						    dec_var_T1,
						    unreachable_polygons1[poly1]);
	}
    }

    for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
    {
	for (unsigned int p1 = 0; p1 < unreachable_polygons1[poly1].points.size(); ++p1)
	{
	    const Point &pro_point1 = unreachable_polygons1[poly1].points[p1];

	    introduce_SequentialPointOutsidePolygon(Solver,
						    Context,
						    dec_var_X1 + pro_point1.x(),
						    dec_var_Y1 + pro_point1.y(),
						    dec_var_T2,
						    dec_var_X2,
						    dec_var_Y2,
						    dec_var_T1,
						    polygon2);
	}
    }
/*
    for (unsigned int p1 = 0; p1 < polygon1.points.size(); ++p1)
    {
	const Point &point1 = polygon1.points[p1];

	introduce_PointOutsidePolygon(Solver,
				      Context,
				      dec_var_X1 + point1.x(),
				      dec_var_Y1 + point1.y(),
				      dec_var_X2,
				      dec_var_Y2,
				      polygon2);
    }
*/
/*  
    for (unsigned int p2 = 0; p2 < polygon2.points.size(); ++p2)
    {
	const Point &point2 = polygon2.points[p2];

	introduce_PointOutsidePolygon(Solver,
				      Context,
				      dec_var_X2 + point2.x(),
				      dec_var_Y2 + point2.y(),
				      dec_var_X1,
				      dec_var_Y1,
				      polygon1);
    }
*/
}



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
						    const Slic3r::Polygon &unreachable_polygon2)
{
    std::vector<Slic3r::Polygon> _unreachable_polygons1;
    _unreachable_polygons1.push_back(unreachable_polygon1);

    std::vector<Slic3r::Polygon> _unreachable_polygons2;
    _unreachable_polygons2.push_back(unreachable_polygon2);

    introduce_SequentialPolygonOutsideFixedPolygon(Solver,
						   Context,
						   dec_var_X1,
						   dec_var_Y1,
						   dec_var_T1,
						   polygon1,
						   _unreachable_polygons1,
						   dec_value_X2,
						   dec_value_Y2,
						   dec_value_T2,
						   polygon2,
						   _unreachable_polygons2);
}


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
						    const std::vector<Slic3r::Polygon> &unreachable_polygons2)
{
    for (unsigned int p1 = 0; p1 < polygon1.points.size(); ++p1)
    {
	const Point &point1 = polygon1.points[p1];

	for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
	{
	    introduce_SequentialPointOutsideFixedPolygon(Solver,
							 Context,
							 dec_var_X1 + point1.x(),
							 dec_var_Y1 + point1.y(),
							 dec_var_T1,
							 dec_value_X2,
							 dec_value_Y2,
							 dec_value_T2,				      
							 unreachable_polygons2[poly2]);
	}
    }

    for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
    {
	for (unsigned int p2 = 0; p2 < unreachable_polygons2[poly2].points.size(); ++p2)
	{
	    const Point &pro_point2 = unreachable_polygons2[poly2].points[p2];

	    introduce_SequentialFixedPointOutsidePolygon(Solver,
							 Context,
							 dec_value_X2 + pro_point2.x(),
							 dec_value_Y2 + pro_point2.y(),
							 dec_var_T1,
							 dec_var_X1,
							 dec_var_Y1,
							 dec_value_T2,
							 polygon1);	
	}
    }

    for (unsigned int p2 = 0; p2 < polygon2.points.size(); ++p2)
    {
	const Point &point2 = polygon2.points[p2];

	for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
	{
	    introduce_SequentialFixedPointOutsidePolygon(Solver,
							 Context,
							 dec_value_X2 + point2.x(),
							 dec_value_Y2 + point2.y(),
							 dec_value_T2,				      
							 dec_var_X1,
							 dec_var_Y1,
							 dec_var_T1,
							 unreachable_polygons1[poly1]);
	}
    }

    for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
    {
	for (unsigned int p1 = 0; p1 < unreachable_polygons1[poly1].points.size(); ++p1)
	{
	    const Point &pro_point1 = unreachable_polygons1[poly1].points[p1];

	    introduce_SequentialPointOutsideFixedPolygon(Solver,
							 Context,
							 dec_var_X1 + pro_point1.x(),
							 dec_var_Y1 + pro_point1.y(),
							 dec_value_T2,				      
							 dec_value_X2,
							 dec_value_Y2,
							 dec_var_T1,
							 polygon2);
	}
    }

/*
    for (unsigned int p1 = 0; p1 < polygon1.points.size(); ++p1)
    {
	const Point &point1 = polygon1.points[p1];

	introduce_PointOutsideFixedPolygon(Solver,
					   Context,
					   dec_var_X1 + point1.x(),
					   dec_var_Y1 + point1.y(),
					   dec_value_X2,
					   dec_value_Y2,
					   polygon2);
    }
 
    for (unsigned int p2 = 0; p2 < polygon2.points.size(); ++p2)
    {
	const Point &point2 = polygon2.points[p2];

	printf("c: %.3f, %.3f\n", dec_value_X2.as_double(), dec_value_Y2.as_double());
	printf("   %.3f, %.3f\n", (dec_value_X2 + point2.x()).as_double(), (dec_value_Y2 + point2.y()).as_double());

	introduce_FixedPointOutsidePolygon(Solver,
					   Context,
					   dec_value_X2 + point2.x(),
					   dec_value_Y2 + point2.y(),
					   dec_var_X1,
					   dec_var_Y1,
					   polygon1);
    }
*/
}


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
						  const Slic3r::Polygon &unreachable_polygon2)
{
    std::vector<Slic3r::Polygon> _unreachable_polygons1;
    _unreachable_polygons1.push_back(unreachable_polygon1);

    std::vector<Slic3r::Polygon> _unreachable_polygons2;
    _unreachable_polygons2.push_back(unreachable_polygon2);

    introduce_ConsequentialPolygonOutsidePolygon(Solver,
						 Context,
						 dec_var_X1,
						 dec_var_Y1,
						 dec_var_T1,
						 polygon1,
						 _unreachable_polygons1,
						 dec_var_X2,
						 dec_var_Y2,
						 dec_var_T2,
						 polygon2,
						 _unreachable_polygons2);
}


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
						  const std::vector<Slic3r::Polygon> &unreachable_polygons2)
{
    #ifdef DEBUG
    {
	printf("polygon1:\n");
	for (unsigned int p1 = 0; p1 < polygon1.points.size(); ++p1)
	{
	    printf("[%d,%d] ", polygon1.points[p1].x(), polygon1.points[p1].y());
	}
	printf("\n");

	for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
	{
	    printf("pro_polygon1 %d:\n", poly1);
	    for (unsigned int p1 = 0; p1 < unreachable_polygons1[poly1].points.size(); ++p1)
	    {
		printf("[%d,%d] ", unreachable_polygons1[poly1].points[p1].x(), unreachable_polygons1[poly1].points[p1].y());
	    }
	    printf("\n");
	}
	printf("\n");    

	printf("polygon2:\n");
	for (unsigned int p2 = 0; p2 < polygon2.points.size(); ++p2)
	{
	    printf("[%d,%d] ", polygon2.points[p2].x(), polygon2.points[p2].y());
	}
	printf("\n");
    
	for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
	{
	    printf("pro_polygon2 %d:\n", poly2);
	    for (unsigned int p2 = 0; p2 < unreachable_polygons2[poly2].points.size(); ++p2)
	    {
		printf("[%d,%d] ", unreachable_polygons2[poly2].points[p2].x(), unreachable_polygons2[poly2].points[p2].y());
	    }
	    printf("\n");
	}    
	printf("\n");
    }
    #endif

    
    for (unsigned int p1 = 0; p1 < polygon1.points.size(); ++p1)
    {
	const Point &point1 = polygon1.points[p1];

	for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
	{
	    introduce_ConsequentialPointOutsidePolygon(Solver,
						       Context,
						       dec_var_X1 + point1.x(),
						       dec_var_Y1 + point1.y(),
						       dec_var_T1,
						       dec_var_X2,
						       dec_var_Y2,
						       dec_var_T2,
						       unreachable_polygons2[poly2]);
	}
    }
    
    for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
    {
	for (unsigned int p2 = 0; p2 < unreachable_polygons2[poly2].points.size(); ++p2)
	{
	    const Point &pro_point2 = unreachable_polygons2[poly2].points[p2];

	    introduce_ConsequentialPointOutsidePolygon(Solver,
						       Context,
						       dec_var_X2 + pro_point2.x(),
						       dec_var_Y2 + pro_point2.y(),
						       dec_var_T1,
						       dec_var_X1,
						       dec_var_Y1,
						       dec_var_T2,
						       polygon1);	
	}
    }

    for (unsigned int p2 = 0; p2 < polygon2.points.size(); ++p2)
    {
	const Point &point2 = polygon2.points[p2];

	for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
	{
	    introduce_ConsequentialPointOutsidePolygon(Solver,
						       Context,
						       dec_var_X2 + point2.x(),
						       dec_var_Y2 + point2.y(),
						       dec_var_T2,				      
						       dec_var_X1,
						       dec_var_Y1,
						       dec_var_T1,
						       unreachable_polygons1[poly1]);
	}
    }

    for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
    {
	for (unsigned int p1 = 0; p1 < unreachable_polygons1[poly1].points.size(); ++p1)
	{
	    const Point &pro_point1 = unreachable_polygons1[poly1].points[p1];

	    introduce_ConsequentialPointOutsidePolygon(Solver,
						       Context,
						       dec_var_X1 + pro_point1.x(),
						       dec_var_Y1 + pro_point1.y(),
						       dec_var_T2,
						       dec_var_X2,
						       dec_var_Y2,
						       dec_var_T1,
						       polygon2);
	}
    }
}


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
						   const Slic3r::Polygon &unreachable_polygon2)
{
    std::vector<Slic3r::Polygon> _unreachable_polygons1;
    _unreachable_polygons1.push_back(unreachable_polygon1);

    std::vector<Slic3r::Polygon> _unreachable_polygons2;
    _unreachable_polygons2.push_back(unreachable_polygon2);

    introduce_ConsequentialPolygonExternalPolygon(Solver,
						  Context,
						  dec_var_X1,
						  dec_var_Y1,
						  dec_var_T1,
						  polygon1,
						  _unreachable_polygons1,
						  dec_var_X2,
						  dec_var_Y2,
						  dec_var_T2,
						  polygon2,
						  _unreachable_polygons2);
}


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
						   const std::vector<Slic3r::Polygon> &unreachable_polygons2)
{
    for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
    {
	if (unreachable_polygons2[poly2].area() > polygon1.area())
	{	    
	    for (unsigned int p1 = 0; p1 < polygon1.points.size(); ++p1)
	    {
		const Point &point1 = polygon1.points[p1];
		
		introduce_ConsequentialPointOutsidePolygon(Solver,
							   Context,
							   dec_var_X1 + point1.x(),
							   dec_var_Y1 + point1.y(),
							   dec_var_T1,
							   dec_var_X2,
							   dec_var_Y2,
							   dec_var_T2,
							   unreachable_polygons2[poly2]);
	    }
	}
    }
    
    for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
    {
	if (unreachable_polygons2[poly2].area() < polygon1.area())
	{	    	    	
	    for (unsigned int p2 = 0; p2 < unreachable_polygons2[poly2].points.size(); ++p2)
	    {
		const Point &pro_point2 = unreachable_polygons2[poly2].points[p2];
		
		introduce_ConsequentialPointOutsidePolygon(Solver,
							   Context,
							   dec_var_X2 + pro_point2.x(),
							   dec_var_Y2 + pro_point2.y(),
							   dec_var_T1,
							   dec_var_X1,
							   dec_var_Y1,
							   dec_var_T2,
							   polygon1);	
	    }
	}
    }

    for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
    {
	if (unreachable_polygons1[poly1].area() > polygon2.area())
	{	    	    		    	    
	    for (unsigned int p2 = 0; p2 < polygon2.points.size(); ++p2)
	    {
		const Point &point2 = polygon2.points[p2];
		
		introduce_ConsequentialPointOutsidePolygon(Solver,
							   Context,
							   dec_var_X2 + point2.x(),
							   dec_var_Y2 + point2.y(),
							   dec_var_T2,				      
							   dec_var_X1,
							   dec_var_Y1,
							   dec_var_T1,
							   unreachable_polygons1[poly1]);
	    }
	}
    }

    for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
    {
	if (unreachable_polygons1[poly1].area() < polygon2.area())
	{	    	    		    	    	
	    for (unsigned int p1 = 0; p1 < unreachable_polygons1[poly1].points.size(); ++p1)
	    {
		const Point &pro_point1 = unreachable_polygons1[poly1].points[p1];
		
		introduce_ConsequentialPointOutsidePolygon(Solver,
							   Context,
							   dec_var_X1 + pro_point1.x(),
							   dec_var_Y1 + pro_point1.y(),
							   dec_var_T2,
							   dec_var_X2,
							   dec_var_Y2,
							   dec_var_T1,
							   polygon2);
	    }
	}
    }
}


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
						       const Slic3r::Polygon &unreachable_polygon2)
{
    std::vector<Slic3r::Polygon> _unreachable_polygons1;
    _unreachable_polygons1.push_back(unreachable_polygon1);

    std::vector<Slic3r::Polygon> _unreachable_polygons2;
    _unreachable_polygons2.push_back(unreachable_polygon2);

    introduce_ConsequentialPolygonOutsideFixedPolygon(Solver,
						      Context,
						      dec_var_X1,
						      dec_var_Y1,
						      dec_var_T1,
						      polygon1,
						      _unreachable_polygons1,
						      dec_value_X2,
						      dec_value_Y2,
						      dec_value_T2,
						      polygon2,
						      _unreachable_polygons2);
}


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
						       const std::vector<Slic3r::Polygon> &unreachable_polygons2)
{
    for (unsigned int p1 = 0; p1 < polygon1.points.size(); ++p1)
    {
	const Point &point1 = polygon1.points[p1];

	for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
	{
	    introduce_ConsequentialPointOutsideFixedPolygon(Solver,
							    Context,
							    dec_var_X1 + point1.x(),
							    dec_var_Y1 + point1.y(),
							    dec_var_T1,
							    dec_value_X2,
							    dec_value_Y2,
							    dec_value_T2,				      
							    unreachable_polygons2[poly2]);
	}
    }

    for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
    {
	for (unsigned int p2 = 0; p2 < unreachable_polygons2[poly2].points.size(); ++p2)
	{
	    const Point &pro_point2 = unreachable_polygons2[poly2].points[p2];

	    introduce_ConsequentialFixedPointOutsidePolygon(Solver,
							    Context,
							    dec_value_X2 + pro_point2.x(),
							    dec_value_Y2 + pro_point2.y(),
							    dec_var_T1,
							    dec_var_X1,
							    dec_var_Y1,
							    dec_value_T2,
							    polygon1);	
	}
    }

    for (unsigned int p2 = 0; p2 < polygon2.points.size(); ++p2)
    {
	const Point &point2 = polygon2.points[p2];

	for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
	{
	    introduce_ConsequentialFixedPointOutsidePolygon(Solver,
							    Context,
							    dec_value_X2 + point2.x(),
							    dec_value_Y2 + point2.y(),
							    dec_value_T2,				      
							    dec_var_X1,
							    dec_var_Y1,
							    dec_var_T1,
							    unreachable_polygons1[poly1]);
	}
    }

    for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
    {
	for (unsigned int p1 = 0; p1 < unreachable_polygons1[poly1].points.size(); ++p1)
	{
	    const Point &pro_point1 = unreachable_polygons1[poly1].points[p1];

	    introduce_ConsequentialPointOutsideFixedPolygon(Solver,
							    Context,
							    dec_var_X1 + pro_point1.x(),
							    dec_var_Y1 + pro_point1.y(),
							    dec_value_T2,				      
							    dec_value_X2,
							    dec_value_Y2,
							    dec_var_T1,
							    polygon2);
	}
    }
}



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
							const Slic3r::Polygon &unreachable_polygon2)
{
    std::vector<Slic3r::Polygon> _unreachable_polygons1;
    _unreachable_polygons1.push_back(unreachable_polygon1);

    std::vector<Slic3r::Polygon> _unreachable_polygons2;
    _unreachable_polygons2.push_back(unreachable_polygon2);

    introduce_ConsequentialPolygonExternalFixedPolygon(Solver,
						       Context,
						       dec_var_X1,
						       dec_var_Y1,
						       dec_var_T1,
						       polygon1,
						       _unreachable_polygons1,
						       dec_value_X2,
						       dec_value_Y2,
						       dec_value_T2,
						       polygon2,
						       _unreachable_polygons2);
}


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
							const std::vector<Slic3r::Polygon> &unreachable_polygons2)
{
    for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
    {
	if (unreachable_polygons2[poly2].area() > polygon1.area())
	{    
	    for (unsigned int p1 = 0; p1 < polygon1.points.size(); ++p1)
	    {
		const Point &point1 = polygon1.points[p1];
		
		introduce_ConsequentialPointOutsideFixedPolygon(Solver,
								Context,
								dec_var_X1 + point1.x(),
								dec_var_Y1 + point1.y(),
								dec_var_T1,
								dec_value_X2,
								dec_value_Y2,
								dec_value_T2,				      
								unreachable_polygons2[poly2]);
	    }
	}
    }

    for (unsigned int poly2 = 0; poly2 < unreachable_polygons2.size(); ++poly2)
    {
	if (unreachable_polygons2[poly2].area() < polygon1.area())
	{    	    	
	    for (unsigned int p2 = 0; p2 < unreachable_polygons2[poly2].points.size(); ++p2)
	    {
		const Point &pro_point2 = unreachable_polygons2[poly2].points[p2];
		
		introduce_ConsequentialFixedPointOutsidePolygon(Solver,
								Context,
								dec_value_X2 + pro_point2.x(),
								dec_value_Y2 + pro_point2.y(),
								dec_var_T1,
								dec_var_X1,
								dec_var_Y1,
								dec_value_T2,
								polygon1);
	    }
	}
    }

    for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
    {
	if (unreachable_polygons1[poly1].area() > polygon2.area())
	{    	    		        
	    for (unsigned int p2 = 0; p2 < polygon2.points.size(); ++p2)
	    {
		const Point &point2 = polygon2.points[p2];
		
		introduce_ConsequentialFixedPointOutsidePolygon(Solver,
								Context,
								dec_value_X2 + point2.x(),
								dec_value_Y2 + point2.y(),
								dec_value_T2,				      
								dec_var_X1,
								dec_var_Y1,
								dec_var_T1,
								unreachable_polygons1[poly1]);
	    }
	}
    }

    for (unsigned int poly1 = 0; poly1 < unreachable_polygons1.size(); ++poly1)
    {
	if (unreachable_polygons1[poly1].area() < polygon2.area())
	{    	    		        	
	    for (unsigned int p1 = 0; p1 < unreachable_polygons1[poly1].points.size(); ++p1)
	    {
		const Point &pro_point1 = unreachable_polygons1[poly1].points[p1];
		
		introduce_ConsequentialPointOutsideFixedPolygon(Solver,
								Context,
								dec_var_X1 + pro_point1.x(),
								dec_var_Y1 + pro_point1.y(),
								dec_value_T2,				      
								dec_value_X2,
								dec_value_Y2,
								dec_var_T1,
								polygon2);
	    }
	}
    }
}


/*----------------------------------------------------------------*/

void introduce_PolygonWeakNonoverlapping(z3::solver                         &Solver,
					 z3::context                        &Context,
					 const z3::expr_vector              &dec_vars_X,
					 const z3::expr_vector              &dec_vars_Y,
					 const std::vector<Slic3r::Polygon> &polygons)
{
    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    introduce_PolygonOutsidePolygon(Solver,
					    Context,
					    dec_vars_X[i],
					    dec_vars_Y[i],
					    polygons[i],
					    dec_vars_X[j],
					    dec_vars_Y[j],
					    polygons[j]);					    
	}
    }
}


void introduce_SequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						   z3::context                        &Context,
						   const z3::expr_vector              &dec_vars_X,
						   const z3::expr_vector              &dec_vars_Y,
						   const z3::expr_vector              &dec_vars_T,
						   const std::vector<Slic3r::Polygon> &polygons,
    						   const std::vector<Slic3r::Polygon> &unreachable_polygons)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }    

    introduce_SequentialPolygonWeakNonoverlapping(Solver,
						  Context,
						  dec_vars_X,
						  dec_vars_Y,
						  dec_vars_T,
						  polygons,
						  _unreachable_polygons);
}


void introduce_SequentialPolygonWeakNonoverlapping(z3::solver                                       &Solver,
						   z3::context                                      &Context,
						   const z3::expr_vector                            &dec_vars_X,
						   const z3::expr_vector                            &dec_vars_Y,
						   const z3::expr_vector                            &dec_vars_T,
						   const std::vector<Slic3r::Polygon>               &polygons,
						   const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons)
{
    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    introduce_SequentialPolygonOutsidePolygon(Solver,
						      Context,
						      dec_vars_X[i],
						      dec_vars_Y[i],
						      dec_vars_T[i],
						      polygons[i],
						      unreachable_polygons[i],
						      dec_vars_X[j],
						      dec_vars_Y[j],
						      dec_vars_T[j],
						      polygons[j],
						      unreachable_polygons[j]);
	}
    }
}


void introduce_ConsequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						      z3::context                        &Context,
						      const z3::expr_vector              &dec_vars_X,
						      const z3::expr_vector              &dec_vars_Y,
						      const z3::expr_vector              &dec_vars_T,
						      const std::vector<Slic3r::Polygon> &polygons,
						      const std::vector<Slic3r::Polygon> &unreachable_polygons)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }    

    introduce_ConsequentialPolygonWeakNonoverlapping(Solver,
						     Context,
						     dec_vars_X,
						     dec_vars_Y,
						     dec_vars_T,
						     polygons,
						     _unreachable_polygons);
}


void introduce_ConsequentialPolygonWeakNonoverlapping(z3::solver                                       &Solver,
						      z3::context                                      &Context,
						      const z3::expr_vector                            &dec_vars_X,
						      const z3::expr_vector                            &dec_vars_Y,
						      const z3::expr_vector                            &dec_vars_T,
						      const std::vector<Slic3r::Polygon>               &polygons,
						      const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons)
{
    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    introduce_ConsequentialPolygonOutsidePolygon(Solver,
							 Context,
							 dec_vars_X[i],
							 dec_vars_Y[i],
							 dec_vars_T[i],
							 polygons[i],
							 unreachable_polygons[i],
							 dec_vars_X[j],
							 dec_vars_Y[j],
							 dec_vars_T[j],
							 polygons[j],
							 unreachable_polygons[j]);
	}
    }
}


void introduce_PolygonWeakNonoverlapping(z3::solver                         &Solver,
					 z3::context                        &Context,
					 const z3::expr_vector              &dec_vars_X,
					 const z3::expr_vector              &dec_vars_Y,
					 std::vector<Rational>              &dec_values_X,
					 std::vector<Rational>              &dec_values_Y,
					 const std::vector<int>             &fixed,
					 const std::vector<int>             &undecided,
					 const std::vector<Slic3r::Polygon> &polygons)
{
    for (unsigned int i = 0; i < undecided.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < undecided.size(); ++j)
	{
	    introduce_PolygonOutsidePolygon(Solver,
					    Context,
					    dec_vars_X[undecided[i]],
					    dec_vars_Y[undecided[i]],
					    polygons[undecided[i]],
					    dec_vars_X[undecided[j]],
					    dec_vars_Y[undecided[j]],
					    polygons[undecided[j]]);					    
	}
    }

    for (unsigned int i = 0; i < undecided.size(); ++i)
    {
	for (unsigned int j = 0; j < fixed.size(); ++j)
	{
	    introduce_PolygonOutsideFixedPolygon(Solver,
						 Context,
						 dec_vars_X[undecided[i]],
						 dec_vars_Y[undecided[i]],
						 polygons[undecided[i]],
						 dec_values_X[fixed[j]],
						 dec_values_Y[fixed[j]],
						 polygons[fixed[j]]);					    	    
	}
    }
}


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
						   const std::vector<Slic3r::Polygon> &unreachable_polygons)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }    
    
    introduce_SequentialPolygonWeakNonoverlapping(Solver,
						  Context,
						  dec_vars_X,
						  dec_vars_Y,
						  dec_vars_T,
						  dec_values_X,
						  dec_values_Y,
						  dec_values_T,
						  fixed,
						  undecided,
						  polygons,
						  _unreachable_polygons);   
}


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
						   const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons)
{
    for (unsigned int i = 0; i < undecided.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < undecided.size(); ++j)
	{
            #ifdef DEBUG
	    {
		printf("PoP: %d,%d\n", undecided[i], undecided[j]);
	    }
	    #endif
	    introduce_SequentialPolygonOutsidePolygon(Solver,
						      Context,
						      dec_vars_X[undecided[i]],
						      dec_vars_Y[undecided[i]],
						      dec_vars_T[undecided[i]],
						      polygons[undecided[i]],
						      unreachable_polygons[undecided[i]],
						      dec_vars_X[undecided[j]],
						      dec_vars_Y[undecided[j]],
						      dec_vars_T[undecided[j]],
						      polygons[undecided[j]],
						      unreachable_polygons[undecided[j]]);
	}
    }

    for (unsigned int i = 0; i < undecided.size(); ++i)
    {
	for (unsigned int j = 0; j < fixed.size(); ++j)
	{
            #ifdef DEBUG
	    {
		printf("PoFP: %d,%d\n", undecided[i], fixed[j]);
	    }
	    #endif
	    introduce_SequentialPolygonOutsideFixedPolygon(Solver,
							   Context,
							   dec_vars_X[undecided[i]],
							   dec_vars_Y[undecided[i]],
							   dec_vars_T[undecided[i]],
							   polygons[undecided[i]],
							   unreachable_polygons[undecided[i]],
							   dec_values_X[fixed[j]],
							   dec_values_Y[fixed[j]],
							   dec_values_T[fixed[j]],
							   polygons[fixed[j]],
							   unreachable_polygons[fixed[j]]);
	}
    }
}


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
						      const std::vector<Slic3r::Polygon> &unreachable_polygons)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }    
    
    introduce_ConsequentialPolygonWeakNonoverlapping(Solver,
						     Context,
						     dec_vars_X,
						     dec_vars_Y,
						     dec_vars_T,
						     dec_values_X,
						     dec_values_Y,
						     dec_values_T,
						     fixed,
						     undecided,
						     polygons,
						     _unreachable_polygons);   
}


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
						      const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons)
{
    for (unsigned int i = 0; i < undecided.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < undecided.size(); ++j)
	{
            #ifdef DEBUG
	    {
		printf("PoP: %d,%d\n", undecided[i], undecided[j]);
	    }
	    #endif
	    introduce_ConsequentialPolygonExternalPolygon(Solver,
							  Context,
							  dec_vars_X[undecided[i]],
							  dec_vars_Y[undecided[i]],
							  dec_vars_T[undecided[i]],
							  polygons[undecided[i]],
							  unreachable_polygons[undecided[i]],
							  dec_vars_X[undecided[j]],
							  dec_vars_Y[undecided[j]],
							  dec_vars_T[undecided[j]],
							  polygons[undecided[j]],
							  unreachable_polygons[undecided[j]]);
	}
    }

    for (unsigned int i = 0; i < undecided.size(); ++i)
    {
	for (unsigned int j = 0; j < fixed.size(); ++j)
	{
            #ifdef DEBUG
	    {
		printf("PoFP: %d,%d\n", undecided[i], fixed[j]);
	    }
	    #endif
	    introduce_ConsequentialPolygonExternalFixedPolygon(Solver,
							       Context,
							       dec_vars_X[undecided[i]],
							       dec_vars_Y[undecided[i]],
							       dec_vars_T[undecided[i]],
							       polygons[undecided[i]],
							       unreachable_polygons[undecided[i]],
							       dec_values_X[fixed[j]],
							       dec_values_Y[fixed[j]],
							       dec_values_T[fixed[j]],
							       polygons[fixed[j]],
							       unreachable_polygons[fixed[j]]);
	}
    }
}


void introduce_PolygonStrongNonoverlapping(z3::solver                         &Solver,
					   z3::context                        &Context,
					   const z3::expr_vector              &dec_vars_X,
					   const z3::expr_vector              &dec_vars_Y,
					   const std::vector<Slic3r::Polygon> &polygons)
{
    introduce_PolygonWeakNonoverlapping(Solver,
					Context,
					dec_vars_X,
					dec_vars_Y,
					polygons);

    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    for (unsigned int p1 = 0; p1 < polygons[i].points.size(); ++p1)
	    {		
		const Point &point1 = polygons[i].points[p1];
		const Point &next_point1 = polygons[i].points[(p1 + 1) % polygons[i].points.size()];

		for (unsigned int p2 = 0; p2 < polygons[j].points.size(); ++p2)
		{
		    const Point &point2 = polygons[j].points[p2];
		    const Point &next_point2 = polygons[j].points[(p2 + 1) % polygons[j].points.size()];
   
		    introduce_LineNonIntersection(Solver,
						  Context,
						  dec_vars_X[i],
						  dec_vars_Y[i],
						  z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
						  Line(point1, next_point1),
						  dec_vars_X[j],
						  dec_vars_Y[j],
						  z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
						  Line(point2, next_point2));
		    hidden_var_cnt += 2;
		}
	    }
	}
    }
}


bool lines_intersect_(coord_t ax, coord_t ay, coord_t ux, coord_t uy, coord_t bx, coord_t by, coord_t vx, coord_t vy)
{    
    coord_t den = ux * vy - uy * vx;
    coord_t num = vx * ay - vx * by - vy * ax + vy * bx;

    if (fabs(den) < EPSILON)
    {
	return false;	
    }
    else
    {
	double t = (double)num / den;
	
	if (t < 0.0 || t > 1.0)
	{
	    return false;
	}
	else
	{
	    if (abs(vx) > 0)
	    {
		double tt = (ax - bx + t * ux) / vx;
		
		if (tt < 0.0 || tt > 1.0)
		{
		    return false;
		}
		else
		{
		    #ifdef DEBUG
		    {
			printf("t:%.6f\n", t);		    
			printf("tt:%.6f\n", tt);
		    }
		    #endif
		    return true;
		}
	    }
	    else
	    {
		if (abs(vy) > 0)
		{
		    double tt = (ay - by + t * uy) / vy;
		
		    if (tt < 0.0 || tt > 1.0)
		    {
			return false;
		    }
		    else
		    {
			#ifdef DEBUG
			{			    
			    printf("t:%.6f\n", t);
			    printf("tt2:%.6f\n", tt);
			}
			#endif
			return true;
		    }
		}
		else
		{		
		    return false;
		}
	    }
	}
    }
    
    return false;
}


bool lines_intersect(double ax, double ay, double ux, double uy, double bx, double by, double vx, double vy)
{
    double den = ux * vy - uy * vx;
    double num = vx * ay - vx * by - vy * ax + vy * bx;

    if (fabs(den) < EPSILON)
    {
	return false;	
    }
    else
    {
	double t = num / den;
	
	if (t < 0.0 || t > 1.0)
	{
	    return false;
	}
	else
	{
	    if (fabs(vx) > EPSILON)
	    {
		double tt = (ax - bx + t * ux) / vx;
		
		if (tt < 0.0 || tt > 1.0)
		{
		    return false;
		}
		else
		{
		    #ifdef DEBUG
		    {
			printf("t:%.6f\n", t);		    
			printf("tt:%.6f\n", tt);
		    }
		    #endif
		    return true;
		}
	    }
	    else
	    {
		if (fabs(vy) > EPSILON)
		{
		    double tt = (ay - by + t * uy) / vy;
		
		    if (tt < 0.0 || tt > 1.0)
		    {
			return false;
		    }
		    else
		    {
			#ifdef DEBUG
			{			    
			    printf("t:%.6f\n", t);
			    printf("tt2:%.6f\n", tt);
			}
			#endif
			return true;
		    }
		}
		else
		{		
		    return false;
		}
	    }
	}
    }
    
    return false;
}


bool lines_intersect_closed(double ax, double ay, double ux, double uy, double bx, double by, double vx, double vy)
{
    return lines_intersect(ax, ay, ux, uy, bx, by, vx, vy);
}


bool lines_intersect_open(double ax, double ay, double ux, double uy, double bx, double by, double vx, double vy)
{
    double den = ux * vy - uy * vx;
    double num = vx * ay - vx * by - vy * ax + vy * bx;

    if (fabs(den) < EPSILON)
    {
	return false;	
    }
    else
    {
	double t = num / den;
	
	if (t < EPSILON || t > 1.0 - EPSILON)
	{
	    return false;
	}
	else
	{
	    if (fabs(vx) > EPSILON)
	    {
		double tt = (ax - bx + t * ux) / vx;
		
		if (tt < EPSILON || tt > 1.0 - EPSILON)
		{
		    return false;
		}
		else
		{
		    #ifdef DEBUG
		    {
			printf("t:%.6f\n", t);		    
			printf("tt:%.6f\n", tt);
		    }
		    #endif
		    return true;
		}
	    }
	    else
	    {
		if (fabs(vy) > EPSILON)
		{
		    double tt = (ay - by + t * uy) / vy;
		
		    if (tt < EPSILON || tt > 1.0 - EPSILON)
		    {
			return false;
		    }
		    else
		    {
			#ifdef DEBUG
			{			    
			    printf("t:%.6f\n", t);
			    printf("tt2:%.6f\n", tt);
			}
			#endif
			return true;
		    }
		}
		else
		{		
		    return false;
		}
	    }
	}
    }
    
    return false;
}


bool refine_PolygonWeakNonoverlapping(z3::solver                         &Solver,
				      z3::context                        &Context,
				      const z3::expr_vector              &dec_vars_X,
				      const z3::expr_vector              &dec_vars_Y,
				      const std::vector<double>          &dec_values_X,
				      const std::vector<double>          &dec_values_Y,
				      const std::vector<Slic3r::Polygon> &polygons)
{
    bool refined = false;

    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    for (unsigned int p1 = 0; p1 < polygons[i].points.size(); ++p1)
	    {		
		const Point &point1 = polygons[i].points[p1];
		const Point &next_point1 = polygons[i].points[(p1 + 1) % polygons[i].points.size()];

		for (unsigned int p2 = 0; p2 < polygons[j].points.size(); ++p2)
		{
		    const Point &point2 = polygons[j].points[p2];
		    const Point &next_point2 = polygons[j].points[(p2 + 1) % polygons[j].points.size()];

		    Vec2d intersection(0,0);
		    #ifdef DEBUG
		    {
			/*
			printf("testing: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
			       dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
			       dec_values_X[i] + next_point1.x(), dec_values_Y[i] + next_point1.y(),
			       dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
			       dec_values_X[j] + next_point2.x(), dec_values_Y[j] + next_point2.y());
			*/
		    }
		    #endif

		    /* Seems not working, report an intersection even if there is none, using out own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
		    if (lines_intersect(dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
					next_point1.x() - point1.x(), next_point1.y() - point1.y(),
					dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
					next_point2.x() - point2.x(), next_point2.y() - point2.y()))
			
		    {
			#ifdef DEBUG
			{
			    /*
			    printf("intersect: %d (%.3f,%.3f) - [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", hidden_var_cnt, intersection.x(), intersection.y(),
				
			       dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
			       dec_values_X[i] + next_point1.x(), dec_values_Y[i] + next_point1.y(),
			       dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
			       dec_values_X[j] + next_point2.x(), dec_values_Y[j] + next_point2.y());
			    */
			}
			#endif
			
			introduce_LineNonIntersection(Solver,
						      Context,
						      dec_vars_X[i],
						      dec_vars_Y[i],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
						      Line(point1, next_point1),
						      dec_vars_X[j],
						      dec_vars_Y[j],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
						      Line(point2, next_point2));
			hidden_var_cnt += 2;
				    
			refined = true;
		    }
		}
	    }	    
	}
    }
    return refined;
}


bool refine_PolygonWeakNonoverlapping(z3::solver                         &Solver,
				      z3::context                        &Context,
				      const z3::expr_vector              &dec_vars_X,
				      const z3::expr_vector              &dec_vars_Y,
				      const z3::expr_vector              &dec_values_X,
				      const z3::expr_vector              &dec_values_Y,
				      const std::vector<Slic3r::Polygon> &polygons)
{
    bool refined = false;

    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    for (unsigned int p1 = 0; p1 < polygons[i].points.size(); ++p1)
	    {		
		const Point &point1 = polygons[i].points[p1];
		const Point &next_point1 = polygons[i].points[(p1 + 1) % polygons[i].points.size()];

		for (unsigned int p2 = 0; p2 < polygons[j].points.size(); ++p2)
		{
		    const Point &point2 = polygons[j].points[p2];
		    const Point &next_point2 = polygons[j].points[(p2 + 1) % polygons[j].points.size()];

		    Vec2d intersection(0, 0);
		    #ifdef DEBUG
		    {
			printf("testing: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
			       dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
			       dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
			       dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
			       dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
		    }
		    #endif

		    /* Seems not working, report an intersection even if there is none, using out own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
		    if (lines_intersect(dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
					next_point1.x() - point1.x(), next_point1.y() - point1.y(),
					dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
					next_point2.x() - point2.x(), next_point2.y() - point2.y()))
			
		    {
			#ifdef DEBUG
			{
			    printf("intersect: %d (%.3f,%.3f) - [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", hidden_var_cnt, intersection.x(), intersection.y(),				
			       dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
			       dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
			       dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
			       dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
			}
			#endif
			
			introduce_LineNonIntersection(Solver,
						      Context,
						      dec_vars_X[i],
						      dec_vars_Y[i],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
						      Line(point1, next_point1),
						      dec_vars_X[j],
						      dec_vars_Y[j],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
						      Line(point2, next_point2));
			hidden_var_cnt += 2;
			
			refined = true;
		    }
		}
	    }	    
	}
    }
    return refined;
}


bool refine_PolygonWeakNonoverlapping(z3::solver                         &Solver,
				      z3::context                        &Context,
				      const z3::expr_vector              &dec_vars_X,
				      const z3::expr_vector              &dec_vars_Y,
				      const std::vector<Rational>        &dec_values_X,
				      const std::vector<Rational>        &dec_values_Y,
				      const std::vector<Slic3r::Polygon> &polygons)
{
    bool refined = false;

    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    for (unsigned int p1 = 0; p1 < polygons[i].points.size(); ++p1)
	    {		
		const Point &point1 = polygons[i].points[p1];
		const Point &next_point1 = polygons[i].points[(p1 + 1) % polygons[i].points.size()];

		for (unsigned int p2 = 0; p2 < polygons[j].points.size(); ++p2)
		{
		    const Point &point2 = polygons[j].points[p2];
		    const Point &next_point2 = polygons[j].points[(p2 + 1) % polygons[j].points.size()];

		    Vec2d intersection(0, 0);
		    #ifdef DEBUG
		    {
			printf("testing mi: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
			       dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
			       dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
			       dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
			       dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
		    }
		    #endif

		    /* Seems not working, report an intersection even if there is none, using out own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
		    if (lines_intersect(dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
					next_point1.x() - point1.x(), next_point1.y() - point1.y(),
					dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
					next_point2.x() - point2.x(), next_point2.y() - point2.y()))
			
		    {
			#ifdef DEBUG
			{
			    printf("intersect: %d (%.3f,%.3f) - [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", hidden_var_cnt, intersection.x(), intersection.y(),				
			       dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
			       dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
			       dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
			       dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
			}
			#endif
			
			introduce_LineNonIntersection(Solver,
						      Context,
						      dec_vars_X[i],
						      dec_vars_Y[i],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
						      Line(point1, next_point1),
						      dec_vars_X[j],
						      dec_vars_Y[j],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
						      Line(point2, next_point2));
			hidden_var_cnt += 2;
			
			refined = true;
		    }
		}
	    }	    
	}
    }
    return refined;
}


bool refine_SequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						z3::context                        &Context,
						const z3::expr_vector              &dec_vars_X,
						const z3::expr_vector              &dec_vars_Y,
						const z3::expr_vector              &dec_vars_T,	
						const std::vector<double>          &dec_values_X,
						const std::vector<double>          &dec_values_Y,	
						const std::vector<double>          &dec_values_T,
						const std::vector<Slic3r::Polygon> &polygons,
						const std::vector<Slic3r::Polygon> &unreachable_polygons)
{
    bool refined = false;

    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    if (dec_values_T[i] > dec_values_T[j])
	    {
		for (unsigned int p1 = 0; p1 < polygons[i].points.size(); ++p1)
		{		
		    const Point &point1 = polygons[i].points[p1];
		    const Point &next_point1 = polygons[i].points[(p1 + 1) % polygons[i].points.size()];
		    
		    for (unsigned int p2 = 0; p2 < unreachable_polygons[j].points.size(); ++p2)
		    {
			const Point &point2 = unreachable_polygons[j].points[p2];
			const Point &next_point2 = unreachable_polygons[j].points[(p2 + 1) % unreachable_polygons[j].points.size()];
			
                        #ifdef DEBUG
			{
	  		    printf("testing ni %d %d (%d,%d): [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", i, j, p1, p2,
			           dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
			           dec_values_X[i] + next_point1.x(), dec_values_Y[i] + next_point1.y(),
			           dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
			           dec_values_X[j] + next_point2.x(), dec_values_Y[j] + next_point2.y());
			}
		        #endif

			if (lines_intersect(dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
					    next_point1.x() - point1.x(), next_point1.y() - point1.y(),
					    dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
					    next_point2.x() - point2.x(), next_point2.y() - point2.y()))
			
			{
			    #ifdef DEBUG
			    {
			    /*
			    printf("intersect: %d (%.3f,%.3f) - [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", hidden_var_cnt, intersection.x(), intersection.y(),
				
			       dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
			       dec_values_X[i] + next_point1.x(), dec_values_Y[i] + next_point1.y(),
			       dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
			       dec_values_X[j] + next_point2.x(), dec_values_Y[j] + next_point2.y());
			    */
			    }
			    #endif

			    introduce_SequentialLineNonIntersection(Solver,
								    Context,
								    dec_vars_X[i],
								    dec_vars_Y[i],
								    dec_vars_T[i],								    
								    z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
								    Line(point1, next_point1),
								    dec_vars_X[j],
								    dec_vars_Y[j],
								    dec_vars_T[j],								    
								    z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
								    Line(point2, next_point2));
			    hidden_var_cnt += 2;
	    
			    refined = true;
			}
		    }
		}	    
	    }
	    else
	    {
		if (dec_values_T[i] < dec_values_T[j])
		{
		    for (unsigned int p1 = 0; p1 < unreachable_polygons[i].points.size(); ++p1)
		    {		
			const Point &point1 = unreachable_polygons[i].points[p1];
			const Point &next_point1 = unreachable_polygons[i].points[(p1 + 1) % unreachable_polygons[i].points.size()];
		    
			for (unsigned int p2 = 0; p2 < polygons[j].points.size(); ++p2)
			{
			    const Point &point2 = polygons[j].points[p2];
			    const Point &next_point2 = polygons[j].points[(p2 + 1) % polygons[j].points.size()];
			
                            #ifdef DEBUG
			    {
			/*
	  		    printf("testing: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
			           dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
			           dec_values_X[i] + next_point1.x(), dec_values_Y[i] + next_point1.y(),
			           dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
			           dec_values_X[j] + next_point2.x(), dec_values_Y[j] + next_point2.y());
			*/
			    }
		            #endif

			    if (lines_intersect(dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
						next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
						next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				
			    {
			        #ifdef DEBUG
				{
			    /*
			    printf("intersect: %d (%.3f,%.3f) - [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", hidden_var_cnt, intersection.x(), intersection.y(),
				
			       dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
			       dec_values_X[i] + next_point1.x(), dec_values_Y[i] + next_point1.y(),
			       dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
			       dec_values_X[j] + next_point2.x(), dec_values_Y[j] + next_point2.y());
			    */
				}
			        #endif

				introduce_SequentialLineNonIntersection(Solver,
									Context,
									dec_vars_X[j],
									dec_vars_Y[j],
									dec_vars_T[j],
									z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
									Line(point2, next_point2),
									dec_vars_X[i],
									dec_vars_Y[i],
									dec_vars_T[i],
									z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
									Line(point1, next_point1));
				hidden_var_cnt += 2;
					    
				refined = true;
			    }
			}
		    }	    		    
		}
		else
		{
		    #ifdef DEBUG
		    {
			printf("Time collision: %.3f, %.3f\n", dec_values_T[i], dec_values_T[j]);
		    }
		    #endif
		    assert(false);
		}
	    }
	}
    }
    return refined;
}


bool refine_SequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						z3::context                        &Context,
						const z3::expr_vector              &dec_vars_X,
						const z3::expr_vector              &dec_vars_Y,
						const z3::expr_vector              &dec_vars_T,	
						const std::vector<Rational>        &dec_values_X,
						const std::vector<Rational>        &dec_values_Y,	
						const std::vector<Rational>        &dec_values_T,
						const std::vector<Slic3r::Polygon> &polygons,
						const std::vector<Slic3r::Polygon> &unreachable_polygons)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }

    return refine_SequentialPolygonWeakNonoverlapping(Solver,
						      Context,
						      dec_vars_X,
						      dec_vars_Y,
						      dec_vars_T,	
						      dec_values_X,
						      dec_values_Y,	
						      dec_values_T,
						      polygons,
						      _unreachable_polygons);
}


bool refine_SequentialPolygonWeakNonoverlapping(z3::solver                                       &Solver,
						z3::context                                      &Context,
						const z3::expr_vector                            &dec_vars_X,
						const z3::expr_vector                            &dec_vars_Y,
						const z3::expr_vector                            &dec_vars_T,	
						const std::vector<Rational>                      &dec_values_X,
						const std::vector<Rational>                      &dec_values_Y,	
						const std::vector<Rational>                      &dec_values_T,
						const std::vector<Slic3r::Polygon>               &polygons,
						const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons)
{
    bool refined = false;

    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    if (dec_values_T[i] > dec_values_T[j])
	    {
		for (unsigned int p1 = 0; p1 < polygons[i].points.size(); ++p1)
		{		
		    const Point &point1 = polygons[i].points[p1];
		    const Point &next_point1 = polygons[i].points[(p1 + 1) % polygons[i].points.size()];

		    for (unsigned int poly2 = 0; poly2 < unreachable_polygons[j].size(); ++poly2)
		    {
			#ifdef DEBUG
			{			    
			    printf("temporal: %.3f %.3f [ij: %d,%d]\n", dec_values_T[i].as_double(), dec_values_T[j].as_double(), i, j);
			    printf("proto X1: %ld, %ld, %ld\n", unreachable_polygons.size(), unreachable_polygons[j].size(), unreachable_polygons[j][poly2].points.size());
			}
			#endif
			       
			for (unsigned int p2 = 0; p2 < unreachable_polygons[j][poly2].points.size(); ++p2)
			{
			    const Point &point2 = unreachable_polygons[j][poly2].points[p2];
			    const Point &next_point2 = unreachable_polygons[j][poly2].points[(p2 + 1) % unreachable_polygons[j][poly2].points.size()];
			
                            #ifdef DEBUG
			    {
				printf("testing alpha %d %d (%d,%d): [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", i, j, p1, p2,
				       dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
				       dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
				       dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
				       dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
			    }
		            #endif

			    if (lines_intersect(dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
						next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
						next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				
			    {
			        #ifdef DEBUG
				{
				    printf("temps: [ij: %d,%d] [%.3f, %.3f]\n", i, j,
					   dec_values_T[i].as_double(),
					   dec_values_T[j].as_double());
						   
				    printf("dec_values: [%.3f, %.3f] [%.3f,%.3f]\n",
					   dec_values_X[i].as_double(),
					   dec_values_Y[i].as_double(),
					   dec_values_X[j].as_double(),
					   dec_values_Y[j].as_double());
				    
				    printf("intersect 1: %d [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   hidden_var_cnt, 				
					   dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
					   dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
					   dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
					   dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
				}
			        #endif

				introduce_SequentialLineNonIntersection(Solver,
									Context,
									dec_vars_X[i],
									dec_vars_Y[i],
									dec_vars_T[i],								    
									z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
									Line(point1, next_point1),
									dec_vars_X[j],
									dec_vars_Y[j],
									dec_vars_T[j],								    
									z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
									Line(point2, next_point2));
				hidden_var_cnt += 2;
					    
				refined = true;
			    }
			}
		    }
		}	    
	    }
	    else
	    {
		if (dec_values_T[i] < dec_values_T[j])
		{
		    for (unsigned int poly1 = 0; poly1 < unreachable_polygons[i].size(); ++poly1)
		    {
			for (unsigned int p1 = 0; p1 < unreachable_polygons[i][poly1].points.size(); ++p1)
			{
			    #ifdef DEBUG
			    {
				printf("proto2: %ld, %ld, %ld\n", unreachable_polygons.size(), unreachable_polygons[i].size(), unreachable_polygons[i][poly1].points.size());
				//getchar();
			    }
			    #endif
			    
			    const Point &point1 = unreachable_polygons[i][poly1].points[p1];
			    const Point &next_point1 = unreachable_polygons[i][poly1].points[(p1 + 1) % unreachable_polygons[i][poly1].points.size()];
		    
			    for (unsigned int p2 = 0; p2 < polygons[j].points.size(); ++p2)
			    {
				const Point &point2 = polygons[j].points[p2];
				const Point &next_point2 = polygons[j].points[(p2 + 1) % polygons[j].points.size()];
			
                                #ifdef DEBUG
				{
				    printf("testing beta: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
					   dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
					   dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
					   dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
				}
		                #endif

				if (lines_intersect(dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
						    next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						    dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
						    next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				
				{
			            #ifdef DEBUG
				    {
					printf("temps: [ij: %d,%d] [%.3f, %.3f]\n", i, j,
					       dec_values_T[i].as_double(),
					       dec_values_T[j].as_double());
						   
					printf("dec_values: [%.3f, %.3f] [%.3f,%.3f]\n",
					       dec_values_X[i].as_double(),
					       dec_values_Y[i].as_double(),
					       dec_values_X[j].as_double(),
					       dec_values_Y[j].as_double());
					
					printf("intersect 2: %d [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					       hidden_var_cnt, 				
					       dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
					       dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
					       dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
					       dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
				    }
			            #endif

				    introduce_SequentialLineNonIntersection(Solver,
									    Context,
									    dec_vars_X[j],
									    dec_vars_Y[j],
									    dec_vars_T[j],
									    z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
									    Line(point2, next_point2),
									    dec_vars_X[i],
									    dec_vars_Y[i],
									    dec_vars_T[i],
									    z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
									    Line(point1, next_point1));
				    hidden_var_cnt += 2;
	    
				    /*
				    introduce_SequentialLineNonIntersection(Solver,
									    Context,
									    dec_vars_X[i],
									    dec_vars_Y[i],
									    dec_vars_T[i],
									    z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
									    Line(point1, next_point1),
									    dec_vars_X[j],
									    dec_vars_Y[j],
									    dec_vars_T[j],
									    z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
									    Line(point2, next_point2));

				    */
				    refined = true;
				}
			    }
			}
		    }	    		    
		}
		else
		{
		    #ifdef DEBUG
		    {
			printf("Time collision: %.3f, %.3f\n", dec_values_T[i].as_double(), dec_values_T[j].as_double());
		    }
		    #endif
		    assert(false);
		}
	    }
	}
    }
    return refined;
}


bool refine_ConsequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						   z3::context                        &Context,
						   const z3::expr_vector              &dec_vars_X,
						   const z3::expr_vector              &dec_vars_Y,
						   const z3::expr_vector              &dec_vars_T,	
						   const std::vector<double>          &dec_values_X,
						   const std::vector<double>          &dec_values_Y,	
						   const std::vector<double>          &dec_values_T,
						   const std::vector<Slic3r::Polygon> &polygons,
						   const std::vector<Slic3r::Polygon> &unreachable_polygons)
{
    bool refined = false;

    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    if (dec_values_T[i] > dec_values_T[j])
	    {
		for (unsigned int p1 = 0; p1 < polygons[i].points.size(); ++p1)
		{		
		    const Point &point1 = polygons[i].points[p1];
		    const Point &next_point1 = polygons[i].points[(p1 + 1) % polygons[i].points.size()];
		    
		    for (unsigned int p2 = 0; p2 < unreachable_polygons[j].points.size(); ++p2)
		    {
			const Point &point2 = unreachable_polygons[j].points[p2];
			const Point &next_point2 = unreachable_polygons[j].points[(p2 + 1) % unreachable_polygons[j].points.size()];
			
                        #ifdef DEBUG
			{
	  		    printf("testing ni %d %d (%d,%d): [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", i, j, p1, p2,
			           dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
			           dec_values_X[i] + next_point1.x(), dec_values_Y[i] + next_point1.y(),
			           dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
			           dec_values_X[j] + next_point2.x(), dec_values_Y[j] + next_point2.y());
			}
		        #endif

			if (lines_intersect(dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
					    next_point1.x() - point1.x(), next_point1.y() - point1.y(),
					    dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
					    next_point2.x() - point2.x(), next_point2.y() - point2.y()))
			
			{
			    #ifdef DEBUG
			    {
				/*
			    printf("intersect: %d (%.3f,%.3f) - [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", hidden_var_cnt, intersection.x(), intersection.y(),
				
			       dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
			       dec_values_X[i] + next_point1.x(), dec_values_Y[i] + next_point1.y(),
			       dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
			       dec_values_X[j] + next_point2.x(), dec_values_Y[j] + next_point2.y());
				*/
			    }
			    #endif

			    introduce_ConsequentialLineNonIntersection(Solver,
								       Context,
								       dec_vars_X[i],
								       dec_vars_Y[i],
								       dec_vars_T[i],								    
								       z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
								       Line(point1, next_point1),
								       dec_vars_X[j],
								       dec_vars_Y[j],
								       dec_vars_T[j],								    
								       z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
								       Line(point2, next_point2));
			    hidden_var_cnt += 2;
				    
			    refined = true;
			}
		    }
		}	    
	    }
	    else
	    {
		if (dec_values_T[i] < dec_values_T[j])
		{
		    for (unsigned int p1 = 0; p1 < unreachable_polygons[i].points.size(); ++p1)
		    {		
			const Point &point1 = unreachable_polygons[i].points[p1];
			const Point &next_point1 = unreachable_polygons[i].points[(p1 + 1) % unreachable_polygons[i].points.size()];
		    
			for (unsigned int p2 = 0; p2 < polygons[j].points.size(); ++p2)
			{
			    const Point &point2 = polygons[j].points[p2];
			    const Point &next_point2 = polygons[j].points[(p2 + 1) % polygons[j].points.size()];
			
                            #ifdef DEBUG
			    {
	  		    printf("testing: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
			           dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
			           dec_values_X[i] + next_point1.x(), dec_values_Y[i] + next_point1.y(),
			           dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
			           dec_values_X[j] + next_point2.x(), dec_values_Y[j] + next_point2.y());
			    }
		            #endif

			    if (lines_intersect(dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
						next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
						next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				
			    {
			        #ifdef DEBUG
				{
				    /*
				    printf("intersect: %d (%.3f,%.3f) - [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", hidden_var_cnt, intersection.x(), intersection.y(),
					   dec_values_X[i] + point1.x(), dec_values_Y[i] + point1.y(),
					   dec_values_X[i] + next_point1.x(), dec_values_Y[i] + next_point1.y(),
					   dec_values_X[j] + point2.x(), dec_values_Y[j] + point2.y(),
					   dec_values_X[j] + next_point2.x(), dec_values_Y[j] + next_point2.y());
				    */
				}
			        #endif

				introduce_ConsequentialLineNonIntersection(Solver,
									   Context,
									   dec_vars_X[j],
									   dec_vars_Y[j],
									   dec_vars_T[j],
									   z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
									   Line(point2, next_point2),
									   dec_vars_X[i],
									   dec_vars_Y[i],
									   dec_vars_T[i],
									   z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
									   Line(point1, next_point1));
				hidden_var_cnt += 2;
	    
				refined = true;
			    }
			}
		    }	    		    
		}
		else
		{
		    #ifdef DEBUG
		    {
			printf("Time collision: %.3f, %.3f\n", dec_values_T[i], dec_values_T[j]);
		    }
		    #endif
		    assert(false);
		}
	    }
	}
    }
    return refined;
}


bool refine_ConsequentialPolygonWeakNonoverlapping(z3::solver                         &Solver,
						   z3::context                        &Context,
						   const z3::expr_vector              &dec_vars_X,
						   const z3::expr_vector              &dec_vars_Y,
						   const z3::expr_vector              &dec_vars_T,	
						   const std::vector<Rational>        &dec_values_X,
						   const std::vector<Rational>        &dec_values_Y,	
						   const std::vector<Rational>        &dec_values_T,
						   const std::vector<Slic3r::Polygon> &polygons,
						   const std::vector<Slic3r::Polygon> &unreachable_polygons)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }

    return refine_ConsequentialPolygonWeakNonoverlapping(Solver,
							 Context,
							 dec_vars_X,
							 dec_vars_Y,
							 dec_vars_T,	
							 dec_values_X,
							 dec_values_Y,	
							 dec_values_T,
							 polygons,
							 _unreachable_polygons);
}


bool refine_ConsequentialPolygonWeakNonoverlapping(z3::solver                                       &Solver,
						   z3::context                                      &Context,
						   const z3::expr_vector                            &dec_vars_X,
						   const z3::expr_vector                            &dec_vars_Y,
						   const z3::expr_vector                            &dec_vars_T,	
						   const std::vector<Rational>                      &dec_values_X,
						   const std::vector<Rational>                      &dec_values_Y,	
						   const std::vector<Rational>                      &dec_values_T,
						   const std::vector<Slic3r::Polygon>               &polygons,
						   const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons)
{
    bool refined = false;

    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    if (dec_values_T[i] > dec_values_T[j])
	    {
		for (unsigned int p1 = 0; p1 < polygons[i].points.size(); ++p1)
		{		
		    const Point &point1 = polygons[i].points[p1];
		    const Point &next_point1 = polygons[i].points[(p1 + 1) % polygons[i].points.size()];

		    for (unsigned int poly2 = 0; poly2 < unreachable_polygons[j].size(); ++poly2)
		    {
			#ifdef DEBUG
			{			    
			    printf("temporal: %.3f %.3f [ij: %d,%d]\n", dec_values_T[i].as_double(), dec_values_T[j].as_double(), i, j);
			    printf("proto X1: %ld, %ld, %ld\n", unreachable_polygons.size(), unreachable_polygons[j].size(), unreachable_polygons[j][poly2].points.size());
			//getchar();
			}
			#endif
			       
			for (unsigned int p2 = 0; p2 < unreachable_polygons[j][poly2].points.size(); ++p2)
			{
			    const Point &point2 = unreachable_polygons[j][poly2].points[p2];
			    const Point &next_point2 = unreachable_polygons[j][poly2].points[(p2 + 1) % unreachable_polygons[j][poly2].points.size()];
			
                            #ifdef DEBUG
			    {
				printf("testing alpha %d %d (%d,%d): [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", i, j, p1, p2,
				       dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
				       dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
				       dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
				       dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
			    }
		            #endif

			    if (lines_intersect(dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
						next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
						next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				
			    {
			        #ifdef DEBUG
				{
				    printf("temps: [ij: %d,%d] [%.3f, %.3f]\n", i, j,
					   dec_values_T[i].as_double(),
					   dec_values_T[j].as_double());
						   
				    printf("dec_values: [%.3f, %.3f] [%.3f,%.3f]\n",
					   dec_values_X[i].as_double(),
					   dec_values_Y[i].as_double(),
					   dec_values_X[j].as_double(),
					   dec_values_Y[j].as_double());
				    
				    printf("intersect 1: %d [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   hidden_var_cnt, 				
					   dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
					   dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
					   dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
					   dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
				}
			        #endif

				introduce_ConsequentialLineNonIntersection(Solver,
									   Context,
									   dec_vars_X[i],
									   dec_vars_Y[i],
									   dec_vars_T[i],								    
									   z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
									   Line(point1, next_point1),
									   dec_vars_X[j],
									   dec_vars_Y[j],
									   dec_vars_T[j],								    
									   z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
									   Line(point2, next_point2));
				hidden_var_cnt += 2;
	    
				refined = true;
			    }
			}
		    }
		}	    
	    }
	    else
	    {
		if (dec_values_T[i] < dec_values_T[j])
		{
		    for (unsigned int poly1 = 0; poly1 < unreachable_polygons[i].size(); ++poly1)
		    {
			for (unsigned int p1 = 0; p1 < unreachable_polygons[i][poly1].points.size(); ++p1)
			{
			    #ifdef DEBUG
			    {
				printf("proto2: %ld, %ld, %ld\n", unreachable_polygons.size(), unreachable_polygons[i].size(), unreachable_polygons[i][poly1].points.size());
				//getchar();
			    }
			    #endif
			    
			    const Point &point1 = unreachable_polygons[i][poly1].points[p1];
			    const Point &next_point1 = unreachable_polygons[i][poly1].points[(p1 + 1) % unreachable_polygons[i][poly1].points.size()];
		    
			    for (unsigned int p2 = 0; p2 < polygons[j].points.size(); ++p2)
			    {
				const Point &point2 = polygons[j].points[p2];
				const Point &next_point2 = polygons[j].points[(p2 + 1) % polygons[j].points.size()];
			
                                #ifdef DEBUG
				{
				    printf("testing beta: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
					   dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
					   dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
					   dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
				}
		                #endif

				if (lines_intersect(dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
						    next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						    dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
						    next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				
				{
			            #ifdef DEBUG
				    {
					printf("temps: [ij: %d,%d] [%.3f, %.3f]\n", i, j,
					       dec_values_T[i].as_double(),
					       dec_values_T[j].as_double());
						   
					printf("dec_values: [%.3f, %.3f] [%.3f,%.3f]\n",
					       dec_values_X[i].as_double(),
					       dec_values_Y[i].as_double(),
					       dec_values_X[j].as_double(),
					       dec_values_Y[j].as_double());
					
					printf("intersect 2: %d [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					       hidden_var_cnt, 				
					       dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
					       dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
					       dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
					       dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
				    }
			            #endif

				    introduce_ConsequentialLineNonIntersection(Solver,
									       Context,
									       dec_vars_X[j],
									       dec_vars_Y[j],
									       dec_vars_T[j],
									       z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
									       Line(point2, next_point2),
									       dec_vars_X[i],
									       dec_vars_Y[i],
									       dec_vars_T[i],
									       z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
									       Line(point1, next_point1));
				    hidden_var_cnt += 2;
	    
				    /*
				    introduce_SequentialLineNonIntersection(Solver,
									    Context,
									    dec_vars_X[i],
									    dec_vars_Y[i],
									    dec_vars_T[i],
									    z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
									    Line(point1, next_point1),
									    dec_vars_X[j],
									    dec_vars_Y[j],
									    dec_vars_T[j],
									    z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
									    Line(point2, next_point2));

				    */
				    refined = true;
				}
			    }
			}
		    }	    		    
		}
		else
		{
		    #ifdef DEBUG
		    {
			printf("Time collision: %.3f, %.3f\n", dec_values_T[i].as_double(), dec_values_T[j].as_double());
		    }
		    #endif
		    assert(false);
		}
	    }
	}
    }
    return refined;
}


/*----------------------------------------------------------------*/

void introduce_PolygonWeakNonoverlappingAgainstFixed(z3::solver                         &Solver,
						     z3::context                        &Context,
						     const z3::expr_vector              &dec_vars_X,
						     const z3::expr_vector              &dec_vars_Y,
						     const z3::expr_vector              &dec_values_X,
						     const z3::expr_vector              &dec_values_Y,	
						     const std::vector<int>             &decided,
						     const std::vector<int>             &undecided,
						     const std::vector<Slic3r::Polygon> &polygons)
{
    for (unsigned int i = 0; i < undecided.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < undecided.size(); ++j)
	{
	    introduce_PolygonOutsidePolygon(Solver,
					    Context,
					    dec_vars_X[undecided[i]],
					    dec_vars_Y[undecided[i]],
					    polygons[undecided[i]],
					    dec_vars_X[undecided[j]],
					    dec_vars_Y[undecided[j]],
					    polygons[undecided[j]]);					    
	}
    }

    for (unsigned int i = 0; i < undecided.size(); ++i)
    {
	for (unsigned int j = 0; j < decided.size(); ++j)
	{
	    introduce_PolygonOutsidePolygon(Solver,
					    Context,
					    dec_vars_X[undecided[i]],
					    dec_vars_Y[undecided[i]],
					    polygons[undecided[i]],
					    dec_values_X[decided[j]],
					    dec_values_Y[decided[j]],
					    polygons[decided[j]]);
	}
    }
}


bool refine_PolygonWeakNonoverlapping(z3::solver                         &Solver,
				      z3::context                        &Context,
				      const z3::expr_vector              &dec_vars_X,
				      const z3::expr_vector              &dec_vars_Y,
				      const z3::expr_vector              &dec_values_X,
				      const z3::expr_vector              &dec_values_Y,
				      const std::vector<int>             &fixed,
				      const std::vector<int>             &undecided,
				      const std::vector<Slic3r::Polygon> &polygons)
{
    bool refined = false;

    for (unsigned int i = 0; i < undecided.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < undecided.size(); ++j)
	{
	    for (unsigned int p1 = 0; p1 < polygons[undecided[i]].points.size(); ++p1)
	    {		
		const Point &point1 = polygons[undecided[i]].points[p1];
		const Point &next_point1 = polygons[undecided[i]].points[(p1 + 1) % polygons[undecided[i]].points.size()];

		for (unsigned int p2 = 0; p2 < polygons[undecided[j]].points.size(); ++p2)
		{
		    const Point &point2 = polygons[undecided[j]].points[p2];
		    const Point &next_point2 = polygons[undecided[j]].points[(p2 + 1) % polygons[undecided[j]].points.size()];

		    Vec2d intersection (0, 0);
		    #ifdef DEBUG
		    {
			/*
			printf("testing: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
			       dec_values_X[undecided[i]].as_double() + point1.x(), dec_values_Y[undecided[i]].as_double() + point1.y(),
			       dec_values_X[undecided[i]].as_double() + next_point1.x(), dec_values_Y[undecided[i]].as_double() + next_point1.y(),
			       dec_values_X[undecided[j]].as_double() + point2.x(), dec_values_Y[undecided[j]].as_double() + point2.y(),
			       dec_values_X[undecided[j]].as_double() + next_point2.x(), dec_values_Y[undecided[j]].as_double() + next_point2.y());
			*/
		    }
		    #endif

		    /* Seems not working, report an intersection even if there is none, using our own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[undecided[i]] + point1.x(), dec_values_Y[undecided[i]] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[undecided[j]] + point2.x(), dec_values_Y[undecided[j]] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
		    if (lines_intersect(dec_values_X[undecided[i]].as_double() + point1.x(), dec_values_Y[undecided[i]].as_double() + point1.y(),
					next_point1.x() - point1.x(), next_point1.y() - point1.y(),
					dec_values_X[undecided[j]].as_double() + point2.x(), dec_values_Y[undecided[j]].as_double() + point2.y(),
					next_point2.x() - point2.x(), next_point2.y() - point2.y()))
			
		    {
			#ifdef DEBUG
			{
			    /*
			    printf("intersect: %d (%.3f,%.3f) - [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", hidden_var_cnt, intersection.x(), intersection.y(),				
			       dec_values_X[undecided[i]].as_double() + point1.x(), dec_values_Y[undecided[i]].as_double() + point1.y(),
			       dec_values_X[undecided[i]].as_double() + next_point1.x(), dec_values_Y[undecided[i]].as_double() + next_point1.y(),
			       dec_values_X[undecided[j]].as_double() + point2.x(), dec_values_Y[undecided[j]].as_double() + point2.y(),
			       dec_values_X[undecided[j]].as_double() + next_point2.x(), dec_values_Y[undecided[j]].as_double() + next_point2.y());
			    */
			}
			#endif
			
			introduce_LineNonIntersection(Solver,
						      Context,
						      dec_vars_X[undecided[i]],
						      dec_vars_Y[undecided[i]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
						      Line(point1, next_point1),
						      dec_vars_X[undecided[j]],
						      dec_vars_Y[undecided[j]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
						      Line(point2, next_point2));
			hidden_var_cnt += 2;
				    
			refined = true;
		    }
		}
	    }	    
	}
    }
    
    for (unsigned int i = 0; i < undecided.size(); ++i)
    {
	for (unsigned int j = 0; j < fixed.size(); ++j)
	{
	    for (unsigned int p1 = 0; p1 < polygons[undecided[i]].points.size(); ++p1)
	    {		
		const Point &point1 = polygons[undecided[i]].points[p1];
		const Point &next_point1 = polygons[undecided[i]].points[(p1 + 1) % polygons[undecided[i]].points.size()];

		for (unsigned int p2 = 0; p2 < polygons[fixed[j]].points.size(); ++p2)
		{
		    const Point &point2 = polygons[fixed[j]].points[p2];
		    const Point &next_point2 = polygons[fixed[j]].points[(p2 + 1) % polygons[fixed[j]].points.size()];

		    Vec2d intersection(0, 0);
		    #ifdef DEBUG
		    {
			/*
			printf("testing: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
			       dec_values_X[undecided[i]].as_double() + point1.x(), dec_values_Y[undecided[i]].as_double() + point1.y(),
			       dec_values_X[undecided[i]].as_double() + next_point1.x(), dec_values_Y[undecided[i]].as_double() + next_point1.y(),
			       dec_values_X[fixed[j]].as_double() + point2.x(), dec_values_Y[fixed[j]].as_double() + point2.y(),
			       dec_values_X[fixed[j]].as_double() + next_point2.x(), dec_values_Y[fixed[j]].as_double() + next_point2.y());
			*/
		    }
		    #endif

		    /* Seems not working, report an intersection even if there is none, using our own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[undecided[i]] + point1.x(), dec_values_Y[undecided[i]] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[fixed[j]] + point2.x(), dec_values_Y[fixed[j]] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
		    
		    if (lines_intersect(dec_values_X[undecided[i]].as_double() + point1.x(), dec_values_Y[undecided[i]].as_double() + point1.y(),
					next_point1.x() - point1.x(), next_point1.y() - point1.y(),
					dec_values_X[fixed[j]].as_double() + point2.x(), dec_values_Y[fixed[j]].as_double() + point2.y(),
					next_point2.x() - point2.x(), next_point2.y() - point2.y()))
			
		    {
			#ifdef DEBUG
			{
			    /*
			    printf("intersect: %d (%.3f,%.3f) - [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", hidden_var_cnt, intersection.x(), intersection.y(),				
			       dec_values_X[undecided[i]].as_double() + point1.x(), dec_values_Y[undecided[i]].as_double() + point1.y(),
			       dec_values_X[undecided[i]].as_double() + next_point1.x(), dec_values_Y[undecided[i]].as_double() + next_point1.y(),
			       dec_values_X[fixed[j]].as_double() + point2.x(), dec_values_Y[fixed[j]].as_double() + point2.y(),
			       dec_values_X[fixed[j]].as_double() + next_point2.x(), dec_values_Y[fixed[j]].as_double() + next_point2.y());
			    */
			}
			#endif
			
			introduce_LineNonIntersection(Solver,
						      Context,
						      dec_vars_X[undecided[i]],
						      dec_vars_Y[undecided[i]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
						      Line(point1, next_point1),
						      dec_vars_X[fixed[j]],
						      dec_vars_Y[fixed[j]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
						      Line(point2, next_point2));
			hidden_var_cnt += 2;
	    
			refined = true;
		    }		    		    
		}
	    }
	}
    }
	
    return refined;
}


bool refine_PolygonWeakNonoverlapping(z3::solver                         &Solver,
				      z3::context                        &Context,
				      const z3::expr_vector              &dec_vars_X,
				      const z3::expr_vector              &dec_vars_Y,
				      const std::vector<Rational>        &dec_values_X,
				      const std::vector<Rational>        &dec_values_Y,
				      const std::vector<int>             &fixed,
				      const std::vector<int>             &undecided,
				      const std::vector<Slic3r::Polygon> &polygons)
{
    bool refined = false;

    #ifdef DEBUG
    {	
	printf("Refining ***************************\n");
    }
    #endif
    for (unsigned int i = 0; i < undecided.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < undecided.size(); ++j)
	{
            #ifdef DEBUG
	    {		    
		printf("------------------------> Polygons: %d,%d\n", undecided[i], undecided[j]);
	    }
	    #endif
	    for (unsigned int p1 = 0; p1 < polygons[undecided[i]].points.size(); ++p1)
	    {		
		const Point &point1 = polygons[undecided[i]].points[p1];
		const Point &next_point1 = polygons[undecided[i]].points[(p1 + 1) % polygons[undecided[i]].points.size()];

		for (unsigned int p2 = 0; p2 < polygons[undecided[j]].points.size(); ++p2)
 		{
		    const Point &point2 = polygons[undecided[j]].points[p2];
		    const Point &next_point2 = polygons[undecided[j]].points[(p2 + 1) % polygons[undecided[j]].points.size()];

		    Vec2d intersection (0, 0);
		    #ifdef DEBUG
		    {
			printf("%d,%d - %ld,%ld,%ld,%ld  %ld,%ld,%ld,%ld\n", undecided[i], undecided[j],
			       dec_values_X[undecided[i]].numerator,
			       dec_values_X[undecided[i]].denominator,
			       dec_values_Y[undecided[i]].numerator,
			       dec_values_Y[undecided[i]].denominator,			       
			       dec_values_X[undecided[j]].numerator,
			       dec_values_X[undecided[j]].denominator,
			       dec_values_Y[undecided[j]].numerator,
			       dec_values_Y[undecided[j]].denominator);

			printf("point1: %d,%d,%d,%d\n", point1.x(), point1.y(), next_point1.x(), next_point1.y());
			printf("point2: %d,%d,%d,%d\n", point2.x(), point2.y(), next_point2.x(), next_point2.y());

			printf("%ld,%ld\n", (dec_values_X[undecided[i]] + point1.x()).numerator, (dec_values_X[undecided[i]] + point1.x()).denominator);
			printf("%ld,%ld\n", (dec_values_X[undecided[j]] + point1.x()).numerator, (dec_values_X[undecided[j]] + point1.x()).denominator);			
						
			printf("testing gamma: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
			       (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
			       (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
			       (dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
			       (dec_values_X[undecided[j]] + next_point2.x()).as_double(), (dec_values_Y[undecided[j]] + next_point2.y()).as_double());
		    }
		    #endif

		    /* Seems not working, report an intersection even if there is none, using our own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[undecided[i]] + point1.x(), dec_values_Y[undecided[i]] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[undecided[j]] + point2.x(), dec_values_Y[undecided[j]] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
		    if (lines_intersect((dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					next_point1.x() - point1.x(), next_point1.y() - point1.y(),
					(dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
					next_point2.x() - point2.x(), next_point2.y() - point2.y()))
			
		    {
			#ifdef DEBUG
			{
			    printf("Intersecting: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
				   (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
				   (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
				   (dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
				   (dec_values_X[undecided[j]] + next_point2.x()).as_double(), (dec_values_Y[undecided[j]] + next_point2.y()).as_double());
			}
			#endif
			
			introduce_LineNonIntersection(Solver,
						      Context,
						      dec_vars_X[undecided[i]],
						      dec_vars_Y[undecided[i]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
						      Line(point1, next_point1),
						      dec_vars_X[undecided[j]],
						      dec_vars_Y[undecided[j]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
						      Line(point2, next_point2));
			hidden_var_cnt += 2;
	    
			refined = true;
		    }
		}
	    }	    
	}
    }
    
    for (unsigned int i = 0; i < undecided.size(); ++i)
    {
	for (unsigned int j = 0; j < fixed.size(); ++j)
	{
            #ifdef DEBUG
	    {	
		printf("Fixo ------------------------> Polygons: %d,%d\n", undecided[i], fixed[j]);
	    }
	    #endif
	    for (unsigned int p1 = 0; p1 < polygons[undecided[i]].points.size(); ++p1)
	    {		
		const Point &point1 = polygons[undecided[i]].points[p1];
		const Point &next_point1 = polygons[undecided[i]].points[(p1 + 1) % polygons[undecided[i]].points.size()];

		for (unsigned int p2 = 0; p2 < polygons[fixed[j]].points.size(); ++p2)
		{
		    const Point &point2 = polygons[fixed[j]].points[p2];
		    const Point &next_point2 = polygons[fixed[j]].points[(p2 + 1) % polygons[fixed[j]].points.size()];

		    Vec2d intersection(0, 0);
		    #ifdef DEBUG
		    {
			printf("testing delta: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
			       (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
			       (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
			       (dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
			       (dec_values_X[fixed[j]] + next_point2.x()).as_double(), (dec_values_Y[fixed[j]] + next_point2.y()).as_double());
		    }
		    #endif

		    /* Seems not working, report an intersection even if there is none, using our own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[undecided[i]] + point1.x(), dec_values_Y[undecided[i]] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[fixed[j]] + point2.x(), dec_values_Y[fixed[j]] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
		    
		    if (lines_intersect((dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					next_point1.x() - point1.x(), next_point1.y() - point1.y(),
					(dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
					next_point2.x() - point2.x(), next_point2.y() - point2.y()))
			
		    {
			#ifdef DEBUG
			{
			    printf("Intersecting: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
				   (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
				   (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
				   (dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
				   (dec_values_X[fixed[j]] + next_point2.x()).as_double(), (dec_values_Y[fixed[j]] + next_point2.y()).as_double());
			}
			#endif

			/*
			introduce_LineNonIntersection(Solver,
						      dec_vars_X[undecided[i]],
						      dec_vars_Y[undecided[i]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
						      Line(point1, next_point1),
						      dec_vars_X[fixed[j]],
						      dec_vars_Y[fixed[j]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
						      Line(point2, next_point2));
			*/
			introduce_LineNonIntersectionAgainstFixedLine(Solver,
								      Context,
								      dec_vars_X[undecided[i]],
								      dec_vars_Y[undecided[i]],
								      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
								      Line(point1, next_point1),
								      dec_values_X[fixed[j]],
								      dec_values_Y[fixed[j]],
								      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
								      Line(point2, next_point2));
			hidden_var_cnt += 2;
	    
			refined = true;
		    }		    		    
		}
	    }
	}
    }
	
    return refined;
}


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
						const std::vector<Slic3r::Polygon> &unreachable_polygons)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }    

    return refine_SequentialPolygonWeakNonoverlapping(Solver,
						      Context,
						      dec_vars_X,
						      dec_vars_Y,
						      dec_vars_T,
						      dec_values_X,
						      dec_values_Y,
						      dec_values_T,
						      fixed,
						      undecided,
						      polygons,
						      _unreachable_polygons);
}


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
						const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons)
{
    bool refined = false;

    #ifdef DEBUG
    {	
	printf("Refining *************************** alpha\n");

	for (unsigned int i = 0; i < undecided.size(); ++i)
	{
	    printf("%d: %.3f,%.3f [%.3f]\n",
		   undecided[i],
		   dec_values_X[undecided[i]].as_double(),
		   dec_values_Y[undecided[i]].as_double(),
		   dec_values_T[undecided[i]].as_double()); 
	}
    }
    #endif
    
    for (unsigned int i = 0; i < undecided.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < undecided.size(); ++j)
	{
	    if (dec_values_T[undecided[i]] > dec_values_T[undecided[j]])
	    {
		#ifdef DEBUG
		{	
		    printf("------------------------> Polygons: %d,%d\n", undecided[i], undecided[j]);
		}
		#endif
		for (unsigned int p1 = 0; p1 < polygons[undecided[i]].points.size(); ++p1)
		{		
		    const Point &point1 = polygons[undecided[i]].points[p1];
		    const Point &next_point1 = polygons[undecided[i]].points[(p1 + 1) % polygons[undecided[i]].points.size()];

		    for (unsigned int poly2 = 0; poly2 < unreachable_polygons[undecided[j]].size(); ++poly2)
		    {
			for (unsigned int p2 = 0; p2 < unreachable_polygons[undecided[j]][poly2].points.size(); ++p2)
			{
			    const Point &point2 = unreachable_polygons[undecided[j]][poly2].points[p2];
			    const Point &next_point2 = unreachable_polygons[undecided[j]][poly2].points[(p2 + 1) % unreachable_polygons[undecided[j]][poly2].points.size()];

			    Vec2d intersection (0, 0);
		            #ifdef DEBUG
			    {
				printf("%d,%d - %ld,%ld,%ld,%ld  %ld,%ld,%ld,%ld\n", undecided[i], undecided[j],
				       dec_values_X[undecided[i]].numerator,
				       dec_values_X[undecided[i]].denominator,
				       dec_values_Y[undecided[i]].numerator,
				       dec_values_Y[undecided[i]].denominator,			       
				       dec_values_X[undecided[j]].numerator,
				       dec_values_X[undecided[j]].denominator,
				       dec_values_Y[undecided[j]].numerator,
				       dec_values_Y[undecided[j]].denominator);

				printf("point1: %d,%d,%d,%d\n", point1.x(), point1.y(), next_point1.x(), next_point1.y());
				printf("point2: %d,%d,%d,%d\n", point2.x(), point2.y(), next_point2.x(), next_point2.y());

				printf("%ld,%ld\n", (dec_values_X[undecided[i]] + point1.x()).numerator, (dec_values_X[undecided[i]] + point1.x()).denominator);
				printf("%ld,%ld\n", (dec_values_X[undecided[j]] + point2.x()).numerator, (dec_values_X[undecided[j]] + point2.x()).denominator);			
						
				printf("testing epsilon: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
				       (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
				       (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
				       (dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
				       (dec_values_X[undecided[j]] + next_point2.x()).as_double(), (dec_values_Y[undecided[j]] + next_point2.y()).as_double());
			    }
		            #endif

		    /* Seems not working, report an intersection even if there is none, using our own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[undecided[i]] + point1.x(), dec_values_Y[undecided[i]] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[undecided[j]] + point2.x(), dec_values_Y[undecided[j]] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
			    if (lines_intersect((dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
						next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						(dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
						next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				
			    {
			        #ifdef DEBUG
				{
				    printf("Intersecting: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					   (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
					   (dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
					   (dec_values_X[undecided[j]] + next_point2.x()).as_double(), (dec_values_Y[undecided[j]] + next_point2.y()).as_double());
				}
			        #endif

			    /*
			    introduce_LineNonIntersection(Solver,
							  dec_vars_X[undecided[i]],
							  dec_vars_Y[undecided[i]],
							  z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
							  Line(point1, next_point1),
							  dec_vars_X[undecided[j]],
							  dec_vars_Y[undecided[j]],
							  z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
							  Line(point2, next_point2));
			    */
			    
				introduce_SequentialLineNonIntersection(Solver,
									Context,
									dec_vars_X[undecided[i]],
									dec_vars_Y[undecided[i]],
									dec_vars_T[undecided[i]],								    
									z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
									Line(point1, next_point1),
									dec_vars_X[undecided[j]],
									dec_vars_Y[undecided[j]],
									dec_vars_T[undecided[j]],								    
									z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
									Line(point2, next_point2));
				hidden_var_cnt += 2;
	    
				refined = true;
			    }
			}
		    }
		}	    
	    }
	    else
	    {
		if (dec_values_T[undecided[i]] < dec_values_T[undecided[j]])
		{
		    #ifdef DEBUG
		    {	
			printf("------------------------> Polygons: %d,%d\n", undecided[i], undecided[j]);
		    }
		    #endif
		    for (unsigned int poly1 = 0; poly1 < unreachable_polygons[undecided[i]].size(); ++poly1)
		    {
			for (unsigned int p1 = 0; p1 < unreachable_polygons[undecided[i]][poly1].points.size(); ++p1)
			{		
			    const Point &point1 = unreachable_polygons[undecided[i]][poly1].points[p1];
			    const Point &next_point1 = unreachable_polygons[undecided[i]][poly1].points[(p1 + 1) % unreachable_polygons[undecided[i]][poly1].points.size()];

			    for (unsigned int p2 = 0; p2 < polygons[undecided[j]].points.size(); ++p2)
			    {
				const Point &point2 = polygons[undecided[j]].points[p2];
				const Point &next_point2 = polygons[undecided[j]].points[(p2 + 1) % polygons[undecided[j]].points.size()];

				Vec2d intersection (0, 0);
		                #ifdef DEBUG
				{
				    printf("%d,%d - %ld,%ld,%ld,%ld  %ld,%ld,%ld,%ld\n", undecided[i], undecided[j],
					   dec_values_X[undecided[i]].numerator,
					   dec_values_X[undecided[i]].denominator,
					   dec_values_Y[undecided[i]].numerator,
					   dec_values_Y[undecided[i]].denominator,			       
					   dec_values_X[undecided[j]].numerator,
					   dec_values_X[undecided[j]].denominator,
					   dec_values_Y[undecided[j]].numerator,
					   dec_values_Y[undecided[j]].denominator);

				    printf("point1: %d,%d,%d,%d\n", point1.x(), point1.y(), next_point1.x(), next_point1.y());
				    printf("point2: %d,%d,%d,%d\n", point2.x(), point2.y(), next_point2.x(), next_point2.y());

				    printf("%ld,%ld\n", (dec_values_X[undecided[i]] + point1.x()).numerator, (dec_values_X[undecided[i]] + point1.x()).denominator);
				    printf("%ld,%ld\n", (dec_values_X[undecided[j]] + point2.x()).numerator, (dec_values_X[undecided[j]] + point2.x()).denominator);			
						
				    printf("testing iota: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					   (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
					   (dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
					   (dec_values_X[undecided[j]] + next_point2.x()).as_double(), (dec_values_Y[undecided[j]] + next_point2.y()).as_double());
				}
		                #endif

		    /* Seems not working, report an intersection even if there is none, using our own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[undecided[i]] + point1.x(), dec_values_Y[undecided[i]] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[undecided[j]] + point2.x(), dec_values_Y[undecided[j]] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
				if (lines_intersect((dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
						    next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						    (dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
						    next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				    
				{
			            #ifdef DEBUG
				    {
					printf("Intersecting: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					       (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					       (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
					       (dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
					       (dec_values_X[undecided[j]] + next_point2.x()).as_double(), (dec_values_Y[undecided[j]] + next_point2.y()).as_double());
				    }
			            #endif

			    /*
			    introduce_LineNonIntersection(Solver,
							  dec_vars_X[undecided[i]],
							  dec_vars_Y[undecided[i]],
							  z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
							  Line(point1, next_point1),
							  dec_vars_X[undecided[j]],
							  dec_vars_Y[undecided[j]],
							  z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
							  Line(point2, next_point2));
			    */

				    #ifdef DEBUG
				    {	
					printf("Hidden var: %d\n", hidden_var_cnt);
				    }
				    #endif
				    introduce_SequentialLineNonIntersection(Solver,
									    Context,
									    dec_vars_X[undecided[j]],
									    dec_vars_Y[undecided[j]],
									    dec_vars_T[undecided[j]],								    
									    z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
									    Line(point2, next_point2),
									    dec_vars_X[undecided[i]],
									    dec_vars_Y[undecided[i]],
									    dec_vars_T[undecided[i]],								    
									    z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
									    Line(point1, next_point1));
				    hidden_var_cnt += 2;
	    
				    refined = true;
				}
			    }
			}
		    }	    		    
		}
		else
		{
		    assert(false);
		}
	    }
	}
    }
    
    for (unsigned int i = 0; i < undecided.size(); ++i)
    {
	for (unsigned int j = 0; j < fixed.size(); ++j)
	{
	    if (dec_values_T[undecided[i]] > dec_values_T[fixed[j]])
	    {
		#ifdef DEBUG
		{	
		    printf("Fixo iota ------------------------> Polygons: %d,%d\n", undecided[i], fixed[j]);
		    printf("Times iota: %.3f, %.3f\n", dec_values_T[undecided[i]].as_double(), dec_values_T[fixed[j]].as_double());
		}
		#endif
		
		for (unsigned int p1 = 0; p1 < polygons[undecided[i]].points.size(); ++p1)
		{		
		    const Point &point1 = polygons[undecided[i]].points[p1];
		    const Point &next_point1 = polygons[undecided[i]].points[(p1 + 1) % polygons[undecided[i]].points.size()];

		    for (unsigned int poly2 = 0; poly2 < unreachable_polygons[fixed[j]].size(); ++poly2)
		    {
			for (unsigned int p2 = 0; p2 < unreachable_polygons[fixed[j]][poly2].points.size(); ++p2)
			{
			    const Point &point2 = unreachable_polygons[fixed[j]][poly2].points[p2];
			    const Point &next_point2 = unreachable_polygons[fixed[j]][poly2].points[(p2 + 1) % unreachable_polygons[fixed[j]][poly2].points.size()];
			    
			    Vec2d intersection(0, 0);
	 	            #ifdef DEBUG
			    {
				/*
				printf("testing kappa: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
				       (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
				       (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
				       (dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
				       (dec_values_X[fixed[j]] + next_point2.x()).as_double(), (dec_values_Y[fixed[j]] + next_point2.y()).as_double());
				*/
			    }
		            #endif

		    /* Seems not working, report an intersection even if there is none, using our own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[undecided[i]] + point1.x(), dec_values_Y[undecided[i]] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[fixed[j]] + point2.x(), dec_values_Y[fixed[j]] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
		    
			    if (lines_intersect((dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
						next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						(dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
						next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				
			    {
			        #ifdef DEBUG
				{
				    printf("Intersecting: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					   (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
					   (dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
					   (dec_values_X[fixed[j]] + next_point2.x()).as_double(), (dec_values_Y[fixed[j]] + next_point2.y()).as_double());

				    printf("testing iota decs: [%.3f,%.3f] [%.3f,%.3f]\n",
					   dec_values_X[undecided[i]].as_double(),
					   dec_values_Y[undecided[i]].as_double(),
					   dec_values_X[fixed[j]].as_double(),
					   dec_values_Y[fixed[j]].as_double());					   				    
				}
			        #endif

			/*
			introduce_LineNonIntersection(Solver,
						      dec_vars_X[undecided[i]],
						      dec_vars_Y[undecided[i]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
						      Line(point1, next_point1),
						      dec_vars_X[fixed[j]],
						      dec_vars_Y[fixed[j]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
						      Line(point2, next_point2));
			*/
				#ifdef DEBUG
				{	
				    printf("Hidden var iota: %d\n", hidden_var_cnt);
				}
				#endif
				/*
				int hidden_var1 = hidden_var_cnt++;
				int hidden_var2 = hidden_var_cnt++;				
				*/
				introduce_SequentialLineNonIntersectionAgainstFixedLine(Solver,
											Context,
											dec_vars_X[undecided[i]],
											dec_vars_Y[undecided[i]],
											dec_vars_T[undecided[i]],
											z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
											Line(point1, next_point1),
											dec_values_X[fixed[j]],
											dec_values_Y[fixed[j]],
											dec_values_T[fixed[j]],	
											z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
											Line(point2, next_point2));
				hidden_var_cnt += 2;				
				refined = true;
			    }
			}
		    }
		}
	    }
	    else
	    {
		if (dec_values_T[undecided[i]] < dec_values_T[fixed[j]])
		{
		    #ifdef DEBUG
		    {	
			printf("Times: %.3f, %.3f\n", dec_values_T[undecided[i]].as_double(), dec_values_T[fixed[j]].as_double());		    
			printf("Fixo kappa ------------------------> Polygons: %d,%d\n", undecided[i], fixed[j]);
		    }
		    #endif
		    
		    for (unsigned int poly1 = 0; poly1 < unreachable_polygons[undecided[i]].size(); ++poly1)
		    {
			for (unsigned int p1 = 0; p1 < unreachable_polygons[undecided[i]][poly1].points.size(); ++p1)
			{		
			    const Point &point1 = unreachable_polygons[undecided[i]][poly1].points[p1];
			    const Point &next_point1 = unreachable_polygons[undecided[i]][poly1].points[(p1 + 1) % unreachable_polygons[undecided[i]][poly1].points.size()];
		    
			    for (unsigned int p2 = 0; p2 < polygons[fixed[j]].points.size(); ++p2)
			    {
				const Point &point2 = polygons[fixed[j]].points[p2];
				const Point &next_point2 = polygons[fixed[j]].points[(p2 + 1) % polygons[fixed[j]].points.size()];

				Vec2d intersection(0, 0);
  	 	                #ifdef DEBUG
				{
				    printf("testing lambda: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					   (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
					   (dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
					   (dec_values_X[fixed[j]] + next_point2.x()).as_double(), (dec_values_Y[fixed[j]] + next_point2.y()).as_double());

				    printf("testing kappa decs: [%.3f,%.3f] [%.3f,%.3f]\n",
					   dec_values_X[undecided[i]].as_double(),
					   dec_values_Y[undecided[i]].as_double(),
					   dec_values_X[fixed[j]].as_double(),
					   dec_values_Y[fixed[j]].as_double());
				}
		                #endif

		    /* Seems not working, report an intersection even if there is none, using our own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[undecided[i]] + point1.x(), dec_values_Y[undecided[i]] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[fixed[j]] + point2.x(), dec_values_Y[fixed[j]] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
		    
				if (lines_intersect((dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
						    next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						    (dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
						    next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				    
				{
			            #ifdef DEBUG
				    {
					printf("Intersecting: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					       (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					       (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
					       (dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
					       (dec_values_X[fixed[j]] + next_point2.x()).as_double(), (dec_values_Y[fixed[j]] + next_point2.y()).as_double());
				    }
			            #endif

			/*
			introduce_LineNonIntersection(Solver,
						      dec_vars_X[undecided[i]],
						      dec_vars_Y[undecided[i]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
						      Line(point1, next_point1),
						      dec_vars_X[fixed[j]],
						      dec_vars_Y[fixed[j]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
						      Line(point2, next_point2));
			*/
				    #ifdef DEBUG
				    {	
					printf("Hidden var kappa: %d\n", hidden_var_cnt);
				    }
				    #endif
				    introduce_SequentialFixedLineNonIntersectionAgainstLine(Solver,
											    Context,
											    dec_values_X[fixed[j]],
											    dec_values_Y[fixed[j]],
											    dec_values_T[fixed[j]],
											    z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
											    Line(point2, next_point2),
											    dec_vars_X[undecided[i]],
											    dec_vars_Y[undecided[i]],
											    dec_vars_T[undecided[i]],
											    z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
											    Line(point1, next_point1));
				    hidden_var_cnt += 2;
				    
				    refined = true;
				}
			    }
			}
		    }	   
		}
		else
		{
                    #ifdef DEBUG
		    {	
			printf("Times: %.3f, %.3f (%d,%d)\n", dec_values_T[undecided[i]].as_double(), dec_values_T[fixed[j]].as_double(), undecided[i], fixed[j]);
			cout.flush();
		    }
		    #endif
		    assert(false);
		}
	    }	       
	}
    }
	
    return refined;
}


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
						   const std::vector<Slic3r::Polygon> &unreachable_polygons)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }    

    return refine_ConsequentialPolygonWeakNonoverlapping(Solver,
							 Context,
							 dec_vars_X,
							 dec_vars_Y,
							 dec_vars_T,
							 dec_values_X,
							 dec_values_Y,
							 dec_values_T,
							 fixed,
							 undecided,
							 polygons,
							 _unreachable_polygons);
}


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
						   const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons)
{
    bool refined = false;

    #ifdef DEBUG
    {	
	printf("Refining *************************** alpha\n");

	for (unsigned int i = 0; i < undecided.size(); ++i)
	{
	    printf("%d: %.3f,%.3f [%.3f]\n",
		   undecided[i],
		   dec_values_X[undecided[i]].as_double(),
		   dec_values_Y[undecided[i]].as_double(),
		   dec_values_T[undecided[i]].as_double()); 
	}
    }
    #endif
    
    for (unsigned int i = 0; i < undecided.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < undecided.size(); ++j)
	{	    
	    if (dec_values_T[undecided[i]].is_Positive() && dec_values_T[undecided[j]].is_Positive() && dec_values_T[undecided[i]] > dec_values_T[undecided[j]])
	    {
		#ifdef DEBUG
		{	
		    printf("------------------------> Polygons: %d,%d\n", undecided[i], undecided[j]);
		}
		#endif
		for (unsigned int p1 = 0; p1 < polygons[undecided[i]].points.size(); ++p1)
		{		
		    const Point &point1 = polygons[undecided[i]].points[p1];
		    const Point &next_point1 = polygons[undecided[i]].points[(p1 + 1) % polygons[undecided[i]].points.size()];

		    for (unsigned int poly2 = 0; poly2 < unreachable_polygons[undecided[j]].size(); ++poly2)
		    {
			for (unsigned int p2 = 0; p2 < unreachable_polygons[undecided[j]][poly2].points.size(); ++p2)
			{
			    const Point &point2 = unreachable_polygons[undecided[j]][poly2].points[p2];
			    const Point &next_point2 = unreachable_polygons[undecided[j]][poly2].points[(p2 + 1) % unreachable_polygons[undecided[j]][poly2].points.size()];

			    Vec2d intersection (0, 0);
		            #ifdef DEBUG
			    {
				printf("%d,%d - %ld,%ld,%ld,%ld  %ld,%ld,%ld,%ld\n", undecided[i], undecided[j],
				       dec_values_X[undecided[i]].numerator,
				       dec_values_X[undecided[i]].denominator,
				       dec_values_Y[undecided[i]].numerator,
				       dec_values_Y[undecided[i]].denominator,			       
				       dec_values_X[undecided[j]].numerator,
				       dec_values_X[undecided[j]].denominator,
				       dec_values_Y[undecided[j]].numerator,
				       dec_values_Y[undecided[j]].denominator);

				printf("point1: %d,%d,%d,%d\n", point1.x(), point1.y(), next_point1.x(), next_point1.y());
				printf("point2: %d,%d,%d,%d\n", point2.x(), point2.y(), next_point2.x(), next_point2.y());

				printf("%ld,%ld\n", (dec_values_X[undecided[i]] + point1.x()).numerator, (dec_values_X[undecided[i]] + point1.x()).denominator);
				printf("%ld,%ld\n", (dec_values_X[undecided[j]] + point2.x()).numerator, (dec_values_X[undecided[j]] + point2.x()).denominator);			
						
				printf("testing epsilon: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
				       (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
				       (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
				       (dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
				       (dec_values_X[undecided[j]] + next_point2.x()).as_double(), (dec_values_Y[undecided[j]] + next_point2.y()).as_double());
			    }
		            #endif

		    /* Seems not working, report an intersection even if there is none, using our own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[undecided[i]] + point1.x(), dec_values_Y[undecided[i]] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[undecided[j]] + point2.x(), dec_values_Y[undecided[j]] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
			    if (lines_intersect((dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
						next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						(dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
						next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				
			    {
			        #ifdef DEBUG
				{
				    printf("Intersecting: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					   (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
					   (dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
					   (dec_values_X[undecided[j]] + next_point2.x()).as_double(), (dec_values_Y[undecided[j]] + next_point2.y()).as_double());
				}
			        #endif

			    /*
			    introduce_LineNonIntersection(Solver,
							  dec_vars_X[undecided[i]],
							  dec_vars_Y[undecided[i]],
							  z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
							  Line(point1, next_point1),
							  dec_vars_X[undecided[j]],
							  dec_vars_Y[undecided[j]],
							  z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
							  Line(point2, next_point2));
			    */
			    
				introduce_ConsequentialLineNonIntersection(Solver,
									   Context,
									   dec_vars_X[undecided[i]],
									   dec_vars_Y[undecided[i]],
									   dec_vars_T[undecided[i]],								    
									   z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
									   Line(point1, next_point1),
									   dec_vars_X[undecided[j]],
									   dec_vars_Y[undecided[j]],
									   dec_vars_T[undecided[j]],								    
									   z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
									   Line(point2, next_point2));
				hidden_var_cnt += 2;
					    
				refined = true;
			    }
			}
		    }
		}	    
	    }
	    else
	    {
		if (dec_values_T[undecided[i]].is_Positive() && dec_values_T[undecided[j]].is_Positive() && dec_values_T[undecided[i]] < dec_values_T[undecided[j]])
		{
		    #ifdef DEBUG
		    {	
			printf("------------------------> Polygons: %d,%d\n", undecided[i], undecided[j]);
		    }
		    #endif
		    for (unsigned int poly1 = 0; poly1 < unreachable_polygons[undecided[i]].size(); ++poly1)
		    {
			for (unsigned int p1 = 0; p1 < unreachable_polygons[undecided[i]][poly1].points.size(); ++p1)
			{		
			    const Point &point1 = unreachable_polygons[undecided[i]][poly1].points[p1];
			    const Point &next_point1 = unreachable_polygons[undecided[i]][poly1].points[(p1 + 1) % unreachable_polygons[undecided[i]][poly1].points.size()];

			    for (unsigned int p2 = 0; p2 < polygons[undecided[j]].points.size(); ++p2)
			    {
				const Point &point2 = polygons[undecided[j]].points[p2];
				const Point &next_point2 = polygons[undecided[j]].points[(p2 + 1) % polygons[undecided[j]].points.size()];

				Vec2d intersection (0, 0);
		                #ifdef DEBUG
				{
				    printf("%d,%d - %ld,%ld,%ld,%ld  %ld,%ld,%ld,%ld\n", undecided[i], undecided[j],
					   dec_values_X[undecided[i]].numerator,
					   dec_values_X[undecided[i]].denominator,
					   dec_values_Y[undecided[i]].numerator,
					   dec_values_Y[undecided[i]].denominator,			       
					   dec_values_X[undecided[j]].numerator,
					   dec_values_X[undecided[j]].denominator,
					   dec_values_Y[undecided[j]].numerator,
					   dec_values_Y[undecided[j]].denominator);

				    printf("point1: %d,%d,%d,%d\n", point1.x(), point1.y(), next_point1.x(), next_point1.y());
				    printf("point2: %d,%d,%d,%d\n", point2.x(), point2.y(), next_point2.x(), next_point2.y());

				    printf("%ld,%ld\n", (dec_values_X[undecided[i]] + point1.x()).numerator, (dec_values_X[undecided[i]] + point1.x()).denominator);
				    printf("%ld,%ld\n", (dec_values_X[undecided[j]] + point2.x()).numerator, (dec_values_X[undecided[j]] + point2.x()).denominator);			
						
				    printf("testing iota: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					   (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
					   (dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
					   (dec_values_X[undecided[j]] + next_point2.x()).as_double(), (dec_values_Y[undecided[j]] + next_point2.y()).as_double());
				}
		                #endif

		    /* Seems not working, report an intersection even if there is none, using our own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[undecided[i]] + point1.x(), dec_values_Y[undecided[i]] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[undecided[j]] + point2.x(), dec_values_Y[undecided[j]] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
				if (lines_intersect((dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
						    next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						    (dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
						    next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				    
				{
			            #ifdef DEBUG
				    {
					printf("Intersecting: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					       (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					       (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
					       (dec_values_X[undecided[j]] + point2.x()).as_double(), (dec_values_Y[undecided[j]] + point2.y()).as_double(),
					       (dec_values_X[undecided[j]] + next_point2.x()).as_double(), (dec_values_Y[undecided[j]] + next_point2.y()).as_double());
				    }
			            #endif

			    /*
			    introduce_LineNonIntersection(Solver,
							  dec_vars_X[undecided[i]],
							  dec_vars_Y[undecided[i]],
							  z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
							  Line(point1, next_point1),
							  dec_vars_X[undecided[j]],
							  dec_vars_Y[undecided[j]],
							  z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
							  Line(point2, next_point2));
			    */

				    #ifdef DEBUG
				    {	
					printf("Hidden var: %d\n", hidden_var_cnt);
				    }
				    #endif
				    introduce_ConsequentialLineNonIntersection(Solver,
									       Context,
									       dec_vars_X[undecided[j]],
									       dec_vars_Y[undecided[j]],
									       dec_vars_T[undecided[j]],								    
									       z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
									       Line(point2, next_point2),
									       dec_vars_X[undecided[i]],
									       dec_vars_Y[undecided[i]],
									       dec_vars_T[undecided[i]],								    
									       z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
									       Line(point1, next_point1));
				    hidden_var_cnt += 2;
	    
				    refined = true;
				}
			    }
			}
		    }	    		    
		}
		else
		{
		    #ifdef DEBUG
		    {
			printf("The pair is not effective: %d,%d\n", undecided[i], undecided[j]);
		    }
		    #endif
		}
	    }
	}
    }

    
    for (unsigned int i = 0; i < undecided.size(); ++i)
    {
	for (unsigned int j = 0; j < fixed.size(); ++j)
	{	    
	    if (dec_values_T[undecided[i]].is_Positive() && dec_values_T[fixed[j]].is_Positive() && dec_values_T[undecided[i]] > dec_values_T[fixed[j]])
	    {
		#ifdef DEBUG
		{	
		    printf("Fixo iota ------------------------> Polygons: %d,%d\n", undecided[i], fixed[j]);
		    printf("Times iota: %.3f, %.3f\n", dec_values_T[undecided[i]].as_double(), dec_values_T[fixed[j]].as_double());
		}
		#endif
		for (unsigned int p1 = 0; p1 < polygons[undecided[i]].points.size(); ++p1)
		{		
		    const Point &point1 = polygons[undecided[i]].points[p1];
		    const Point &next_point1 = polygons[undecided[i]].points[(p1 + 1) % polygons[undecided[i]].points.size()];

		    for (unsigned int poly2 = 0; poly2 < unreachable_polygons[fixed[j]].size(); ++poly2)
		    {
			for (unsigned int p2 = 0; p2 < unreachable_polygons[fixed[j]][poly2].points.size(); ++p2)
			{
			    const Point &point2 = unreachable_polygons[fixed[j]][poly2].points[p2];
			    const Point &next_point2 = unreachable_polygons[fixed[j]][poly2].points[(p2 + 1) % unreachable_polygons[fixed[j]][poly2].points.size()];
			    
			    Vec2d intersection(0, 0);
	 	            #ifdef DEBUG
			    {
				/*
				printf("testing kappa: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
				       (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
				       (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
				       (dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
				       (dec_values_X[fixed[j]] + next_point2.x()).as_double(), (dec_values_Y[fixed[j]] + next_point2.y()).as_double());
				*/
			    }
		            #endif

		    /* Seems not working, report an intersection even if there is none, using our own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[undecided[i]] + point1.x(), dec_values_Y[undecided[i]] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[fixed[j]] + point2.x(), dec_values_Y[fixed[j]] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
		    
			    if (lines_intersect((dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
						next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						(dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
						next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				
			    {
			        #ifdef DEBUG
				{
				    printf("Intersecting: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					   (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
					   (dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
					   (dec_values_X[fixed[j]] + next_point2.x()).as_double(), (dec_values_Y[fixed[j]] + next_point2.y()).as_double());

				    printf("testing iota decs: [%.3f,%.3f] [%.3f,%.3f]\n",
					   dec_values_X[undecided[i]].as_double(),
					   dec_values_Y[undecided[i]].as_double(),
					   dec_values_X[fixed[j]].as_double(),
					   dec_values_Y[fixed[j]].as_double());					   				    
				}
			        #endif

			/*
			introduce_LineNonIntersection(Solver,
						      dec_vars_X[undecided[i]],
						      dec_vars_Y[undecided[i]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
						      Line(point1, next_point1),
						      dec_vars_X[fixed[j]],
						      dec_vars_Y[fixed[j]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
						      Line(point2, next_point2));
			*/
				#ifdef DEBUG
				{	
				    printf("Hidden var iota: %d\n", hidden_var_cnt);
				}
				#endif
				/*
				int hidden_var1 = hidden_var_cnt++;
				int hidden_var2 = hidden_var_cnt++;				
				*/
				introduce_ConsequentialLineNonIntersectionAgainstFixedLine(Solver,
											   Context,
											   dec_vars_X[undecided[i]],
											   dec_vars_Y[undecided[i]],
											   dec_vars_T[undecided[i]],
											   z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
											   Line(point1, next_point1),
											   dec_values_X[fixed[j]],
											   dec_values_Y[fixed[j]],
											   dec_values_T[fixed[j]],	
											   z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
											   Line(point2, next_point2));
				hidden_var_cnt += 2;				
				refined = true;
			    }
			}
		    }
		}
	    }
	    else
	    {	
		if (dec_values_T[undecided[i]].is_Positive() && dec_values_T[fixed[j]].is_Positive() && dec_values_T[undecided[i]] < dec_values_T[fixed[j]])
		{
		    #ifdef DEBUG
		    {	
			printf("Times: %.3f, %.3f\n", dec_values_T[undecided[i]].as_double(), dec_values_T[fixed[j]].as_double());		    
			printf("Fixo kappa ------------------------> Polygons: %d,%d\n", undecided[i], fixed[j]);
		    }
		    #endif
		    
		    for (unsigned int poly1 = 0; poly1 < unreachable_polygons[undecided[i]].size(); ++poly1)
		    {
			for (unsigned int p1 = 0; p1 < unreachable_polygons[undecided[i]][poly1].points.size(); ++p1)
			{		
			    const Point &point1 = unreachable_polygons[undecided[i]][poly1].points[p1];
			    const Point &next_point1 = unreachable_polygons[undecided[i]][poly1].points[(p1 + 1) % unreachable_polygons[undecided[i]][poly1].points.size()];
		    
			    for (unsigned int p2 = 0; p2 < polygons[fixed[j]].points.size(); ++p2)
			    {
				const Point &point2 = polygons[fixed[j]].points[p2];
				const Point &next_point2 = polygons[fixed[j]].points[(p2 + 1) % polygons[fixed[j]].points.size()];

				Vec2d intersection(0, 0);
  	 	                #ifdef DEBUG
				{
				    printf("testing lambda: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					   (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
					   (dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
					   (dec_values_X[fixed[j]] + next_point2.x()).as_double(), (dec_values_Y[fixed[j]] + next_point2.y()).as_double());

				    printf("testing kappa decs: [%.3f,%.3f] [%.3f,%.3f]\n",
					   dec_values_X[undecided[i]].as_double(),
					   dec_values_Y[undecided[i]].as_double(),
					   dec_values_X[fixed[j]].as_double(),
					   dec_values_Y[fixed[j]].as_double());
				}
		                #endif

		    /* Seems not working, report an intersection even if there is none, using our own lines_intersect() instead
		    if (Slic3r::Geometry::segment_segment_intersection(Vec2d(dec_values_X[undecided[i]] + point1.x(), dec_values_Y[undecided[i]] + point1.y()),
								       Vec2d(next_point1.x() - point1.x(), next_point1.y() - point1.y()),
								       Vec2d(dec_values_X[fixed[j]] + point2.x(), dec_values_Y[fixed[j]] + point2.y()),
								       Vec2d(next_point2.x() - point2.x(), next_point2.y() - point2.y()),
								       intersection))
		    */
		    
				if (lines_intersect((dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
						    next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						    (dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
						    next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				    
				{
			            #ifdef DEBUG
				    {
					printf("Intersecting: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					       (dec_values_X[undecided[i]] + point1.x()).as_double(), (dec_values_Y[undecided[i]] + point1.y()).as_double(),
					       (dec_values_X[undecided[i]] + next_point1.x()).as_double(), (dec_values_Y[undecided[i]] + next_point1.y()).as_double(),
					       (dec_values_X[fixed[j]] + point2.x()).as_double(), (dec_values_Y[fixed[j]] + point2.y()).as_double(),
					       (dec_values_X[fixed[j]] + next_point2.x()).as_double(), (dec_values_Y[fixed[j]] + next_point2.y()).as_double());
				    }
			            #endif

			/*
			introduce_LineNonIntersection(Solver,
						      dec_vars_X[undecided[i]],
						      dec_vars_Y[undecided[i]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
						      Line(point1, next_point1),
						      dec_vars_X[fixed[j]],
						      dec_vars_Y[fixed[j]],
						      z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt++)).c_str())),
						      Line(point2, next_point2));
			*/
				    #ifdef DEBUG
				    {	
					printf("Hidden var kappa: %d\n", hidden_var_cnt);
				    }
				    #endif
				    introduce_ConsequentialFixedLineNonIntersectionAgainstLine(Solver,
											       Context,
											       dec_values_X[fixed[j]],
											       dec_values_Y[fixed[j]],
											       dec_values_T[fixed[j]],
											       z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt)).c_str())),
											       Line(point2, next_point2),
											       dec_vars_X[undecided[i]],
											       dec_vars_Y[undecided[i]],
											       dec_vars_T[undecided[i]],
											       z3::expr(Context.real_const(("hidden-var-" + to_string(hidden_var_cnt + 1)).c_str())),
											       Line(point1, next_point1));
				    hidden_var_cnt += 2;
	    
				    refined = true;
				}
			    }
			}
		    }	   
		}
		else
		{
                    #ifdef DEBUG
		    {	
			printf("Times: %.3f, %.3f (%d,%d)\n", dec_values_T[undecided[i]].as_double(), dec_values_T[fixed[j]].as_double(), undecided[i], fixed[j]);
			cout.flush();
		    }
		    #endif
		    
                    #ifdef DEBUG
		    {
			printf("The pair is not effective: %d,%d\n", undecided[i], fixed[j]);
		    }
		    #endif
		}
	    }	       
	}
    }
	
    return refined;
}


/*----------------------------------------------------------------*/

bool check_PointsOutsidePolygons(const std::vector<Rational>                      &dec_values_X,
				 const std::vector<Rational>                      &dec_values_Y,	
				 const std::vector<Rational>                      &dec_values_T,
				 const std::vector<Slic3r::Polygon>               &polygons,
				 const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons)
{
    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    if (dec_values_T[i] > dec_values_T[j])
	    {
		for (unsigned int p1 = 0; p1 < polygons[i].points.size(); ++p1)
		{
		    const Point &point1 = polygons[i].points[p1];
		    
		    #ifdef DEBUG
		    {
			printf(">----------------\n");
		    }
		    #endif
		    
		    for (unsigned int poly2 = 0; poly2 < unreachable_polygons[j].size(); ++poly2)
		    {
			if (unreachable_polygons[j][poly2].points.size() >= 3)
			{
			    bool always_inside_halfplane = true;

		            #ifdef DEBUG
			    {
				printf("....\n");
			    }
		            #endif			    
			    
			    for (unsigned int p2 = 0; p2 < unreachable_polygons[j][poly2].points.size(); ++p2)
			    {
				const Point &point2 = unreachable_polygons[j][poly2].points[p2];
				const Point &next_point2 = unreachable_polygons[j][poly2].points[(p2 + 1) % unreachable_polygons[j][poly2].points.size()];
			    
				Line line(point2, next_point2);
				Vector normal = line.normal();
				
				double outside =  (normal.x() * (dec_values_X[i].as_double() + point1.x()))
				                + (normal.y() * (dec_values_Y[i].as_double() + point1.y()))
				                - (normal.x() * dec_values_X[j].as_double())
				                - (normal.x() * line.a.x())
					        - (normal.y() * dec_values_Y[j].as_double())
					        - (normal.y() * line.a.y());

				#ifdef DEBUG
				{
				    printf("Tested point: %d, %d\n", point1.x(), point1.y());
				    printf("Point: %d, %d\n", point2.x(), point2.y());
				    printf("Next point: %d, %d\n", next_point2.x(), next_point2.y());				    				    
				    printf("X[i]: %.3f, Y[i]: %.3f, X[j]: %.3f, Y[j]: %.3f\n", dec_values_X[i].as_double(), dec_values_Y[i].as_double(), dec_values_X[j].as_double(), dec_values_Y[j].as_double());				    
				    printf("Outside 1: %.3f\n", outside);
				}
				#endif

				if (outside > -EPSILON)
				{
				    always_inside_halfplane = false;
				    break;
				}
			    }
			    if (always_inside_halfplane)
			    {
				return false;
			    }
			}
		    }
		}
	    }
	    else if (dec_values_T[i] < dec_values_T[j])
	    {
		for (unsigned int p2 = 0; p2 < polygons[j].points.size(); ++p2)
		{
		    const Point &point2 = polygons[j].points[p2];

		    #ifdef DEBUG
		    {
			printf("<----------------\n");
		    }
		    #endif
				
		    for (unsigned int poly1 = 0; poly1 < unreachable_polygons[i].size(); ++poly1)
		    {	       	
			if (unreachable_polygons[i][poly1].points.size() >= 3)
			{
			    bool always_inside_halfplane = true;

		            #ifdef DEBUG
			    {
				printf("....\n");
			    }
		            #endif			    			    
						
			    for (unsigned int p1 = 0; p1 < unreachable_polygons[i][poly1].points.size(); ++p1)
			    {			    
				const Point &point1 = unreachable_polygons[i][poly1].points[p1];
				const Point &next_point1 = unreachable_polygons[i][poly1].points[(p1 + 1) % unreachable_polygons[i][poly1].points.size()];

				Line line(point1, next_point1);
				Vector normal = line.normal();
				
				double outside =  (normal.x() * (dec_values_X[j].as_double() + point2.x()))
				                + (normal.y() * (dec_values_Y[j].as_double() + point2.y()))		 
				                - (normal.x() * dec_values_X[i].as_double())
				                - (normal.x() * line.a.x())
					        - (normal.y() * dec_values_Y[i].as_double())
				                - (normal.y() * line.a.y());

				#ifdef DEBUG
				{
				    printf("Tested point: %.3f, %.3f\n", dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y());				    
				    printf("Point: %.3f, %.3f\n", point1.x() + dec_values_X[i].as_double(), point1.y() + dec_values_Y[i].as_double());
				    printf("Next point: %.3f, %.3f\n", next_point1.x() + dec_values_X[i].as_double(), next_point1.y() + dec_values_Y[i].as_double());				    
				    printf("X[i]: %.3f, Y[i]: %.3f, X[j]: %.3f, Y[j]: %.3f\n", dec_values_X[i].as_double(), dec_values_Y[i].as_double(), dec_values_X[j].as_double(), dec_values_Y[j].as_double());
				    printf("Outside 2: %.3f\n", outside);
				}
				#endif				

				if (outside > -EPSILON)
				{
				    always_inside_halfplane = false;
				    break;
				}				
			    }
			    if (always_inside_halfplane)
			    {
				return false;
			    }
			}
		    }
		}		
	    }
	    else
	    {
                #ifdef DEBUG
		{
		    printf("Time collision: %.3f, %.3f\n", dec_values_T[i].as_double(), dec_values_T[j].as_double());
		}
		#endif
		assert(false);		
	    }
   	}
    }
    #ifdef DEBUG
    {
	printf("Points DONE !!!\n");
    }
    #endif

    return true;
}


bool check_PolygonLineIntersections(const std::vector<Rational>                      &dec_values_X,
				    const std::vector<Rational>                      &dec_values_Y,	
				    const std::vector<Rational>                      &dec_values_T,
				    const std::vector<Slic3r::Polygon>               &polygons,
				    const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons)
{
    for (unsigned int i = 0; i < polygons.size() - 1; ++i)
    {
	for (unsigned int j = i + 1; j < polygons.size(); ++j)
	{
	    if (dec_values_T[i] > dec_values_T[j])
	    {
		for (unsigned int p1 = 0; p1 < polygons[i].points.size(); ++p1)
		{		
		    const Point &point1 = polygons[i].points[p1];
		    const Point &next_point1 = polygons[i].points[(p1 + 1) % polygons[i].points.size()];

		    for (unsigned int poly2 = 0; poly2 < unreachable_polygons[j].size(); ++poly2)
		    {
			#ifdef DEBUG
			{			    
			    printf("temporal: %.3f %.3f [ij: %d,%d]\n", dec_values_T[i].as_double(), dec_values_T[j].as_double(), i, j);
			    printf("proto X1: %ld, %ld, %ld\n", unreachable_polygons.size(), unreachable_polygons[j].size(), unreachable_polygons[j][poly2].points.size());
			}
			#endif
			       
			for (unsigned int p2 = 0; p2 < unreachable_polygons[j][poly2].points.size(); ++p2)
			{
			    const Point &point2 = unreachable_polygons[j][poly2].points[p2];
			    const Point &next_point2 = unreachable_polygons[j][poly2].points[(p2 + 1) % unreachable_polygons[j][poly2].points.size()];
			
                            #ifdef DEBUG
			    {
				printf("testing alpha %d %d (%d,%d): [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n", i, j, p1, p2,
				       dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
				       dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
				       dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
				       dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
			    }
		            #endif

			    if (lines_intersect_open(dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
						     next_point1.x() - point1.x(), next_point1.y() - point1.y(),
						     dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
						     next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				
			    {
			        #ifdef DEBUG
				{
				    printf("temps: [ij: %d,%d] [%.3f, %.3f]\n", i, j,
					   dec_values_T[i].as_double(),
					   dec_values_T[j].as_double());
						   
				    printf("dec_values: [%.3f, %.3f] [%.3f,%.3f]\n",
					   dec_values_X[i].as_double(),
					   dec_values_Y[i].as_double(),
					   dec_values_X[j].as_double(),
					   dec_values_Y[j].as_double());
				    
				    printf("intersect 1: %d [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   hidden_var_cnt, 				
					   dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
					   dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
					   dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
					   dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
				}
			        #endif

				return false;
			    }
			}
		    }
		}	    
	    }
	    else
	    {
		if (dec_values_T[i] < dec_values_T[j])
		{
		    for (unsigned int poly1 = 0; poly1 < unreachable_polygons[i].size(); ++poly1)
		    {
			for (unsigned int p1 = 0; p1 < unreachable_polygons[i][poly1].points.size(); ++p1)
			{
			    #ifdef DEBUG
			    {
				printf("proto2: %ld, %ld, %ld\n", unreachable_polygons.size(), unreachable_polygons[i].size(), unreachable_polygons[i][poly1].points.size());
			    }
			    #endif
			    
			    const Point &point1 = unreachable_polygons[i][poly1].points[p1];
			    const Point &next_point1 = unreachable_polygons[i][poly1].points[(p1 + 1) % unreachable_polygons[i][poly1].points.size()];
		    
			    for (unsigned int p2 = 0; p2 < polygons[j].points.size(); ++p2)
			    {
				const Point &point2 = polygons[j].points[p2];
				const Point &next_point2 = polygons[j].points[(p2 + 1) % polygons[j].points.size()];
			
                                #ifdef DEBUG
				{
				    printf("testing beta: [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					   dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
					   dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
					   dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
					   dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
				}
		                #endif

				if (lines_intersect_open(dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
							 next_point1.x() - point1.x(), next_point1.y() - point1.y(),
							 dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
							 next_point2.x() - point2.x(), next_point2.y() - point2.y()))
				
				{
			            #ifdef DEBUG
				    {
					printf("temps: [ij: %d,%d] [%.3f, %.3f]\n", i, j,
					       dec_values_T[i].as_double(),
					       dec_values_T[j].as_double());
						   
					printf("dec_values: [%.3f, %.3f] [%.3f,%.3f]\n",
					       dec_values_X[i].as_double(),
					       dec_values_Y[i].as_double(),
					       dec_values_X[j].as_double(),
					       dec_values_Y[j].as_double());
					
					printf("intersect 2: %d [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f] [%.3f,%.3f]\n",
					       hidden_var_cnt, 				
					       dec_values_X[i].as_double() + point1.x(), dec_values_Y[i].as_double() + point1.y(),
					       dec_values_X[i].as_double() + next_point1.x(), dec_values_Y[i].as_double() + next_point1.y(),
					       dec_values_X[j].as_double() + point2.x(), dec_values_Y[j].as_double() + point2.y(),
					       dec_values_X[j].as_double() + next_point2.x(), dec_values_Y[j].as_double() + next_point2.y());
				    }
			            #endif

				    return false;
				}
			    }
			}
		    }	    		    
		}
		else
		{
		    #ifdef DEBUG
		    {
			printf("Time collision: %.3f, %.3f\n", dec_values_T[i].as_double(), dec_values_T[j].as_double());
		    }
		    #endif
		    assert(false);
		}
	    }
	}
    }

    #ifdef DEBUG
    {
	printf("Lines DONE !!!\n");
    }
    #endif    
    
    return true;
}


/*----------------------------------------------------------------*/

void extract_DecisionValuesFromModel(const z3::model     &Model,
				     const string_map    &dec_var_names_map,
				     std::vector<double> &dec_values_X,
				     std::vector<double> &dec_values_Y)
{
    for (unsigned int i = 0; i < Model.size(); ++i)
    {
	double value = Model.get_const_interp(Model[i]).as_double();

	switch (Model[i].name().str()[0])
	{
	case 'X':
	{	    
	    string_map::const_iterator var_item = dec_var_names_map.find(Model[i].name().str());	
	    if (var_item != dec_var_names_map.end())
	    {
		dec_values_X[var_item->second] = value;
	    }
	    break;
	}
	case 'Y':
	{
	    string_map::const_iterator var_item = dec_var_names_map.find(Model[i].name().str());	
	    if (var_item != dec_var_names_map.end())
	    {
		dec_values_Y[var_item->second] = value;
	    }	    
	    break;
	}
	default:
	{
	    break;
	}
	}
    }
}


void extract_DecisionValuesFromModel(const z3::model     &Model,
				     z3::context         &Context,				     
				     const string_map    &dec_var_names_map,
				     z3::expr_vector     &dec_values_X,
				     z3::expr_vector     &dec_values_Y)
{
    z3::expr_vector unordered_values_X(Context);
    z3::expr_vector unordered_values_Y(Context);    
    
    std::map<int, int> value_indices_X;
    std::map<int, int> value_indices_Y;    
    
    for (unsigned int i = 0; i < Model.size(); ++i)
    {
	z3::expr value = Model.get_const_interp(Model[i]);

	#ifdef DEBUG
	{	
	    printf("extracted: %.3f (%s)\n", value.as_double(), Model[i].name().str().c_str());
	}
	#endif

	switch (Model[i].name().str()[0])
	{
	case 'X':
	{
	    string_map::const_iterator var_item = dec_var_names_map.find(Model[i].name().str());
	    if (var_item != dec_var_names_map.end())
	    {
		value_indices_X[var_item->second] = i;
		unordered_values_X.push_back(z3::expr(Context.real_val(value.numerator().as_int64(), value.denominator().as_int64())));
		
		#ifdef DEBUG
		{	
		    printf("saved: %.3f\n", unordered_values_X.back()->as_double());
		}
		#endif
	    }
	    break;
	}
	case 'Y':
	{
	    string_map::const_iterator var_item = dec_var_names_map.find(Model[i].name().str());	
	    if (var_item != dec_var_names_map.end())
	    {
		value_indices_Y[var_item->second] = i;
		unordered_values_Y.push_back(z3::expr(Context.real_val(value.numerator().as_int64(), value.denominator().as_int64())));

		#ifdef DEBUG
		{	
		    printf("saved: %.3f\n", unordered_values_Y.back()->as_double());
		}
		#endif
	    }
	    break;
	}
	default:
	{
	    break;
	}
	}
    }

    dec_values_X.resize(0);
    dec_values_Y.resize(0);

    for (std::map<int, int>::const_iterator value = value_indices_X.begin(); value != value_indices_X.end(); ++value)
    {
	dec_values_X.push_back(unordered_values_X[value->second]);
    }
    for (std::map<int, int>::const_iterator value = value_indices_Y.begin(); value != value_indices_Y.end(); ++value)
    {
	dec_values_Y.push_back(unordered_values_Y[value->second]);
    }    
}


void extract_DecisionValuesFromModel(const z3::model       &Model,
				     const string_map      &dec_var_names_map,
				     std::vector<Rational> &dec_values_X,
				     std::vector<Rational> &dec_values_Y)
{
    for (unsigned int i = 0; i < Model.size(); ++i)
    {
	z3::expr value = Model.get_const_interp(Model[i]);

	#ifdef DEBUG
	{	
	    printf("extracted: %.3f (%s)\n", value.as_double(), Model[i].name().str().c_str());
	}
	#endif

	switch (Model[i].name().str()[0])
	{
	case 'X':
	{
	    string_map::const_iterator var_item = dec_var_names_map.find(Model[i].name().str());
	    if (var_item != dec_var_names_map.end())
	    {
		#ifdef DEBUG
		{	
		    printf("saving X: %d <-- %.3f, %ld, %ld\n", var_item->second, value.as_double(), value.numerator().as_int64(), value.denominator().as_int64());
		}
		#endif
		//dec_values_X[var_item->second] = Rational(value.numerator().as_int64(), value.denominator().as_int64());
		dec_values_X[var_item->second] = Rational(value);
		//dec_values_X[var_item->second] = value;
	    }
	    break;
	}
	case 'Y':
	{
	    string_map::const_iterator var_item = dec_var_names_map.find(Model[i].name().str());	
	    if (var_item != dec_var_names_map.end())
	    {
		#ifdef DEBUG
		{	
		    printf("saving Y: %d <-- %.3f, %ld, %ld\n", var_item->second, value.as_double(), value.numerator().as_int64(), value.denominator().as_int64());
		}
		#endif
		//printf("saving: %d <-- %.3f\n", var_item->second, value.as_double());
		//dec_values_Y[var_item->second] = Rational(value.numerator().as_int64(), value.denominator().as_int64());
		dec_values_Y[var_item->second] = Rational(value);
	    }
	    break;
	}
	default:
	{
	    break;
	}
	}
    }
}


void extract_DecisionValuesFromModel(const z3::model       &Model,
				     const string_map      &dec_var_names_map,
				     std::vector<Rational> &dec_values_X,
				     std::vector<Rational> &dec_values_Y,
				     std::vector<Rational> &dec_values_T)
{
    for (unsigned int i = 0; i < Model.size(); ++i)
    {
	z3::expr value = Model.get_const_interp(Model[i]);
        #ifdef DEBUG
	{	
	    printf("extracted: %.3f (%s)\n", value.as_double(), Model[i].name().str().c_str());
	}
	#endif

	switch (Model[i].name().str()[0])
	{
	case 'X':
	{
	    string_map::const_iterator var_item = dec_var_names_map.find(Model[i].name().str());
	    if (var_item != dec_var_names_map.end())
	    {
		#ifdef DEBUG
		{	
		    printf("saving X: %d <-- %.3f, %ld, %ld\n", var_item->second, value.as_double(), value.numerator().as_int64(), value.denominator().as_int64());
		}
		#endif
		//dec_values_X[var_item->second] = Rational(value.numerator().as_int64(), value.denominator().as_int64());
		dec_values_X[var_item->second] = Rational(value);
		//dec_values_X[var_item->second] = value;
	    }
	    break;
	}
	case 'Y':
	{
	    string_map::const_iterator var_item = dec_var_names_map.find(Model[i].name().str());	
	    if (var_item != dec_var_names_map.end())
	    {
		#ifdef DEBUG
		{	
		    printf("saving Y: %d <-- %.3f, %ld, %ld\n", var_item->second, value.as_double(), value.numerator().as_int64(), value.denominator().as_int64());
		}
		#endif
		//printf("saving: %d <-- %.3f\n", var_item->second, value.as_double());
		//dec_values_Y[var_item->second] = Rational(value.numerator().as_int64(), value.denominator().as_int64());
		dec_values_Y[var_item->second] = Rational(value);
	    }
	    break;
	}
	case 'T':
	{
	    string_map::const_iterator var_item = dec_var_names_map.find(Model[i].name().str());	
	    if (var_item != dec_var_names_map.end())
	    {
		#ifdef DEBUG
		{	
		    printf("saving T: %d <-- %.3f, %ld, %ld\n", var_item->second, value.as_double(), value.numerator().as_int64(), value.denominator().as_int64());
		}
		#endif
		//printf("saving: %d <-- %.3f\n", var_item->second, value.as_double());
		//dec_values_T[var_item->second] = Rational(value.numerator().as_int64(), value.denominator().as_int64());
		dec_values_T[var_item->second] = Rational(value);
	    }
	    break;
	}	
	default:
	{
	    break;
	}
	}
    }
}


void build_WeakPolygonNonoverlapping(z3::solver                         &Solver,
				     z3::context                        &Context,
				     const std::vector<Slic3r::Polygon> &polygons,
				     z3::expr_vector                    &dec_vars_X,
				     z3::expr_vector                    &dec_vars_Y,
				     std::vector<double>                &dec_values_X,
				     std::vector<double>                &dec_values_Y,
				     string_map                         &dec_var_names_map)
{
    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	string name = "X_pos-" + to_string(i);
	
	dec_vars_X.push_back(z3::expr(Context.real_const(name.c_str())));
	dec_var_names_map[name] = i;
    }

    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	string name = "Y_pos-" + to_string(i);
	
	dec_vars_Y.push_back(z3::expr(Context.real_const(name.c_str())));
	dec_var_names_map[name] = i;	
    }

    dec_values_X.resize(polygons.size(), 0.0);
    dec_values_Y.resize(polygons.size(), 0.0);

    introduce_PolygonWeakNonoverlapping(Solver,
					Context,
					dec_vars_X,
					dec_vars_Y,
					polygons);
}


void build_WeakPolygonNonoverlapping(z3::solver                         &Solver,
				     z3::context                        &Context,
				     const std::vector<Slic3r::Polygon> &polygons,
				     z3::expr_vector                    &dec_vars_X,
				     z3::expr_vector                    &dec_vars_Y,
				     z3::expr_vector                    &dec_values_X,
				     z3::expr_vector                    &dec_values_Y,
				     string_map                         &dec_var_names_map)
{
    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	string name = "X_pos-" + to_string(i);
	
	dec_vars_X.push_back(z3::expr(Context.real_const(name.c_str())));
	dec_var_names_map[name] = i;
    }

    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	string name = "Y_pos-" + to_string(i);
	
	dec_vars_Y.push_back(z3::expr(Context.real_const(name.c_str())));
	dec_var_names_map[name] = i;
    }

    dec_values_X.resize(polygons.size());
    dec_values_Y.resize(polygons.size());    

    introduce_PolygonWeakNonoverlapping(Solver,
					Context,
					dec_vars_X,
					dec_vars_Y,
					polygons);
}


void build_WeakPolygonNonoverlapping(z3::solver                         &Solver,
				     z3::context                        &Context,
				     const std::vector<Slic3r::Polygon> &polygons,
				     z3::expr_vector                    &dec_vars_X,
				     z3::expr_vector                    &dec_vars_Y,
				     std::vector<Rational>              &dec_values_X,
				     std::vector<Rational>              &dec_values_Y,
				     const std::vector<int>             &fixed,
				     const std::vector<int>             &undecided,
				     string_map                         &dec_var_names_map)
{
    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	string name = "X_pos-" + to_string(i);
	
	dec_vars_X.push_back(z3::expr(Context.real_const(name.c_str())));
	dec_var_names_map[name] = i;
    }

    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	string name = "Y_pos-" + to_string(i);
	
	dec_vars_Y.push_back(z3::expr(Context.real_const(name.c_str())));
	dec_var_names_map[name] = i;	
    }

    introduce_PolygonWeakNonoverlapping(Solver,
					Context,
					dec_vars_X,
					dec_vars_Y,
					dec_values_X,
					dec_values_Y,
					fixed,
					undecided,					
					polygons);
}


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
					       string_map                         &dec_var_names_map)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }    

    build_SequentialWeakPolygonNonoverlapping(Solver,
					      Context,
					      polygons,
					      _unreachable_polygons,
					      dec_vars_X,
					      dec_vars_Y,
					      dec_vars_T,
					      dec_values_X,
					      dec_values_Y,
					      dec_values_T,
					      fixed,
					      undecided,
					      dec_var_names_map);
}

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
					       string_map                                       &dec_var_names_map)
{
    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	string name = "X_pos-" + to_string(i);
	
	dec_vars_X.push_back(z3::expr(Context.real_const(name.c_str())));
	dec_var_names_map[name] = i;
    }

    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	string name = "Y_pos-" + to_string(i);
	
	dec_vars_Y.push_back(z3::expr(Context.real_const(name.c_str())));
	dec_var_names_map[name] = i;	
    }

    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	string name = "T_time-" + to_string(i);
	
	dec_vars_T.push_back(z3::expr(Context.real_const(name.c_str())));
	dec_var_names_map[name] = i;	
    }
    
    introduce_SequentialPolygonWeakNonoverlapping(Solver,
						  Context,
						  dec_vars_X,
						  dec_vars_Y,
						  dec_vars_T,					
						  dec_values_X,
						  dec_values_Y,
						  dec_values_T,					
						  fixed,
						  undecided,					
						  polygons,
						  unreachable_polygons);
}


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
						  string_map                         &dec_var_names_map)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }    

    build_ConsequentialWeakPolygonNonoverlapping(Solver,
						 Context,
						 polygons,
						 _unreachable_polygons,
						 dec_vars_X,
						 dec_vars_Y,
						 dec_vars_T,
						 dec_values_X,
						 dec_values_Y,
						 dec_values_T,
						 fixed,
						 undecided,
						 dec_var_names_map);
}


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
						  string_map                                       &dec_var_names_map)
{
    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	string name = "X_pos-" + to_string(i);
	
	dec_vars_X.push_back(z3::expr(Context.real_const(name.c_str())));
	dec_var_names_map[name] = i;
    }

    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	string name = "Y_pos-" + to_string(i);
	
	dec_vars_Y.push_back(z3::expr(Context.real_const(name.c_str())));
	dec_var_names_map[name] = i;	
    }

    for (unsigned int i = 0; i < polygons.size(); ++i)
    {
	string name = "T_time-" + to_string(i);
	
	dec_vars_T.push_back(z3::expr(Context.real_const(name.c_str())));
	dec_var_names_map[name] = i;	
    }
    
    introduce_ConsequentialPolygonWeakNonoverlapping(Solver,
						     Context,
						     dec_vars_X,
						     dec_vars_Y,
						     dec_vars_T,					
						     dec_values_X,
						     dec_values_Y,
						     dec_values_T,					
						     fixed,
						     undecided,					
						     polygons,
						     unreachable_polygons);
}


bool optimize_WeakPolygonNonoverlapping(z3::solver                         &Solver,
					z3::context                        &Context,
					const SolverConfiguration          &solver_configuration,
					const z3::expr_vector              &dec_vars_X,
					const z3::expr_vector              &dec_vars_Y,
					std::vector<double>                &dec_values_X,
					std::vector<double>                &dec_values_Y,
					const string_map                   &dec_var_names_map,
					const std::vector<Slic3r::Polygon> &polygons)
{

    z3::set_param("timeout", solver_configuration.optimization_timeout.c_str());
    
    int last_solvable_bounding_box_size = -1;

    int maximum_bounding_box_size = MAX(solver_configuration.x_plate_bounding_box_size, solver_configuration.y_plate_bounding_box_size);
    
    for (int bounding_box_size = maximum_bounding_box_size; bounding_box_size > solver_configuration.minimum_bounding_box_size;
	 bounding_box_size -= solver_configuration.bounding_box_size_optimization_step)
    {
	#ifdef DEBUG
	{
	    printf("BB: %d\n", bounding_box_size);
	}
	#endif
	z3::expr_vector bounding_box_assumptions(Context);

	for (unsigned int i = 0; i < polygons.size(); ++i)
	{
	    assume_BedBoundingBox(dec_vars_X[i], dec_vars_Y[i], polygons[i], bounding_box_size, bounding_box_size, bounding_box_assumptions);
	}
	    
	bool sat = false;

	switch (Solver.check(bounding_box_assumptions))
	{
	case z3::sat:
	{	    
	    sat = true;	    
	    break;
	}
	case z3::unsat:	
	{
	    sat = false;	    
	    break;
	}
	case z3::unknown:
	{
	    sat = false;
	    break;
	}
	default:
	{
	    break;
	}
	}
	
	if (sat)
	{		
	    z3::model Model(Solver.get_model());
	    
	    extract_DecisionValuesFromModel(Model,
					    dec_var_names_map,
					    dec_values_X,
					    dec_values_Y);
	    
	    while (true)
	    {		
		bool refined = refine_PolygonWeakNonoverlapping(Solver,
								Context,
								dec_vars_X,
								dec_vars_Y,
								dec_values_X,
								dec_values_Y,
								polygons);
		
		bool refined_sat = false;
		
		if (refined)
		{
		    switch (Solver.check(bounding_box_assumptions))
		    {
		    case z3::sat:
		    {	    
			refined_sat = true;	    
			break;
		    }
		    case z3::unsat:	
		    {
			refined_sat = false;	    
			break;
		    }
		    case z3::unknown:
		    {
			refined_sat = false;
			break;
		    }
		    default:
		    {
			break;
		    }
		    }
		    
		    if (refined_sat)
		    {
			z3::model Model(Solver.get_model());
			
			extract_DecisionValuesFromModel(Model,
							dec_var_names_map,
							dec_values_X,
							dec_values_Y);

                        #ifdef DEBUG
			{
			    printf("Refined positions:\n");
			    for (unsigned int i = 0; i < polygons.size(); ++i)
			    {
				printf("  %.3f, %.3f\n", dec_values_X[i], dec_values_Y[i]);
			    }
			}
			#endif
		    }
		    else
		    {
			break;
		    }
		}
		else
		{
		    last_solvable_bounding_box_size = bounding_box_size;
		    break;
		}
	    }
	}
	else
	{
	    break;
	}		    
    }
    if (last_solvable_bounding_box_size > 0)
    {
	return true;
    }

    return false;
}


bool optimize_WeakPolygonNonoverlapping(z3::solver                         &Solver,
					z3::context                        &Context,
					const SolverConfiguration          &solver_configuration,
					const z3::expr_vector              &dec_vars_X,
					const z3::expr_vector              &dec_vars_Y,
					z3::expr_vector                    &dec_values_X,
					z3::expr_vector                    &dec_values_Y,
					const string_map                   &dec_var_names_map,
					const std::vector<Slic3r::Polygon> &polygons)
{
    Z3_global_param_set("timeout", solver_configuration.optimization_timeout.c_str());        
    int last_solvable_bounding_box_size = -1;

    int maximum_bounding_box_size = MAX(solver_configuration.x_plate_bounding_box_size, solver_configuration.y_plate_bounding_box_size);
    
    for (int bounding_box_size = maximum_bounding_box_size; bounding_box_size > solver_configuration.minimum_bounding_box_size;
	 bounding_box_size -= solver_configuration.bounding_box_size_optimization_step)
    {
        #ifdef DEBUG
	{
	    printf("BB: %d\n", bounding_box_size);
	}
	#endif
	z3::expr_vector bounding_box_assumptions(Context);

	for (unsigned int i = 0; i < polygons.size(); ++i)
	{
	    assume_BedBoundingBox(dec_vars_X[i], dec_vars_Y[i], polygons[i], bounding_box_size, bounding_box_size, bounding_box_assumptions);
	}
	    
	bool sat = false;

	switch (Solver.check(bounding_box_assumptions))
	{
	case z3::sat:
	{	    
	    sat = true;	    
	    break;
	}
	case z3::unsat:	
	{
	    sat = false;	    
	    break;
	}
	case z3::unknown:
	{
	    sat = false;
	    break;
	}
	default:
	{
	    break;
	}
	}
	
	if (sat)
	{		
	    z3::model Model(Solver.get_model());
	    
	    extract_DecisionValuesFromModel(Model,
					    Context,
					    dec_var_names_map,
					    dec_values_X,
					    dec_values_Y);
	    
	    while (true)
	    {		
		bool refined = refine_PolygonWeakNonoverlapping(Solver,
								Context,
								dec_vars_X,
								dec_vars_Y,
								dec_values_X,
								dec_values_Y,
								polygons);
		
		bool refined_sat = false;
		
		if (refined)
		{
		    switch (Solver.check(bounding_box_assumptions))
		    {
		    case z3::sat:
		    {	    
			refined_sat = true;	    
			break;
		    }
		    case z3::unsat:	
		    {
			refined_sat = false;	    
			break;
		    }
		    case z3::unknown:
		    {
			refined_sat = false;
			break;
		    }
		    default:
		    {
			break;
		    }
		    }
		    
		    if (refined_sat)
		    {
			z3::model Model(Solver.get_model());
			
			extract_DecisionValuesFromModel(Model,
							Context,
							dec_var_names_map,
							dec_values_X,
							dec_values_Y);

                        #ifdef DEBUG
			{
			    printf("Refined positions:\n");
			    for (unsigned int i = 0; i < polygons.size(); ++i)
			    {
				printf("  %.3f, %.3f\n", dec_values_X[i].as_double(), dec_values_Y[i].as_double());
			    }
			}
			#endif
		    }
		    else
		    {
			break;
		    }
		}
		else
		{
		    last_solvable_bounding_box_size = bounding_box_size;
		    break;
		}
	    }
	}
	else
	{
	    break;
	}		    
    }
    if (last_solvable_bounding_box_size > 0)
    {
	return true;
    }

    return false;
}


bool optimize_WeakPolygonNonoverlapping(z3::solver                         &Solver,
					z3::context                        &Context,
					const SolverConfiguration          &solver_configuration,
					const z3::expr_vector              &dec_vars_X,
					const z3::expr_vector              &dec_vars_Y,
					std::vector<Rational>              &dec_values_X,
					std::vector<Rational>              &dec_values_Y,
					const string_map                   &dec_var_names_map,
					const std::vector<Slic3r::Polygon> &polygons)
{

    z3::set_param("timeout", solver_configuration.optimization_timeout.c_str());
    int last_solvable_bounding_box_size = -1;

    int maximum_bounding_box_size = MAX(solver_configuration.x_plate_bounding_box_size, solver_configuration.y_plate_bounding_box_size);
    
    for (int bounding_box_size = maximum_bounding_box_size; bounding_box_size > solver_configuration.minimum_bounding_box_size;
	 bounding_box_size -= solver_configuration.bounding_box_size_optimization_step)
    {
	#ifdef DEBUG
	{
	    printf("BB: %d\n", bounding_box_size);
	}
	#endif
	z3::expr_vector bounding_box_assumptions(Context);

	for (unsigned int i = 0; i < polygons.size(); ++i)
	{
	    assume_BedBoundingBox(dec_vars_X[i], dec_vars_Y[i], polygons[i], bounding_box_size, bounding_box_size, bounding_box_assumptions);
	}
	    
	bool sat = false;

	switch (Solver.check(bounding_box_assumptions))
	{
	case z3::sat:
	{	    
	    sat = true;	    
	    break;
	}
	case z3::unsat:	
	{
	    sat = false;	    
	    break;
	}
	case z3::unknown:
	{
	    sat = false;
	    break;
	}
	default:
	{
	    break;
	}
	}
	
	if (sat)
	{		
	    z3::model Model(Solver.get_model());
	    
	    extract_DecisionValuesFromModel(Model,
					    dec_var_names_map,
					    dec_values_X,
					    dec_values_Y);
	    
	    while (true)
	    {		
		bool refined = refine_PolygonWeakNonoverlapping(Solver,
								Context,
								dec_vars_X,
								dec_vars_Y,
								dec_values_X,
								dec_values_Y,
								polygons);
		
		bool refined_sat = false;
		
		if (refined)
		{
		    switch (Solver.check(bounding_box_assumptions))
		    {
		    case z3::sat:
		    {	    
			refined_sat = true;	    
			break;
		    }
		    case z3::unsat:	
		    {
			refined_sat = false;	    
			break;
		    }
		    case z3::unknown:
		    {
			refined_sat = false;
			break;
		    }
		    default:
		    {
			break;
		    }
		    }
		    
		    if (refined_sat)
		    {
			z3::model Model(Solver.get_model());
			
			extract_DecisionValuesFromModel(Model,
							dec_var_names_map,
							dec_values_X,
							dec_values_Y);

                        #ifdef DEBUG
			{
			    printf("Refined positions:\n");
			    for (unsigned int i = 0; i < polygons.size(); ++i)
			    {
				printf("  %ld/%ld, %ld/%ld\n", dec_values_X[i].numerator, dec_values_X[i].denominator, dec_values_Y[i].numerator, dec_values_Y[i].denominator);
			    }
			}
			#endif
		    }
		    else
		    {
			break;
		    }
		}
		else
		{
		    last_solvable_bounding_box_size = bounding_box_size;
		    break;
		}
	    }
	}
	else
	{
	    break;
	}		    
    }
    if (last_solvable_bounding_box_size > 0)
    {
	return true;
    }

    return false;
}


/*----------------------------------------------------------------*/

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
					const std::vector<Slic3r::Polygon> &polygons)
{
    z3::set_param("timeout", solver_configuration.optimization_timeout.c_str());        
    int last_solvable_bounding_box_size = -1;

    int maximum_bounding_box_size = MAX(solver_configuration.x_plate_bounding_box_size, solver_configuration.y_plate_bounding_box_size);
    
    for (int bounding_box_size = maximum_bounding_box_size; bounding_box_size > solver_configuration.minimum_bounding_box_size;
	 bounding_box_size -= solver_configuration.bounding_box_size_optimization_step)
    {
	#ifdef DEBUG
	{
	    printf("BB: %d\n", bounding_box_size);
	}
	#endif
	
	z3::expr_vector bounding_box_assumptions(Context);

	for (unsigned int i = 0; i < undecided.size(); ++i)
	{
	    assume_BedBoundingBox(dec_vars_X[undecided[i]], dec_vars_Y[undecided[i]], polygons[undecided[i]], bounding_box_size, bounding_box_size, bounding_box_assumptions);
	}

	bool sat = false;

	switch (Solver.check(bounding_box_assumptions))
	{
	case z3::sat:
	{	    
	    sat = true;	    
	    break;
	}
	case z3::unsat:	
	{
	    sat = false;	    
	    break;
	}
	case z3::unknown:
	{
	    sat = false;
	    break;
	}
	default:
	{
	    break;
	}
	}
	
	if (sat)
	{
	    z3::model Model(Solver.get_model());

	    extract_DecisionValuesFromModel(Model,
					    Context,
					    dec_var_names_map,
					    dec_values_X,
					    dec_values_Y);
	    
	    while (true)
	    {
		bool refined = refine_PolygonWeakNonoverlapping(Solver,
								Context,
								dec_vars_X,
								dec_vars_Y,
								dec_values_X,
								dec_values_Y,
								fixed,
								undecided,
								polygons);
		
		bool refined_sat = false;
		
		if (refined)
		{
		    switch (Solver.check(bounding_box_assumptions))
		    {
		    case z3::sat:
		    {	    
			refined_sat = true;	    
			break;
		    }
		    case z3::unsat:	
		    {
			refined_sat = false;	    
			break;
		    }
		    case z3::unknown:
		    {
			refined_sat = false;
			break;
		    }
		    default:
		    {
			break;
		    }
		    }
		    
		    if (refined_sat)
		    {
			z3::model Model(Solver.get_model());
			
			extract_DecisionValuesFromModel(Model,
							Context,
							dec_var_names_map,
							dec_values_X,
							dec_values_Y);

                        #ifdef DEBUG
			{
			    printf("Refined positions:\n");
			    for (unsigned int i = 0; i < undecided.size(); ++i)
			    {
				printf("  %.3f, %.3f\n", dec_values_X[undecided[i]].as_double(), dec_values_Y[undecided[i]].as_double());
			    }
			}
			#endif
		    }
		    else
		    {
			break;
		    }
		}
		else
		{
		    last_solvable_bounding_box_size = bounding_box_size;
		    break;
		}
	    }
	}
	else
	{
	    break;
	}		    
    }
    if (last_solvable_bounding_box_size > 0)
    {
	return true;
    }

    return false;    
}


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
					const std::vector<Slic3r::Polygon> &polygons)
{
    z3::set_param("timeout", solver_configuration.optimization_timeout.c_str());        
    int last_solvable_bounding_box_size = -1;
    
    std::vector<Rational> local_dec_values_X = dec_values_X;
    std::vector<Rational> local_dec_values_Y = dec_values_Y;

    int maximum_bounding_box_size = MAX(solver_configuration.x_plate_bounding_box_size, solver_configuration.y_plate_bounding_box_size);
    
    for (int bounding_box_size = maximum_bounding_box_size; bounding_box_size > solver_configuration.minimum_bounding_box_size;
	 bounding_box_size -= solver_configuration.bounding_box_size_optimization_step)
    {
	#ifdef DEBUG
	{
	    printf("BBX: %d\n", bounding_box_size);
	}
	#endif
	
	z3::expr_vector bounding_box_assumptions(Context);

	for (unsigned int i = 0; i < undecided.size(); ++i)
	{
	    assume_BedBoundingBox(dec_vars_X[undecided[i]], dec_vars_Y[undecided[i]], polygons[undecided[i]], bounding_box_size, bounding_box_size, bounding_box_assumptions);
	}

	bool sat = false;

	switch (Solver.check(bounding_box_assumptions))
	{
	case z3::sat:
	{
	    sat = true;	    
	    break;
	}
	case z3::unsat:	
	{
	    sat = false;	    
	    break;
	}
	case z3::unknown:
	{
	    sat = false;
	    break;
	}
	default:
	{
	    break;
	}
	}
	
	if (sat)
	{
	    z3::model Model(Solver.get_model());

	    extract_DecisionValuesFromModel(Model,
					    dec_var_names_map,
					    local_dec_values_X,
					    local_dec_values_Y);
	    
	    while (true)
	    {
		bool refined = refine_PolygonWeakNonoverlapping(Solver,
								Context,
								dec_vars_X,
								dec_vars_Y,
								local_dec_values_X,
								local_dec_values_Y,
								fixed,
								undecided,
								polygons);
	       
		
		if (refined)
		{
		    bool refined_sat = false;

		    switch (Solver.check(bounding_box_assumptions))
		    {
		    case z3::sat:
		    {
			refined_sat = true;	    
			break;
		    }
		    case z3::unsat:	
		    {
			refined_sat = false;	    
			break;
		    }
		    case z3::unknown:
		    {
			refined_sat = false;
			break;
		    }
		    default:
		    {
			break;
		    }
		    }
		    
		    if (refined_sat)
		    {
			z3::model Model(Solver.get_model());
			
			extract_DecisionValuesFromModel(Model,
							dec_var_names_map,
							local_dec_values_X,
							local_dec_values_Y);

                        #ifdef DEBUG
			{
			    printf("Refined positions:\n");
			    for (unsigned int i = 0; i < undecided.size(); ++i)
			    {
				printf("  %ld/%ld, %ld/%ld\n",
				       local_dec_values_X[undecided[i]].numerator,
				       local_dec_values_X[undecided[i]].denominator,
				       local_dec_values_Y[undecided[i]].numerator,
				       local_dec_values_Y[undecided[i]].denominator);
			    }
			}
			#endif
		    }
		    else
		    {
			break;
		    }
		}
		else
		{
		    last_solvable_bounding_box_size = bounding_box_size;

		    dec_values_X = local_dec_values_X;
		    dec_values_Y = local_dec_values_Y;
		    break;
		}
	    }
	}
	else
	{
	    break;
	}		    
    }
    if (last_solvable_bounding_box_size > 0)
    {
	return true;
    }

    return false;    
}



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
						  const std::vector<Slic3r::Polygon> &unreachable_polygons)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }    
    
    return optimize_SequentialWeakPolygonNonoverlapping(Solver,
							Context,
							solver_configuration,
							dec_vars_X,
							dec_vars_Y,
							dec_vars_T,
							dec_values_X,
							dec_values_Y,
							dec_values_T,
							fixed,
							undecided,
							dec_var_names_map,
							polygons,
							_unreachable_polygons);
}


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
						  const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons)
{
    z3::set_param("timeout", solver_configuration.optimization_timeout.c_str());
    
    int last_solvable_bounding_box_size = -1;

    std::vector<Rational> local_dec_values_X = dec_values_X;
    std::vector<Rational> local_dec_values_Y = dec_values_Y;
    std::vector<Rational> local_dec_values_T = dec_values_T;

    int maximum_bounding_box_size = MAX(solver_configuration.x_plate_bounding_box_size, solver_configuration.y_plate_bounding_box_size);
    
    for (int bounding_box_size = maximum_bounding_box_size; bounding_box_size > solver_configuration.minimum_bounding_box_size;
	 bounding_box_size -= solver_configuration.bounding_box_size_optimization_step)
    {
	#ifdef DEBUG
	{
	    printf("BBX: %d\n", bounding_box_size);
	}
	#endif
	
	z3::expr_vector bounding_box_assumptions(Context);

	for (unsigned int i = 0; i < undecided.size(); ++i)
	{
	    assume_BedBoundingBox(dec_vars_X[undecided[i]], dec_vars_Y[undecided[i]], polygons[undecided[i]], bounding_box_size, bounding_box_size, bounding_box_assumptions);
	}

	bool sat = false;

	#ifdef DEBUG
	{
	    printf("Solving 11 ...\n");
	}
	#endif
	
	switch (Solver.check(bounding_box_assumptions))
	{
	case z3::sat:
	{
	    sat = true;	    
	    break;
	}
	case z3::unsat:	
	{
	    sat = false;	    
	    break;
	}
	case z3::unknown:
	{
	    sat = false;
	    break;
	}
	default:
	{
	    break;
	}
	}	
	#ifdef DEBUG
	{
	    printf("Solving 11 ... finished\n");
	}
	#endif
	
	if (sat)
	{
	    #ifdef DEBUG
	    {
		printf("First SAT\n");
	    }
	    #endif
	    z3::model Model(Solver.get_model());

	    extract_DecisionValuesFromModel(Model,
					    dec_var_names_map,
					    local_dec_values_X,
					    local_dec_values_Y,
					    local_dec_values_T);

	    while (true)
	    {
		bool refined = refine_SequentialPolygonWeakNonoverlapping(Solver,
									  Context,
									  dec_vars_X,
									  dec_vars_Y,
									  dec_vars_T,
									  local_dec_values_X,
									  local_dec_values_Y,
									  local_dec_values_T,
									  fixed,
									  undecided,
									  polygons,
									  unreachable_polygons);
	       
		
		if (refined)
		{
		    bool refined_sat = false;

		    #ifdef DEBUG
		    {
			printf("Solving 12 ...\n");
		    }
		    #endif
		    
		    switch (Solver.check(bounding_box_assumptions))
		    {
		    case z3::sat:
		    {
			refined_sat = true;	    
			break;
		    }
		    case z3::unsat:	
		    {
			refined_sat = false;	    
			break;
		    }
		    case z3::unknown:
		    {
			refined_sat = false;
			break;
		    }
		    default:
		    {
			break;
		    }
		    }
		    #ifdef DEBUG
		    {
			printf("Solving 12 ... finished: %d\n", refined_sat);
		    }
		    #endif

		    /*
		    printf("Printing solver status:\n");
		    cout << Solver << "\n";
		    */
		    		    
		    if (refined_sat)
		    {
			#ifdef DEBUG
			{
			    printf("Refined SAT\n");
			}
			#endif
			z3::model Model(Solver.get_model());

			/*
			printf("Printing smt status:\n");
			cout << Solver.to_smt2() << "\n";	    			
			*/
			
			extract_DecisionValuesFromModel(Model,
							dec_var_names_map,
							local_dec_values_X,
							local_dec_values_Y,
							local_dec_values_T);

                        #ifdef DEBUG
			{
			    printf("Refined positions:\n");
			    for (unsigned int i = 0; i < undecided.size(); ++i)
			    {
				printf("  i:%d, undecided[i]:%d: %ld/%ld (%.3f), %ld/%ld (%.3f) [%ld/%ld (%.3f)]\n",
				       i, undecided[i],
				       local_dec_values_X[undecided[i]].numerator,
				       local_dec_values_X[undecided[i]].denominator,
				       local_dec_values_X[undecided[i]].as_double(),  
				       local_dec_values_Y[undecided[i]].numerator,
				       local_dec_values_Y[undecided[i]].denominator,
				       local_dec_values_Y[undecided[i]].as_double(),				       
				       local_dec_values_T[undecided[i]].numerator,
				       local_dec_values_T[undecided[i]].denominator,
				       local_dec_values_T[undecided[i]].as_double());
			    }
			}
			#endif
		    }
		    else
		    {
			#ifdef DEBUG
			{
			    printf("Refined UNSAT\n");
			}
			#endif
			if (last_solvable_bounding_box_size > 0)
			{
			    return true;
			}
			else
			{
			    return false;
			}
		    }
		}
		else
		{
		    last_solvable_bounding_box_size = bounding_box_size;

		    dec_values_X = local_dec_values_X;
		    dec_values_Y = local_dec_values_Y;
		    dec_values_T = local_dec_values_T;		    
		    break;
		}
	    }
	}
	else
	{
	    #ifdef DEBUG
	    {
		printf("First UNSAT\n");
	    }
	    #endif
	    if (last_solvable_bounding_box_size > 0)
	    {
		return true;
	    }
	    else
	    {
		return false;
	    }
	}		    
    }
    return false;
}


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
							  const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons)
{
    z3::set_param("timeout", solver_configuration.optimization_timeout.c_str());    
    //z3::set_param("parallel.enable", "true");
    
    int last_solvable_bounding_box_size = -1;

    std::vector<Rational> local_dec_values_X = dec_values_X;
    std::vector<Rational> local_dec_values_Y = dec_values_Y;
    std::vector<Rational> local_dec_values_T = dec_values_T;

    int box_min_x = 0;
    int box_max_x = solver_configuration.x_plate_bounding_box_size;
    int box_min_y = 0;
    int box_max_y = solver_configuration.y_plate_bounding_box_size;
    
    while (box_min_x < box_max_x && box_min_y < box_max_y)
    {
	#ifdef DEBUG
	{
	    printf("BBX: %d, %d, %d, %d\n", box_min_x, box_max_x, box_min_y, box_max_y);
	}
	#endif
	
	z3::expr_vector bounding_box_assumptions(Context);

	for (unsigned int i = 0; i < undecided.size(); ++i)
	{
	    assume_BedBoundingBox(dec_vars_X[undecided[i]],
				   dec_vars_Y[undecided[i]],
				   polygons[undecided[i]],
				   box_min_x,
				   box_min_y,
				   box_max_x,
				   box_max_y,
				   bounding_box_assumptions);
	}

	bool sat = false;

	#ifdef DEBUG
	{
	    printf("Solving 11 ...\n");
	}
	#endif
//	Solver.add(bounding_box_assumptions);
	
	switch (Solver.check(bounding_box_assumptions))
	{
	case z3::sat:
	{
	    sat = true;	    
	    break;
	}
	case z3::unsat:	
	{
	    sat = false;	    
	    break;
	}
	case z3::unknown:
	{
	    sat = false;
	    break;
	}
	default:
	{
	    break;
	}
	}
	
	if (sat)
	{
	    #ifdef DEBUG
	    {
		printf("First SAT\n");
	    }
	    #endif
	    z3::model Model(Solver.get_model());

	    extract_DecisionValuesFromModel(Model,
					    dec_var_names_map,
					    local_dec_values_X,
					    local_dec_values_Y,
					    local_dec_values_T);

	    while (true)
	    {
		bool refined = refine_SequentialPolygonWeakNonoverlapping(Solver,
									  Context,
									  dec_vars_X,
									  dec_vars_Y,
									  dec_vars_T,
									  local_dec_values_X,
									  local_dec_values_Y,
									  local_dec_values_T,
									  fixed,
									  undecided,
									  polygons,
									  unreachable_polygons);
	       
		
		if (refined)
		{
		    bool refined_sat = false;

		    #ifdef DEBUG
		    {
			printf("Solving 12 ...\n");
		    }
		    #endif

		    /*
		    z3::check_result result = Solver.check(bounding_box_assumptions);
		    printf("check_result: %d\n", result);
		    */
		    switch (Solver.check(bounding_box_assumptions))
		    {
		    case z3::sat:
		    {
			refined_sat = true;	    
			break;
		    }
		    case z3::unsat:	
		    {
			refined_sat = false;	    
			break;
		    }
		    case z3::unknown:
		    {
			refined_sat = false;
			break;
		    }
		    default:
		    {
			break;
		    }
		    }

                    #ifdef DEBUG
		    {
			printf("Solving 12 ... finished: %d\n", refined_sat);
		    }
		    #endif

		    /*
		    printf("Printing solver status:\n");
		    cout << Solver << "\n";
		    */
		    		    
		    if (refined_sat)
		    {
			#ifdef DEBUG
			{
			    printf("Refined SAT\n");
			}
			#endif
			z3::model Model(Solver.get_model());

			/*
			printf("Printing smt status:\n");
			cout << Solver.to_smt2() << "\n";	    			
			*/
			
			extract_DecisionValuesFromModel(Model,
							dec_var_names_map,
							local_dec_values_X,
							local_dec_values_Y,
							local_dec_values_T);

                        #ifdef DEBUG
			{
			    printf("Refined positions:\n");
			    for (unsigned int i = 0; i < undecided.size(); ++i)
			    {
				printf("  i:%d, undecided[i]:%d: %ld/%ld (%.3f), %ld/%ld (%.3f) [%ld/%ld (%.3f)]\n",
				       i, undecided[i],
				       local_dec_values_X[undecided[i]].numerator,
				       local_dec_values_X[undecided[i]].denominator,
				       local_dec_values_X[undecided[i]].as_double(),  
				       local_dec_values_Y[undecided[i]].numerator,
				       local_dec_values_Y[undecided[i]].denominator,
				       local_dec_values_Y[undecided[i]].as_double(),				       
				       local_dec_values_T[undecided[i]].numerator,
				       local_dec_values_T[undecided[i]].denominator,
				       local_dec_values_T[undecided[i]].as_double());
			    }
			}
			#endif
		    }
		    else
		    {
			#ifdef DEBUG
			{
			    printf("Refined UNSAT\n");
			}
			#endif
			if (last_solvable_bounding_box_size > 0)
			{
			    return true;
			}
			else
			{
			    return false;
			}
		    }
		}
		else
		{
		    last_solvable_bounding_box_size = box_max_x;

		    dec_values_X = local_dec_values_X;
		    dec_values_Y = local_dec_values_Y;
		    dec_values_T = local_dec_values_T;		    
		    break;
		}
	    }
	}
	else
	{
	    #ifdef DEBUG
	    {
		printf("First UNSAT\n");
	    }
	    #endif
	    if (last_solvable_bounding_box_size > 0)
	    {
		return true;
	    }
	    else
	    {
		return false;
	    }
	}


	box_min_x += solver_configuration.bounding_box_size_optimization_step;
	box_max_x -= solver_configuration.bounding_box_size_optimization_step;	
	
	box_min_y += solver_configuration.bounding_box_size_optimization_step;
	box_max_y -= solver_configuration.bounding_box_size_optimization_step;

	if (box_min_x >= box_max_x || box_min_y >= box_max_y)
	{
	    break;
	}	
    }
    return false;
}


bool checkArea_SequentialWeakPolygonNonoverlapping(coord_t                                           box_min_x,
						   coord_t                                           box_min_y,
						   coord_t                                           box_max_x,
						   coord_t                                           box_max_y,
						   const std::vector<int>                           &fixed,
						   const std::vector<int>                           &undecided,
						   const std::vector<Slic3r::Polygon>               &polygons,
						   const std::vector<std::vector<Slic3r::Polygon> > &SEQ_UNUSED(unreachable_polygons))
{

    double check_area = (box_max_x - box_min_x) * (box_max_y - box_min_y);
    double polygon_area = calc_PolygonArea(fixed, undecided, polygons);

    #ifdef DEBUG
    {
	printf("Fast checkging for box: %d, %d, %d, %d\n", box_min_x, box_min_y, box_max_x, box_max_y);    
	printf("Check area: %.3f\n", check_area);
	printf("Polygon area: %.3f\n", polygon_area);
    }
    #endif
    
    if (check_area < polygon_area)
    {
	return false;
    }
    
    return true;
}


bool checkExtens_SequentialWeakPolygonNonoverlapping(coord_t                                           box_min_x,
						     coord_t                                           box_min_y,
						     coord_t                                           box_max_x,
						     coord_t                                           box_max_y,
						     std::vector<Rational>                            &dec_values_X,
						     std::vector<Rational>                            &dec_values_Y,
						     const std::vector<int>                           &fixed,
						     const std::vector<int>                           &undecided,
						     const std::vector<Slic3r::Polygon>               &polygons,
						     const std::vector<std::vector<Slic3r::Polygon> > &SEQ_UNUSED(unreachable_polygons))
{
    double min_X, max_X, min_Y, max_Y;

    if (!fixed.empty())
    {
	BoundingBox polygon_box = get_extents(polygons[fixed[0]]);
		    
	min_X = dec_values_X[fixed[0]].as_double() + polygon_box.min.x();
	min_Y = dec_values_Y[fixed[0]].as_double() + polygon_box.min.y();
	
	max_X = dec_values_X[fixed[0]].as_double() + polygon_box.max.x();
	max_Y = dec_values_Y[fixed[0]].as_double() + polygon_box.max.y();		    
    
	for (unsigned int i = 1; i < fixed.size(); ++i)
	{	
	    BoundingBox polygon_box = get_extents(polygons[fixed[i]]);

	    double next_min_X = dec_values_X[fixed[i]].as_double() + polygon_box.min.x();
			
	    if (next_min_X < min_X)
	    {
		min_X = next_min_X;
	    }
	    double next_min_Y = dec_values_Y[fixed[i]].as_double() + polygon_box.min.y();

	    if (next_min_Y < min_Y)
	    {
		min_Y = next_min_Y;
	    }

	    double next_max_X = dec_values_X[fixed[i]].as_double() + polygon_box.max.x();

	    if (next_max_X > max_X)
	    {
		max_X = next_max_X;
	    }	    
	    double next_max_Y = dec_values_Y[fixed[i]].as_double() + polygon_box.max.y();

	    if (next_max_Y > max_Y)
	    {
		max_Y = next_max_Y;
	    }	    	    
	}
	
	#ifdef DEBUG
	{
	    printf("Box:%d,%d,%d,%d\n", box_min_x, box_max_x, box_min_y, box_max_y);
	    printf("Fix:%.3f,%.3f,%.3f,%.3f\n", min_X, max_X, min_Y, max_Y);
	}
	#endif
	
	if (min_X < box_min_x || max_X > box_max_x || min_Y < box_min_y || max_Y > box_max_y)
	{
	    return false;
	}
    }
    return true;
}


bool optimize_SequentialWeakPolygonNonoverlappingBinaryCentered(z3::solver                                       &Solver,
								z3::context                                      &Context,
								const SolverConfiguration                        &solver_configuration,
								coord_t                                          &box_half_x_max,
								coord_t                                          &box_half_y_max,
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
								const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons)
{
    z3::set_param("timeout", solver_configuration.optimization_timeout.c_str());    
    //z3::set_param("parallel.enable", "true");
    
    coord_t last_solvable_bounding_box_size = -1;

    std::vector<Rational> local_dec_values_X = dec_values_X;
    std::vector<Rational> local_dec_values_Y = dec_values_Y;
    std::vector<Rational> local_dec_values_T = dec_values_T;        

    coord_t half_x_min = 0;
    coord_t half_x_max = box_half_x_max;

    coord_t half_y_min = 0;
    coord_t half_y_max = box_half_y_max;

    while ((half_x_max - half_x_min) > 1 && (half_y_max - half_y_min) > 1)
    {
	#ifdef DEBUG
	{
	    printf("Halves: %d, %d, %d, %d\n", half_x_min, half_x_max, half_y_min, half_y_max);
	}
	#endif

	bool size_solvable = false;
	
	z3::expr_vector bounding_box_assumptions(Context);

	coord_t box_min_x = (half_x_max + half_x_min) / 2;
	coord_t box_max_x = solver_configuration.x_plate_bounding_box_size - box_min_x;
	coord_t box_min_y = (half_y_max + half_y_min) / 2;
	coord_t box_max_y = solver_configuration.y_plate_bounding_box_size - box_min_y;

	#ifdef DEBUG
	{
	    printf("BBX: %d, %d, %d, %d\n", box_min_x, box_max_x, box_min_y, box_max_y);
	}
	#endif

	for (unsigned int i = 0; i < undecided.size(); ++i)
	{
	    assume_BedBoundingBox(dec_vars_X[undecided[i]],
				  dec_vars_Y[undecided[i]],
				  polygons[undecided[i]],
				  box_min_x,
				  box_min_y,
				  box_max_x,
				  box_max_y,
				  bounding_box_assumptions);
	}	

	bool sat = false;

	#ifdef DEBUG
	{
	    printf("Solving 11 ...\n");
	}
	#endif
//	Solver.add(bounding_box_assumptions);

	if (checkArea_SequentialWeakPolygonNonoverlapping(box_min_x,
							  box_min_y,
							  box_max_x,
							  box_max_y,
							  fixed,
							  undecided,	       
							  polygons,
							  unreachable_polygons))
	{
	    switch (Solver.check(bounding_box_assumptions))
	    {
	    case z3::sat:
	    {
		sat = true;	    
		break;
	    }
	    case z3::unsat:	
	    {
		sat = false;	    
		break;
	    }
	    case z3::unknown:
	    {
		sat = false;
		break;
	    }
	    default:
	    {
		break;
	    }
	    }
	}
	else
	{
	    sat = false;
	}

	if (sat)
	{
	    #ifdef DEBUG
	    {
		printf("First SAT\n");
	    }
	    #endif
	    z3::model Model(Solver.get_model());

	    extract_DecisionValuesFromModel(Model,
					    dec_var_names_map,
					    local_dec_values_X,
					    local_dec_values_Y,
					    local_dec_values_T);

	    while (true)
	    {
		bool refined = refine_SequentialPolygonWeakNonoverlapping(Solver,
									  Context,
									  dec_vars_X,
									  dec_vars_Y,
									  dec_vars_T,
									  local_dec_values_X,
									  local_dec_values_Y,
									  local_dec_values_T,
									  fixed,
									  undecided,
									  polygons,
									  unreachable_polygons);
		
		if (refined)
		{
		    bool refined_sat = false;

		    #ifdef DEBUG
		    {
			printf("Solving 12 ...\n");
		    }
		    #endif

		    if (checkArea_SequentialWeakPolygonNonoverlapping(box_min_x,
								      box_min_y,
								      box_max_x,
								      box_max_y,
								      fixed,
								      undecided,	       
								      polygons,
								      unreachable_polygons))
		    {
			switch (Solver.check(bounding_box_assumptions))
			{
			case z3::sat:
			{
			    refined_sat = true;	    
			    break;
			}
			case z3::unsat:	
			{
			    refined_sat = false;	    
			    break;
			}
			case z3::unknown:
			{
			    refined_sat = false;
			    break;
			}
			default:
			{
			    break;
			}
			}
		    }
		    else
		    {
			refined_sat = false;
		    }
		    
		    #ifdef DEBUG
		    {
			printf("Solving 12 ... finished: %d\n", refined_sat);
		    }
		    #endif

		    /*
		    printf("Printing solver status:\n");
		    cout << Solver << "\n";
		    */
		    		    
		    if (refined_sat)
		    {
			#ifdef DEBUG
			{
			    printf("Refined SAT\n");
			}
			#endif
			z3::model Model(Solver.get_model());

			/*
			printf("Printing smt status:\n");
			cout << Solver.to_smt2() << "\n";	    			
			*/
			
			extract_DecisionValuesFromModel(Model,
							dec_var_names_map,
							local_dec_values_X,
							local_dec_values_Y,
							local_dec_values_T);

                        #ifdef DEBUG
			{
			    printf("Refined positions:\n");
			    for (unsigned int i = 0; i < undecided.size(); ++i)
			    {
				printf("  i:%d, undecided[i]:%d: %ld/%ld (%.3f), %ld/%ld (%.3f) [%ld/%ld (%.3f)]\n",
				       i, undecided[i],
				       local_dec_values_X[undecided[i]].numerator,
				       local_dec_values_X[undecided[i]].denominator,
				       local_dec_values_X[undecided[i]].as_double(),  
				       local_dec_values_Y[undecided[i]].numerator,
				       local_dec_values_Y[undecided[i]].denominator,
				       local_dec_values_Y[undecided[i]].as_double(),				       
				       local_dec_values_T[undecided[i]].numerator,
				       local_dec_values_T[undecided[i]].denominator,
				       local_dec_values_T[undecided[i]].as_double());
			    }
			}
			#endif
		    }
		    else
		    {
			#ifdef DEBUG
			{
			    printf("Refined UNSAT\n");
			}
			#endif
			size_solvable = false;
			break;
		    }
		}
		else
		{
		    last_solvable_bounding_box_size = box_max_x;

		    dec_values_X = local_dec_values_X;
		    dec_values_Y = local_dec_values_Y;
		    dec_values_T = local_dec_values_T;		    

		    size_solvable = true;
		    break;
		}
	    }
	}
	else
	{
            #ifdef DEBUG
	    {
		printf("First UNSAT\n");
	    }
	    #endif
	    
	    if (last_solvable_bounding_box_size > 0)
	    {
		size_solvable = false;
	    }
	    else
	    {
		size_solvable = false;		
	    }
	}

	coord_t half_x_med = (half_x_max + half_x_min) / 2;
	coord_t half_y_med = (half_y_max + half_y_min) / 2;
		    
	if (size_solvable)
	{
	    #ifdef DEBUG
	    {
		printf("Solvable\n");
	    }
	    #endif
	    half_x_min = half_x_med;
	    half_y_min = half_y_med;	    
	}
	else
	{
	    #ifdef DEBUG
	    {
		printf("Unsolvable\n");
	    }
	    #endif
	    half_x_max = half_x_med;
	    half_y_max = half_y_med;	    
	}
	#ifdef DEBUG
	{
	    printf("Halves augmented: X:[%d,%d] Y:[%d,%d]\n", half_x_min, half_x_max, half_y_min, half_y_max);
	}
	#endif       
    }
   
    if (last_solvable_bounding_box_size > 0)
    {
	box_half_x_max = half_x_max;
	box_half_y_max = half_y_max;
	
	return true;
    }    
    return false;
}


#ifdef PROFILE
double init_cumul = 0.0;
clock_t init_start, init_finish;

double refine_cumul = 0.0;
clock_t refine_start, refine_finish;

double recheck_SAT_cumul = 0.0;
double recheck_UNSAT_cumul = 0.0;
double recheck_INDET_cumul = 0.0;
clock_t recheck_start, recheck_finish;
#endif

bool optimize_ConsequentialWeakPolygonNonoverlappingBinaryCentered(z3::solver                                       &Solver,
								   z3::context                                      &Context,
								   const SolverConfiguration                        &solver_configuration,
								   coord_t                                          &box_half_x_max,
								   coord_t                                          &box_half_y_max,
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
								   const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons,
								   const z3::expr_vector                            &presence_constraints)
{
    z3::set_param("timeout", solver_configuration.optimization_timeout.c_str());    
    //z3::set_param("parallel.enable", "true");
    
    coord_t last_solvable_bounding_box_size = -1;

    std::vector<Rational> local_dec_values_X = dec_values_X;
    std::vector<Rational> local_dec_values_Y = dec_values_Y;
    std::vector<Rational> local_dec_values_T = dec_values_T;

    coord_t half_x_min = 0;
    coord_t half_x_max = box_half_x_max;

    coord_t half_y_min = 0;
    coord_t half_y_max = box_half_y_max;
            
    while ((half_x_max - half_x_min) > 1 && (half_y_max - half_y_min) > 1)
    {
	#ifdef DEBUG
	{
	    printf("Halves: %d, %d, %d, %d\n", half_x_min, half_x_max, half_y_min, half_y_max);
	}
	#endif

	bool size_solvable = false;
	
	z3::expr_vector bounding_box_assumptions(Context);

	coord_t box_min_x = (half_x_max + half_x_min) / 2;
	coord_t box_max_x = solver_configuration.x_plate_bounding_box_size - box_min_x;
	coord_t box_min_y = (half_y_max + half_y_min) / 2;
	coord_t box_max_y = solver_configuration.y_plate_bounding_box_size - box_min_y;

	#ifdef DEBUG
	{
	    printf("BBX: %d, %d, %d, %d\n", box_min_x, box_max_x, box_min_y, box_max_y);
	}
	#endif

	z3::expr_vector	complete_assumptions(Context);

	for (unsigned int i = 0; i < presence_constraints.size(); ++i)
	{
	    complete_assumptions.push_back(presence_constraints[i]);
	}

	for (unsigned int i = 0; i < undecided.size(); ++i)
	{
	    assume_BedBoundingBox(dec_vars_X[undecided[i]],
				  dec_vars_Y[undecided[i]],
				  polygons[undecided[i]],
				  box_min_x,
				  box_min_y,
				  box_max_x,
				  box_max_y,
				  complete_assumptions);
	}	

	bool sat = false;

	#ifdef DEBUG
	{
	    printf("Solving 11 ...\n");
	}
	#endif

	if (checkArea_SequentialWeakPolygonNonoverlapping(box_min_x,
							  box_min_y,
							  box_max_x,
							  box_max_y,
							  fixed,
							  undecided,	       
							  polygons,
							  unreachable_polygons))
	{
	    #ifdef PROFILE
	    {
		init_start = clock();
	    }
	    #endif
	    
	    switch (Solver.check(complete_assumptions))
	    {
	    case z3::sat:
	    {
		sat = true;	    
		break;
	    }
	    case z3::unsat:	
	    {
		sat = false;	    
		break;
	    }
	    case z3::unknown:
	    {
		sat = false;
		break;
	    }
	    default:
	    {
		break;
	    }
	    }

	    #ifdef PROFILE
	    {
		init_finish = clock();
		init_cumul += (init_finish - init_start) / (double)CLOCKS_PER_SEC;
	    }
	    #endif
	}
	else
	{
	    sat = false;
	}

	if (sat)
	{
	    #ifdef DEBUG
	    {
		printf("First SAT\n");
	    }
	    #endif
	    z3::model Model(Solver.get_model());

	    extract_DecisionValuesFromModel(Model,
					    dec_var_names_map,
					    local_dec_values_X,
					    local_dec_values_Y,
					    local_dec_values_T);

	    while (true)
	    {
		#ifdef PROFILE
		{
		    refine_start = clock();
		}
		#endif
		bool refined = refine_ConsequentialPolygonWeakNonoverlapping(Solver,
									     Context,
									     dec_vars_X,
									     dec_vars_Y,
									     dec_vars_T,
									     local_dec_values_X,
									     local_dec_values_Y,
									     local_dec_values_T,
									     fixed,
									     undecided,
									     polygons,
									     unreachable_polygons);
		#ifdef PROFILE
		{
		    refine_finish = clock();
		    refine_cumul += (refine_finish - refine_start) / (double)CLOCKS_PER_SEC;
		}
		#endif
	       
		if (refined)
		{
		    bool refined_sat = false;

		    #ifdef DEBUG
		    {
			printf("Solving 12 ...\n");
		    }
		    #endif

		    if (checkArea_SequentialWeakPolygonNonoverlapping(box_min_x,
								      box_min_y,
								      box_max_x,
								      box_max_y,
								      fixed,
								      undecided,	       
								      polygons,
								      unreachable_polygons))
		    {
			#ifdef PROFILE
			{
			    recheck_start = clock();
			}
			#endif
			    
			switch (Solver.check(complete_assumptions))
			{
			case z3::sat:
			{
			    #ifdef PROFILE
			    {
				recheck_finish = clock();
				recheck_SAT_cumul += (recheck_finish - recheck_start) / (double)CLOCKS_PER_SEC;
			    }
			    #endif
			    
			    refined_sat = true;	    
			    break;
			}
			case z3::unsat:	
			{
			    #ifdef PROFILE
			    {			    
				recheck_finish = clock();
				recheck_UNSAT_cumul += (recheck_finish - recheck_start) / (double)CLOCKS_PER_SEC;
			    }
			    #endif
			    refined_sat = false;	    
			    break;
			}
			case z3::unknown:
			{
                            #ifdef PROFILE
			    {
				recheck_finish = clock();
				recheck_INDET_cumul += (recheck_finish - recheck_start) / (double)CLOCKS_PER_SEC;
			    }
			    #endif			    
			    refined_sat = false;
			    break;
			}
			default:
			{
			    break;
			}
			}
		    }
		    else
		    {
			refined_sat = false;
		    }
		    
		    #ifdef DEBUG
		    {
			printf("Solving 12 ... finished: %d\n", refined_sat);
		    }
		    #endif

		    /*
		    printf("Printing solver status:\n");
		    cout << Solver << "\n";
		    */
		    		    
		    if (refined_sat)
		    {
			#ifdef DEBUG
			{
			    printf("Refined SAT\n");
			}
			#endif
			z3::model Model(Solver.get_model());

			/*
			printf("Printing smt status:\n");
			cout << Solver.to_smt2() << "\n";	    			
			*/

			extract_DecisionValuesFromModel(Model,
							dec_var_names_map,
							local_dec_values_X,
							local_dec_values_Y,
							local_dec_values_T);

                        #ifdef DEBUG
			{
			    printf("Refined positions:\n");
			    for (unsigned int i = 0; i < undecided.size(); ++i)
			    {
				printf("  i:%d, undecided[i]:%d: %ld/%ld (%.3f), %ld/%ld (%.3f) [%ld/%ld (%.3f)]\n",
				       i, undecided[i],
				       local_dec_values_X[undecided[i]].numerator,
				       local_dec_values_X[undecided[i]].denominator,
				       local_dec_values_X[undecided[i]].as_double(),  
				       local_dec_values_Y[undecided[i]].numerator,
				       local_dec_values_Y[undecided[i]].denominator,
				       local_dec_values_Y[undecided[i]].as_double(),				       
				       local_dec_values_T[undecided[i]].numerator,
				       local_dec_values_T[undecided[i]].denominator,
				       local_dec_values_T[undecided[i]].as_double());
			    }
			}
			#endif
		    }
		    else
		    {
			#ifdef DEBUG
			{
			    printf("Refined UNSAT\n");
			}
			#endif
			size_solvable = false;
			break;
		    }
		}
		else
		{
		    last_solvable_bounding_box_size = box_max_x;

		    dec_values_X = local_dec_values_X;
		    dec_values_Y = local_dec_values_Y;
		    dec_values_T = local_dec_values_T;		    

		    size_solvable = true;
		    break;
		}
	    }
	}
	else
	{
            #ifdef DEBUG
	    {
		printf("First UNSAT\n");
	    }
	    #endif
	    
	    if (last_solvable_bounding_box_size > 0)
	    {
		size_solvable = false;
	    }
	    else
	    {
		size_solvable = false;		
	    }
	}

	coord_t half_x_med = (half_x_max + half_x_min) / 2;
	coord_t half_y_med = (half_y_max + half_y_min) / 2;
		    
	if (size_solvable)
	{
	    #ifdef DEBUG
	    {
		printf("Solvable\n");
	    }
	    #endif
	    half_x_min = half_x_med;
	    half_y_min = half_y_med;	    
	}
	else
	{
	    #ifdef DEBUG
	    {
		printf("Unsolvable\n");
	    }
	    #endif
	    half_x_max = half_x_med;
	    half_y_max = half_y_med;	    
	}
	#ifdef DEBUG
	{
	    printf("Halves augmented: X:[%d,%d] Y:[%d,%d]\n", half_x_min, half_x_max, half_y_min, half_y_max);
	}
	#endif
    }
   
    if (last_solvable_bounding_box_size > 0)
    {	
	box_half_x_max = half_x_max;
	box_half_y_max = half_y_max;

	#ifdef PROFILE
	{
	    printf("Init  : %.3f\n", init_cumul);
	    printf("Refine: %.3f\n", refine_cumul);
	
	    printf("Recheck SAT  : %.3f\n", recheck_SAT_cumul);
	    printf("Recheck UNSAT: %.3f\n", recheck_UNSAT_cumul);
	    printf("Recheck INDET: %.3f\n", recheck_INDET_cumul);
	}
	#endif
	
	return true;
    }    
    return false;
}


/*----------------------------------------------------------------*/

void augment_TemporalSpread(const SolverConfiguration &solver_configuration,
			    std::vector<Rational>     &dec_values_T,
			    const std::vector<int>    &decided_polygons)
{
    std::map<double, int, std::less<double>> sorted_polygons;

    #ifdef DEBUG
    {
	printf("Origo\n");
	for (unsigned int i = 0; i < dec_values_T.size(); ++i)
	{
	    printf("%.3f\n", dec_values_T[i].as_double());
	}
    }
    #endif
	
    for (unsigned int i = 0; i < decided_polygons.size(); ++i)
    {
	sorted_polygons[dec_values_T[decided_polygons[i]].as_double()] = decided_polygons[i];
    }
    
    int time = SEQ_GROUND_PRESENCE_TIME + 2 * solver_configuration.temporal_spread * solver_configuration.object_group_size;

    for (const auto& sorted_polygon: sorted_polygons)
    {
	dec_values_T[sorted_polygon.second] = Rational(time);
	time += 2 * solver_configuration.temporal_spread * solver_configuration.object_group_size;
    }

    #ifdef DEBUG
    {
	printf("Augment\n");
	for (unsigned int i = 0; i < dec_values_T.size(); ++i)
	{
	    printf("%.3f\n", dec_values_T[i].as_double());
	}
    }
    #endif
}


bool optimize_SubglobalPolygonNonoverlapping(const SolverConfiguration          &solver_configuration,
					     std::vector<Rational>              &dec_values_X,
					     std::vector<Rational>              &dec_values_Y,
					     const std::vector<Slic3r::Polygon> &polygons,
					     const std::vector<int>             &undecided_polygons,
					     std::vector<int>                   &decided_polygons,
					     std::vector<int>                   &remaining_polygons)   
{
    vector<int> undecided;

    decided_polygons.clear();
    remaining_polygons.clear();
    
    dec_values_X.resize(polygons.size());
    dec_values_Y.resize(polygons.size());
    
    for (unsigned int curr_polygon = 0; curr_polygon < polygons.size(); /* nothing */)
    {
	bool optimized = false;

	int remaining_polygon = 0;
	for(int object_group_size = MIN((unsigned int)solver_configuration.object_group_size, polygons.size() - curr_polygon); object_group_size > 0; --object_group_size)
	{
	    z3::context z_context;
	    z3::solver z_solver(z_context);
	
	    z3::expr_vector local_dec_vars_X(z_context);
	    z3::expr_vector local_dec_vars_Y(z_context);
	    
	    vector<Rational> local_values_X;
	    vector<Rational> local_values_Y;

	    local_values_X.resize(polygons.size());
	    local_values_Y.resize(polygons.size());

	    for (unsigned int i = 0; i < decided_polygons.size(); ++i)
	    {
                #ifdef DEBUG
		{
		    printf("Decided: %d %.3f, %.3f\n", decided_polygons[i], dec_values_X[decided_polygons[i]].as_double(), dec_values_Y[decided_polygons[i]].as_double());
		}
		#endif
		local_values_X[decided_polygons[i]] = dec_values_X[decided_polygons[i]];
		local_values_Y[decided_polygons[i]] = dec_values_Y[decided_polygons[i]];
	    }
	
	    string_map dec_var_names_map;

	    undecided.clear();

	    for (int i = object_group_size - 1; i >= 0; --i)
	    {
		undecided.push_back(curr_polygon + i + remaining_polygon);
	    }
	    
	    #ifdef DEBUG
	    {
		printf("Undecided\n");
		for (unsigned int j = 0; j < undecided.size(); ++j)
		{
		    printf("  %d\n", undecided[j]);
		}
		printf("Decided\n");
		for (unsigned int j = 0; j < decided_polygons.size(); ++j)
		{
		    printf("  %d\n", decided_polygons[j]);
		}
		printf("Locals\n");
		for (unsigned int j = 0; j < polygons.size(); ++j)
		{
		    printf("X: %ld,%ld  Y: %ld,%ld  \n",
			   local_values_X[j].numerator,
			   local_values_X[j].denominator,
			   local_values_Y[j].numerator,
			   local_values_Y[j].denominator);			   
		}				
	    }
	    #endif

	    build_WeakPolygonNonoverlapping(z_solver,
					    z_context,
					    polygons,
					    local_dec_vars_X,
					    local_dec_vars_Y,
					    local_values_X,
					    local_values_Y,
					    decided_polygons,
					    undecided,				    
					    dec_var_names_map);

            #ifdef DEBUG
	    {
		printf("%ld,%ld\n", local_values_X.size(), local_values_Y.size());

		for (unsigned int i = 0; i < polygons.size(); ++i)
		{
		    printf("poly: %ld\n", polygons[i].points.size());
		    for (unsigned int j = 0; j < polygons[i].points.size(); ++j)
		    {
			printf("    %d,%d\n", polygons[i].points[j].x(), polygons[i].points[j].y());
		    }
		}
	    }
	    #endif

	    optimized = optimize_WeakPolygonNonoverlapping(z_solver,
							   z_context,
							   solver_configuration,
							   local_dec_vars_X,
							   local_dec_vars_Y,
							   local_values_X,
							   local_values_Y,
							   decided_polygons,
							   undecided,
							   dec_var_names_map,
							   polygons);

	    if (optimized)
	    {
		for (unsigned int i = 0; i < undecided.size(); ++i)
		{
		    dec_values_X[undecided[i]] = local_values_X[undecided[i]];
		    dec_values_Y[undecided[i]] = local_values_Y[undecided[i]];
		    decided_polygons.push_back(undecided[i]);
		}
		if (polygons.size() - curr_polygon > (unsigned int)solver_configuration.object_group_size)
		{
		    curr_polygon += solver_configuration.object_group_size;
		}
		else
		{
		    return true;
		}
		break;
	    }
	    else
	    {
                #ifdef DEBUG
		{
		    printf("Remaining polygon: %d\n", curr_polygon + remaining_polygon);
		}
		#endif
		remaining_polygons.push_back(undecided_polygons[curr_polygon + remaining_polygon++]);
	    }
	}
	if (!optimized)
	{
	    if (curr_polygon <= 0)
	    {		
		return false;
	    }
	    else
	    {
		if (polygons.size() - curr_polygon > (unsigned int)solver_configuration.object_group_size)
		{
		    curr_polygon += solver_configuration.object_group_size;
		}
		else
		{
		    return true;
		}
	    }
	}
    }
    return true;
}


bool optimize_SubglobalSequentialPolygonNonoverlapping(const SolverConfiguration          &solver_configuration,
						       std::vector<Rational>              &dec_values_X,
						       std::vector<Rational>              &dec_values_Y,
						       std::vector<Rational>              &dec_values_T,
						       const std::vector<Slic3r::Polygon> &polygons,
						       const std::vector<Slic3r::Polygon> &unreachable_polygons,
						       const std::vector<int>             &undecided_polygons,
						       std::vector<int>                   &decided_polygons,
						       std::vector<int>                   &remaining_polygons)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }

    return optimize_SubglobalSequentialPolygonNonoverlapping(solver_configuration,
							     dec_values_X,
							     dec_values_Y,
							     dec_values_T,
							     polygons,
							     _unreachable_polygons,
							     undecided_polygons,
							     decided_polygons,
							     remaining_polygons);
}


bool optimize_SubglobalSequentialPolygonNonoverlapping(const SolverConfiguration                        &solver_configuration,
						       std::vector<Rational>                            &dec_values_X,
						       std::vector<Rational>                            &dec_values_Y,
						       std::vector<Rational>                            &dec_values_T,
						       const std::vector<Slic3r::Polygon>               &polygons,
						       const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons,
						       const std::vector<int>                           &undecided_polygons,
						       std::vector<int>                                 &decided_polygons,
						       std::vector<int>                                 &remaining_polygons)
{
    vector<int> undecided;

    decided_polygons.clear();
    remaining_polygons.clear();
    
    dec_values_X.resize(polygons.size());
    dec_values_Y.resize(polygons.size());
    dec_values_T.resize(polygons.size());    
    
    for (unsigned int curr_polygon = 0; curr_polygon < polygons.size(); /* nothing */)
    {
	bool optimized = false;

	int remaining_polygon = 0;
	for(int object_group_size = MIN((unsigned int)solver_configuration.object_group_size, polygons.size() - curr_polygon); object_group_size > 0; --object_group_size)
	{
	    z3::set_param("timeout", solver_configuration.optimization_timeout.c_str());
	    
	    z3::context z_context;
	    z3::solver z_solver(z_context);
	
	    z3::expr_vector local_dec_vars_X(z_context);
	    z3::expr_vector local_dec_vars_Y(z_context);
	    z3::expr_vector local_dec_vars_T(z_context);	    
	    
	    vector<Rational> local_values_X;
	    vector<Rational> local_values_Y;
	    vector<Rational> local_values_T;	    

	    local_values_X.resize(polygons.size());
	    local_values_Y.resize(polygons.size());
	    local_values_T.resize(polygons.size());	    

	    for (unsigned int i = 0; i < decided_polygons.size(); ++i)
	    {
		#ifdef DEBUG
		{
		    printf("Decided: %d %.3f, %.3f, %.3f\n",
			   decided_polygons[i],
			   dec_values_X[decided_polygons[i]].as_double(),
			   dec_values_Y[decided_polygons[i]].as_double(),
			   dec_values_T[decided_polygons[i]].as_double());
		}
		#endif
		
		local_values_X[decided_polygons[i]] = dec_values_X[decided_polygons[i]];
		local_values_Y[decided_polygons[i]] = dec_values_Y[decided_polygons[i]];
		local_values_T[decided_polygons[i]] = dec_values_T[decided_polygons[i]];		
	    }
	
	    string_map dec_var_names_map;

	    undecided.clear();

	    /*
	    for (unsigned int i = 0; i < object_group_size; ++i)
	    {
		undecided.push_back(curr_polygon + i);		
	    }
	    */

	    for (int i = object_group_size - 1; i >= 0; --i)
	    {
		undecided.push_back(curr_polygon + i + remaining_polygon);
	    }
	    
	    #ifdef DEBUG
	    {
		printf("Undecided\n");
		for (unsigned int j = 0; j < undecided.size(); ++j)
		{
		    printf("  %d\n", undecided[j]);
		}
		printf("Decided\n");
		for (unsigned int j = 0; j < decided_polygons.size(); ++j)
		{
		    printf("  %d\n", decided_polygons[j]);
		}
		printf("Locals\n");
		for (unsigned int j = 0; j < polygons.size(); ++j)
		{
		    printf("X: %ld,%ld  Y: %ld,%ld  T: %ld,%ld\n",
			   local_values_X[j].numerator,
			   local_values_X[j].denominator,
			   local_values_Y[j].numerator,
			   local_values_Y[j].denominator,
			   local_values_T[j].numerator,
			   local_values_T[j].denominator);			   
			   
		}		
	    }
	    #endif

	    build_SequentialWeakPolygonNonoverlapping(z_solver,
						      z_context,
						      polygons,
						      unreachable_polygons,
						      local_dec_vars_X,
						      local_dec_vars_Y,
						      local_dec_vars_T,
						      local_values_X,
						      local_values_Y,
						      local_values_T,
						      decided_polygons,
						      undecided,				    
						      dec_var_names_map);

	    
	    introduce_SequentialTemporalOrderingAgainstFixed(z_solver,
							     z_context,
							     local_dec_vars_T,
							     local_values_T,
							     decided_polygons,
							     undecided,
							     solver_configuration.temporal_spread,
							     polygons);

	    #ifdef DEBUG
	    {
		printf("%ld,%ld\n", local_values_X.size(), local_values_Y.size());
		
		for (unsigned int i = 0; i < polygons.size(); ++i)
		{
		    printf("poly: %ld\n", polygons[i].points.size());
		    for (unsigned int j = 0; j < polygons[i].points.size(); ++j)
		    {
			printf("    %d,%d\n", polygons[i].points[j].x(), polygons[i].points[j].y());
		    }
		}
	    }
	    #endif

	    optimized = optimize_SequentialWeakPolygonNonoverlapping(z_solver,
								     z_context,
								     solver_configuration,
								     local_dec_vars_X,
								     local_dec_vars_Y,
								     local_dec_vars_T,
								     local_values_X,
								     local_values_Y,
								     local_values_T,
								     decided_polygons,
								     undecided,
								     dec_var_names_map,
								     polygons,
								     unreachable_polygons);


	    if (optimized)
	    {
		/*
		printf("Printing solver status:\n");
		cout << z_solver << "\n";
    
		printf("Printing smt status:\n");
		cout << z_solver.to_smt2() << "\n";	    
		*/		
				
		for (unsigned int i = 0; i < undecided.size(); ++i)
		{
		    dec_values_X[undecided[i]] = local_values_X[undecided[i]];
		    dec_values_Y[undecided[i]] = local_values_Y[undecided[i]];		    
		    dec_values_T[undecided[i]] = local_values_T[undecided[i]];		    
		    decided_polygons.push_back(undecided[i]);
		}
		augment_TemporalSpread(solver_configuration, dec_values_T, decided_polygons);
		
		if (polygons.size() - curr_polygon > (unsigned int)solver_configuration.object_group_size)
		{
		    curr_polygon += solver_configuration.object_group_size;
		}
		else
		{
		    return true;
		}
		break;
	    }
	    else
	    {
		#ifdef DEBUG
		{
		    printf("Remaining polygon: %d\n", curr_polygon + remaining_polygon);
		}
		#endif
		remaining_polygons.push_back(undecided_polygons[curr_polygon + remaining_polygon++]);
	    }
	}
	if (!optimized)
	{
	    if (curr_polygon <= 0)
	    {		
		return false;
	    }
	    else
	    {
		if (polygons.size() - curr_polygon > (unsigned int)solver_configuration.object_group_size)
		{
		    curr_polygon += solver_configuration.object_group_size;
		}
		else
		{
		    return true;
		}
	    }
	}
    }
    return true;    
}


bool optimize_SubglobalSequentialPolygonNonoverlappingCentered(const SolverConfiguration          &solver_configuration,
							       std::vector<Rational>              &dec_values_X,
							       std::vector<Rational>              &dec_values_Y,
							       std::vector<Rational>              &dec_values_T,
							       const std::vector<Slic3r::Polygon> &polygons,
							       const std::vector<Slic3r::Polygon> &unreachable_polygons,
							       const std::vector<int>             &undecided_polygons,
							       std::vector<int>                   &decided_polygons,
							       std::vector<int>                   &remaining_polygons)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }

    return optimize_SubglobalSequentialPolygonNonoverlappingCentered(solver_configuration,
								     dec_values_X,
								     dec_values_Y,
								     dec_values_T,
								     polygons,
								     _unreachable_polygons,
								     undecided_polygons,
								     decided_polygons,
								     remaining_polygons);
}


bool optimize_SubglobalSequentialPolygonNonoverlappingCentered(const SolverConfiguration                        &solver_configuration,
							       std::vector<Rational>                            &dec_values_X,
							       std::vector<Rational>                            &dec_values_Y,
							       std::vector<Rational>                            &dec_values_T,
							       const std::vector<Slic3r::Polygon>               &polygons,
							       const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons,
							       const std::vector<int>                           &undecided_polygons,
							       std::vector<int>                                 &decided_polygons,
							       std::vector<int>                                 &remaining_polygons)
{
    vector<int> undecided;

    decided_polygons.clear();
    remaining_polygons.clear();
    
    dec_values_X.resize(polygons.size());
    dec_values_Y.resize(polygons.size());
    dec_values_T.resize(polygons.size());    
    
    for (unsigned int curr_polygon = 0; curr_polygon < polygons.size(); /* nothing */)
    {
	bool optimized = false;

	int remaining_polygon = 0;
	for(int object_group_size = MIN((unsigned int)solver_configuration.object_group_size, polygons.size() - curr_polygon); object_group_size > 0; --object_group_size)
	{
	    z3::set_param("timeout", solver_configuration.optimization_timeout.c_str());
	    
	    z3::context z_context;
	    z3::solver z_solver(z_context);
	
	    z3::expr_vector local_dec_vars_X(z_context);
	    z3::expr_vector local_dec_vars_Y(z_context);
	    z3::expr_vector local_dec_vars_T(z_context);	    
	    
	    vector<Rational> local_values_X;
	    vector<Rational> local_values_Y;
	    vector<Rational> local_values_T;	    

	    local_values_X.resize(polygons.size());
	    local_values_Y.resize(polygons.size());
	    local_values_T.resize(polygons.size());	    

	    for (unsigned int i = 0; i < decided_polygons.size(); ++i)
	    {
		#ifdef DEBUG
		{
		    printf("Decided: %d %.3f, %.3f, %.3f\n",
			   decided_polygons[i],
			   dec_values_X[decided_polygons[i]].as_double(),
			   dec_values_Y[decided_polygons[i]].as_double(),
			   dec_values_T[decided_polygons[i]].as_double());
		}
		#endif
		
		local_values_X[decided_polygons[i]] = dec_values_X[decided_polygons[i]];
		local_values_Y[decided_polygons[i]] = dec_values_Y[decided_polygons[i]];
		local_values_T[decided_polygons[i]] = dec_values_T[decided_polygons[i]];		
	    }
	
	    string_map dec_var_names_map;

	    undecided.clear();

	    /*
	    for (unsigned int i = 0; i < object_group_size; ++i)
	    {
		undecided.push_back(curr_polygon + i);		
	    }
	    */

	    for (int i = object_group_size - 1; i >= 0; --i)
	    {
		undecided.push_back(curr_polygon + i + remaining_polygon);
	    }
	    
	    #ifdef DEBUG
	    {
		printf("Undecided\n");
		for (unsigned int j = 0; j < undecided.size(); ++j)
		{
		    printf("  %d\n", undecided[j]);
		}
		printf("Decided\n");
		for (unsigned int j = 0; j < decided_polygons.size(); ++j)
		{
		    printf("  %d\n", decided_polygons[j]);
		}
		printf("Locals\n");
		for (unsigned int j = 0; j < polygons.size(); ++j)
		{
		    printf("X: %ld,%ld  Y: %ld,%ld  T: %ld,%ld\n",
			   local_values_X[j].numerator,
			   local_values_X[j].denominator,
			   local_values_Y[j].numerator,
			   local_values_Y[j].denominator,
			   local_values_T[j].numerator,
			   local_values_T[j].denominator);			   
			   
		}		
	    }
	    #endif

	    build_SequentialWeakPolygonNonoverlapping(z_solver,
						      z_context,
						      polygons,
						      unreachable_polygons,
						      local_dec_vars_X,
						      local_dec_vars_Y,
						      local_dec_vars_T,
						      local_values_X,
						      local_values_Y,
						      local_values_T,
						      decided_polygons,
						      undecided,				    
						      dec_var_names_map);

	    
	    introduce_SequentialTemporalOrderingAgainstFixed(z_solver,
							     z_context,
							     local_dec_vars_T,
							     local_values_T,
							     decided_polygons,
							     undecided,
							     solver_configuration.temporal_spread,
							     polygons);

	    #ifdef DEBUG
	    {
		printf("%ld,%ld\n", local_values_X.size(), local_values_Y.size());

		for (unsigned int i = 0; i < polygons.size(); ++i)
		{
		    printf("poly: %ld\n", polygons[i].points.size());
		    for (unsigned int j = 0; j < polygons[i].points.size(); ++j)
		    {
			printf("    %d,%d\n", polygons[i].points[j].x(), polygons[i].points[j].y());
		    }
		}
	    }
	    #endif

	    optimized = optimize_SequentialWeakPolygonNonoverlappingCentered(z_solver,
									     z_context,
									     solver_configuration,
									     local_dec_vars_X,
									     local_dec_vars_Y,
									     local_dec_vars_T,
									     local_values_X,
									     local_values_Y,
									     local_values_T,
									     decided_polygons,
									     undecided,
									     dec_var_names_map,
									     polygons,
									     unreachable_polygons);

	    
	    if (optimized)
	    {
		/*
		printf("Printing solver status:\n");
		cout << z_solver << "\n";
    
		printf("Printing smt status:\n");
		cout << z_solver.to_smt2() << "\n";	    
		*/		
				
		for (unsigned int i = 0; i < undecided.size(); ++i)
		{
		    dec_values_X[undecided[i]] = local_values_X[undecided[i]];
		    dec_values_Y[undecided[i]] = local_values_Y[undecided[i]];		    
		    dec_values_T[undecided[i]] = local_values_T[undecided[i]];		    
		    decided_polygons.push_back(undecided[i]);
		}
		augment_TemporalSpread(solver_configuration, dec_values_T, decided_polygons);
		
		if (polygons.size() - curr_polygon > (unsigned int)solver_configuration.object_group_size)
		{
		    curr_polygon += solver_configuration.object_group_size;
		}
		else
		{
		    return true;
		}
		break;
	    }
	    else
	    {
		#ifdef DEBUG
		{
		    printf("Remaining polygon: %d\n", curr_polygon + remaining_polygon);
		}
		#endif
		remaining_polygons.push_back(undecided_polygons[curr_polygon + remaining_polygon++]);
	    }
	}
	if (!optimized)
	{
	    if (curr_polygon <= 0)
	    {		
		return false;
	    }
	    else
	    {
		if (polygons.size() - curr_polygon > (unsigned int)solver_configuration.object_group_size)
		{
		    curr_polygon += solver_configuration.object_group_size;
		}
		else
		{
		    return true;
		}
	    }
	}
    }
    return true;    
}


bool optimize_SubglobalSequentialPolygonNonoverlappingBinaryCentered(const SolverConfiguration          &solver_configuration,
								     std::vector<Rational>              &dec_values_X,
								     std::vector<Rational>              &dec_values_Y,
								     std::vector<Rational>              &dec_values_T,
								     const std::vector<Slic3r::Polygon> &polygons,
								     const std::vector<Slic3r::Polygon> &unreachable_polygons,
								     const std::vector<int>             &undecided_polygons,
								     std::vector<int>                   &decided_polygons,
								     std::vector<int>                   &remaining_polygons)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }

    return optimize_SubglobalSequentialPolygonNonoverlappingBinaryCentered(solver_configuration,
									   dec_values_X,
									   dec_values_Y,
									   dec_values_T,
									   polygons,
									   _unreachable_polygons,
									   undecided_polygons,
									   decided_polygons,
									   remaining_polygons);
}


bool optimize_SubglobalSequentialPolygonNonoverlappingBinaryCentered(const SolverConfiguration                        &solver_configuration,
								     std::vector<Rational>                            &dec_values_X,
								     std::vector<Rational>                            &dec_values_Y,
								     std::vector<Rational>                            &dec_values_T,
								     const std::vector<Slic3r::Polygon>               &polygons,
								     const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons,
								     const std::vector<int>                           &undecided_polygons,
								     std::vector<int>                                 &decided_polygons,
								     std::vector<int>                                 &remaining_polygons)
{
    vector<int> undecided;

    decided_polygons.clear();
    remaining_polygons.clear();
    
    dec_values_X.resize(polygons.size());
    dec_values_Y.resize(polygons.size());
    dec_values_T.resize(polygons.size());

    coord_t box_half_x_max = solver_configuration.x_plate_bounding_box_size / 2;
    coord_t box_half_y_max = solver_configuration.y_plate_bounding_box_size / 2;	    
    
    for (unsigned int curr_polygon = 0; curr_polygon < polygons.size(); /* nothing */)
    {
	bool optimized = false;

	for(int object_group_size = MIN((unsigned int)solver_configuration.object_group_size, polygons.size() - curr_polygon); object_group_size > 0; --object_group_size)
	{
	    z3::set_param("timeout", solver_configuration.optimization_timeout.c_str());
	    
	    z3::context z_context;
	    z3::solver z_solver(z_context);
	
	    z3::expr_vector local_dec_vars_X(z_context);
	    z3::expr_vector local_dec_vars_Y(z_context);
	    z3::expr_vector local_dec_vars_T(z_context);	    
	    
	    vector<Rational> local_values_X;
    vector<Rational> local_values_Y;
	    vector<Rational> local_values_T;	    

	    local_values_X.resize(polygons.size());
	    local_values_Y.resize(polygons.size());
	    local_values_T.resize(polygons.size());	    

	    for (unsigned int i = 0; i < decided_polygons.size(); ++i)
	    {
		#ifdef DEBUG
		{
		    printf("Decided: %d %.3f, %.3f, %.3f\n",
			   decided_polygons[i],
			   dec_values_X[decided_polygons[i]].as_double(),
			   dec_values_Y[decided_polygons[i]].as_double(),
			   dec_values_T[decided_polygons[i]].as_double());
		}
		#endif
		
		local_values_X[decided_polygons[i]] = dec_values_X[decided_polygons[i]];
		local_values_Y[decided_polygons[i]] = dec_values_Y[decided_polygons[i]];
		local_values_T[decided_polygons[i]] = dec_values_T[decided_polygons[i]];		
	    }
	
	    string_map dec_var_names_map;

	    undecided.clear();

	    /*
	    for (unsigned int i = 0; i < object_group_size; ++i)
	    {
		undecided.push_back(curr_polygon + i);		
	    }
	    */

	    for (int i = 0; i < object_group_size; ++i)
	    {
		undecided.push_back(curr_polygon + i);
	    }
	    
	    #ifdef DEBUG
	    {
		printf("Undecided\n");
		for (unsigned int j = 0; j < undecided.size(); ++j)
		{
		    printf("  %d\n", undecided[j]);
		}
		printf("Decided\n");
		for (unsigned int j = 0; j < decided_polygons.size(); ++j)
		{
		    printf("  %d\n", decided_polygons[j]);
		}
		printf("Locals\n");
		for (unsigned int j = 0; j < polygons.size(); ++j)
		{
		    printf("X: %ld,%ld  Y: %ld,%ld  T: %ld,%ld\n",
			   local_values_X[j].numerator,
			   local_values_X[j].denominator,
			   local_values_Y[j].numerator,
			   local_values_Y[j].denominator,
			   local_values_T[j].numerator,
			   local_values_T[j].denominator);			   
			   
		}
	    }
	    #endif

	    build_SequentialWeakPolygonNonoverlapping(z_solver,
						      z_context,
						      polygons,
						      unreachable_polygons,
						      local_dec_vars_X,
						      local_dec_vars_Y,
						      local_dec_vars_T,
						      local_values_X,
						      local_values_Y,
						      local_values_T,
						      decided_polygons,
						      undecided,				    
						      dec_var_names_map);
	    
	    introduce_SequentialTemporalOrderingAgainstFixed(z_solver,
							     z_context,
							     local_dec_vars_T,
							     local_values_T,
							     decided_polygons,
							     undecided,
							     solver_configuration.temporal_spread,
							     polygons);

	    #ifdef DEBUG
	    {
		printf("%ld,%ld\n", local_values_X.size(), local_values_Y.size());

		for (unsigned int i = 0; i < polygons.size(); ++i)
		{
		    printf("poly: %ld\n", polygons[i].points.size());
		    for (unsigned int j = 0; j < polygons[i].points.size(); ++j)
		    {
			printf("    %d,%d\n", polygons[i].points[j].x(), polygons[i].points[j].y());
		    }
		}
	    }
	    #endif

	    optimized = optimize_SequentialWeakPolygonNonoverlappingBinaryCentered(z_solver,
										   z_context,
										   solver_configuration,
										   box_half_x_max,
										   box_half_y_max,
										   local_dec_vars_X,
										   local_dec_vars_Y,
										   local_dec_vars_T,
										   local_values_X,
										   local_values_Y,
										   local_values_T,
										   decided_polygons,
										   undecided,
										   dec_var_names_map,
										   polygons,
										   unreachable_polygons);

	    
	    if (optimized)
	    {
		/*
		printf("Printing solver status:\n");
		cout << z_solver << "\n";
    
		printf("Printing smt status:\n");
		cout << z_solver.to_smt2() << "\n";	    
		*/		
				
		for (unsigned int i = 0; i < undecided.size(); ++i)
		{
		    dec_values_X[undecided[i]] = local_values_X[undecided[i]];
		    dec_values_Y[undecided[i]] = local_values_Y[undecided[i]];		    
		    dec_values_T[undecided[i]] = local_values_T[undecided[i]];		    
		    decided_polygons.push_back(undecided[i]);
		}
		augment_TemporalSpread(solver_configuration, dec_values_T, decided_polygons);
		
		if (polygons.size() - curr_polygon > (unsigned int)solver_configuration.object_group_size)
		{
		    curr_polygon += solver_configuration.object_group_size;
		}
		else
		{
		    return true;
		}
		break;
	    }
	    else
	    {
		#ifdef DEBUG
		{
		    printf("Remaining polygon: %d\n", curr_polygon + object_group_size - 1);
		}
		#endif
		remaining_polygons.push_back(undecided_polygons[curr_polygon + object_group_size - 1]);
	    }
	}
	if (!optimized)
	{
	    if (curr_polygon <= 0)
	    {		
		return false;
	    }
	    else
	    {
		if (polygons.size() - curr_polygon > (unsigned int)solver_configuration.object_group_size)
		{		    
		    curr_polygon += solver_configuration.object_group_size;

		    for (; curr_polygon < polygons.size(); ++curr_polygon)
		    {
			remaining_polygons.push_back(undecided_polygons[curr_polygon]);
		    }
		    return true;		   
		}
		else
		{
		    return true;
		}
	    }
	}
    }
    return true;    
}


bool optimize_SubglobalConsequentialPolygonNonoverlappingBinaryCentered(const SolverConfiguration          &solver_configuration,
									std::vector<Rational>              &dec_values_X,
									std::vector<Rational>              &dec_values_Y,
									std::vector<Rational>              &dec_values_T,
									const std::vector<Slic3r::Polygon> &polygons,
									const std::vector<Slic3r::Polygon> &unreachable_polygons,
									const std::vector<int>             &previous_polygons,
									const std::vector<int>             &undecided_polygons,
									std::vector<int>                   &decided_polygons,
									std::vector<int>                   &remaining_polygons,
									int                                 objects_done,
									int                                 total_objects,
									std::function<void(int)>            progress_callback)
{
    std::vector<std::vector<Slic3r::Polygon> > _unreachable_polygons;
    _unreachable_polygons.resize(unreachable_polygons.size());

    for (unsigned int poly = 0; poly < unreachable_polygons.size(); ++poly)
    {
	_unreachable_polygons[poly].push_back(unreachable_polygons[poly]);
    }

    return optimize_SubglobalConsequentialPolygonNonoverlappingBinaryCentered(solver_configuration,
									      dec_values_X,
									      dec_values_Y,
									      dec_values_T,
									      polygons,
									      _unreachable_polygons,
									      previous_polygons,
									      undecided_polygons,
									      decided_polygons,
									      remaining_polygons,
									      objects_done,
									      total_objects,
									      progress_callback);
}


#ifdef PROFILE
double build_cumul = 0.0;
clock_t build_start, build_finish;
#endif

bool optimize_SubglobalConsequentialPolygonNonoverlappingBinaryCentered(const SolverConfiguration                        &solver_configuration,
									std::vector<Rational>                            &dec_values_X,
									std::vector<Rational>                            &dec_values_Y,
									std::vector<Rational>                            &dec_values_T,
									const std::vector<Slic3r::Polygon>               &polygons,
									const std::vector<std::vector<Slic3r::Polygon> > &unreachable_polygons,
									const std::vector<int>                           &previous_polygons,
									const std::vector<int>                           &undecided_polygons,
									std::vector<int>                                 &decided_polygons,
									std::vector<int>                                 &remaining_polygons,
									int                                               objects_done,
									int                                               total_objects,
									std::function<void(int)>                          progress_callback)
{
    std::vector<int> undecided;

    decided_polygons.clear();
    remaining_polygons.clear();
    
    dec_values_X.resize(polygons.size());
    dec_values_Y.resize(polygons.size());
    dec_values_T.resize(polygons.size());

    int box_half_x_max = solver_configuration.x_plate_bounding_box_size / 2;
    int box_half_y_max = solver_configuration.y_plate_bounding_box_size / 2;
    
    for (unsigned int curr_polygon = 0; curr_polygon < polygons.size(); /* nothing */)
    {
	bool optimized = false;
	z3::set_param("timeout", solver_configuration.optimization_timeout.c_str());
	    
	z3::context z_context;
	z3::solver z_solver(z_context);
	
	z3::expr_vector local_dec_vars_X(z_context);
	z3::expr_vector local_dec_vars_Y(z_context);
	z3::expr_vector local_dec_vars_T(z_context);	    
	
	vector<Rational> local_values_X;
	vector<Rational> local_values_Y;
	vector<Rational> local_values_T;	    
	
	local_values_X.resize(polygons.size());
	local_values_Y.resize(polygons.size());
	local_values_T.resize(polygons.size());	    
	
	for (unsigned int i = 0; i < decided_polygons.size(); ++i)
	{
            #ifdef DEBUG
	    {
		printf("Decided: %d %.3f, %.3f, %.3f\n",
		       decided_polygons[i],
		       dec_values_X[decided_polygons[i]].as_double(),
		       dec_values_Y[decided_polygons[i]].as_double(),
		       dec_values_T[decided_polygons[i]].as_double());
	    }
	    #endif
	    
	    local_values_X[decided_polygons[i]] = dec_values_X[decided_polygons[i]];
	    local_values_Y[decided_polygons[i]] = dec_values_Y[decided_polygons[i]];
	    local_values_T[decided_polygons[i]] = dec_values_T[decided_polygons[i]];		
	}

	string_map dec_var_names_map;		    
	int object_group_size = MIN((unsigned int)solver_configuration.object_group_size, polygons.size() - curr_polygon);

	undecided.clear();	
	for (int i = 0; i < object_group_size; ++i)
	{
	    undecided.push_back(curr_polygon + i);
	}

	#ifdef PROFILE
	{
	    build_start = clock();
	}
	#endif
	build_ConsequentialWeakPolygonNonoverlapping(z_solver,
						     z_context,
						     polygons,
						     unreachable_polygons,
						     local_dec_vars_X,
						     local_dec_vars_Y,
						     local_dec_vars_T,
						     local_values_X,
						     local_values_Y,
						     local_values_T,
						     decided_polygons,
						     undecided,				    
						     dec_var_names_map);
	#ifdef PROFILE
	{
	    build_finish = clock();
	    build_cumul += (build_finish - build_start) / (double)CLOCKS_PER_SEC;
	}
	#endif

	vector<int> missing;
    
	while(object_group_size > 0)
	{
	    z3::expr_vector presence_assumptions(z_context);
	    assume_ConsequentialObjectPresence(z_context, local_dec_vars_T, undecided, missing, presence_assumptions);
	    
	    #ifdef DEBUG
	    {
		printf("Undecided\n");
		for (unsigned int j = 0; j < undecided.size(); ++j)
		{
		    printf("  %d\n", undecided[j]);
		}
		printf("Decided\n");
		for (unsigned int j = 0; j < decided_polygons.size(); ++j)
		{
		    printf("  %d\n", decided_polygons[j]);
		}
		printf("Locals\n");
		for (unsigned int j = 0; j < polygons.size(); ++j)
		{
		    printf("X: %ld,%ld  Y: %ld,%ld  T: %ld,%ld\n",
			   local_values_X[j].numerator,
			   local_values_X[j].denominator,
			   local_values_Y[j].numerator,
			   local_values_Y[j].denominator,
			   local_values_T[j].numerator,
			   local_values_T[j].denominator);	   
		}		
	    }
	    #endif

	    introduce_ConsequentialTemporalOrderingAgainstFixed(z_solver,
								z_context,
								local_dec_vars_T,
								local_values_T,
								decided_polygons,
								undecided,
								solver_configuration.temporal_spread,
								polygons);

	    introduce_ConsequentialTemporalLepoxAgainstFixed(z_solver,
							     z_context,
							     local_dec_vars_T,
							     local_values_T,
							     decided_polygons,
							     undecided,
							     solver_configuration.temporal_spread,
							     polygons,
							     previous_polygons);	    

	    #ifdef DEBUG
	    {
		printf("%ld,%ld\n", local_values_X.size(), local_values_Y.size());

		for (unsigned int i = 0; i < polygons.size(); ++i)
		{
		    printf("poly: %ld\n", polygons[i].points.size());
		    for (unsigned int j = 0; j < polygons[i].points.size(); ++j)
		    {
			printf("    %d,%d\n", polygons[i].points[j].x(), polygons[i].points[j].y());
		    }
		}
	    }
	    #endif

	    progress_callback((SEQ_PROGRESS_RANGE * (decided_polygons.size() + objects_done)) / total_objects);
	    
	    optimized = optimize_ConsequentialWeakPolygonNonoverlappingBinaryCentered(z_solver,
										      z_context,
										      solver_configuration,
										      box_half_x_max,
										      box_half_y_max,
										      local_dec_vars_X,
										      local_dec_vars_Y,
										      local_dec_vars_T,
										      local_values_X,
										      local_values_Y,
										      local_values_T,
										      decided_polygons,
										      undecided,
										      dec_var_names_map,
										      polygons,
										      unreachable_polygons,
										      presence_assumptions);
	    
	    if (optimized)
	    {
		/*
		printf("Printing solver status:\n");
		cout << z_solver << "\n";
    
		printf("Printing smt status:\n");
		cout << z_solver.to_smt2() << "\n";	    
		*/		
				
		for (unsigned int i = 0; i < undecided.size(); ++i)
		{
		    dec_values_X[undecided[i]] = local_values_X[undecided[i]];
		    dec_values_Y[undecided[i]] = local_values_Y[undecided[i]];		    
		    dec_values_T[undecided[i]] = local_values_T[undecided[i]];		    
		    decided_polygons.push_back(undecided[i]);
		}
		augment_TemporalSpread(solver_configuration, dec_values_T, decided_polygons);
		
		if (polygons.size() - curr_polygon > (unsigned int)solver_configuration.object_group_size)
		{
		    curr_polygon += solver_configuration.object_group_size;
		}
		else
		{
		    curr_polygon += polygons.size() - curr_polygon;
		    progress_callback((SEQ_PROGRESS_RANGE * (decided_polygons.size() + objects_done)) / total_objects);	    
		    return true;
		}
		
		progress_callback((SEQ_PROGRESS_RANGE * (decided_polygons.size() + objects_done)) / total_objects);
		break;
	    }
	    else
	    {
		#ifdef DEBUG
		{
		    printf("Remaining polygon: %d\n", curr_polygon + object_group_size - 1);
		}
		#endif
		remaining_polygons.push_back(undecided_polygons[curr_polygon + object_group_size - 1]);
	    }
	    
	    missing.push_back(undecided.back());
	    undecided.pop_back();	    

	    --object_group_size;

	    progress_callback((SEQ_PROGRESS_RANGE * (decided_polygons.size() + objects_done)) / total_objects);
	}

	#ifdef PROFILE
	{
	    printf("Build: %.3f\n", build_cumul);
	}
	#endif
	
	if (!optimized)
	{
	    if (curr_polygon <= 0)
	    {		
		return false;
	    }
	    else
	    {
		if (polygons.size() - curr_polygon > (unsigned int)solver_configuration.object_group_size)
		{		    
		    curr_polygon += solver_configuration.object_group_size;

		    for (; curr_polygon < polygons.size(); ++curr_polygon)
		    {
			remaining_polygons.push_back(undecided_polygons[curr_polygon]);
		    }
		    return true;		   
		}
		else
		{
		    return true;
		}
	    }
	}
    }
    return true;    
}


/*----------------------------------------------------------------*/

} // namespace Sequential
