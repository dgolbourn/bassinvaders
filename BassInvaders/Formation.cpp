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
	tally = num;
	formationState = FORMATION_ACTIVE;

	for (int i = 0; i < num; i++)
	{
		path.s0 = -i*20;
		Entity* enemy = new monster(path);
		entities.push_back(enemy);
		BassInvaders::theGame->pRM->addEnemy(enemy);
	}
}

monsterLine::~monsterLine()
{
}

/*
 * here we get the state of each enemy in the formation and act on that information if required.
 * We don't do any memory management here as although the formation created the entities, they are
 * removed by the entityManager later.  For this reason, monsterLine::update() should be called immediately before
 * entityManager::removeInactiveEntities().
 */
void monsterLine::update()
{
	std::deque<Entity*>::iterator pos;
	for (pos = entities.begin(); pos != entities.end();)
	{
		/*
		 * if an enemy is dead, score one for the tally and get that enemy off the list!
		 */
		if ((*pos)->getState() == RS_DEAD)
		{
			tally--;
			pos = entities.erase(pos);
		}
		/*
		 * ... otherwise, if the enemy is ready to be removed, remove it from the list anyway
		 */
		else if ((*pos)->canBeRemoved())
		{
			pos = entities.erase(pos);
		}
	}

	/*
	 * if all the entities are gone, flag that this monsterLine is finished.
	 */
	if (entities.size() == 0)
	{
		formationState = FORMATION_FINISHED;
	}

	/*
	 * if all the enemies were killed, do something (like score bonus)
	 */
	if (tally == 0)
	{

	}
}
