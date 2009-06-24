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
	virtual double operator() (double t) = 0;   // the basic operation of a functor is to return a
												// value, just like a function.
};

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
	 * look up table is descrete version of f(s) sampled at the array of
	 * points sa[i] with values fa[i], each array is size "size".
	 */
	spline( const double sa[], const double fa[], size_t size );
	virtual ~spline();
	double operator()(double s);

	static double eq(double freq, void* e); // this is so splines can be used as graphic equalisers for the ffts.
											// not really anything to do with motion of enemies!
};

/*
 * CONSTANT:
 * 		Description:	The constant function always returns the same value.
 *
 */
class constant: public Functor{
protected:
	/* arguments */
	double val;

public:
	/* functor methods */
	constant(double val = 0.) {this->val = val;}
	double operator() (double s) {return val;}
};

/*
 * LINEAR:
 * 		Description:	The linear function increases at a rate given by the
 * 						variable "speed".
 */
class linear: public Functor{
protected:
	/* arguments */
	double speed;

public:
	/* functor methods */
	linear(double speed) {this->speed = speed;}
	double operator() (double s) {return s*speed;}
};

/*
 * SINE:
 * 		Description:	The sine wave oscillates between "amplitude" and
 * 						-"amplitude" with period "period".
 */
class sine: public Functor{
protected:
	/* arguments */
	double amplitude;
	double frequency;
	double phase;

public:
	/* functor methods */
	sine(double amplitude, double period, double phase = 0) { this->phase = phase; this->amplitude = amplitude; this->frequency = 2*M_PI/period;}
	double operator() (double s) {return amplitude*std::sin(phase + (frequency*s));}
};


/********************************************************************************
 * static versions of some common functors used by enemies.
 ********************************************************************************/
class defaultFunctors
{
public:
	static constant monsterConstantY;
	static linear monsterLinearX;
	static sine monsterSineY;
};

#endif /* spline_H_ */
