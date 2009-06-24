/*
 * monster.cpp
 *
 *  Created on: 8/5/09
 *      Author: Darren Golbourn
 */

#include "monster.h"
#include "affineTransform.h"

monster::monster(int32_t x0, int32_t y0, double monsterTime, Path path)
{
	loadMonsterData();
	this->x0 = x0;
	this->y0 = y0;
	type = RT_ENEMY;
	currentState = RS_ACTIVE;
	pendingState = RS_ACTIVE;
	health = 10;
	attackDamage = 10;
	velocityTicks = 10;
	this->path = path;
	s = monsterTime;
	updatePosition();
	localPath = false;
}

monster::monster(int32_t height)
{
	loadMonsterData();
	xpos = SCREEN_WIDTH;
	ypos = height;
	type = RT_ENEMY;
	currentState = RS_ACTIVE;
	pendingState = RS_ACTIVE;

	/* JG TODO: put these in a file */
	health = 10;
	attackDamage = 10;
	velocityTicks = 10;

	/*
	 * This is to set up the path the monster takes across the screen.
	 */
	x0 = xpos;	// Starting x position.
	y0 = ypos;	// Starting y position.

	localPath = true; // monster manages its own path memory.
	path.x = new linear(MONSTER_X_SPEED);	// Linear motion in x.
	path.y = new constant(); // constant y

	s = 0; // Start at "time" s=0.
}

monster::~monster() {
	if (localPath)
	{
		delete path.x;
		delete path.y;
	}
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
	s++; // increase "monster time" by 1.
	updateStates();
}

void monster::updatePosition()
{
	affine_rotate R(0.1);
	affine_translate T(x0, y0);
	path.transformStack.push(&R);
	path.transformStack.push(&T);
	path.get(&xpos, &ypos, s);
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
