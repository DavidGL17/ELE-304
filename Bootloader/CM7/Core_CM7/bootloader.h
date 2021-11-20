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
 * This function will jump to the main app and so never return. All code placed after this function will never execute.
 * This function will first check that the code in the device has not been tampered with. If it has, it will not launch that code and will enter an eternal loop
 *
 */
void bootloader_init();

/**
 * @brief printf for the bootloader
 *
 * @param fmt The string to print through the uart
 */
void printf_bootloader(const char *fmt, ...);

/**
 * @brief Same as printf_bootloader but does not include a \r\n at the end
 *
 * @param fmt The string to print through the uart
 */
void print_bootloader(const char *fmt, ...);

#define PRINTF(...) printf_bootloader(__VA_ARGS__)
#define PRINT(...) print_bootloader(__VA_ARGS__)

#endif /* BOOTLOADER_H_ */
