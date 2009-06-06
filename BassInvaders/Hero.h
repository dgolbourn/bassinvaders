/*
 * Hero.h
 *
 *  Created on: 19-Apr-2009
 *      Author: spijderman
 */

#ifndef HERO_H_
#define HERO_H_

#include "Entity.h"
#include <vector>
#include "Bullet.h"
#include "InputManager.h"
#include "ResourceBundle.h"
#include "EntityManager.h"

#define BODYSPRITE 0 //main body of hero is the first sprite in the vector

#define HERO_X_SPEED 8
#define HERO_Y_SPEED 8

#define DEAD_HEALTH 0
#define DAMAGED_HEALTH 33
/* JG TODO:
 * - Vector of bullets
 */

class Hero: public Entity {
public:
	uint32_t score;
	Hero(ResourceBundle* filename, class EntityManager* pRM);
	~Hero();
	void render(SDL_Surface *pScreen);
	bool canBeRemoved();
	void setActions(ACTIONMASK actions);
	void doCollision(Entity* pOther);
	std::vector<Sprite*> getActiveSpriteList();
	void update();

private:
	void reactToCollision(Entity* pOther);
	void updateStates();
	void loadHeroData(ResourceBundle *fp);
	void doActions();
	class EntityManager* pRM; // the hero needs a handle to the RM so that it can add bullets to it
	uint32_t fireRate; //number of ticks which must pass before another bullet can be fired
	uint32_t lastFireTicks; //tickcount when the last bullet was fired
	bool canShoot; //hero is firing a shot
};

#endif /* HERO_H_ */
