/*
 * Hero.cpp
 *
 *  Created on: 19-Apr-2009
 *      Author: spijderman
 */

#include "Hero.h"
#include "toolkit.h"
#include <fstream>
#include <iostream>

Hero::Hero(ResourceBundle* resource, EntityManager* pRM)
{
	loadHeroData(resource);
	score = 0;
	xvelocity = 0;
	yvelocity = 0;
	xpos = 100;
	ypos = 100;
	type = RT_FRIENDLY;
	currentState = RS_ACTIVE;
	pendingState = RS_ACTIVE;

	this->velocityTicks = 10;

	this->pRM = pRM;

	fireRate = 100;
	canShoot = false;
	lastFireTicks = 0;
}

/* hero manages it's own sprites */
Hero::~Hero()
{
	std::vector<Sprite>::iterator pos;

	for (pos = sprites.begin(); pos !=sprites.end(); ++pos)
	{
		pos->destroy();
	}
}

void Hero::loadHeroData(ResourceBundle* resource)
{
	this->health = *((int*)(*resource)["health"]);
	this->attackDamage = *((int*)(*resource)["attackdamage"]);
	Sprite heroBody(((ResourceBundle**)(*resource)["bodysprite"])[0]);

	sprites.push_back(heroBody);
}

std::vector<Sprite> Hero::getActiveSpriteList()
{
	std::vector<Sprite> ret;
	ret.push_back(sprites[BODYSPRITE]);
	return ret;
}

void Hero::render(SDL_Surface* pScreen)
{
	sprites[BODYSPRITE].setLocation(this->xpos, this->ypos);
	sprites[BODYSPRITE].renderSprite(pScreen);
}

void Hero::setActions(ACTIONMASK actions)
{
	if (actions & ACTION_MOVE_DOWN)
	{
		yvelocity = HERO_Y_SPEED;
	}
	else if (actions & ACTION_MOVE_UP)
	{
		yvelocity = -(HERO_Y_SPEED);
	}
	else
	{
		yvelocity = 0;
	}

	if (actions & ACTION_MOVE_LEFT)
	{
		xvelocity = -(HERO_X_SPEED);
	}
	else if (actions & ACTION_MOVE_RIGHT)
	{
		xvelocity = HERO_X_SPEED;
	}
	else
	{
		xvelocity = 0;
	}

	if (actions & ACTION_SHOOT)
	{
		uint32_t now = SDL_GetTicks();
		uint32_t delta = now - lastFireTicks;
		if (delta >= fireRate)
		{
			canShoot = true;
			lastFireTicks = now;
		}
	}
}

void Hero::doActions()
{
	if (canShoot)
	{
		//DebugPrint(("Firing bullet\n"));
		/* Create a new bullet, stick it in the Entity manager
		 * the x and y pos are where we want the bullet to spawn (i.e. at the nose of the hero craft)*/
		pRM->addBullet(new Bullet(xpos+100, ypos+50)); //JG TODO: magic numbers must go
		canShoot = false;
	}
}

bool Hero::canBeRemoved()
{
	return false;
}

void Hero::update()
{
	doActions();
	updateStates();
}

void Hero::updateStates(){
	currentState = pendingState;

	switch(currentState)
	{
		case RS_ACTIVE:
		{
			sprites[BODYSPRITE].changeState(AS_IDLE);
		}break;

		case RS_DEAD:
		{
			sprites[BODYSPRITE].changeState(AS_DEAD);
		}break;
	}
}

void Hero::doCollision(Entity* pOther)
{
	/* read type of Entity.
	 * if neutral/friend:
	 *  return
	 * if enemy/powerup:
	 *  find out if collision
	 *   if true, call reactToCollision on both Entities
	 */

	switch(pOther->getType())
	{
		case RT_ENEMY:
		case RT_POWERUP:
		{
			if (isCollidingWith(pOther))
			{
				this->reactToCollision(pOther);
				pOther->reactToCollision(this);
			}
		}break;

		case RT_FRIENDLY:
		case RT_NEUTRAL:
		default:
		{
			return;
		}
	}
}

void Hero::reactToCollision(Entity* pOther)
{
	switch(pOther->getType())
	{
		case RT_ENEMY:
		{

			//hero has hit a monster!
			health -= pOther->getAttackDamage();
			if (health <= DEAD_HEALTH)
			{
				changeState(RS_DEAD);
				health = DEAD_HEALTH;
				// JG TODO: maybe kick off game over logic from this point...
			}
			else if (health < DAMAGED_HEALTH)
			{
				sprites[BODYSPRITE].changeState(AS_DAMAGED);
			}

			DebugPrint(("Hero hit by enemy, down to %d health\n", health));

		}break;

		case RT_POWERUP:
		case RT_FRIENDLY:
		case RT_NEUTRAL:
		default:
		{
			// so far nothing, but if we want to have health pickups, we can do it here
			return;
		}
	}
}
