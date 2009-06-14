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

#define MONSTER_X_SPEED -4
#define MONSTER_Y_SPEED 0

#define MAIN_SPRITE 0

class monster: public Entity {
	bool localPath;
public:
	monster(int32_t);
	monster(int32_t x0, int32_t y0, double monsterTime, Path path);
	~monster();
	void render(SDL_Surface *pScreen);
	void doCollision(Entity* pOther);
	std::vector<Sprite*> getActiveSpriteList();
	void update();

	/*
	 * variables and methods which manage the movement of the monster (used by overloaded updatePosition)
	 */
	double s; 		// "monster time" parametrizes the path
	double x0;		// a place to store an offset for x
	double y0;  	// a place to store an offset for y
	Path path; 		// the path the monster is going to take

private:
	void updatePosition();
	void updateStates();
	void loadMonsterData();
	void reactToCollision(Entity* pOther);
};

#endif /* HERO_H_ */
