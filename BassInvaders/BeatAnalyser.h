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
friend BeatAnalyser* BeatManager::analyser(uint32_t coolDown, BeatDetector* d);
	BeatAnalyser(uint32_t coolDown, detector_t* d);
	detector_t *detector; // address of detector_t in beat manager list.
public:
	virtual ~BeatAnalyser();
	beat_info_t beat_info();
};

#endif /* BEATANALYSER_H_ */
