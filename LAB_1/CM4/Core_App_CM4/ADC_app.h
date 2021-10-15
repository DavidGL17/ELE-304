/**
 * @file ADC_app.h
 *
 *	@brief This file contains the definition of the functions allowing the control of the different ADCs and DACs
 *
 * @date 30 sept. 2021
 * @author David González León, Jade Gröli
 */

#ifndef ADC_APP_H_
#define ADC_APP_H_

#include "App_CM4.h"
#include "adc.h"
#include "cmsis_os2.h"
#include "dac.h"
#include "main.h"
#include "stdbool.h"
#include "tim.h"

// Size of the buffer
#define ADC1_DOUBLE_BUFFER_SIZE 50000UL
#define ADC1_DOUBLE_BUFFER_BLOCK_SIZE ADC1_DOUBLE_BUFFER_SIZE / 2

#define ADC2_DOUBLE_BUFFER_SIZE 2048UL
#define ADC2_DOUBLE_BUFFER_BLOCK_SIZE ADC2_DOUBLE_BUFFER_SIZE / 2

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
