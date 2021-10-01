/*
 * shared_memory.h
 *
 *	This file contains the definition of the general variables available on the shared memory
 *
 *  Created on: 30 sept. 2021
 *      Author: xxpow
 */

#ifndef SHARED_MEMORY_H_
#define SHARED_MEMORY_H_

#include "main.h"
#include "stdbool.h"


extern MDMA_HandleTypeDef* mdma_handle;
extern uint16_t* sample_dest_ptr;
#ifdef CORE_CM7
#define SAMPLE_BLOCK_SIZE 1024
extern uint16_t sample_dest[SAMPLE_BLOCK_SIZE];
bool MDMA_init();
#else

#endif


#endif /* SHARED_MEMORY_H_ */
