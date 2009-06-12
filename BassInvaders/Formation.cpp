/*
 * Formation.cpp
 *
 *  Created on: May 24, 2009
 *      Author: Darren Golbourn
 */

#include "Formation.h"
#include "BassInvaders.h"

void randomHorde::update()
{
	if (BI->isBeat())
	{
		monster *M = new(std::nothrow) monster(rand()%SCREEN_HEIGHT-50);

		if (M) BassInvaders::theGame->pRM->addEnemy(M);
	}
}

randomHorde::randomHorde()
{
	BI = BassInvaders::theGame->beat->iterator(COOLDOWN);
}
