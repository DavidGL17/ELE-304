/*
 * message_sending.c
 *
 *  Created on: 4 sept. 2021
 *      Author: xxpow
 */

#include "message_sending.h"
#include "stm32h7xx_it.h"

//Override definition of both functions that are called when SEV interrupt is received

void CM4_SEV_handler(void) { //received by CM7 when sev called by CM4
	//nothing yet
}

void CM7_SEV_handler(void) {	//received by CM4 when sev called by CM7
	//send a message to the printer thread so that it will get the message from the allocated buffer and send it
	//through the uart
}

bool messageSendingInit() {
 //TODO init le reste des attributs nécessaires pour l'envoi de messages
}
