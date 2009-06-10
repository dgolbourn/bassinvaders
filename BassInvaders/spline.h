/*
 * spline.h
 *
 *  Created on: May 19, 2009
 *      Author: Darren Golbourn
 */

#ifndef spline_H_
#define spline_H_
#include "gsl/gsl_spline.h"
#include <cmath>

/*
 * A functor is a class which encapsulates a function
 */
class Functor
{
public:
	virtual ~Functor() {};
	virtual double operator() (double t) = 0;
};

/*
 * A path is the (x,y) location given as a function of a parameter (i.e. time)
 * A path structure contains an x(t) and a y(t) functor which provide the
 * (x,y) position as a function "time".
 */
struct Path
{
	Functor *x; // function that returns x(s)
	Functor *y; // function that returns y(s)
};

/*
 * Shorthand macros to help copy values from a path at point t
 * into x and y;
 *
 * evalpo(	double x [x coordinate],
 * 			double x0 [offset],
 * 			double y [y coordinate],
 * 			double y0 [offset],
 * 			double t [parameter],
 * 			path P)
 */
#define evalpo(X, X0, Y, Y0, t, P) {(X) = (int32_t)((*((P).x))((t)) + (X0)); (Y) = (int32_t)((*((P).y))((t)) + (Y0));}
#define evalp(X, Y, t, P) {(X) = (int32_t)((*((P).x))((t))); (Y) = (int32_t)((*((P).y))((t)));}

/********************************************************************************
 * A few pre-built functors
 ********************************************************************************/

/*
 * SPLINE:
 * 		Description: 	The spline function takes as input a
 * 						look up table of points and then uses an interpolation algorithm
 * 						for points in between.
 */
class spline: public Functor {
	gsl_spline *function;
	gsl_interp_accel *accel;
public:
	/*
	 * look up table is descrete version of y(x) sampled at the array of
	 * points xa[i] with values ya[i], each array is size "size".
	 */
	spline( const double xa[], const double ya[], size_t size );
	virtual ~spline();
	double operator()(double x);

	static double eq(double freq, void* e);
};

/*
 * CONSTANT:
 * 		Description:	The constant function always returns the same value.
 *
 */
class constant: public Functor{
	double val;
public:
	constant(double val = 0.) {this->val = val;}
	double operator() (double s) {return val;}
};

/*
 * LINEAR:
 * 		Description:	The linear function increases at a rate given by the
 * 						variable "speed".
 */
class linear: public Functor{
	double speed;
public:
	linear(double speed) {this->speed = speed;}
	double operator() (double s) {return s*speed;}
};

/*
 * SINE:
 * 		Description:	The sine wave oscillates between "amplitude" and
 * 						-"amplitude" with period "period".
 */
class sine: public Functor{
	double amplitude;
	double period;
	double phase;
public:
	sine(double amplitude, double period, double phase = 0) { this->phase = phase; this->amplitude = amplitude; this->period = 2*M_PI/period;}
	double operator() (double s) {return amplitude*std::sin(phase + (period*s));}
};

#endif /* spline_H_ */
