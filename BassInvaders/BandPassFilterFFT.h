/*
 * BandPassFilterFFT.h
 *
 *  Created on: May 2, 2009
 *      Author: Darren Golbourn
 *
 * Description: This library is a band limiting filter which works by taking an audio chunk
 * 				and transforming it in to frequency data using an FFT.
 *
 * Usage:
 * 		Initialize the filter at the beginning:
 * 			BandPassFilterFFT filter(sample rate <Hz>, chunk size <bytes> );
 *
 * 		Feed the filter a chunk:
 * 			filter.ingest(stream);
 *
 * 		Copy a chunk containing only the frequencies f0 < f < f1 of the ingested chunk:
 * 			uint8_t newstream[chunk size <bytes>];
 * 			filter.band_pass(newstream, f0 <Hz>, f1 <Hz> );
 *
 * 		You only need to set up one filter per [sample rate, chunk size] pair.
 *
 * 		You only need to ingest each chunk once, then you can copy out any number of
 * 		new different band-limited versions of it with band_pass.  When you are finished
 *		with the chunk simply ingest another to replace it.
 */

#ifndef BANDPASSFILTERFFT_H_
#define BANDPASSFILTERFFT_H_

#include <inttypes.h>
#include "gsl/gsl_errno.h"
#include "gsl/gsl_fft_complex.h"
#include "gsl/gsl_interp.h"
#include "audefnmacros.h"

typedef double* fft_t;

class BandPassFilterFFT {
	/*
	 * frequency transform data parameters
	 */
	uint32_t length; 			// length of the sound chunk in bytes
	uint32_t bands; 			// number of bands to partition the stream in to
	uint32_t samples;			// number of 16bit-per-channel stereo samples
	uint32_t freqs; 			// number of positive frequencies

	/*
	 * cached data
	 */
	double *f; 					// array of frequencies
	gsl_interp_accel * faccel; 	// accelerator to help searching for indices of f.
	fft_t fcache;				// cache of frequency domain data.
	fft_t band_data;			// very temporary cache of band-limited frequency domain data.
	gsl_fft_complex_wavetable *wavetable; // wavetable and...
	gsl_fft_complex_workspace *workspace; // ...workspace are used by the fft algorithm.

	/*
	 * internal methods
	 */
	int ffind(double);			// search function for frequency data
	double* fft_alloc(uint8_t* stream); // do the fourier transform
	void fft_inverse(double* band_data, uint8_t *stream);	// do the reverse fourier transform
	void band_window(double*, uint32_t bandhi, uint32_t bandlo); // the band pass filter
public:
	/*
	 * constructor/destructor
	 */
	BandPassFilterFFT(uint32_t, uint32_t);
	virtual ~BandPassFilterFFT();

	/*
	 * "driver" methods
	 */
	void ingest(uint8_t *stream); // eat a stream and fourier-transform it.
	void band_pass(uint8_t *stream, double flo, double fhi); // copy band-limited version of data back to stream.

	/*
	 * utility methods
	 */
	void util_write_freq(char* file); // write frequency data to file.
	void util_eq(uint8_t *stream, double(*eq)(double, void*), void *args); // use the callback to shape frequencies in stream.
	double util_max_freq_band_limited(); // return freq of largest amplitude frequency component
	double util_max_freq(); // return freq of largest amplitude frequency component
};

#endif /* BANDPASSFILTERFFT_H_ */
