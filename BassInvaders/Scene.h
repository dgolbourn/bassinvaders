/*
 * Scene.h
 *
 *  Created on: May 24, 2009
 *      Author: Darren Golbourn
 *
 *      Description:	A scene is a collection of formations, on screen text, events
 *      				and game logic that together make a themed set piece.
 *      				A level is made up of scenes, one scene at a time.
 */

#ifndef SCENE_H_
#define SCENE_H_
#include "Formation.h"
#include <deque>

/*
 * is the scene inactive, currently ongoing or has it finished?
 */
enum sceneState_t
{
	IDLE,	// waiting
	ACTIVE,	// playing
	FINISHED// done
};

class Scene {
	std::deque <Formation*> formations;
public:
	virtual ~Scene() {};
};

#endif /* SCENE_H_ */
