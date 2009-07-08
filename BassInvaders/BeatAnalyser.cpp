/*
 * BeatAnalyser.cpp
 *
 *  Created on: Jul 8, 2009
 *      Author: Darren Golbourn
 */

#include "BeatAnalyser.h"

BeatAnalyser::BeatAnalyser(uint32_t coolDown, BeatDetector* d): BeatIterator(coolDown, d) {

}

BeatAnalyser::~BeatAnalyser() {
}
