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
	BassInvaders::theGame->pRM->addEnemy(new monster(0, path));
}

randomHorde::~randomHorde()
{
}

monsterLine::monsterLine(Path path, uint8_t num)
{
	for (int i = 0; i < num; i++)
	{
		BassInvaders::theGame->pRM->addEnemy(new monster(-i*20, path));
	}
}

monsterLine::~monsterLine()
{
}
