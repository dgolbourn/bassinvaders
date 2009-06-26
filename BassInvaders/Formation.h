/*
 * Formation.h
 *
 *  Created on: May 24, 2009
 *      Author: Darren Golbourn
 *
 *      Description:	This abstract class is just a stub for enemy formations.
 *      				A formation is described as a bunch of Entity entities
 *      				and a little bit of game logic to make them "fun".
 *
 */

#ifndef FORMATION_H_
#define FORMATION_H_

#include "Entity.h"
#include "BeatDetector.h"
#include "spline.h"
#include "path.h"
#include <list>

class Formation {
public:
	virtual ~Formation() {};
};

/*
 * A randomHorde is just the random waves of enemies formed on the beat
 * like we have used in testing.
 */
class randomHorde : public Formation {
public:
	~randomHorde();
	randomHorde( Path path);
};

/*
 * A monsterLine is a row of enemies that follow each other
 */
class monsterLine: public Formation {
public:
	~monsterLine();
	monsterLine(Path path, uint8_t num);
};

#endif /* FORMATION_H_ */
