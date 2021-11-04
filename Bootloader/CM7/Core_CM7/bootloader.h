/**
 * @file  {file_name}
 * @brief 
 * @date  28 oct. 2021
 * @author xxpow
 *
 */
#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#include "main.h"

void bootloader_init();
void printf_bootloader(const char *fmt, ...);

#define PRINTF(...) printf_bootloader(__VA_ARGS__)

#endif /* BOOTLOADER_H_ */
