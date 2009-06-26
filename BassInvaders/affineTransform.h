/*
 * affineTransform.h
 *
 *  Created on: Jun 24, 2009
 *      Author: Darren Golbourn
 */

#ifndef AFFINETRANSFORM_H_
#define AFFINETRANSFORM_H_
#include <boost/numeric/ublas/matrix.hpp>
using namespace boost::numeric::ublas;
/*
 * TRANSLATE: translate points x -> x + dx and y -> y + dy
 */
matrix<double> affine_translate(double dx, double dy);

/*
 * ROTATE: rotate x and y around the origin by theta radians
 */
matrix<double> affine_rotate(double theta);

/*
 * SCALE: rescale x and y around the origin by a factor kx and ky
 */
//matrix<double> affine_scale(double kx, double ky);

#endif /* AFFINETRANSFORM_H_ */
