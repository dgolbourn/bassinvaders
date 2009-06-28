/*
 * spline.cpp
 *
 *  Created on: May 19, 2009
 *      Author: Darren Golbourn
 */

#include "spline.h"
#include "monster.h"
#include "WindowManager.h"

spline::spline( const double xa[], const double ya[], size_t size ){
	accel = gsl_interp_accel_alloc();
	function = gsl_spline_alloc(gsl_interp_cspline, size);
	gsl_spline_init(function, xa, ya, size);
}

spline::~spline() {
	gsl_spline_free(function);
	gsl_interp_accel_free(accel);
}

double spline::operator()(double x){
	return gsl_spline_eval(function, x, accel);
}

double spline::eq(double freq, void* e)
{
	return (*(spline*)e)(freq);
}

linear defaultFunctors::monsterLinearX = linear(MONSTER_X_SPEED);
sine defaultFunctors::monsterSineY = sine(50, SCREEN_WIDTH/6, 0);
constant defaultFunctors::monsterConstantY = constant();
