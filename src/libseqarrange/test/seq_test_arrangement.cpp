/*================================================================*/
/*
 * Author:  Pavel Surynek, 2023 - 2024
 * Company: Prusa Research
 *
 * File:    seq_test_arrangement.cpp
 *
 * Basic steel sheet object arrangement test via
 * Constraint Programming tools.
 */
/*================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

#include "seq_defs.hpp"

#include "seq_test_arrangement.hpp"


/*----------------------------------------------------------------*/


using namespace Gecode;


/*----------------------------------------------------------------*/

class Queens : public Script {
public:
  /// Position of queens on boards
  IntVarArray q;    
  /// Propagation to use for model
    
  Queens(const SizeOptions& opt)
      : Script(opt), q(*this, opt.size(), 0, opt.size()-1) {
      
    const int n = q.size();
    
    for (int i = 0; i<n; i++)
    {
        for (int j = i+1; j<n; j++)
	{
	    rel(*this, q[i] != q[j]);
	    rel(*this, q[i]+i != q[j]+j);
	    rel(*this, q[i]-i != q[j]-j);
        }
    }

    branch(*this, q, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    
  }

  /// Constructor for cloning
  Queens(Queens& s) : Script(s) {
    q.update(*this, s.q);
  }

  /// Perform copying during cloning
  virtual Space*
  copy(void) {
    return new Queens(*this);
  }
    
  /// Print solution    
  virtual void
  print(std::ostream& os) const {
    os << "queens\t";
    
    for (int i = 0; i < q.size(); i++) {
      os << q[i] << ", ";
    }
    os << std::endl;
  }
};


void test_arrangement_1(void)
{
    printf("Testing sheet arrangement 1 ...\n");

    SizeOptions opt("Queens");
    opt.iterations(200);
    opt.size(100);
        
    Script::run<Queens,LDS,SizeOptions>(opt);
    
    
    printf("Testing sheet arrangement 1 ... finished\n");    
}


class SimpleProblem : public Space
{
public:
    IntVarArray vars;
    IntVar X, Y, Z;

public:
    SimpleProblem()
	: vars(*this, 3, 0, 3)
	, X(vars[0])
	, Y(vars[1])
	, Z(vars[2])
    {
	rel(*this, X != Y);
	rel(*this, Y != Z);
	rel(*this, X != Z);

	branch(*this, vars, INT_VAR_SIZE_MIN(), INT_VAL_MIN());		
    };

    SimpleProblem(SimpleProblem &s)
	: Space(s)
    {
	X.update(*this, s.X);
	Y.update(*this, s.Y);
	Z.update(*this, s.Z);
    }

    virtual Space* copy(void) {
	return new SimpleProblem(*this);
    };

    void print(void) const
    {
	/*
	cout << X << endl;
	cout << Y << endl;
	cout << Z << endl;		
	*/
	printf("XYZ: %d, %d, %d\n", X.val(), Y.val(), Z.val());
    };
};


const int sheet_size = 9;

const int Obj1_width  = 5;
const int Obj1_height = 4;

const int Obj2_width  = 3;
const int Obj2_height = 7;

const int Obj3_width  = 5;
const int Obj3_height = 5;

class ArrangementProblem : public Space
{
public:
    IntVarArray vars;
    IntVar Obj1_x, Obj1_y;
    IntVar Obj2_x, Obj2_y;
    IntVar Obj3_x, Obj3_y;        
    
public:
    ArrangementProblem()
	: vars(*this, 6, 0, sheet_size - 1)
	, Obj1_x(vars[0])
	, Obj1_y(vars[1])	  
	, Obj2_x(vars[2])
	, Obj2_y(vars[3])
	, Obj3_x(vars[4])
	, Obj3_y(vars[5])	  
    {
	IntArgs widths(3);
	widths[0] = Obj1_width;
	widths[1] = Obj2_width;
	widths[2] = Obj3_width;

	IntArgs heights(3);
	heights[0] = Obj1_height;
	heights[1] = Obj2_height;
	heights[2] = Obj3_height;

	IntVarArgs Xs(3);
	Xs[0] = Obj1_x;
	Xs[1] = Obj2_x;
	Xs[2] = Obj3_x;

	IntVarArgs Ys(3);	
	Ys[0] = Obj1_y;
	Ys[1] = Obj2_y;
	Ys[2] = Obj3_y;			       

	nooverlap(*this, Xs, widths, Ys, heights);

	rel(*this, Obj1_x + Obj1_width <= sheet_size);
	rel(*this, Obj2_x + Obj2_width <= sheet_size);
	rel(*this, Obj3_x + Obj3_width <= sheet_size);

	rel(*this, Obj2_x == Obj1_x + 5);
	rel(*this, Obj2_y == Obj1_y + 1);

	rel(*this, Obj1_y + Obj1_height <= sheet_size);
	rel(*this, Obj2_y + Obj2_height <= sheet_size);
	rel(*this, Obj3_y + Obj3_height <= sheet_size);		
	
	branch(*this, vars, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    };

    ArrangementProblem(ArrangementProblem &s)
	: Space(s)
    {
	Obj1_x.update(*this, s.Obj1_x);	
	Obj1_y.update(*this, s.Obj1_y);

	Obj2_x.update(*this, s.Obj2_x);	
	Obj2_y.update(*this, s.Obj2_y);

	Obj3_x.update(*this, s.Obj3_x);	
	Obj3_y.update(*this, s.Obj3_y);
    }

    virtual Space* copy(void) {
	return new ArrangementProblem(*this);
    };

    void print(void) const
    {
	/*
	cout << X << endl;
	cout << Y << endl;
	cout << Z << endl;		
	*/
	printf("Position obj1: %d, %d\n", Obj1_x.val(), Obj1_y.val());
	printf("Position obj2: %d, %d\n", Obj2_x.val(), Obj2_y.val());
	printf("Position obj2: %d, %d\n", Obj3_x.val(), Obj3_y.val());
    };
};


const int sheet_resolution = 20;
const int time_resolution = 80;

const int low_Obj1_width  = 5;
const int low_Obj1_height = 4;
const int low_Obj1_duration = 10;

const int low_Obj2_width  = 3;
const int low_Obj2_height = 7;
const int low_Obj2_duration = 15;

const int low_Obj3_width  = 4;
const int low_Obj3_height = 4;
const int low_Obj3_duration = 5;

const int high_Obj1_width  = 2;
const int high_Obj1_height = 2;
const int high_Obj1_duration = 35;

const int kine_width = 100;
const int kine_height = 10;


class SimpleSequentialProblem : public Space
{
public:
    IntVarArray space_vars;
    IntVarArray time_vars;
    IntVarArray kine_vars;
    
    IntVar low_Obj1_x, low_Obj1_y, low_Obj1_t;
    IntVar low_Obj2_x, low_Obj2_y, low_Obj2_t;
    IntVar low_Obj3_x, low_Obj3_y, low_Obj3_t;    
    
    IntVar high_Obj1_x, high_Obj1_y, high_Obj1_t;        
    
public:
    SimpleSequentialProblem()
	: space_vars(*this, 8, 0, sheet_resolution)
  	, time_vars(*this, 4, 0, time_resolution)
	, kine_vars(*this, 2, 0, sheet_resolution)
	  
	, low_Obj1_x(space_vars[0])
	, low_Obj1_y(space_vars[1])
	, low_Obj1_t(time_vars[0])
	  
	, low_Obj2_x(space_vars[2])
	, low_Obj2_y(space_vars[3])
	, low_Obj2_t(time_vars[1])

	, low_Obj3_x(space_vars[4])
	, low_Obj3_y(space_vars[5])
	, low_Obj3_t(time_vars[2])	  

	, high_Obj1_x(space_vars[6])
	, high_Obj1_y(space_vars[7])
	, high_Obj1_t(time_vars[3])	  	  
    {
	printf("alpha 1\n");
	BoolVar low_Obj1_present = expr(*this, low_Obj1_t >= 0);
	BoolVar low_Obj2_present = expr(*this, low_Obj2_t >= 0);
	BoolVar low_Obj3_present = expr(*this, low_Obj3_t >= 0);	
	BoolVar high_Obj1_present = expr(*this, high_Obj1_t >= 0);
	//BoolVar low_Obj1_above_high_Obj1 = expr(*this, low_Obj1_t >= high_Obj1_t);	

	printf("alpha 2\n");	
	BoolVarArgs objects_present(4);
	objects_present[0] = low_Obj1_present;
	objects_present[1] = low_Obj2_present;
	objects_present[2] = low_Obj3_present;	
	objects_present[3] = high_Obj1_present;
	//objects_present[3] = low_Obj1_above_high_Obj1;			
	
	IntArgs kine_widths(1);
	IntArgs kine_heights(1);

	IntVar kine_Obj1_x(kine_vars[0]);
	IntVar kine_Obj1_y(kine_vars[1]);
	printf("alpha 3\n");
	kine_widths[0] = kine_width;
	kine_heights[0] = kine_height;

	rel(*this, kine_Obj1_x == high_Obj1_x + high_Obj1_width);
	rel(*this, kine_Obj1_y == high_Obj1_y - 3);

	IntArgs widths(4);
	widths[0] = low_Obj1_width;
	widths[1] = low_Obj2_width;
	widths[2] = low_Obj3_width;	
	widths[3] = high_Obj1_width;
	//widths[3] = kine_width;

	IntArgs heights(4);
	heights[0] = low_Obj1_height;
	heights[1] = low_Obj2_height;
	heights[2] = low_Obj3_height;	
	heights[3] = high_Obj1_height;
	//heights[3] = kine_height;
	printf("alpha 4\n");
	
	IntVarArgs Xs(4);
	Xs[0] = low_Obj1_x;
	Xs[1] = low_Obj2_x;
	Xs[2] = low_Obj3_x;	
	Xs[3] = high_Obj1_x;
	//Xs[3] = kine_Obj1_x;

	IntVarArgs Ys(4);	
	Ys[0] = low_Obj1_y;
	Ys[1] = low_Obj2_y;
	Ys[2] = low_Obj3_y;	
	Ys[3] = high_Obj1_y;
	//Ys[3] = kine_Obj1_y;	
	printf("alpha 5\n");
	
	nooverlap(*this, Xs, widths, Ys, heights/*, objects_present*/);
	printf("alpha 6\n");

	rel(*this, low_Obj1_t >= low_Obj2_t + low_Obj2_duration || low_Obj2_t >= low_Obj1_t + low_Obj1_duration);
	rel(*this, low_Obj1_t >= low_Obj3_t + low_Obj3_duration || low_Obj3_t >= low_Obj1_t + low_Obj1_duration);
	rel(*this, low_Obj2_t >= low_Obj3_t + low_Obj3_duration || low_Obj3_t >= low_Obj2_t + low_Obj2_duration);	

	rel(*this, low_Obj3_t >= high_Obj1_t + high_Obj1_duration || high_Obj1_t >= low_Obj3_t + low_Obj3_duration);	
	rel(*this, low_Obj2_t >= high_Obj1_t + high_Obj1_duration || high_Obj1_t >= low_Obj2_t + low_Obj2_duration);
	rel(*this, low_Obj1_t >= high_Obj1_t + high_Obj1_duration || high_Obj1_t >= low_Obj1_t + low_Obj1_duration);		
	
	rel(*this, low_Obj1_x + low_Obj1_width <= sheet_resolution);
	rel(*this, low_Obj2_x + low_Obj2_width <= sheet_resolution);
	rel(*this, low_Obj3_x + low_Obj3_width <= sheet_resolution);	
	rel(*this, high_Obj1_x + high_Obj1_width <= sheet_resolution);	

	rel(*this, low_Obj1_y + low_Obj1_height <= sheet_resolution);
	rel(*this, low_Obj2_y + low_Obj2_height <= sheet_resolution);
	rel(*this, low_Obj3_y + low_Obj3_height <= sheet_resolution);	
	rel(*this, high_Obj1_y + high_Obj1_height <= sheet_resolution);
	
	rel(*this, low_Obj1_t + low_Obj1_duration <= time_resolution);
	rel(*this, low_Obj2_t + low_Obj2_duration <= time_resolution);
	rel(*this, low_Obj3_t + low_Obj3_duration <= time_resolution);	
	rel(*this, high_Obj1_t + high_Obj1_duration <= time_resolution);

	rel(*this, low_Obj1_t < high_Obj1_t || low_Obj1_x >= kine_Obj1_x + kine_width || kine_Obj1_x >= low_Obj1_x + low_Obj1_width
					    || low_Obj1_y >= kine_Obj1_y + kine_height || kine_Obj1_y >= low_Obj1_y + low_Obj1_height);

	rel(*this, low_Obj2_t < high_Obj1_t || low_Obj2_x >= kine_Obj1_x + kine_width || kine_Obj1_x >= low_Obj2_x + low_Obj2_width
					    || low_Obj2_y >= kine_Obj1_y + kine_height || kine_Obj1_y >= low_Obj2_y + low_Obj2_height);

	rel(*this, low_Obj3_t < high_Obj1_t || low_Obj3_x >= kine_Obj1_x + kine_width || kine_Obj1_x >= low_Obj3_x + low_Obj3_width
					    || low_Obj3_y >= kine_Obj1_y + kine_height || kine_Obj1_y >= low_Obj3_y + low_Obj3_height);		

	rel(*this, high_Obj1_t < 10);
	rel(*this, high_Obj1_x == 0);	
	
	printf("alpha 7\n");
	
	branch(*this, space_vars, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
	branch(*this, time_vars, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
	branch(*this, kine_vars, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
	printf("alpha 8\n");	
    };

    SimpleSequentialProblem(SimpleSequentialProblem &s)
	: Space(s)
    {
	low_Obj1_x.update(*this, s.low_Obj1_x);	
	low_Obj1_y.update(*this, s.low_Obj1_y);
	low_Obj1_t.update(*this, s.low_Obj1_t);	

	low_Obj2_x.update(*this, s.low_Obj2_x);	
	low_Obj2_y.update(*this, s.low_Obj2_y);
	low_Obj2_t.update(*this, s.low_Obj2_t);

	low_Obj3_x.update(*this, s.low_Obj3_x);	
	low_Obj3_y.update(*this, s.low_Obj3_y);
	low_Obj3_t.update(*this, s.low_Obj3_t);		

	high_Obj1_x.update(*this, s.high_Obj1_x);	
	high_Obj1_y.update(*this, s.high_Obj1_y);
	high_Obj1_t.update(*this, s.high_Obj1_t);	
    }

    virtual Space* copy(void) {
	return new SimpleSequentialProblem(*this);
    };

    void print(void) const
    {
	/*
	cout << X << endl;
	cout << Y << endl;
	cout << Z << endl;		
	*/
	printf("Position low obj1 : x:%d, y:%d, t:%d\n", low_Obj1_x.val(), low_Obj1_y.val(), low_Obj1_t.val());
	printf("Position low obj2 : x:%d, y:%d, t:%d\n", low_Obj2_x.val(), low_Obj2_y.val(), low_Obj2_t.val());
	printf("Position low obj3 : x:%d, y:%d, t:%d\n", low_Obj3_x.val(), low_Obj3_y.val(), low_Obj3_t.val());	
	printf("Position high obj1: x:%d, y:%d, t:%d\n", high_Obj1_x.val(), high_Obj1_y.val(), high_Obj1_t.val());
    };
};


int complex_sheet_resolution = 30;

int complex_sheet_resolution_min = 10;
int complex_sheet_resolution_max = 200;

int complex_time_resolution = 1000;
int complex_height_threshold = 25;

const int complex_Obj_count = 10;

int complex_Obj_widths[complex_Obj_count];
int complex_Obj_heights[complex_Obj_count];
int complex_Obj_durations[complex_Obj_count];

const int min_width = 4;
const int max_width = 20;

const int min_height = 4;
const int max_height = 20;

const int min_duration = 2;
const int max_duration = 50;

const int gantry_left_height = 10;
const int gantry_left_shift = 4;

const int gantry_right_height = 10;
const int gantry_right_shift = 4;


class ComplexSequentialProblem : public Space
{
public:
    IntVarArray space_vars;
    IntVarArray time_vars;
    IntVarArray kine_vars;
    
    IntVar complex_Obj_x[complex_Obj_count];
    IntVar complex_Obj_y[complex_Obj_count];
    IntVar complex_Obj_t[complex_Obj_count];

    IntVar gantry_left_y[complex_Obj_count];
    IntVar gantry_right_y[complex_Obj_count];        
    
public:
    ComplexSequentialProblem()
	: space_vars(*this, 2 * complex_Obj_count, 0, complex_sheet_resolution)
  	, time_vars(*this, complex_Obj_count, 0, complex_time_resolution)
	, kine_vars(*this, 2 * complex_Obj_count, 0, complex_sheet_resolution)
    {
	/*
	int width_span =  max_width - min_width;
	int height_span =  max_height - min_height;
	int duration_span =  max_duration - min_duration;		
	
	for (int i = 0; i < complex_Obj_count; ++i)
	{
	    complex_Obj_widths[i] = min_width + rand() % width_span;
	    complex_Obj_heights[i] = min_height + rand() % height_span;
	    complex_Obj_durations[i] = min_duration + rand() % duration_span;	    	    
	}
	*/
	for (int i = 0; i < complex_Obj_count; ++i)
	{
	    complex_Obj_x[i] = space_vars[2 * i];
	    complex_Obj_y[i] = space_vars[2 * i + 1];
	    complex_Obj_t[i] = time_vars[i];
	    
	    gantry_left_y[i] = kine_vars[2 * i];
	    gantry_right_y[i] = kine_vars[2 * i + 1];
	}

	IntArgs widths(complex_Obj_count);
	IntArgs heights(complex_Obj_count);
	
	IntVarArgs X_vars(complex_Obj_count);
	IntVarArgs Y_vars(complex_Obj_count);		

	for (int i =  0; i < complex_Obj_count; ++i)
	{
	    widths[i] = complex_Obj_widths[i];
	    heights[i] = complex_Obj_heights[i];

	    X_vars[i] = complex_Obj_x[i];
	    Y_vars[i] = complex_Obj_y[i];
	}
	    	
	nooverlap(*this, X_vars, widths, Y_vars, heights, IPL_BND);   

	for (int i = 0; i < complex_Obj_count - 1; ++i)
	{
	    for (int j = i + 1; j < complex_Obj_count; ++j)
	    {
		rel(*this, complex_Obj_t[i] >= complex_Obj_t[j] + complex_Obj_durations[j] || complex_Obj_t[j] >= complex_Obj_t[i] + complex_Obj_durations[i], IPL_BND);		
	    }
	}

	for (int i = 0; i < complex_Obj_count; ++i)
	{
	    rel(*this, complex_Obj_x[i] + complex_Obj_widths[i] <= complex_sheet_resolution, IPL_BND);
	    rel(*this, complex_Obj_y[i] + complex_Obj_heights[i] <= complex_sheet_resolution, IPL_BND);  
	}

	for (int i = 0; i < complex_Obj_count; ++i)
	{
	    if (complex_Obj_durations[i] >= complex_height_threshold)
	    {
		rel(*this, gantry_left_y[i] == complex_Obj_y[i] + gantry_left_shift);
		rel(*this, gantry_right_y[i] == complex_Obj_y[i] + gantry_right_shift);		
	    }
	}
	
	for (int i = 0; i < complex_Obj_count; ++i)
	{
	    if (complex_Obj_durations[i] >= complex_height_threshold)
	    {
		for (int j = 0; j < complex_Obj_count; ++j)
		{
		    if (j != i)
		    {
		    rel(*this, complex_Obj_t[j] < complex_Obj_t[i]
			|| complex_Obj_y[j] >= gantry_right_y[i] + gantry_right_height
			|| gantry_right_y[i] >= complex_Obj_y[j] + complex_Obj_heights[j], IPL_BND);
		    
		    rel(*this, complex_Obj_t[j] < complex_Obj_t[i]
			|| complex_Obj_y[j] >= gantry_left_y[i] + gantry_left_height
			|| gantry_left_y[i] >= complex_Obj_y[j] + complex_Obj_heights[j], IPL_BND);		    
		    }
		}
	    }
	}       	

	branch(*this, space_vars, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
	branch(*this, time_vars, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
	branch(*this, kine_vars, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    };

    ComplexSequentialProblem(ComplexSequentialProblem &s)
	: Space(s)
    {
	for (int i = 0; i < complex_Obj_count; ++i)
	{
	    complex_Obj_x[i].update(*this, s.complex_Obj_x[i]);	
	    complex_Obj_y[i].update(*this, s.complex_Obj_y[i]);
	    complex_Obj_t[i].update(*this, s.complex_Obj_t[i]);
	    
	    gantry_left_y[i].update(*this, s.gantry_left_y[i]);
	    gantry_right_y[i].update(*this, s.gantry_right_y[i]);
	}
    }

    virtual Space* copy(void) {
	return new ComplexSequentialProblem(*this);
    };

    void print(void) const
    {
	for (int i = 0; i < complex_Obj_count; ++i)
	{	
	    printf("Position object: x:%d, y:%d, t:%d (w:%d, h:%d, d:%d)\n",
		   complex_Obj_x[i].val(),
		   complex_Obj_y[i].val(),
		   complex_Obj_t[i].val(),
		   complex_Obj_widths[i],
		   complex_Obj_heights[i],
		   complex_Obj_durations[i]);
	}
	
	for (int i = 0; i < complex_Obj_count; ++i)
	{
	    if (complex_Obj_durations[i] >= complex_height_threshold)
	    {
		printf("Gantry position (%d): left_y:%d right_y:%d\n", i,
		       gantry_left_y[i].val(),
		       gantry_right_y[i].val());
	    }
	}
    };
};


int complex_sheet_resolution_X = 200;

int complex_sheet_resolution_X_min = 10;
int complex_sheet_resolution_X_max = 200;


int complex_sheet_resolution_Y = 30;

int complex_sheet_resolution_Y_min = 10;
int complex_sheet_resolution_Y_max = 200;


void generate_random_complex_objects(void)
{
    int width_span =  max_width - min_width;
    int height_span =  max_height - min_height;
    int duration_span =  max_duration - min_duration;
	
    for (int i = 0; i < complex_Obj_count; ++i)
    {
	printf("Generating random object %d ...\n", i);
	complex_Obj_widths[i] = min_width + rand() % width_span;
	complex_Obj_heights[i] = min_height + rand() % height_span;
	complex_Obj_durations[i] = min_duration + rand() % duration_span;	    	    
    }
}


class ComplexSequentialProblemXY : public Space
{
public:
    IntVarArray space_vars_X;
    IntVarArray space_vars_Y;
    
    IntVarArray time_vars;
    
    IntVarArray kine_vars_L;
    IntVarArray kine_vars_R;    
    
    IntVar complex_Obj_x[complex_Obj_count];
    IntVar complex_Obj_y[complex_Obj_count];
    IntVar complex_Obj_t[complex_Obj_count];

    IntVar gantry_left_y[complex_Obj_count];
    IntVar gantry_right_y[complex_Obj_count];        
    
public:
    ComplexSequentialProblemXY()
	: space_vars_X(*this, 2 * complex_Obj_count, 0, complex_sheet_resolution_X)
	, space_vars_Y(*this, 2 * complex_Obj_count, 0, complex_sheet_resolution_Y)
  	, time_vars(*this, complex_Obj_count, 0, complex_time_resolution)	  
	, kine_vars_L(*this, complex_Obj_count, 0, complex_sheet_resolution_Y)
	, kine_vars_R(*this, complex_Obj_count, 0, complex_sheet_resolution_Y)	  
    {
	for (int i = 0; i < complex_Obj_count; ++i)
	{
	    complex_Obj_x[i] = space_vars_X[2 * i];
	    complex_Obj_y[i] = space_vars_X[2 * i + 1];
	    
	    complex_Obj_t[i] = time_vars[i];
	    
	    gantry_left_y[i] = kine_vars_L[i];
	    gantry_right_y[i] = kine_vars_R[i];
	}

	IntArgs widths(complex_Obj_count);
	IntArgs heights(complex_Obj_count);
	
	IntVarArgs X_vars(complex_Obj_count);
	IntVarArgs Y_vars(complex_Obj_count);		

	for (int i =  0; i < complex_Obj_count; ++i)
	{
	    widths[i] = complex_Obj_widths[i];
	    heights[i] = complex_Obj_heights[i];

	    X_vars[i] = complex_Obj_x[i];
	    Y_vars[i] = complex_Obj_y[i];
	}
	    	
	nooverlap(*this, X_vars, widths, Y_vars, heights, IPL_BND);

	for (int i = 0; i < complex_Obj_count - 1; ++i)
	{
	    for (int j = i + 1; j < complex_Obj_count; ++j)
	    {
		rel(*this, complex_Obj_t[i] >= complex_Obj_t[j] + complex_Obj_durations[j] || complex_Obj_t[j] >= complex_Obj_t[i] + complex_Obj_durations[i], IPL_BND);		
	    }
	}

	for (int i = 0; i < complex_Obj_count; ++i)
	{
	    rel(*this, complex_Obj_x[i] + complex_Obj_widths[i] <= complex_sheet_resolution_X, IPL_BND);
	    rel(*this, complex_Obj_y[i] + complex_Obj_heights[i] <= complex_sheet_resolution_Y, IPL_BND);  
	}


	for (int i = 0; i < complex_Obj_count; ++i)
	{
	    if (complex_Obj_durations[i] >= complex_height_threshold)
	    {
		rel(*this, gantry_left_y[i] == complex_Obj_y[i] + gantry_left_shift);
		rel(*this, gantry_right_y[i] == complex_Obj_y[i] + gantry_right_shift);		
	    }
	}

	for (int i = 0; i < complex_Obj_count; ++i)
	{
	    if (complex_Obj_durations[i] >= complex_height_threshold)
	    {
		for (int j = 0; j < complex_Obj_count; ++j)
		{
		    if (j != i)
		    {
			rel(*this, complex_Obj_t[j] < complex_Obj_t[i]
			    || complex_Obj_y[j] >= gantry_right_y[i] + gantry_right_height
			    || gantry_right_y[i] >= complex_Obj_y[j] + complex_Obj_heights[j], IPL_BND);
		    
			rel(*this, complex_Obj_t[j] < complex_Obj_t[i]
			    || complex_Obj_y[j] >= gantry_left_y[i] + gantry_left_height
			    || gantry_left_y[i] >= complex_Obj_y[j] + complex_Obj_heights[j], IPL_BND);		    
		    }
		}
	    }
	}       	

	branch(*this, space_vars_Y, INT_VAR_SIZE_MIN(), INT_VAL_MIN());	
	branch(*this, space_vars_X, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
	branch(*this, time_vars, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
	branch(*this, kine_vars_L, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
	branch(*this, kine_vars_R, INT_VAR_SIZE_MIN(), INT_VAL_MIN());	
    };

    ComplexSequentialProblemXY(ComplexSequentialProblemXY &s)
	: Space(s)
    {
	for (int i = 0; i < complex_Obj_count; ++i)
	{
	    complex_Obj_x[i].update(*this, s.complex_Obj_x[i]);	
	    complex_Obj_y[i].update(*this, s.complex_Obj_y[i]);
	    complex_Obj_t[i].update(*this, s.complex_Obj_t[i]);
	    
	    gantry_left_y[i].update(*this, s.gantry_left_y[i]);
	    gantry_right_y[i].update(*this, s.gantry_right_y[i]);
	}
    }

    virtual Space* copy(void) {
	return new ComplexSequentialProblemXY(*this);
    };

    void print(void) const
    {
	for (int i = 0; i < complex_Obj_count; ++i)
	{	
	    printf("Position object: x:%d, y:%d, t:%d (w:%d, h:%d, d:%d)\n",
		   complex_Obj_x[i].val(),
		   complex_Obj_y[i].val(),
		   complex_Obj_t[i].val(),
		   complex_Obj_widths[i],
		   complex_Obj_heights[i],
		   complex_Obj_durations[i]);
	}
	
	for (int i = 0; i < complex_Obj_count; ++i)
	{
	    if (complex_Obj_durations[i] >= complex_height_threshold)
	    {
		printf("Gantry position (%d): left_y:%d right_y:%d\n", i,
		       gantry_left_y[i].val(),
		       gantry_right_y[i].val());
	    }
	}
    };
};


void test_arrangement_2(void)
{ 
    printf("Testing sheet arrangement 2 ...\n");

    SimpleProblem *simple_problem = new SimpleProblem();
    DFS<SimpleProblem> engine(simple_problem);
    delete simple_problem;

    while (SimpleProblem *simple_P = engine.next())	
    {
	simple_P->print();
	delete simple_P;
    }
    Search::Statistics stat = engine.statistics();
    printf("Statistics: %ld, %ld, %ld, %ld, %ld\n", stat.fail, stat.node, stat.depth, stat.restart, stat.nogood);
       
    printf("Testing sheet arrangement 2 ... finished\n");
}


void test_arrangement_3(void)
{ 
    printf("Testing sheet arrangement 3 ...\n");

    ArrangementProblem *arrangement_problem = new ArrangementProblem();
    DFS<ArrangementProblem> engine(arrangement_problem);
    delete arrangement_problem;

    while (ArrangementProblem *arrangement_P = engine.next())	
    {
	arrangement_P->print();
	delete arrangement_P;	
	printf("\n");
	getchar();
    }
    Search::Statistics stat = engine.statistics();
    printf("Statistics: %ld, %ld, %ld, %ld, %ld\n", stat.fail, stat.node, stat.depth, stat.restart, stat.nogood);
       
    printf("Testing sheet arrangement 3 ... finished\n");
}


void test_arrangement_4(void)
{ 
    printf("Testing sheet arrangement 4 ...\n");

    SimpleSequentialProblem *sequential_problem = new SimpleSequentialProblem();
    DFS<SimpleSequentialProblem> engine(sequential_problem);
    delete sequential_problem;

    while (SimpleSequentialProblem *sequential_P = engine.next())	
    {
	sequential_P->print();
	delete sequential_P;

	Search::Statistics stat = engine.statistics();
	printf("Statistics: %ld, %ld, %ld, %ld, %ld\n", stat.fail, stat.node, stat.depth, stat.restart, stat.nogood);	
	printf("\n");
	getchar();
    }
       
    printf("Testing sheet arrangement 4 ... finished\n");
}


void test_arrangement_5(void)
{ 
    printf("Testing sheet arrangement 5 ...\n");
    generate_random_complex_objects();

    ComplexSequentialProblem *sequential_problem = new ComplexSequentialProblem();
    Search::Options options;
    options.cutoff = Search::Cutoff::linear();
    options.nogoods_limit = 512;
    DFS<ComplexSequentialProblem> engine(sequential_problem, options);
    delete sequential_problem;

    clock_t begin = clock();

    while (ComplexSequentialProblem *sequential_P = engine.next())	
    {
	sequential_P->print();
	clock_t end = clock();
	printf("Time (CPU): %.3fs\n", (end - begin) / (double)CLOCKS_PER_SEC);
	
	delete sequential_P;

	Search::Statistics stat = engine.statistics();
	printf("Statistics: %ld, %ld, %ld, %ld, %ld\n", stat.fail, stat.node, stat.depth, stat.restart, stat.nogood);	
	printf("\n");
	getchar();
    }
    clock_t end = clock();
    printf("Time (CPU): %.3fs\n", (end - begin) / (double)CLOCKS_PER_SEC);    
       
    printf("Testing sheet arrangement 5 ... finished\n");
}


void test_arrangement_6(void)
{ 
    printf("Testing sheet arrangement 6 ...\n");
    generate_random_complex_objects();

    complex_sheet_resolution = complex_sheet_resolution_max;
    
    while (complex_sheet_resolution > complex_sheet_resolution_min)
    {
	printf("Trying sheet resolution = %d\n", complex_sheet_resolution);
	
	ComplexSequentialProblem *sequential_problem = new ComplexSequentialProblem();
	
	Search::Options options;
	/*
	options.cutoff = Search::Cutoff::linear();
	options.nogoods_limit = 16 * 512;
	*/
	options.stop = Search::Stop::time(512);	
	
	DFS<ComplexSequentialProblem> engine(sequential_problem, options);	
	delete sequential_problem;
	
	clock_t begin = clock();

	ComplexSequentialProblem *sequential_P = engine.next();
	    
	if (sequential_P != NULL)
	{
	    sequential_P->print();
	    clock_t end = clock();
	    printf("Time (CPU): %.3fs\n", (end - begin) / (double)CLOCKS_PER_SEC);	   
	    
	    Search::Statistics stat = engine.statistics();
	    printf("Statistics: %ld, %ld, %ld, %ld, %ld\n", stat.fail, stat.node, stat.depth, stat.restart, stat.nogood);	
	    printf("\n");
	    printf("SUCCESS\n");
	}
	else
	{
	    clock_t end = clock();
	    printf("Time (CPU): %.3fs\n", (end - begin) / (double)CLOCKS_PER_SEC);

	    Search::Statistics stat = engine.statistics();
	    printf("Statistics: %ld, %ld, %ld, %ld, %ld\n", stat.fail, stat.node, stat.depth, stat.restart, stat.nogood);	

	    printf("FAIL\n");
	}
	delete sequential_P;	
	
	complex_sheet_resolution -= 1;
	
    }       
    printf("Testing sheet arrangement 6 ... finished\n");
}


void test_arrangement_7(void)
{ 
    printf("Testing sheet arrangement 7 ...\n");
    generate_random_complex_objects();    

    complex_sheet_resolution_X = complex_sheet_resolution_X_max;
    
    while (complex_sheet_resolution_X > complex_sheet_resolution_X_min)
    {
	printf("Trying sheet resolution X = %d, Y = %d\n", complex_sheet_resolution_X, complex_sheet_resolution_Y);
	
	ComplexSequentialProblemXY *sequential_problem = new ComplexSequentialProblemXY();
	Search::Options options;
	/*
	options.cutoff = Search::Cutoff::linear();
	options.nogoods_limit = 16 * 512;
	*/
	options.stop = Search::Stop::time(10 * 512);
	
	DFS<ComplexSequentialProblemXY> engine(sequential_problem, options);
	delete sequential_problem;
	
	clock_t begin = clock();
	
	ComplexSequentialProblemXY *sequential_P = engine.next();
	
	if (sequential_P != NULL)
	{
	    sequential_P->print();
	    
	    clock_t end = clock();
	    printf("Time (CPU): %.3fs\n", (end - begin) / (double)CLOCKS_PER_SEC);	   
	    
	    Search::Statistics stat = engine.statistics();
	    printf("Statistics: %ld, %ld, %ld, %ld, %ld\n", stat.fail, stat.node, stat.depth, stat.restart, stat.nogood);	
	    printf("SUCCESS\n");
	}
	else
	{
	    clock_t end = clock();
	    printf("Time (CPU): %.3fs\n", (end - begin) / (double)CLOCKS_PER_SEC);
	    Search::Statistics stat = engine.statistics();
	    printf("Statistics: %ld, %ld, %ld, %ld, %ld\n", stat.fail, stat.node, stat.depth, stat.restart, stat.nogood);
	    
	    printf("FAIL\n");
	}
	delete sequential_P;	
	
	complex_sheet_resolution_X -= 1;
	
    }       
    printf("Testing sheet arrangement 7 ... finished\n");
}



/*----------------------------------------------------------------*/

int main(int UNUSED(argc), char **UNUSED(argv))
{
//    test_arrangement_1();
//    test_arrangement_2();
//    test_arrangement_3();
//    test_arrangement_4();
//    test_arrangement_5();
//    test_arrangement_6();
    test_arrangement_7();    
    
    return 0;
}

