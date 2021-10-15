/**
 * @file shared_memory.h
 *
 *	@brief This file contains the definition of functions controlling the mdma
 *
 * @date 30 sept. 2021
 * @author David González León, Jade Gröli
 */

#ifndef SHARED_MEMORY_H_
#define SHARED_MEMORY_H_

#include "main.h"
#include "stdbool.h"

// These two pointers are stored in the shared memory so both cores can access it
extern MDMA_HandleTypeDef *mdma_handle;
extern uint16_t *sample_dest_ptr;
#ifdef CORE_CM7
#define SAMPLE_BLOCK_SIZE 1024
extern uint16_t sample_dest[SAMPLE_BLOCK_SIZE];
/**
 * @brief Inits the mdma, and stores the handle in the shared memory
 *
 * @return true if there was no problem initializing
 * @return false if there was a problem initializing
 */
bool MDMA_init();
#else

#endif

#endif /* SHARED_MEMORY_H_ */
