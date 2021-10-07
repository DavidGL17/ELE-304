/*
 * DSP_handler.h
 *
 *  Created on: Oct 6, 2021
 *      Author: xxpow
 */

#ifndef DSP_HANDLER_H_
#define DSP_HANDLER_H_

#include "arm_math.h"
#include "main.h"
#include <stdbool.h>

void rfft(uint16_t* samples);
bool DSP_init();

#endif /* DSP_HANDLER_H_ */
