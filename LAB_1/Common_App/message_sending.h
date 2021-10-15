/**
 * @file message_sending.h
 *
 * @brief This file defines the functions that allow both cores to communicate with each other
 *
 * @date 4 sept. 2021
 * @author  David González León, Jade Gröli
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

#define CORE_MESSAGE_INVALID_MESSAGE 0x0 // Default value of the message_info variable
/**
 * Message from CM7 to CM4 to inform that there are bytes to be sent to the usart
 * The bytes to send must be placed in pool_buf_cm7
 */
#define CORE_MESSAGE_CM7_CM4_NEW_PRINT 0x1

// Function that implements the interrupt routine logic. Called by the two interrupt functions of
// the CM4 and CM7 cores
void SEV_handler(void);

// init function, called by the CM7 core only
#ifdef CORE_CM7
void cleanInitMessageSending(void);
#endif
bool messageSendingInit();

// Message sending function
void sendMessage(uint32_t message, void *args);

#endif /* MESSAGE_SENDING_H_ */
