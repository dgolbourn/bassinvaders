/*
 * audefnmacros.h
 *
 *  Created on: May 2, 2009
 *      Author: Darren Golbourn
 *
 *   Description: These macros are used to index the arrays used by audio data.
 *
 *   Audio data is in the form of an int16_t array where the ints are arranged as
 *   RLRLRLRL... where L is left speaker, R is right speaker.  The array is often
 *   passed around as a uint8_t*, char* or void* and needs casting back to int16_t
 *   in order to use the LEFT/RIGHT macros below.
 *
 *   Frequency data is in an array of doubles in the form RIRIRIRI where R is real,
 *   I is imaginary.  The RI pairs are ordered by frequency
 *   from 0 to highest positive then from highest negative to lowest negative:
 *	 		0,
 *			lowest positive,
 *			...
 *			highest positive/negative,
 *			...
 *			lowest negative.
 *
 *	where highest means highest by absolute value and lowest means lowest
 *	by absolute value.
 */

#ifndef AUDIO_STREAM_FMT_MACROS
#define AUDIO_STREAM_FMT_MACROS
// Macros for real/imaginary right/left parts of data stream
	#define REAL(z,i) ((z)[2*(i)])
	#define IMAG(z,i) ((z)[2*(i)+1])
	#define RIGHT(z,i) ((z)[2*(i)])
	#define LEFT(z,i) ((z)[2*(i)+1])

// Macros for positive/negative frequencies in frequency domain data
	#define POSITIVE(i,size) (i)
	#define NEGATIVE(i,size) ((i)>0?((size)-(i)):i)
#endif /* AUDEFNMACROS_H_ */
