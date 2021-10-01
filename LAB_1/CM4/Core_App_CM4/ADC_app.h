/*
 * ADCapp.c
 *
 *  Created on: Aug 26, 2021
 *      Author: David González León
 */

#ifndef ADC_APP_H_
#define ADC_APP_H_

#include "App.h"
#include "adc.h"
#include "dac.h"
#include "cmsis_os2.h"
#include "main.h"
#include "tim.h"
#include "stdbool.h"

// Size of the buffer
#define ADC1_DOUBLE_BUFFER_SIZE 50000UL
#define ADC1_DOUBLE_BUFFER_BLOCK_SIZE ADC1_DOUBLE_BUFFER_SIZE / 2

#define ADC2_DOUBLE_BUFFER_SIZE 2048UL
#define ADC2_DOUBLE_BUFFER_BLOCK_SIZE ADC2_DOUBLE_BUFFER_SIZE/2

// MSG to the running adc handler thread
#define MSG_ADC1_CONVERSION_START 0x1
#define MSG_ADC1_CONVERSION_STOP 0x2
#define MSG_ADC1_BUFFER_HALF_COMPLETE 0x3
#define MSG_ADC1_BUFFER_COMPLETE 0x4
#define MSG_ADC_ERROR 0x5
#define MSG_ADC2_BUFFER_HALF_COMPLETE 0x6
#define MSG_ADC2_BUFFER_COMPLETE 0x7


// Message queue id. Should be used to send messages to the adc handler thread
osMessageQueueId_t adcMessageQueueId;

/**
 * @brief Initializes the adc and starts a thread that handles all the adc's interuptions, and
 * allows for activation/deactivation of the adc through messages
 *
 * @return true if the adc was correctly initialized
 * @return false if there was a problem while initializing the adc
 */
bool adcAppInit(void);

#endif /* ADCAPP_C_ */
