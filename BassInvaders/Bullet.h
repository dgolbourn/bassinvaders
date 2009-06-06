/*
 * Bullet.h
 *
 *  Created on: 19-Apr-2009
 *      Author: spijderman
 */

#ifndef BULLET_H_
#define BULLET_H_

#include "Entity.h"
#include "ResourceBundle.h"

#define BULLET_X_SPEED 10
#define BULLET_Y_SPEED 0

#define BULLET_RED_SPRITE_INDEX 0
/*JG TODO: for now, let's just have one type of bullet.
 * However, we will eventually want to have multiple kinds.
 * We will do this with the bullet having N sprites, one for each ammo type.
 * The type is set at instantiation time.
 */

class Bullet: public Entity {
public:
	Bullet(int32_t xpos, int32_t ypos);
	~Bullet();
	void render(SDL_Surface *pScreen);
	std::vector<Sprite*> getActiveSpriteList();
	void doCollision(Entity* pOther);
	void update();

private:
	void loadBulletData();
	void updateStates();
	void reactToCollision(Entity* pOther);
};

#endif /* BULLET_H_ */
