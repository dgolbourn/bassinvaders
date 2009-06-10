/*
 * spline.h
 *
 *  Created on: May 19, 2009
 *      Author: Darren Golbourn
 */

#ifndef spline_H_
#define spline_H_
#include "gsl/gsl_spline.h"

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
 * A structure with a couple of functions in it
 */
struct parametricFunctor
{
	Functor *x;
	Functor *y;
};

/*
 * shorthand macro to help copy values from a parametric function at point t
 * into x and y;
 *
 * evalp(double x [x coordinate], double y [y coordinate], double t [parameter], parametricFunctor P)
 */
#define evalp(X, Y, t, P) {(X) = (*((P).x))((t)); (Y) = (*((P).y))((t));}

/*
 * A spline is a type of functor that encapsulates a look up table and an interpolation algorithm
 */
class spline: Functor {
	gsl_spline *function;
	gsl_interp_accel *accel;
public:
	spline( const double xa[], const double ya[], size_t size );
	virtual ~spline();
	double operator()(double x);

	static double eq(double freq, void* e);
};

#endif /* spline_H_ */
