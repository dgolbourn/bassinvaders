/*
 * BandPassFilterFFT.cpp
 *
 *  Created on: May 2, 2009
 *      Author: Darren Golbourn
 */

#include "BandPassFilterFFT.h"
#include <iostream>
#include "gsl/gsl_blas.h"
#include "gsl/gsl_vector.h"
/*
 * Initialize the FFT Band-pass filter
 */
BandPassFilterFFT::BandPassFilterFFT(uint32_t sample_rate, uint32_t chunk_size){
	length = chunk_size;
	samples = length/4;
	freqs = samples/2;

	/* allocate fast Fourier transform stuff */
	workspace = gsl_fft_complex_workspace_alloc (samples);
	wavetable = gsl_fft_complex_wavetable_alloc (samples);

	/* frequency array */
	double delta = 1./(sample_rate-1);
	f = new double[samples];
	f[0] = 0;
	for (int i=1; i<=(int)freqs; i++){
		f[samples-i]=-i/(samples*delta);
		f[i]=i/(samples*delta);
	}

	faccel = gsl_interp_accel_alloc () ;
	fcache = new double[2*samples];
	band_data = new double[2*samples];
}

/**
 * Free the allocated data
 */
BandPassFilterFFT::~BandPassFilterFFT() {
	delete[] f;
	delete[] fcache;
	delete[] band_data;

	gsl_interp_accel_free(faccel);
	gsl_fft_complex_wavetable_free (wavetable);
	gsl_fft_complex_workspace_free (workspace);
}

/**
 * find the nearest index i to frequency freq in fft such that
 * f[i] < freq <f[i+1]
 */
int BandPassFilterFFT::ffind(double freq){
	return gsl_interp_accel_find (faccel, f, freqs, freq );
}

/*
 * transform stream in to frequency domain
 */
fft_t BandPassFilterFFT::fft_alloc(uint8_t* stream)
{
	for (uint32_t i =0; i<samples; i++){
		REAL(fcache,i)=(double)RIGHT((int16_t*)stream,i);
		IMAG(fcache,i)=(double)LEFT((int16_t*)stream,i);
	}

	/* Fourier transform */
	gsl_fft_complex_forward(fcache, 1, samples, wavetable, workspace);

	return fcache;
}

/*
 * transform frequency domain into stream
 */
void BandPassFilterFFT::fft_inverse(double* band_data, uint8_t *stream) {
	gsl_fft_complex_inverse(band_data, 1, samples, wavetable, workspace); // Then reverse Fourier transform back to the time domain...

	for (uint32_t i =0; i<samples; i++){ // ...and copy the data back in to the stream.
		RIGHT((int16_t*)stream,i)=(int16_t)REAL(band_data,i);
		LEFT((int16_t*)stream,i)=(int16_t)IMAG(band_data,i);
	}
}

/*
 * fourier transform a stream and place the frequency data into the cache
 */
void BandPassFilterFFT::ingest(uint8_t *stream)
{
	fft_alloc(stream);
}

/*
 * copy a chunk containing only frequencies between flo and fhi into band data and then
 * inverse fourier transform it back into the time-domain stream.
 */
void BandPassFilterFFT::band_pass(uint8_t *stream, double flo, double fhi)
{
	band_window(band_data, ffind(fhi), ffind(flo));
	fft_inverse(band_data, stream);
}

void BandPassFilterFFT::band_pass_with_copy(uint8_t *stream, double *freq, double flo, double fhi)
{
	band_window(band_data, ffind(fhi), ffind(flo));

	for (uint32_t i =0; i<samples; i++){ // copy data to freq.
		RIGHT(freq,i)=REAL(band_data,i);
		LEFT(freq,i)=IMAG(band_data,i);
	}

	fft_inverse(band_data, stream);
}

/*
 * using the frequency fft data, band pass between frequencies in f[bandlo] <= f <= f[bandhi]
 */
void BandPassFilterFFT::band_window(double *band_data, uint32_t bandhi, uint32_t bandlo)
{
	/*
	 * Split the data in to frequency bands.
	 */
	for(uint32_t i=0; i<=freqs; i++) // Go over each frequency...
	{
		if ((i < bandhi) && (i>=bandlo)) // ...and copy over the frequencies in the band window...
		{
			REAL(band_data,POSITIVE(i,samples)) = REAL(fcache,POSITIVE(i,samples));
			IMAG(band_data,POSITIVE(i,samples)) = IMAG(fcache,POSITIVE(i,samples));
			REAL(band_data,NEGATIVE(i,samples)) = REAL(fcache,NEGATIVE(i,samples));
			IMAG(band_data,NEGATIVE(i,samples)) = IMAG(fcache,NEGATIVE(i,samples));
		}
		else // ...and set the other frequencies to zero.
		{
			REAL(band_data,POSITIVE(i,samples)) = 0.;
			IMAG(band_data,POSITIVE(i,samples)) = 0.;
			REAL(band_data,NEGATIVE(i,samples)) = 0.;
			IMAG(band_data,NEGATIVE(i,samples)) = 0.;
		}
	}
}

/*
 * returns the highest amplitude frequency in the band_limited stream.
 */
double BandPassFilterFFT::util_max_freq_band_limited()
{
	/*
	 * find index of biggest amplitude value...
	 */
	gsl_vector_const_view V = gsl_vector_const_view_array(band_data+1, 2*samples-1);
	CBLAS_INDEX_t ind = gsl_blas_idamax(&V.vector);

	/*
	 * ...then work out what frequency that means and return it.
	 */
	return f[(ind+1)/2];
}

/*
 * returns the highest amplitude frequency in the stream.
 */
double BandPassFilterFFT::util_max_freq(double* freq)
{
	if (freq == NULL) freq = fcache;

	/*
	 * find index of biggest amplitude value...
	 */
	gsl_vector_const_view V = gsl_vector_const_view_array(fcache+1, 2*samples-1);
	CBLAS_INDEX_t ind = gsl_blas_idamax(&V.vector);

	/*
	 * ...then work out what frequency that means and return it.
	 */
	return f[(ind+1)/2];
}

/**
 * Utility function to write data to file.
 * save the graphs y1 = re[z(x)] and y2 = im[z(x)]
 */
void BandPassFilterFFT::util_write_freq(char* file)
{
	FILE *fp = fopen(file,"w");

	/* write the data to file. Column 1 = x, Column 2 = re(z), Column 3 = im(z).*/
	for (uint32_t i =0; i<samples; i++){
		fprintf(fp,"%f %f %f\n", f[i], REAL(band_data,i), IMAG(band_data,i));
	    fflush(fp);
	}

    fclose(fp);
}

/*
 * graphical equaliser using the callback function eq(freq, user_data) to shape the frequencies in stream.
 */
void BandPassFilterFFT::util_eq(uint8_t *stream, double(*eq)(double, void*), void *args)
{
	/*
	 * EQ the data
	 */
	for(uint32_t i=0; i<=freqs; i++) // Go over each frequency.
	{
		double Eq = eq(f[i], args);
		REAL(band_data,POSITIVE(i,samples)) = Eq * REAL(fcache,POSITIVE(i,samples));
		IMAG(band_data,POSITIVE(i,samples)) = Eq * IMAG(fcache,POSITIVE(i,samples));
		REAL(band_data,NEGATIVE(i,samples)) = Eq * REAL(fcache,NEGATIVE(i,samples));
		IMAG(band_data,NEGATIVE(i,samples)) = Eq * IMAG(fcache,NEGATIVE(i,samples));
	}

	fft_inverse(band_data, stream);
}
