/*
 * DSP_handler.c
 *
 *  Created on: Oct 6, 2021
 *      Author: xxpow
 */

#include "DSP_handler.h"
#include "print_server.h"

arm_rfft_fast_instance_f32 rfft_pointer __attribute__ ((aligned(4))) __attribute__ ((section(".DTCMRAM")));
#define SAMPLE_SIZE 1024
#define SAMPLING_FREQUENCE 100000
#define NUMBER_OF_BIN_PRINTED 36
#define IFFT_FLAG 0

void rfft(uint16_t *samples) {
	float fSamples[SAMPLE_SIZE];
	for (uint32_t i = 0; i < SAMPLE_SIZE; ++i) {
		fSamples[i] = 1.0 * samples[i];
	}
	float outputBuffer[SAMPLE_SIZE];
	arm_rfft_fast_f32(&rfft_pointer, fSamples, outputBuffer, IFFT_FLAG);
	float finalOutputBuffer[SAMPLE_SIZE / 2];
	arm_cmplx_mag_f32(outputBuffer, finalOutputBuffer, SAMPLE_SIZE / 2);
	uint32_t pIndex;
	float maxVal;
	uint32_t val;
	for (int i = 0; i < NUMBER_OF_BIN_PRINTED; ++i) {
		arm_max_f32(finalOutputBuffer, (SAMPLE_SIZE / 2) - 1, &maxVal, &pIndex);
		val = maxVal / 0xFFFF;
		PRINTF("Max nr %d, value %d at BIN %d at frequence %F\n", i, val, pIndex, pIndex * (((float)SAMPLING_FREQUENCE) / SAMPLE_SIZE));
		finalOutputBuffer[pIndex] = 0;
	}

}

bool DSP_init() {
	if (arm_rfft_fast_init_f32(&rfft_pointer, SAMPLE_SIZE) != ARM_MATH_SUCCESS) {

		return false;
	}
	return true;
}
