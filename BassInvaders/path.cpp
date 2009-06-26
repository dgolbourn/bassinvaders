/*
 * path.cpp
 *
 *  Created on: Jun 24, 2009
 *      Author: Darren Golbourn
 */

#include "path.h"
#include <boost/numeric/ublas/io.hpp>

Path::Path() {
	x = NULL;
	y = NULL;

	defaultStack = identity_matrix<double>(3);
	transformStack = identity_matrix<double>(3);
}

Path::~Path() {
}

void Path::get(int32_t *xpos, int32_t *ypos, double s)
{
	if (x == NULL || y == NULL || xpos == NULL || ypos == NULL ) return;

    vector<double> pos(3);
    pos(0) = (*x)(s);
	pos(1) = (*y)(s);
	pos(2) = 1;

	pos = prec_prod(defaultStack, pos);
	pos = prec_prod(transformStack, pos);
	transformStack = identity_matrix<double>(3);

	(*xpos) = (int32_t)pos(0);
	(*ypos) = (int32_t)pos(1);
}
