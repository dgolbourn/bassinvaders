/*
 * path.cpp
 *
 *  Created on: Jun 24, 2009
 *      Author: Darren Golbourn
 */

#include "path.h"

Path::Path() {
	x = NULL;
	y = NULL;
}

Path::~Path() {
}

void Path::get(int32_t *xpos, int32_t *ypos, double s)
{
	if (x == NULL || y == NULL || xpos == NULL || ypos == NULL ) return;

	double tempX = (*x)(s);
	double tempY = (*y)(s);

	while (!transformStack.empty())
	{
		(*transformStack.front())(&tempX, &tempY);
	    transformStack.pop();
	}

	(*xpos) = (int32_t)tempX;
	(*ypos) = (int32_t)tempY;
}
