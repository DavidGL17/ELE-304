/*
 * App.h
 *
 *  Created on: Aug 19, 2021
 *      Author: David González León
 */

#ifndef APP_H_
#define APP_H_

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

#endif /* APP_H_ */
