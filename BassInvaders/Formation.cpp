/*
 * Formation.cpp
 *
 *  Created on: May 24, 2009
 *      Author: Darren Golbourn
 */

#include "Formation.h"
#include "BassInvaders.h"

randomHorde::randomHorde(Path path)
{
	BassInvaders::theGame->pRM->addEnemy(new monster(path));
}

randomHorde::~randomHorde()
{
}

monsterLine::monsterLine(Path path, uint8_t num)
{
	for (int i = 0; i < num; i++)
	{
		path.s0 = -i*20;
		BassInvaders::theGame->pRM->addEnemy(new monster(path));
	}
}

monsterLine::~monsterLine()
{
}
