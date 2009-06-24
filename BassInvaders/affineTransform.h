/*
 * affineTransform.h
 *
 *  Created on: Jun 24, 2009
 *      Author: Darren Golbourn
 */

#ifndef AFFINETRANSFORM_H_
#define AFFINETRANSFORM_H_
#include <cmath>

/*
 * template for a general affineTransform,
 */
class affineTransform {
public:
	virtual ~affineTransform() {};
	virtual void operator() (double* xpos, double* ypos) = 0;
};

/*
 * TRANSLATE: translate points x -> x + dx and y -> y + dy
 */
class affine_translate : public affineTransform{
public:
	double dx;
	double dy;
	affine_translate(double dx, double dy) {this->dx = dx; this->dy = dy;}
	void operator() (double *xpos, double *ypos) { (*xpos) += dx; (*ypos) += dy;}
};

/*
 * ROTATE: rotate x and y around the origin by theta radians
 */
class affine_rotate : public affineTransform{
public:
	double theta;
	affine_rotate(double theta) {this->theta = theta;}
	void operator() (double *xpos, double *ypos){

		(*xpos) = (*xpos) * cos(theta) + (*ypos) * sin(theta);
		(*ypos) = (*xpos) * -sin(theta) + (*ypos) * cos(theta);
	}
};

/*
 * SCALE: rescale x and y around the origin by a factor kx and ky
 */
class affine_scale : public affineTransform{
public:
	double kx;
	double ky;
	affine_scale(double kx, double ky) {this->kx = kx; this->ky = ky;}
	void operator() (double *xpos, double *ypos){
		(*xpos) *= kx;
		(*ypos) *= ky;
	}
};

/*
 * SHEAR: move the y coordinate by and amount k * y (rotate first to shear around a different line, then rotate back)
 */
class affine_shear : public affineTransform{
public:
	double k;
	affine_shear(double k) {this->k = k;}
	void operator() (double *xpos, double *ypos){
		(*ypos) += k * (*ypos);
	}
};
#endif /* AFFINETRANSFORM_H_ */
