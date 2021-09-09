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

//Override definition of both functions that are called when SEV interrupt is received
void SEV_handler(void) {
	switch (*message_info_ptr) {	//specific behaviour depending on the message received
#ifdef CORE_CM7 //in core CM7
	//nothing yet
#else //In core CM4
	case CORE_MESSAGE_CM7_CM4_NEW_PRINT:
		PRINTF("%s", (char* )message_content_ptr);	//TODO add in the printf the content of the buffer given by cm7
		break;
#endif
	default:
		break;
	}
	message_sending_ready = 1;
}
bool messageSendingInit() {
#ifdef CORE_CM4//if on core CM4
	//do initialisation if needed
	message_sending_ready = 1; //Indicate that message sending is ready on CM4
#else //if on core CM7
	//do initialisation if needed
	while (!message_sending_ready){
		//wait for cm4 to init message sending
	}
#endif
	return true;
}

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
		for (int i = 0; i < PS_PRINT_BUFFER_SIZE; ++i) {
			pool_buf_cm7[i] = ((uint8_t*)arg)[i];
		}
		message_content_ptr = (void*) pool_buf_cm7;
		break;
#else //Messages only on Core CM4
#endif
	default:
		break;
	}
	*message_info_ptr = message;
}

bool checkMessageValue(uint32_t message) {
	if (message > CORE_MESSAGE_MAX_VALUE) {
		return false;
	}
	return true;
}
