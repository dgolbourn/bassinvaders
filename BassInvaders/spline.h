/*
 * spline.h
 *
 *  Created on: May 19, 2009
 *      Author: Darren Golbourn
 */

#ifndef spline_H_
#define spline_H_
#include "gsl/gsl_spline.h"

class spline {
	gsl_spline *function;
	gsl_interp_accel *accel;
public:
	spline( const double xa[], const double ya[], size_t size );
	virtual ~spline();
	double get(double x);

	static double eq(double freq, void* e);
};

#endif /* spline_H_ */
