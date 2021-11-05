/**
 * @file  bootloader.h
 * @brief Defines the main public functions of the bootloader

 * @date  28 oct. 2021
 * @author David González León, Jade Gröli
 *
 */
#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#include "main.h"

/**
 * @brief Initiates the bootloader
 *
 * This function will jump to the main app and so never return. All code placed after this function will never execute
 *
 */
void bootloader_init();

/**
 * @brief printf for the bootloader
 *
 * @param fmt The string to print through the uart
 */
void printf_bootloader(const char *fmt, ...);

#define PRINTF(...) printf_bootloader(__VA_ARGS__)

#endif /* BOOTLOADER_H_ */
