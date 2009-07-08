/*
 * BeatAnalyser.h
 *
 *  Created on: Jul 8, 2009
 *      Author: Darren Golbourn
 */

#ifndef BEATANALYSER_H_
#define BEATANALYSER_H_

#include "BeatDetector.h"
#include "BeatManager.h"

class BeatAnalyser: public BeatIterator {
	BeatAnalyser(uint32_t coolDown, BeatDetector* B);
public:
	virtual ~BeatAnalyser();
};

#endif /* BEATANALYSER_H_ */
