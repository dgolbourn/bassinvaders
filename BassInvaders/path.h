/*
 * path.h
 *
 *  Created on: Jun 24, 2009
 *      Author: Darren Golbourn
 *
 * A path is the (x,y) location given as a function of a parameter (i.e. time)
 * A path structure contains an x(s) and a y(s) functor which provide the
 * (x,y) position as a function of "time".
 *
 * A path also contains a stack of transformations.  multiply transformations onto the
 * stack and they will be evaluated when you do a get command.
 */

#ifndef PATH_H_
#define PATH_H_
#include "spline.h"
#include "affineTransform.h"
#include <boost/numeric/ublas/matrix.hpp>

using namespace boost::numeric::ublas;

class Path {
public:
	Functor *x; // function that returns x(s)
	Functor *y; // function that returns y(s)
	double s0; // start time for this path
	double ds; // rate of change of s.

	matrix<double> defaultStack;
	matrix<double> transformStack;

	Path();
	virtual ~Path();
	void get(int32_t *xpos, int32_t *ypos, double s);
};
#endif /* PATH_H_ */
