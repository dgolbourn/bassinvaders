/*
 * Entity.cpp
 *
 *  Created on: 19-Apr-2009
 *      Author: spijderman
 */

#include "Entity.h"
#include "toolkit.h"

Entity::Entity() {}

Entity::~Entity()
{
	/* you'd think it would make sense to free up all the sdl surfaces that
	 * were allocated to make this Entity here, but actually they are
	 * handled by the Resource Bundle.
	 */
}

void Entity::setVelocity(int32_t xvelocity, int32_t yvelocity)
{
	this->xvelocity = xvelocity;
	this->yvelocity = yvelocity;
}

void Entity::changeState(Entitiestate_t newState)
{
	if (currentState != newState)	pendingState = newState;
}

Entitiestate_t Entity::getState()
{
	return currentState;
}

EntityType_t Entity::getType()
{
	return type;
}

int32_t Entity::getHealth()
{
	return health;
}

uint32_t Entity::getAttackDamage()
{
	return attackDamage;
}

bool Entity::isOffScreenX(int32_t screenWidth, int32_t buffer )
{
	return ( (xpos < -buffer) || (xpos > (screenWidth+buffer)) );
}

bool Entity::isOffScreenY(int32_t screenHeight, int32_t buffer )
{
	return (  (ypos < -buffer) || (ypos > (screenHeight+buffer)) );
}

bool Entity::canBeRemoved()
{
	if (isOffScreenX() || isOffScreenY()) return true;

	if (currentState == RS_DEAD)
	{
		std::vector<Sprite*> mySprites = this->getActiveSpriteList();
		std::vector<Sprite*>::iterator myPos;

		for (myPos = mySprites.begin(); myPos != mySprites.end(); ++myPos)
		{
			if ((*myPos)->getAnimationState() != AS_DEAD)	return false;
		}

		return true;
	}

	return false;
}

void Entity::updatePosition()
{
	xpos += xvelocity;
	ypos += yvelocity;
}

void Entity::move()
{
	uint32_t now = SDL_GetTicks();
	uint32_t delta = now - lastTickCount;

	if (delta > velocityTicks)
	{
		updatePosition();
		lastTickCount = now;
	}
}

bool Entity::isCollidingWith(Entity* pOther)
{
	/* if either Entity is dead, they can't be colliding*/
	if ((pOther->getState() == RS_DEAD) || (this->getState() == RS_DEAD))
		return false;

	/* get local active sprite list
	 * get pOther active sprite list
	 *
	 * iterate comparing all to all...
	 * return true if any collide
	 */
	std::vector<Sprite*> mySprites = this->getActiveSpriteList();
	std::vector<Sprite*> otherSprites = pOther->getActiveSpriteList();

	std::vector<Sprite*>::iterator myPos;
	std::vector<Sprite*>::iterator otherPos;

	for (myPos = mySprites.begin(); myPos != mySprites.end(); ++myPos)
	{
		for (otherPos = otherSprites.begin(); otherPos != otherSprites.end(); ++otherPos)
		{
			if ((*myPos)->isCollidingWith((*otherPos)->getCollisionRects()))
			{
				return true;
			}
		}
	}

	return false;
}
