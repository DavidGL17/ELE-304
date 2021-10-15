/**
 * @file App.h
 *
 * @brief defines the functions related to the main app of the CM4
 *
 * @date Aug 19, 2021
 * @author David González León, Jade Gröli
 */

#ifndef APP_CM4_H_
#define APP_CM4_H_

typedef unsigned int uint;

#include "ADC_app.h"
#include "main.h"
#include "print_server.h"
#include "usart.h"

/**
 * @brief Handles the initializing of the different parts of the app, and launches the main thread of the app
 *
 */
void AppInit();

#endif /* APP_CM4_H_ */
