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
	static int32_t speed;
	void update();

private:
	void updateStates();
	void loadMonsterData();
	void reactToCollision(Entity* pOther);
};

#endif /* HERO_H_ */
