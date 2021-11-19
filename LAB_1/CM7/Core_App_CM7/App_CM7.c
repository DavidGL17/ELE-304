/**
 * @file App_CM7.c
 *
 * @brief This file defines the main functions of the CM7 App
 *
 * @date 5 sept. 2021
 * @author David González León, Jade Gröli
 */

#include "App_CM7.h"
#include "DSP_handler.h"
#include "mdma_handler.h"
#include "message_sending.h"
#include "print_server.h"

osMessageQueueId_t cm7_message_queue_id = NULL;

void app_CM7();



void preOsStartupInit() {
	cleanInitMessageSending();
	cleanInitPrintServer();
}

bool AppCM7Init() {
	bool result = messageSendingInit();
	result &= PrintServerInit();

	//Create message queue
	/* Define PrintServer message queue attributes */
	osMessageQueueAttr_t cm7_message_queue_attributes = { .name = "CM7MessageQueue" };
	/* Create PrintServer message queue */
	cm7_message_queue_id = osMessageQueueNew(MESSAGE_QUEUE_CM7_SIZE, sizeof(uint32_t), &cm7_message_queue_attributes);
	if (cm7_message_queue_id == NULL)
		return false;

	// Launch main app thread
	osThreadAttr_t app_CM7_attributes = { .name = "app_CM7", .priority = osPriorityNormal };
	osThreadNew(app_CM7, NULL, &app_CM7_attributes);

	if (result) {
		PRINTF("CM7 message sending and print server correctly initialized");
	}

	if (MDMA_init()) {
		PRINTF("MDMA initialized correctly");
	}

	if (DSP_init()) {
		PRINTF("DSP initialized correctly");
	}

	return true;
}

void app_CM7() {
	while (1) {
		uint32_t msg;
		if (osMessageQueueGet(cm7_message_queue_id, &msg, NULL, osWaitForever) != osOK) {
			PRINTF("Error while receiving a message in adc app thread");
			continue;
		}
		switch (msg) {
		case CM7_MESS_MDMA_TRANSFER_CPLT:
			rfft(sample_dest);
			break;
		default:
			break;
		}
	}
}
