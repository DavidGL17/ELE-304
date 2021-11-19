/**
 * @file App_CM7.h
 *
 * @brief This file defines the main functions of the CM7 App
 *
 * @date 5 sept. 2021
 * @author David González León, Jade Gröli
 */

#ifndef APP_CM7_H_
#define APP_CM7_H_

#include "main.h"
#include "stdbool.h"
#include "cmsis_os2.h" /* CMSIS RTOS v2 header file */

#define MESSAGE_QUEUE_CM7_SIZE 224
#define CM7_MESS_MDMA_TRANSFER_CPLT 0x1
extern osMessageQueueId_t cm7_message_queue_id;

/**
 * @brief Inits the variables that must be initialized before the OS starts
 *
 */
void preOsStartupInit();

/**
 * @brief Starts the CM7 App and initializes all its components
 *
 * @return true if the cm7 app launched correctly
 * @return false if there was an error while launching the cm7 app
 */
bool AppCM7Init();

#endif /* APP_CM7_H_ */
