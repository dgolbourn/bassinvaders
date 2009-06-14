/*
 * Sprite.h
 *
 *  Created on: 19-Apr-2009
 *      Author: spijderman
 */

#ifndef SPRITE_H_
#define SPRITE_H_

#include <vector>
#include <stdio.h>
#include <string>
#include <SDL/SDL.h>
#include "ResourceBundle.h"

using namespace std;

//typedef SDL_Rect CollisionRect_t;

struct CollisionRect_t {
	int16_t x;
	int16_t y;
	int16_t w;
	int16_t h;

	CollisionRect_t()
	{
		x = 0; y = 0; w = 0; h = 0;
	}
};

enum AnimationState_t
{
	AS_INIT    = 0x0, // the only time it is valid to set a sprite to INIT is in the ctor, and the pending state MUST be set to something else...
	AS_IDLE    = 0x00000001,
	AS_DAMAGED = 0x00000002,
	AS_DYING   = 0x00000004,
	AS_DEAD    = 0x00000008,//DEAD is always the last state and has no data associated with it
	AS_STATES_SIZE = AS_DEAD
};

struct AnimationStateData_t
{
	AnimationState_t state; //if this is set to 0, it can be assumed that no data exists for this state
	AnimationState_t nextState; //some states are transitional and only run once then move to the next state
	uint32_t currentAnimationStep;
	uint32_t numberOfAnimationSteps;
	SDL_Rect sheetStartsAt; // coordinates of first item
	int32_t spriteWidth;
	int32_t spriteHeight;
	uint32_t ticksPerStep;
	uint32_t lastAnimTickCount;
	SDL_Surface* spriteSheet;
	std::vector<CollisionRect_t> collisionRects;

	AnimationStateData_t();
	void debugPrint();
};

class Sprite {
public:
	Sprite(ResourceBundle* resources);
	~Sprite();
	void changeState(AnimationState_t state);
	void renderSprite(SDL_Surface* pScreen);
	void setLocation(int32_t xpos, int32_t ypos);
	bool isCollidingWith(std::vector<CollisionRect_t> other);
	std::vector<CollisionRect_t> getCollisionRects();
	AnimationState_t getPendingAnimationState();
	AnimationState_t getAnimationState();
	void destroy(); //manually clean up sprite memory

private:
	void loadSpriteData(ResourceBundle * fp);
	void update();
	uint8_t getNextAnimationStep(const AnimationStateData_t *pStateData);

private:
	AnimationState_t currentState;
	AnimationState_t pendingState;
	uint32_t statesSupported; // bitmask of which AnimationStates are implemented by this sprite
	AnimationStateData_t animationStateData[AS_STATES_SIZE]; // space-ineffecient but fast-indexed array of animation states
	int32_t xpos;
	int32_t ypos;
};

#endif /* SPRITE_H_ */
