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
 */

#ifndef PATH_H_
#define PATH_H_
#include "spline.h"
#include <queue>
#include "affineTransform.h"

class Path {
public:
	Functor *x; // function that returns x(s)
	Functor *y; // function that returns y(s)
	std::queue<affineTransform*> transformStack; // stack of affine transforms to apply to coordinates.

	Path();
	virtual ~Path();
	void get(int32_t *xpos, int32_t *ypos, double s);
};
#endif /* PATH_H_ */
