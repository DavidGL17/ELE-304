/*
 * App_CM7.c
 *
 *  Created on: 5 sept. 2021
 *      Author: xxpow
 */

#include "App_CM7.h"
#include "print_server.h"
#include "message_sending.h"

bool AppCM7Init() {
	bool result = messageSendingInit();
	result &= PrintServerInit();
	if (result) {
		PRINTF("CM7 message sending and print server correctly initialized");
	}
	return result;
}
