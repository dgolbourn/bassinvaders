/*
 * Hero.h
 *
 *  Created on: 19-Apr-2009
 *      Author: spijderman
 */

#ifndef HERO_H_
#define HERO_H_

#include "Renderable.h"
#include <vector>
#include "Bullet.h"
#include "InputManager.h"
#include "ResourceBundle.h"
#include "RenderableManager.h"

#define BODYSPRITE 0 //main body of hero is the first sprite in the vector

#define HERO_X_SPEED 5
#define HERO_Y_SPEED 5

#define DEAD_HEALTH 0
#define DAMAGED_HEALTH 33
/* JG TODO:
 * - Vector of bullets
 */

class Hero: public Renderable {
public:
	uint32_t score;
	Hero(ResourceBundle* filename, class RenderableManager* pRM);
	virtual ~Hero();

	virtual bool isOffScreen(uint32_t screenWidth, uint32_t screenHeight);
	virtual void render(SDL_Surface *pScreen);
	virtual bool canBeRemoved();
	void setActions(ACTIONMASK actions);
	virtual void doCollision(Renderable* pOther);
	virtual std::vector<Sprite> getActiveSpriteList();
	virtual void reactToCollision(Renderable* pOther);

protected:
	virtual void updateStates();

private:
	void loadHeroData(ResourceBundle *fp);
	void doActions();

private:
	class RenderableManager* pRM; // the hero needs a handle to the RM so that it can add bullets to it
	uint32_t fireRate; //number of ticks which must pass before another bullet can be fired
	uint32_t lastFireTicks; //tickcount when the last bullet was fired
	bool canShoot; //hero is firing a shot
};

#endif /* HERO_H_ */
