/*
 * App.c
 *
 *  Created on: Aug 19, 2021
 *      Author: David González León
 */

#include "App.h"
#include "message_sending.h"

/**
 * @brief The function used in the main thread of the CM4 core
 *
 */
void app_CM4();

void AppInit() {
	// Initialize uart and adc
	if (PrintServerInit(&huart3)) {
		PRINTF("CM4 : UART initialized correctly");
	}

	if (adcAppInit()) {
		PRINTF("CM4 : ADC initialized correctly");
	}

	if (messageSendingInit()) {
		PRINTF("CM4 : Message sending initialized correctly");
	}

	// Launch main app thread
	osThreadAttr_t app_CM4_attributes = { .name = "app_CM4", .priority = osPriorityNormal };
	osThreadNew(app_CM4, NULL, &app_CM4_attributes);
}

void app_CM4() {
	//uint msg;
	while (1) {
		/*msg = MSG_ADC_CONVERSION_START;
		osMessageQueuePut(adcMessageQueueId, &msg, 0, osWaitForever);
		osDelay(10000);

		msg = MSG_ADC_CONVERSION_STOP;
		osMessageQueuePut(adcMessageQueueId, &msg, 0, osWaitForever);
		osDelay(10000);*/
	}
}
