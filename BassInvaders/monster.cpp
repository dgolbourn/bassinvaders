/*
 * monster.cpp
 *
 *  Created on: 8/5/09
 *      Author: Darren Golbourn
 */

#include "monster.h"

monster::monster(int32_t height)
{
	loadMonsterData();

	xvelocity = MONSTER_X_SPEED;
	yvelocity = MONSTER_Y_SPEED;
	xpos = SCREEN_WIDTH;
	ypos = height;
	type = RT_ENEMY;
	currentState = RS_ACTIVE;
	pendingState = RS_ACTIVE;

	/* JG TODO: put these in a file */
	health = 10;
	attackDamage = 10;
	velocityTicks = 10;

	path = NULL;
	s = 0;
}

monster::~monster() {
}

void monster::loadMonsterData()
{
	ResourceBundle* resource = &(*(ResourceBundle::getResource("resources/sprites/monster.info")));
	Sprite monsterBody(resource);

	sprites.push_back(monsterBody);
}

void monster::render(SDL_Surface* pScreen)
{
	sprites[MAIN_SPRITE].setLocation(this->xpos, this->ypos);
	sprites[MAIN_SPRITE].renderSprite(pScreen);
}

std::vector<Sprite*> monster::getActiveSpriteList()
{
	std::vector<Sprite*> ret;
	ret.push_back(&(sprites[MAIN_SPRITE]));
	return ret;
}

void monster::update()
{
	updateStates();
}

void monster::updatePosition()
{
	if (path == NULL)
	{
		xpos += xvelocity;
		ypos += yvelocity;
	}
	else evalp(xpos, ypos, s, *path)
}

void monster::updateStates()
{
	currentState = pendingState;

	switch(currentState)
	{
		case RS_ACTIVE:
		{
			sprites[MAIN_SPRITE].changeState(AS_IDLE);
		}break;

		case RS_DEAD:
		{
			/* the monster has been killed, so trigger the dying animation */
			sprites[MAIN_SPRITE].changeState(AS_DYING);
		}break;
	}
}

void monster::doCollision(Entity* pOther)
{
	/* read type of Entity.
	 * if enemy/powerup:
 	 *  return
	 * if friendly: (i.e. bad for monsters)
	 *   find out if collision
	 *   if true, call reactToCollision on both Entities
	 */

	switch(pOther->getType())
	{
		case RT_FRIENDLY: // hero ship and bullets are "friendly"
		{
			if (isCollidingWith(pOther))
			{
				this->reactToCollision(pOther);
				pOther->reactToCollision(this);
			}
		}break;

		case RT_POWERUP:
		case RT_ENEMY:
		case RT_NEUTRAL:
		default:
		{
			return;
		}
	}
}

void monster::reactToCollision(Entity* pOther)
{
	switch(pOther->getType())
	{
		case RT_FRIENDLY: // hero ship and bullets are "friendly"
		{
			//monster has been hit by a hero or a bullet!
			health -= pOther->getAttackDamage();
			if (health < 0)
			{
				changeState(RS_DEAD);
			}

			DebugPrint(("Monster hit by friendly, down to %d health\n", health));
		}break;

		case RT_POWERUP:
		case RT_ENEMY:
		case RT_NEUTRAL:
		default:
		{
			return;
		}
	}
}
