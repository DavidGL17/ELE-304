/*
 * App_CM7.c
 *
 *  Created on: 5 sept. 2021
 *      Author: xxpow
 */

#include "App_CM7.h"
#include "print_server.h"
#include "message_sending.h"

void app_CM7();

void preOsStartupInit(){
	cleanInitMessageSending();
	cleanInitPrintServer();
}

bool AppCM7Init() {
	bool result = messageSendingInit();
	result &= PrintServerInit();

	// Launch main app thread
	osThreadAttr_t app_CM7_attributes = { .name = "app_CM7", .priority = osPriorityNormal };
	osThreadNew(app_CM7, NULL, &app_CM7_attributes);

	if (result) {
		PRINTF("CM7 message sending and print server correctly initialized");
	}


	return result;
}

void app_CM7() {
	while (1) {
		PRINTF("Message from CM7");
		osDelay(1000);
	}
}
