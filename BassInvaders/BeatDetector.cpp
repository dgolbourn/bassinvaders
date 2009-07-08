/*
 * BeatDetector.cpp
 *
 *  Created on: May 2, 2009
 *      Author: Darren Golbourn
 */

#include "BeatDetector.h"
void BeatDetector::process( void *udata, uint8_t *stream, int len)
{
	((BeatDetector*)udata)->detect(stream);
}

BeatBroadcaster* BeatDetector::broadcaster(uint32_t cooldown){
	return new BeatBroadcaster(cooldown, this);
}

BeatBroadcaster::BeatBroadcaster(uint32_t cooldown, BeatDetector* detector)
{
	iterator = detector->iterator(cooldown);
}

void BeatBroadcaster::isBeat(){
	if (iterator->isBeat())
	{
		std::vector<BeatListener*>::iterator i;

		for(i=listeners.begin(); i != listeners.end(); ++i) {
			(*i)->onBeat();
		}
	}
}

BeatIterator::BeatIterator(uint32_t coolDown, BeatDetector* b) {
	this-> coolDown = coolDown;
	lastTickCount = SDL_GetTicks();
	this->b=b;
}

beat_t BeatIterator::isBeat(){
	uint32_t now = SDL_GetTicks();
	uint32_t delta = now - lastTickCount;

	beat_t B = b->isBeat();

	if ((delta > coolDown) && B)
	{
		lastTickCount = now;
		return B;
	}

	return 0;
}

BeatIterator* BeatDetector::iterator(uint32_t cooldown)
{
	return new BeatIterator(cooldown, this);
}

BeatDetector::BeatDetector(uint32_t history_len, double sensitivity, uint32_t samples){
	this->samples = samples;
	this->history_len = history_len;
	this->sensitivity = sensitivity;
	H = new history<double>(history_len);
	beat = false;
}

BeatDetector::~BeatDetector() {
	delete H;
}

void BeatDetector::detect(uint8_t* stream){
	/* calculate local average energy */
	double average_energy = 0;

	for(uint32_t i = 0; i<samples; i++)
	{
		double L = (double)LEFT((int16_t *) stream, i);
		double R = (double)RIGHT((int16_t *) stream, i);
		average_energy += L*L + R*R;
	}

	/* if local average is greater than S x global average then it's a beat! */
	beat = ((average_energy * history_len) > (sensitivity * H->total)) * average_energy;

	/* update history buffer */
	H->record(average_energy);
}

beat_t BeatDetector::isBeat(){
	return beat;
}
