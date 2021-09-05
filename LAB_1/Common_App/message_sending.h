/*
 * message_sending.h
 *
 *  Created on: 4 sept. 2021
 *      Author: xxpow
 */

#ifndef MESSAGE_SENDING_H_
#define MESSAGE_SENDING_H_

#include "main.h"
#include "stdbool.h"

/**
 * Messages def
 *	syntax : CORE_MESSAGE_FROM_TO_INFO
 */

#define CORE_MESSAGE_CM7_CM4_NEW_PRINT 0x0
#define CORE_MESSAGE_CM4_CM7_PRINT_OK 0x1

/**
 * Storage space for message sending
 */
//Define a pointer to the message holding space, and possibly one to the message additional content buffer
#ifndef SHM_PTR
#define SHM_PTR
volatile uint32_t *message_info_ptr __attribute__ ((aligned(4))) __attribute__ ((section(".RAM_D3_SHM")));
volatile void *message_content_ptr __attribute__ ((aligned(4))) __attribute__ ((section(".RAM_D3_SHM")));
#endif
//Functions that implement the interrupt routine logic. Called by the two interrupt functions of the CM4 and CM7 cores
void CM4_SEV_handler(void);
void CM7_SEV_handler(void);

//init function, called by the CM7 core only
#ifdef CORE_CM7
bool messageSendingInit();
#endif

#endif /* MESSAGE_SENDING_H_ */
