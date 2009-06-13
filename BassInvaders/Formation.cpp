/*
 * Formation.cpp
 *
 *  Created on: May 24, 2009
 *      Author: Darren Golbourn
 */

#include "Formation.h"
#include "BassInvaders.h"

randomHorde::randomHorde(int32_t xpos, int32_t ypos, Path path)
{
	BassInvaders::theGame->pRM->addEnemy(new monster(xpos, ypos, 0, path));
}

randomHorde::~randomHorde()
{
}

monsterLine::monsterLine(int32_t xpos, int32_t ypos, Path path, uint8_t num)
{
	for (int i = 0; i < num; i++)
	{
		BassInvaders::theGame->pRM->addEnemy(new monster(xpos, ypos, -i*20, path));
	}
}

monsterLine::~monsterLine()
{
}
