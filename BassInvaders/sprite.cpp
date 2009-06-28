/*
 * Sprite.cpp
 *
 *  Created on: 19-Apr-2009
 *      Author: spijderman
 */

#include "sprite.h"
#include "toolkit.h"

//#define DEBUG_COLLISIONS

AnimationStateData_t::AnimationStateData_t()
{
	state = AS_INIT;
	nextState = AS_INIT;
	currentAnimationStep = 0;
	numberOfAnimationSteps = 0;
	sheetStartsAt.h = 0;
	sheetStartsAt.w = 0;
	sheetStartsAt.x = 0;
	sheetStartsAt.y = 0;
	spriteWidth = 0;
	spriteHeight = 0;
	ticksPerStep = 0;
	lastAnimTickCount = 0;
	spriteSheet = NULL;
}

void AnimationStateData_t::debugPrint()
{
	cout << "state: " << state << endl;
	cout << "next state: " << nextState << endl;
	cout << "current animation step: " << currentAnimationStep << endl;
	cout << "number of animation steps: " << numberOfAnimationSteps << endl;
	cout << "sheet h: " << sheetStartsAt.h << " w: " << sheetStartsAt.w << " x: " << sheetStartsAt.x << " y: " << sheetStartsAt.y << endl;
	cout << "sprite width: " << spriteWidth << endl;
	cout << "sprite height: " << spriteHeight << endl;
	cout << "ticks/step: " << ticksPerStep << endl;
	cout << "last tick count: " << lastAnimTickCount << endl;
	cout << "sprite sheet address: " << spriteSheet << endl;
	cout << "collision vector length: " << collisionRects.size() << endl;
}

Sprite::Sprite(ResourceBundle * resources/*, BassInvaders * game*/) {
	/* take a text file as a parameter containing all the data for all the states
	 * then pass file into a function which populates an AnimationState_t
	  * note: in real game, will need to store checksums of all data files to confirm vanilla operation*/
	loadSpriteData(resources);

	currentState = AS_INIT;
	pendingState = AS_IDLE;

	xpos = 0;
	ypos = 0;
}

Sprite::~Sprite() {
	/* DON'T free up the surfaces in here.
	 * It's possible that the object was created on the stack
	 * then copied into a container class.
	 * If the owner of the container behaves nicely they will
	 * call the destroy() function when they are done with the sprite
	 */

}

void Sprite::destroy()
{
	uint32_t state = AS_IDLE;

	while(state < AS_STATES_SIZE)
	{
		SDL_FreeSurface(animationStateData[state].spriteSheet);
		state<<=1;
	}
}

void Sprite::changeState(AnimationState_t newState)
{
	if ((currentState == newState)
		|| (pendingState == newState))
	{
		return;
	}

	if (animationStateData[newState].state == 0)
	{
		DebugPrint(("Can't transition to state with no data\n"));
		return;
	}

	switch(currentState)
	{
		case AS_INIT:
		case AS_IDLE:
		case AS_DAMAGED:
		{
			pendingState = newState;

		}break;

		case AS_DYING:
		case AS_DEAD:
		default:
		{
			/* cannot transition away from dying or dead.
			 * Once a sprite is dead, it should be removed and cleaned up */
		}break;
	}
}

void Sprite::renderSprite(SDL_Surface *pScreen)
{
	AnimationStateData_t* pTempState;

	update();

	switch(currentState)
	{
		case AS_IDLE:
		{
			pTempState = &(animationStateData[AS_IDLE]);
		} break;
		case AS_DAMAGED:
		{
			pTempState = &(animationStateData[AS_DAMAGED]);
		} break;
		case AS_DYING:
		{
			pTempState = &(animationStateData[AS_DYING]);
		} break;
		case AS_DEAD:
		case AS_INIT:
		default:
		{
			/* initialising, dead sprites (or bad states) do not get rendered */
			return;
		}break;
	}

	if (pTempState->state == 0)
	{
		DebugPrint(("No data for this state %x\n", currentState));
		return;
	}

	//this cuts the appropriate frame out of the sprite sheet
	SDL_Rect spriteRect;
	spriteRect.x = pTempState->sheetStartsAt.x
					+ (pTempState->currentAnimationStep * pTempState->spriteWidth)
					+ pTempState->currentAnimationStep;
	spriteRect.y = pTempState->sheetStartsAt.y;
	spriteRect.w = pTempState->spriteWidth;
	spriteRect.h = pTempState->spriteHeight;

#ifdef DEBUG_COLLISIONS
	std::vector<CollisionRect_t>::iterator pos;

	for (pos = pTempState->collisionRects.begin(); pos != pTempState->collisionRects.end(); ++pos)
	{
		SDL_Rect foo;
		foo.x = pos->x;
		foo.y = pos->y;
		foo.h = pos->h;
		foo.w = pos->w;
		SDL_FillRect(pScreen, &foo, SDL_MapRGB( pScreen->format, 255, 255, 255 ));
	}
#endif
	if (pTempState->spriteSheet == NULL)
	{
		DebugPrint(("!!!!WTF NULL SPRITESHEET!!!!\n"));
	}

	DrawToSurface(xpos,
				  ypos,
				  pTempState->spriteSheet,
				  pScreen,
				  &spriteRect);
}

uint8_t Sprite::getNextAnimationStep(const AnimationStateData_t* pStateData)
{
	if (pStateData->currentAnimationStep == (pStateData->numberOfAnimationSteps - 1))
	{
		return 0;
	}
	else
	{
		return (pStateData->currentAnimationStep+1);
	}
}

void Sprite::update()
{
	AnimationStateData_t* pCurrentState = &animationStateData[currentState];

	/* this may be the last frame in a single pass state. if so , setup the next state here*/
	if (pCurrentState->currentAnimationStep == (pCurrentState->numberOfAnimationSteps-1))
	{
		pendingState = pCurrentState->nextState;
	}

	/* change state if we are not in the one we
	 * should be in
	 */
	if (pendingState != currentState)
	{
		currentState = pendingState;

		if (currentState == AS_DEAD) return;

		pCurrentState = &animationStateData[currentState];
		pCurrentState->currentAnimationStep = 0;
		pCurrentState->lastAnimTickCount = SDL_GetTicks();
	}

	/* special case - dead state
	 * we do nothing if the sprite is meant to be dead*/
	if (currentState == AS_DEAD)
	{
		return;
	}

	/* figure out which animation step we are in */
	uint32_t now = SDL_GetTicks();
	uint32_t delta = now - pCurrentState->lastAnimTickCount;
	if (delta > pCurrentState->ticksPerStep)
	{
		pCurrentState->currentAnimationStep = getNextAnimationStep(pCurrentState);
		pCurrentState->lastAnimTickCount = now;
	}
}

void Sprite::loadSpriteData(ResourceBundle * resource)
{
	/*read first line, should be number of states
	 * then loop through file parsing each line until
	 * we read all the data we expected
	 * you should probably add some error handling code at some point*/
	AnimationStateData_t* pData = NULL;
	uint32_t numberOfStates;
	uint32_t R=0;
	uint32_t G=0;
	uint32_t B=0;
	uint32_t numberOfCollisionRects;
	AnimationState_t state;

	numberOfStates = GET_RESOURCE(int32_t, *resource, "numberofstates", 0);

	//memset(animationStateData, 0, (sizeof(AnimationStateData_t) * AS_STATES_SIZE));
	ResourceBundle * currentState;
	for (uint32_t i = 0; i<numberOfStates; i++)
	{
		currentState = GET_RESOURCE(ResourceBundle*, *resource, "statefiles", i);
		state = GET_RESOURCE(AnimationState_t, *currentState, "state", 0);

		pData = &(animationStateData[state]);
		pData->state = state;

		R = GET_RESOURCE(int32_t, *currentState, "colorkey", 0); //((int*)((*currentState)["colorkey"]))[0];
		G = GET_RESOURCE(int32_t, *currentState, "colorkey", 1); //((int*)((*currentState)["colorkey"]))[1];
		B = GET_RESOURCE(int32_t, *currentState, "colorkey", 2); //((int*)((*currentState)["colorkey"]))[2];

		pData->nextState = GET_RESOURCE(AnimationState_t, *currentState, "nextstate", 0);
		pData->numberOfAnimationSteps = GET_RESOURCE(int32_t, *currentState, "numberofanimationsteps", 0);
		pData->ticksPerStep = GET_RESOURCE(int32_t, *currentState, "ticksperstep", 0);

		pData->sheetStartsAt.x = GET_RESOURCE(int32_t, *currentState, "sheetstartsat", 0);
		pData->sheetStartsAt.y = GET_RESOURCE(int32_t, *currentState, "sheetstartsat", 1);

		pData->spriteWidth = GET_RESOURCE(int32_t, *currentState, "spritesize", 0);
		pData->spriteHeight = GET_RESOURCE(int32_t, *currentState, "spritesize", 1);

		numberOfCollisionRects = GET_RESOURCE(int32_t, *currentState, "numberofrects", 0);

		for (uint32_t j = 0; j<numberOfCollisionRects; ++j)
		{
			CollisionRect_t rect;

			rect.x = GET_RESOURCE(int32_t*, *currentState, "rect", j)[0];
			rect.y = GET_RESOURCE(int32_t*, *currentState, "rect", j)[1];
			rect.w = GET_RESOURCE(int32_t*, *currentState, "rect", j)[2];
			rect.h = GET_RESOURCE(int32_t*, *currentState, "rect", j)[3];
			DebugPrint(("loading rect (%u,%u,%u,%u)\n", rect.x, rect.y,rect.w, rect.h ));
			pData->collisionRects.push_back(rect);
		}

		// this doesn't work with GET_RESOURCE, I guess because filename doesn't return an array maybe?
		pData->spriteSheet = (SDL_Surface*)((*currentState)["filename"]);
		uint32_t colorkey = SDL_MapRGB( pData->spriteSheet->format, R, G, B );
		SDL_SetColorKey( pData->spriteSheet, SDL_SRCCOLORKEY, colorkey );

		if (pData->collisionRects.begin() == pData->collisionRects.end())
		{
			cout << (("Loaded sprite without a collision box!\n")) << endl;
		}

		//pData->debugPrint();
	}
}

void Sprite::setLocation(int32_t xpos, int32_t ypos)
{
	int32_t xdelta = xpos - this->xpos;
	int32_t ydelta = ypos - this->ypos;

	/* update the collision rects now that the position has been updated */
	std::vector<CollisionRect_t>::iterator pos;
	uint32_t state = AS_IDLE;
	AnimationStateData_t* pAnimState;

	/* iterate through each sprite state, and then through each collision rect list,
	 * updating the collision rect postions if we've changed position.
	 * don't waste time doing any of this if the change is zero */
	if ((ydelta != 0 ) || (xdelta != 0))
	{
		while(state < AS_STATES_SIZE)
		{
			pAnimState = &(animationStateData[state]);

			for(pos=pAnimState->collisionRects.begin(); pos!=pAnimState->collisionRects.end(); ++pos)
			{
				if (ydelta != 0)
				{
					pos->y += ydelta;
				}

				if (xdelta != 0)
				{
					pos->x += xdelta;
				}
			}
			state<<=1;
		}
	}
	/* and finally set the position for the sprite object itself*/
	this->xpos = xpos;
	this->ypos = ypos;
}

/* returns the collision boxes of the current animation state*/
std::vector<CollisionRect_t> Sprite::getCollisionRects()
{
	std::vector<CollisionRect_t> CR;

	CR = animationStateData[currentState].collisionRects;

	return CR;
}

AnimationState_t Sprite::getPendingAnimationState()
{
	return pendingState;
}

AnimationState_t Sprite::getAnimationState()
{
	return currentState;
}

bool Sprite::isCollidingWith(std::vector<CollisionRect_t> other)
{
	if ((currentState == AS_INIT)
		|| (currentState == AS_DEAD)
		|| (currentState == AS_DYING))
	{
		return false;
	}

	std::vector<CollisionRect_t>::iterator myRects;
	std::vector<CollisionRect_t>::iterator otherRects;
	AnimationStateData_t* pData = &(animationStateData[currentState]);

	for (myRects = pData->collisionRects.begin(); myRects != pData->collisionRects.end(); ++myRects)
	{
		int32_t top1,bottom1,left1,right1;

		top1    = myRects->y;
		bottom1 = top1+myRects->h;
		left1   = myRects->x;
		right1  = left1+myRects->w;

		for (otherRects = other.begin(); otherRects != other.end(); ++otherRects)
		{
			int32_t top2,bottom2,left2,right2;
			top2    = otherRects->y;
			bottom2 = top2+otherRects->h;
			left2   = otherRects->x;
			right2  = left2+otherRects->w;

			if ((top1 > bottom2) || (left1>right2) || (bottom1<top2) || (right1<left2))
			{
				//not a collision
				continue;
			}
			else
			{
				//DebugPrint(("collision get! (%u,%u,%u,%u) with (%u,%u,%u,%u)!\n", left1, top1, right1, bottom1, left2, top2, right2, bottom2));
				return true;
			}
		}
	}

	return false;
}
