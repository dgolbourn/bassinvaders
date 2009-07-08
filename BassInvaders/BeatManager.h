/*
 * BeatManager.h
 *
 *  Created on: Jun 27, 2009
 *      Author: Darren Golbourn
 *
 *      Description: 	The beat manager provides a centralised interface for the beat detectors
 *      				and frequency band splitting.
 */

#ifndef BEATMANAGER_H_
#define BEATMANAGER_H_
#include <list>
#include "BandPassFilterFFT.h"

class BeatDetector;

/*
 * This structure contains all the meta-data about the detector
 * that is used by the manager to know whether to band-limit the
 * stream etc.
 */
struct detector_t
{
	BeatDetector *detector; // address of detector

	/*
	 * if the stream is frequency band limited...
	 */
	bool isBandLimited;
	double f_lo; // low freq.
	double f_hi; // high freq.

	/*
	 * data characterisation
	 */
	bool isFindMaxFreq;
	double max_freq; // highest amplitude frequency
};

/*
 * The management interface
 */
class BeatManager {
	std::list<detector_t> detectors; // vector of beat detectors

	/*
	 * frequency band stuff
	 */
	BandPassFilterFFT *fft; // fourier tranform
	uint8_t *stream; // stream cache

	/*
	 * audio config parameters
	 */
	uint32_t samplesPerChunk;
	uint32_t sampleFreq;
	uint32_t historyLen;

	/*
	 * ingest audio stream into fft, and feed different band-limited streams to the beat detectors.
	 */
	void partition(uint8_t *stream);

public:
	/*
	 * constructors and destructors
	 */
	BeatManager(uint32_t, uint32_t);
	virtual ~BeatManager();

	/*
	 * create a BeatDetector
	 */
	BeatDetector* detector(double senstivity, double f_lo, double f_hi);
	BeatDetector* detector(double senstivity);

	/*
	 * unregister detector (because it is going to be deleted)
	 */
	void unregister(BeatDetector*);

	/*
	 * static method to use with SDL music callback
	 */
	void static process( void *udata, uint8_t *stream);
};
#endif /* BEATMANAGER_H_ */
