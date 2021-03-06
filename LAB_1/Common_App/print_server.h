/**
 * @file  print_server.h
 * @brief This file contains all the function prototypes and configuration definitions for the print_server.c file.
 * @date  2021-08-12
 * @author David González León, Jade Gröli
 *
 */
#ifndef INCLUSION_GUARD_PRINT_SERVER_H
#define INCLUSION_GUARD_PRINT_SERVER_H

#include "cmsis_os.h" /* CMSIS RTOS header file */
#include "main.h"
#include <stdbool.h>

/**
 * @brief Define print buffer size, i.e. maximum length of a printout
 */
#define PS_PRINT_BUFFER_SIZE 224

/**
 * @brief Define pool and message queue size, i.e. maximum number of unsent printouts
 */
#define PS_POOL_AND_MESSAGE_QUEUE_SIZE 40

/**
 * @brief Make the UART handle used by the PrintServer available globally.
 */
extern UART_HandleTypeDef *uart3_handle;

#ifdef CORE_CM4
/**
 * @brief Function for initializing the PrintServer on the CM4 side.
 *
 * This function configures a memory pool for printouts, a message queue for sending
 * printout to PrintServer thread.
 *
 * @param[in] huart Pointer to a handle for the UART to be used for printout.
 *
 * @retval true  If initialization was successful.
 * @retval false If initialization failed.
 */
bool PrintServerInit(void *huart);

void SEVMessageHandling(uint8_t *buff);
#else
/**
 * @brief Function for initializing the PrintServer on the CM7 side.
 *
 * This function configures the transmission of the messages to the CM4, so that they can be sent
 * through the UART
 *
 * @retval true  If initialization was successful.
 * @retval false If initialization failed.
 */
bool PrintServerInit();
#endif

/**
 * @brief Function for sending formatted print output to the UART.
 *
 * This function takes a variable number of arguments and formats a string that is sent to UART.
 *
 * @param[in] fmt Pointer to a string that contains formatting options.
 * @param[in] ... Variable number of arguments to enter the formatted string.
 */
void PrintServerPrintf(const char *fmt, ...);

void sendToUART(uint8_t *buff);

#ifdef CORE_CM7
void cleanInitPrintServer(void);
#endif
/**
 * @brief PRINTF macro for simplicity.
 */
#if 1
#define PRINTF(...) PrintServerPrintf(__VA_ARGS__)
#else
#define PRINTF(...)                                                                                                                                                                \
   do {                                                                                                                                                                            \
   } while (0)
#endif

#endif /* INCLUSION_GUARD_PRINT_SERVER_H */
