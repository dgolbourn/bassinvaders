/*
 * BeatManager.cpp
 *
 *  Created on: Jun 27, 2009
 *      Author: Darren Golbourn
 */

#include "BeatManager.h"
#include "BeatDetector.h"
#include "BeatAnalyser.h"
#include <iostream>

BeatManager::BeatManager(uint32_t samplesPerChunk, uint32_t sampleFreq)
{
	this->samplesPerChunk = samplesPerChunk;
	this->sampleFreq = sampleFreq;
	uint32_t sampleLen = samplesPerChunk *4;
	fft = new BandPassFilterFFT (sampleFreq, sampleLen);
	this->historyLen = (int) (1.0 / ((double)(samplesPerChunk)/(double)(sampleFreq)));
	stream = new uint8_t[sampleLen];
	freq = new double[samplesPerChunk*2];

}

BeatManager::~BeatManager()
{
	delete []stream;
	delete []freq;
	delete fft;
}

void BeatManager::process( void *udata, uint8_t *stream)
{
	((BeatManager*)udata)->partition(stream);
}

BeatDetector* BeatManager::detector(double senstivity)
{
	detector_t D;// a new detector struct
	D.detector = new BeatDetector(historyLen, senstivity, samplesPerChunk); // add a new beat detector
	D.isBandLimited = false; // is it band limited?
	detectors.push_front(D);// push onto the container

	return D.detector;// return the detector address
}

BeatDetector* BeatManager::detector(double senstivity, double f_lo, double f_hi)
{
	detector_t D;	// a new detector struct
	D.detector = new BeatDetector(historyLen, senstivity, samplesPerChunk); // add a new beat detector
	D.isBandLimited = true; // the detector is band limited...
	D.f_lo = f_lo;	// lower freq
	D.f_hi = f_hi;	// higher freq
	detectors.push_front(D); // push onto the container

	return D.detector;	// return the detector address
}

/*
 * It is an arduous process to search the list to
 * remove a detector.  But that's not a problem,
 * this wont happen very much and the list will normally be very short
 * so the potential searching overhead is justified.
 */
void BeatManager::unregister(BeatDetector *D)
{
	std::list<detector_t>::iterator iter;
	/*
	 * search through the list...
	 */
	for (iter = detectors.begin(); iter != detectors.end(); iter++)
	{
		/*
		 * and once you find the detector, remove it from the list and exit the function.
		 */
		if ((*iter).detector == D)
		{
			detectors.erase(iter);
			return;
		}
	}
}

BeatAnalyser* BeatManager::analyser(uint32_t coolDown, BeatDetector* d)
{
	std::list<detector_t>::iterator iter;
	/*
	 * search through the list...
	 */
	for (iter = detectors.begin(); iter != detectors.end(); iter++)
	{
		/*
		 * and once you find the detector, create a new analyser.
		 */
		if ((*iter).detector == d)
		{
			return new BeatAnalyser(coolDown, &(*iter));
		}
	}

	// not a registered BeatDetector
	return NULL;
}


void BeatManager::partition(uint8_t *stream)
{
	/*
	 * Fourier transform the audio chunk
	 */
	fft->ingest(stream);

	std::list<detector_t>::iterator iter;
	for (iter = detectors.begin(); iter != detectors.end(); iter++)
	{
		/*
		 * if the detector is to be fed a band-limited stream, sort that out now.
		 */
		if ((*iter).isBandLimited)
		{
			fft->band_pass_with_copy(this->stream, freq, (*iter).f_lo, (*iter).f_hi);
			(*iter).detector->detect(this->stream);

			/*
			 * if the detector is to determine some characteristics of the stream, do that now.
			 */
			if ((*iter).isFindMaxFreq)
			{
				(*iter).data.max_freq = fft->util_max_freq(freq);
			}
		}
		else
		{
			/*
			 * if the stream required by the detector is just the raw stream
			 * then pass through and act on it here.
			 */
			(*iter).detector->detect(stream);

			if ((*iter).isFindMaxFreq)
			{
				(*iter).data.max_freq = fft->util_max_freq();
			}
		}
	}

}
