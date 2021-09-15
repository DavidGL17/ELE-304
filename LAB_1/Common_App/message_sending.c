/*
 * message_sending.c
 *
 *  Created on: 4 sept. 2021
 *      Author: xxpow
 */

#include "message_sending.h"
#include "stdbool.h"
#include "stm32h7xx_it.h"
#include "print_server.h"

/**
 * Storage space for message sending
 */
#pragma GCC optimize("O0")
//Define a pointer to the message holding space, and possibly one to the message additional content buffer, and one to
//specify if there is a message being sent
volatile uint32_t message_info __attribute__ ((aligned(4))) __attribute__ ((section(".RAM_D3_SHM")));
volatile void *message_content_ptr __attribute__ ((aligned(4))) __attribute__ ((section(".RAM_D3_SHM")));
volatile uint32_t message_sending_ready __attribute__ ((aligned(4))) __attribute__ ((section(".RAM_D3_SHM")));

//Override definition of both functions that are called when SEV interrupt is received
void SEV_handler(void) {
	switch (message_info) {	//specific behaviour depending on the message received
#ifdef CORE_CM7 //in core CM7
	//nothing yet
#else //In core CM4
	case CORE_MESSAGE_CM7_CM4_NEW_PRINT:
		SEVMessageHandling((uint8_t*) message_content_ptr);
		break;
#endif
	default:
		return;
	}
	message_sending_ready = 1;
}
bool messageSendingInit() {
#ifdef CORE_CM4//if on core CM4
	//do initialisation if needed
	message_sending_ready = 1; //Indicate that message sending is ready on CM4
#else //if on core CM7
	while (!message_sending_ready){
		//wait for CM4 to initialize his app
	}
#endif
	return true;
}

#ifdef CORE_CM7
void cleanInitMessageSending(void) {
	message_sending_ready = 0;
	message_info = CORE_MESSAGE_INVALID_MESSAGE;
	message_content_ptr = NULL;
}
#endif

bool checkMessageValue(uint32_t message);	//checks if the message is a defined message

void sendMessage(uint32_t message, void *arg) {
	if (!checkMessageValue(message)) {
		return;
	}
	while (!message_sending_ready) {
		//wait for message_sending to be ready
		//Possibility to improve with mutex/semaphore, dont know if there are any given by the os
	}
	message_sending_ready = 0;
	switch (message) {
	//Messages on both cores
#ifdef CORE_CM7 //Messages only on Core CM7
	case CORE_MESSAGE_CM7_CM4_NEW_PRINT:
		message_content_ptr = arg;
		break;
#else //Messages only on Core CM4
#endif
	default:
		return;
	}
	message_info = message;
	__SEV();
}

bool checkMessageValue(uint32_t message) {
	if (message > CORE_MESSAGE_MAX_VALUE) {
		return false;
	}
	return true;
}
