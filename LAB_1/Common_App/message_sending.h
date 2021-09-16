/*
 * message_sending.h
 *
 *  Created on: 4 sept. 2021
 *      Author: xxpow
 */

#ifndef MESSAGE_SENDING_H_
#define MESSAGE_SENDING_H_

//Choose either SEV or HSEM as a method of communication
#define CORE_COMMUNICATION_METHOD_SEV
#define CORE_COMMUNICATION_METHOD_HSEM

#include "main.h"
#include "stdbool.h"

#ifdef CORE_COMMUNICATION_METHOD_SEV
/**
 * Messages def
 *	syntax : CORE_MESSAGE_FROM_TO_INFO
 */

#define CORE_MESSAGE_MAX_VALUE 0x1

#define CORE_MESSAGE_INVALID_MESSAGE 0x0 //Default value of the message_info variable
/**
 * Message from CM7 to CM4 to inform that there are bytes to be sent to the usart
 * The bytes to send must be placed in pool_buf_cm7
 */
#define CORE_MESSAGE_CM7_CM4_NEW_PRINT 0x1


//Function that implements the interrupt routine logic. Called by the two interrupt functions of the CM4 and CM7 cores
void SEV_handler(void);
#endif
//init function, called by the CM7 core only
#ifdef CORE_CM7
void cleanInitMessageSending(void);
#endif
bool messageSendingInit();

//Message sending function
void sendMessage(uint32_t message, void* args);

#endif /* MESSAGE_SENDING_H_ */
