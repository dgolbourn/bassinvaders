/*
 * BeatAnalyser.cpp
 *
 *  Created on: Jul 8, 2009
 *      Author: Darren Golbourn
 */

#include "BeatAnalyser.h"

BeatAnalyser::BeatAnalyser(uint32_t coolDown, detector_t* d): BeatIterator(coolDown, d->detector) {
	detector = d;
	detector->isFindMaxFreq = true;
}

BeatAnalyser::~BeatAnalyser() {
}

beat_info_t BeatAnalyser::beat_info()
{
	return detector->data;
}
