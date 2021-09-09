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

#define CORE_MESSAGE_MAX_VALUE 0x1

/**
 * Message from CM7 to CM4 to inform that there are bytes to be sent to the usart
 * The bytes to send must be placed in pool_buf_cm7
 */
#define CORE_MESSAGE_CM7_CM4_NEW_PRINT 0x0
#define CORE_MESSAGE_CM4_CM7_PRINT_OK 0x1

/**
 * Storage space for message sending
 */
//Define a pointer to the message holding space, and possibly one to the message additional content buffer, and one to
//specify if there is a message being sent
volatile uint32_t *message_info_ptr __attribute__ ((aligned(4))) __attribute__ ((section(".RAM_D3_SHM")));
volatile void *message_content_ptr __attribute__ ((aligned(4))) __attribute__ ((section(".RAM_D3_SHM")));
volatile uint32_t message_sending_ready __attribute__ ((aligned(4))) __attribute__ ((section(".RAM_D3_SHM"))) = 0;
//Define two buffer spaces for each core
#define SIZE_OF_POOL 1000
uint8_t pool_buf_cm4[SIZE_OF_POOL] __attribute__ ((aligned(32))) __attribute__ ((section(".RAM_D3_SHM")));
uint8_t pool_buf_cm7[SIZE_OF_POOL] __attribute__ ((aligned(32))) __attribute__ ((section(".RAM_D3_SHM")));
//Function that implements the interrupt routine logic. Called by the two interrupt functions of the CM4 and CM7 cores
void SEV_handler(void);

//init function, called by the CM7 core only
#ifdef CORE_CM7
bool messageSendingInit();
#endif

//Message sending function
void sendMessage(uint32_t message, void* args);

#endif /* MESSAGE_SENDING_H_ */
