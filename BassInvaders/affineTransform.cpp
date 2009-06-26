#include "affineTransform.h"

/*
 * TRANSLATE: translate points x -> x + dx and y -> y + dy
 */
matrix<double> affine_translate(double dx, double dy)
{
	matrix<double> M = identity_matrix<double>(3,3);
	M(0,2) = dx;
	M(1,2) = dy;
	return M;
}

/*
 * ROTATE: rotate x and y around the origin by theta radians
 */
matrix<double> affine_rotate(double theta)
{
	matrix<double> M = zero_matrix<double>(3,3);
	double c = cos(theta);
	double s = sin(theta);
	M(0,0) = c;
	M(1,0) = s;
	M(1,1) = c;
	M(0,1) = -s;
	M(2,2) = 1;
	return M;
}

/*
 * SCALE: rescale x and y around the origin by a factor kx and ky
 */
/*matrix<double> affine_scale(double kx, double ky)
{
	zero_matrix<double> M (3, 3);
	return M;
}*/
