/**
 * @file DSP_handler.h
 *
 * @brief This file contains the definition of the functions that allow to control the DSP
 *
 *	@date Oct 6, 2021
 * @author  David González León, Jade Gröli
 */

#ifndef DSP_HANDLER_H_
#define DSP_HANDLER_H_

#include "arm_math.h"
#include "main.h"
#include <stdbool.h>

/**
 * @brief Does an rfft of the given samples and outputs the 36 highest values
 *
 * @param samples the samples to analyse
 */
void rfft(uint16_t *samples);

/**
 * @brief Inits the dsp
 *
 * @return true If all went well
 * @return false if there was an error
 */
bool DSP_init();

#endif /* DSP_HANDLER_H_ */
