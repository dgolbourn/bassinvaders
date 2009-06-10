/*
 * monster.h
 *
 *  Created on: 8/5/09
 *      Author: Darren Golbourn
 */

#ifndef MONSTER_H_
#define MONSTER_H_

#include "Entity.h"
#include <vector>
#include "InputManager.h"
#include "ResourceBundle.h"
#include "toolkit.h"
#include <fstream>
#include "WindowManager.h"
#include <iostream>
#include "spline.h"

#define MONSTER_X_SPEED -10
#define MONSTER_Y_SPEED 0

#define MAIN_SPRITE 0

class monster: public Entity {
public:
	monster(int32_t);
	~monster();
	void render(SDL_Surface *pScreen);
	void doCollision(Entity* pOther);
	std::vector<Sprite*> getActiveSpriteList();
	void update();

	double s; // parametrizes the path
	parametricFunctor *path; // the path the monster is going to take

private:
	void updatePosition();
	void updateStates();
	void loadMonsterData();
	void reactToCollision(Entity* pOther);
};

#endif /* HERO_H_ */
