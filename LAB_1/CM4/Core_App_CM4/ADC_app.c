/*
 * ADC_app.c
 *
 *  Created on: Aug 26, 2021
 *      Author: David González León
 */

#include "ADC_app.h"
#include "stdbool.h"
#include "stm32h7xx_hal_adc.h"
#include "mdma_handler.h"

// The buffer used by the DMA to store the conversion data
static __IO uint16_t double_1ch_conversion_data_adc1[ADC1_DOUBLE_BUFFER_SIZE] __attribute__((aligned(32)));

static __IO uint16_t double_1ch_conversion_data_adc2[ADC2_DOUBLE_BUFFER_SIZE] __attribute__((aligned(32)));

#define DAC_TIM5_CLK_SPEED 100000 //120MHz/1200
#define DAC_SQUARE_WAVE_MAX_FREQ 1000
#define DAC_BUFFER_MAX_VAL 4095 // valeur en volt output par le dac(val max)
#define DAC_BUFFER_LENGTH DAC_TIM5_CLK_SPEED/DAC_SQUARE_WAVE_MAX_FREQ

uint16_t dacBuffer[DAC_BUFFER_LENGTH];

/**
 * @brief The function that handles the adc, from starting/stopping it to analyzing the data. Uses a
 * message queue to get inputs on what action to perform
 *
 */
void adcHandlerThread();

// Pointers of the adc and timer, used to get clearer code
ADC_HandleTypeDef *adc1Pointer = &hadc1;
ADC_HandleTypeDef *adc2Pointer = &hadc2;
DAC_HandleTypeDef *dacPointer = &hdac1;
TIM_HandleTypeDef *tim2Pointer = &htim2;
TIM_HandleTypeDef *tim3Pointer = &htim3;
TIM_HandleTypeDef *tim5Pointer = &htim5;

// Indicates if the adc1 is currently running
bool isAdc1Running;
// Indicates if the adc1 is currently running
bool isAdc2Running;

bool adcAppInit(void) {
	// Initialize the adc message queue
	osMessageQueueAttr_t adc_message_queue_attributes = { .name = "adc_message_queue" };
	adcMessageQueueId = osMessageQueueNew(16, sizeof(uint), &adc_message_queue_attributes);

	if (adcMessageQueueId == NULL) {
		PRINTF("Error initializing the adc message queue");
		return false;
	}

	//Configurate DAC
	//Fill the dac buffer
	for (int i = 0; i < DAC_BUFFER_LENGTH; ++i) {
		dacBuffer[i] = i < DAC_BUFFER_LENGTH / 2 ? 0 : DAC_BUFFER_MAX_VAL;
	}

	if (HAL_TIM_Base_Start(tim5Pointer)) {
		PRINTF("Error starting the tim5");
	}
	if (HAL_DAC_Start_DMA(dacPointer, DAC1_CHANNEL_1, (uint32_t*) dacBuffer, DAC_BUFFER_LENGTH, DAC_ALIGN_12B_R) != HAL_OK) {
		PRINTF("Error initializing the dac1 values");
		return false;
	}
	if (HAL_DAC_Start(dacPointer, DAC1_CHANNEL_1) != HAL_OK) {
		PRINTF("Error starting the dac1");
		return false;
	}

	// Calibrate ADC1
	if (HAL_ADCEx_Calibration_Start(adc1Pointer, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
		PRINTF("Error while callibrating the adc1");
		return false;
	}
	isAdc1Running = false;

	//Configure ADC2
	if (HAL_ADCEx_Calibration_Start(adc2Pointer, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
		PRINTF("Error while callibrating the adc2");
		return false;
	}
	if (HAL_ADC_Start_DMA(adc2Pointer, (uint32_t*) double_1ch_conversion_data_adc2, ADC2_DOUBLE_BUFFER_SIZE) != HAL_OK) {
		PRINTF("Error while launching the adc2");
		return false;
	}
	if (HAL_TIM_Base_Start(tim2Pointer) != HAL_OK) {
		PRINTF("Error while starting tim2");
		return false;
	}

	isAdc2Running = true;

	//Start thread
	osThreadAttr_t adc_app_thread_attributes = { .name = "adcHandlerThread", .priority = osPriorityNormal };

	if (!osThreadNew(adcHandlerThread, NULL, &adc_app_thread_attributes)) {
		PRINTF("Error creating the adc app thread");
		return false;
	}

	return true;
}

void adcHandlerThread() {
	uint16_t *sampleStart;
	uint32_t lastTick = 0;

	const uint32_t mvReference = 3300, resolutionBits = 65535;

	while (1) {
		uint32_t msg;
		if (osMessageQueueGet(adcMessageQueueId, &msg, NULL, osWaitForever) != osOK) {
			PRINTF("Error while receiving a message in adc app thread");
			continue;
		}

		// Process msg
		switch (msg) {
		case MSG_ADC1_CONVERSION_START:
			if (isAdc1Running) { // ADC already started
				continue;
			}

			// Start DMA transfer and timer
			if (HAL_ADC_Start_DMA(adc1Pointer, (uint32_t*) double_1ch_conversion_data_adc1,
			ADC1_DOUBLE_BUFFER_SIZE) != HAL_OK) {
				PRINTF("ADC_THREAD : Error while starting the DMA");
			} else if (HAL_TIM_Base_Start(tim3Pointer) != HAL_OK) {
				PRINTF("ADC_THREAD : Error while starting the timer");
			} else {
				PRINTF("ADC conversion started...");
				isAdc1Running = true;
			}
			continue;
		case MSG_ADC1_CONVERSION_STOP:
			if (!isAdc1Running) { // ADC already stopped
				continue;
			}

			// Stop DMA transfer and timer
			if (HAL_ADC_Stop_DMA(adc1Pointer) != HAL_OK) {
				PRINTF("ADC_THREAD : Error while stopping the DMA");
			} else if (HAL_TIM_Base_Stop(tim3Pointer) != HAL_OK) {
				PRINTF("ADC_THREAD : Error while stopping the timer");
			} else {
				PRINTF("ADC conversion stopped...");
				isAdc1Running = false;
			}
			continue;
		case MSG_ADC1_BUFFER_HALF_COMPLETE:
			sampleStart = (uint16_t*) &double_1ch_conversion_data_adc1[0];
			break;
		case MSG_ADC1_BUFFER_COMPLETE:
			sampleStart = (uint16_t*) &double_1ch_conversion_data_adc1[ADC1_DOUBLE_BUFFER_BLOCK_SIZE];
			break;
		case MSG_ADC_ERROR:
			PRINTF("ADC_THREAD : Error while sampling");
			continue;
		case MSG_ADC2_BUFFER_HALF_COMPLETE:
			//start mdma transfer
			HAL_MDMA_Start_IT(mdma_handle, (uint32_t) double_1ch_conversion_data_adc2, (uint32_t) sample_dest_ptr, ADC2_DOUBLE_BUFFER_BLOCK_SIZE * 2, 1);
			continue;
		case MSG_ADC2_BUFFER_COMPLETE:
			//start mdma transfer
			HAL_MDMA_Start_IT(mdma_handle, (uint32_t) &double_1ch_conversion_data_adc2[ADC2_DOUBLE_BUFFER_BLOCK_SIZE], (uint32_t) sample_dest_ptr,
			ADC2_DOUBLE_BUFFER_BLOCK_SIZE * 2, 1);
			continue;
		}

		// Compute average for adc1 sampling
		uint32_t average = 0;
		for (uint i = 0; i < ADC1_DOUBLE_BUFFER_BLOCK_SIZE; ++i) {
			average += *(sampleStart + i);
		}
		average /= ADC1_DOUBLE_BUFFER_BLOCK_SIZE;

		// Convert average into mV
		uint32_t result = (average * mvReference) / resolutionBits;
		uint32_t currentTick = HAL_GetTick();
		PRINTF("Average Volt : %u mV. Time since last printout is %u ms.", result, currentTick - lastTick);
		lastTick = currentTick;

	}
}

// Overide of the interuption routines functions of the adc

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
	uint32_t msg;
	if (hadc == adc1Pointer) {
		msg = MSG_ADC1_BUFFER_HALF_COMPLETE;
	} else if (hadc == adc2Pointer) {
		msg = MSG_ADC2_BUFFER_HALF_COMPLETE;
	} else {
		return;
	}
	osMessageQueuePut(adcMessageQueueId, &msg, 0, 0);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	uint32_t msg;
	if (hadc == adc1Pointer) {
		msg = MSG_ADC1_BUFFER_COMPLETE;
	} else if (hadc == adc2Pointer) {
		msg = MSG_ADC2_BUFFER_COMPLETE;
	} else {
		return;
	}
	osMessageQueuePut(adcMessageQueueId, &msg, 0, 0);
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
	uint msg = MSG_ADC_ERROR;
	osMessageQueuePut(adcMessageQueueId, &msg, 0, 0);
}
