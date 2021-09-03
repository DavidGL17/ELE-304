/*
 * ADC_app.c
 *
 *  Created on: Aug 26, 2021
 *      Author: David González León
 */

#include "ADC_app.h"
#include "stdbool.h"
#include "stm32h7xx_hal_adc.h"

// The buffer used by the DMA to store the conversion data
static __IO uint16_t double_1ch_conversion_data[ADC_DOUBLE_BUFFER_SIZE] __attribute__((aligned(32)));

/**
 * @brief The function that handles the adc, from starting/stopping it to analyzing the data. Uses a
 * message queue to get inputs on what action to perform
 *
 */
void adcHandlerThread();

// Pointers of the adc and timer, used to get clearer code
ADC_HandleTypeDef *adcPointer = &hadc1;
TIM_HandleTypeDef *timPointer = &htim3;

// Indicates if the adc is currently running
bool isAdcRunning;

bool adcAppInit(void) {
	// Initialize the adc message queue
	osMessageQueueAttr_t adc_message_queue_attributes = { .name = "adc_message_queue" };
	adcMessageQueueId = osMessageQueueNew(16, sizeof(uint), &adc_message_queue_attributes);

	if (adcMessageQueueId == NULL) {
		PRINTF("Error initializing the adc message queue");
		return false;
	}

	// Calibrate ADC
	if (HAL_ADCEx_Calibration_Start(adcPointer, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
		PRINTF("Error while callibrating the adc");
		return false;
	}

	osThreadAttr_t adc_app_thread_attributes = { .name = "adcHandlerThread", .priority = osPriorityNormal };

	if (!osThreadNew(adcHandlerThread, NULL, &adc_app_thread_attributes)) {
		PRINTF("Error creating the adc app thread");
		return false;
	}
	isAdcRunning = false;

	return true;
}

void adcHandlerThread() {
	uint16_t *sampleStart;
	uint32_t lastTick = 0;

	const uint32_t mvReference = 3300,resolutionBits = 65535;

	while (1) {
		uint32_t msg;
		if (osMessageQueueGet(adcMessageQueueId, &msg, NULL, osWaitForever) != osOK) {
			PRINTF("Error while receiving a message in adc app thread");
			continue;
		}

		// Process msg
		switch (msg) {
		case MSG_ADC_CONVERSION_START:
			if (isAdcRunning) { // ADC already started
				continue;
			}

			// Start DMA transfer and timer
			if (HAL_ADC_Start_DMA(adcPointer, (uint32_t*) double_1ch_conversion_data,
			ADC_DOUBLE_BUFFER_SIZE) != HAL_OK) {
				PRINTF("ADC_THREAD : Error while starting the DMA");
			} else if (HAL_TIM_Base_Start(timPointer) != HAL_OK) {
				PRINTF("ADC_THREAD : Error while starting the timer");
			} else {
				PRINTF("ADC conversion started...");
				isAdcRunning = true;
			}
			continue;
		case MSG_ADC_CONVERSION_STOP:
			if (!isAdcRunning) { // ADC already stopped
				continue;
			}

			// Stop DMA transfer and timer
			if (HAL_ADC_Stop_DMA(adcPointer) != HAL_OK) {
				PRINTF("ADC_THREAD : Error while stopping the DMA");
			} else if (HAL_TIM_Base_Stop(timPointer) != HAL_OK) {
				PRINTF("ADC_THREAD : Error while stopping the timer");
			} else {
				PRINTF("ADC conversion stopped...");
				isAdcRunning = false;
			}
			continue;
		case MSG_ADC_BUFFER_HALF_COMPLETE:
			sampleStart = (uint16_t*) &double_1ch_conversion_data[0];
			break;
		case MSG_ADC_BUFFER_COMPLETE:
			sampleStart = (uint16_t*) &double_1ch_conversion_data[ADC_DOUBLE_BUFFER_BLOCK_SIZE];
			break;
		case MSG_ADC_ERROR:
			PRINTF("ADC_THREAD : Error while sampling");
			continue;
		}

		// Compute average
		uint32_t average = 0;
		for (uint i = 0; i < ADC_DOUBLE_BUFFER_BLOCK_SIZE; ++i) {
			average += *(sampleStart + i);
		}
		average /= ADC_DOUBLE_BUFFER_BLOCK_SIZE;

		// Convert average into mV
		uint32_t result = (average * mvReference) / resolutionBits;
		uint32_t currentTick = HAL_GetTick();
		PRINTF("Average Volt : %u mV. Time since last printout is %u ms.", result, currentTick - lastTick);
		lastTick = currentTick;
	}
}

// Overide of the interuption routines functions of the adc

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
	uint msg = MSG_ADC_BUFFER_HALF_COMPLETE;
	osMessageQueuePut(adcMessageQueueId, &msg, 0, 0);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	uint msg = MSG_ADC_BUFFER_COMPLETE;
	osMessageQueuePut(adcMessageQueueId, &msg, 0, 0);
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
	uint msg = MSG_ADC_ERROR;
	osMessageQueuePut(adcMessageQueueId, &msg, 0, 0);
}
