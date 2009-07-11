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
class BeatAnalyser;

/*
 * This data type is used to hold any metrics that we want to return from the beat analyser.
 * The beat analyser and the beat manager will probably have to work together to extract
 * computationally expensive data in an efficient way.
 */
struct beat_info_t{
	double max_freq; // highest amplitude frequency
	/*
	 * can put more things in here when we think of them.
	 */
};

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
	//uint8_t *stream; // stream cache

	/*
	 * data characterisation
	 */
	bool isFindMaxFreq;
	beat_info_t data; // highest amplitude frequency
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
	double *freq; // stream cache

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
	 * create a BeatAnalyser
	 */
	BeatAnalyser* analyser(uint32_t coolDown, BeatDetector* d);

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
