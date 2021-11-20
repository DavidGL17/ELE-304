/**
 * @file bootloader.c
 * @author David González León, Jade Gröli
 * @brief Main code of the bootloader
 * @version 0.1
 * @date 20-11-2021
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "bootloader.h"
#include "check_security_protections.h"
#include "cmox_crypto.h"
#include "firmware_authentification.h"
#include "usart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* For convenience, define a pointer to UART handle */
UART_HandleTypeDef *uart3_handle = &huart3;

#define PS_PRINT_BUFFER_SIZE 224

#define APP_REGION_ROM_START 0x08020400

typedef volatile uint32_t vuint32;
#define UNIQUE_ID_ADRESS_BASE 0x1FF1E800

#define UNIQUE_ID_LOW *(vuint32 *)(UNIQUE_ID_ADRESS_BASE + 0x0)
#define UNIQUE_ID_MID *(vuint32 *)(UNIQUE_ID_ADRESS_BASE + 0x4)
#define UNIQUE_ID_HIGH *(vuint32 *)(UNIQUE_ID_ADRESS_BASE + 0x8)

extern uint32_t _sdata;
extern uint32_t _edata;

#define RAM_D1_START ((uint32_t)&_sdata)
#define RAM_D1_END ((uint32_t)&_edata)

/**
 * @brief Erases all the ram
 *
 */
void SRAM_Erase() {
	uint32_t *pRam;
	for (pRam = (uint32_t*) RAM_D1_START; pRam < (uint32_t*) RAM_D1_END; pRam++) {
		*pRam = 0U;
	}
}

static void PrintServerCRLF(char *const buf) {
	uint32_t len = strlen(buf);

	if ((len > 1)
			&& (((buf[len - 2] == '\r') && (buf[len - 1] == '\n')) || ((buf[len - 2] == '\n') && (buf[len - 1] == '\r')))) {
		return;
	}

	if ((buf[len - 1] == '\r') || (buf[len - 1] == '\n')) {
		len--;
	}

	if (len == PS_PRINT_BUFFER_SIZE - 1U) {
		len -= 2U;
	} else if (len == PS_PRINT_BUFFER_SIZE - 2U) {
		len -= 1U;
	}
	buf[len] = '\r';
	len++;
	buf[len] = '\n';
	len++;
	buf[len] = '\0';
}

void printf_bootloader(const char *fmt, ...) {
	/* va_list is a type to hold information about variable arguments */
	va_list args;
	/* va_start must be called before accessing variable argument list */
	va_start(args, fmt);

	uint8_t buff[PS_PRINT_BUFFER_SIZE];

	/* The vsnprintf() function formats and stores a series of characters and
	 * values in the buffer target-string. The vsnprintf() function works just
	 * like the snprintf() function, except that arg_ptr points to a list of
	 * arguments whose number can vary from call to call in the program. These
	 * arguments should be initialized by the va_start function for each call.
	 * In contrast, the snprintf() function can have a list of arguments, but
	 * the number of arguments in that list is fixed when you compile the program.
	 *
	 * The vsnprintf() function converts each entry in the argument list according
	 * to the corresponding format specifier in format. The format has the same form
	 * and function as the format string for the printf() function. */
	vsnprintf((char*) buff, PS_PRINT_BUFFER_SIZE, fmt, args);

	/* va_end should be executed before the function returns whenever
	 * va_start has been previously used in that function */
	va_end(args);

	/* Make sure formatted printout ends with \r\n */
	PrintServerCRLF((char*) buff);

	if (HAL_UART_Transmit(uart3_handle, buff, strlen((char*) buff), 100) == HAL_OK) {
		while (HAL_UART_GetState(uart3_handle) == HAL_BUSY) {
			// Wait for uart to finish transfer
		}
		return;
	}
	printf_bootloader("Error in printf bootloader");
}

void print_bootloader(const char *fmt, ...) {
	/* va_list is a type to hold information about variable arguments */
	va_list args;
	/* va_start must be called before accessing variable argument list */
	va_start(args, fmt);

	uint8_t buff[PS_PRINT_BUFFER_SIZE];

	/* The vsnprintf() function formats and stores a series of characters and
	 * values in the buffer target-string. The vsnprintf() function works just
	 * like the snprintf() function, except that arg_ptr points to a list of
	 * arguments whose number can vary from call to call in the program. These
	 * arguments should be initialized by the va_start function for each call.
	 * In contrast, the snprintf() function can have a list of arguments, but
	 * the number of arguments in that list is fixed when you compile the program.
	 *
	 * The vsnprintf() function converts each entry in the argument list according
	 * to the corresponding format specifier in format. The format has the same form
	 * and function as the format string for the printf() function. */
	vsnprintf((char*) buff, PS_PRINT_BUFFER_SIZE, fmt, args);

	/* va_end should be executed before the function returns whenever
	 * va_start has been previously used in that function */
	va_end(args);

	if (HAL_UART_Transmit(uart3_handle, buff, strlen((char*) buff), 100) == HAL_OK) {
		while (HAL_UART_GetState(uart3_handle) == HAL_BUSY) {
			// Wait for uart to finish transfer
		}
		return;
	}
	printf_bootloader("Error in print bootloader");
}

void bootloader_init() {
	PRINTF("ID_low is %x, ID_middle is %x, ID_high is %x\n", UNIQUE_ID_LOW, UNIQUE_ID_MID, UNIQUE_ID_HIGH);

	// secure boot
	cmox_initialize(NULL);
	CheckApplyStaticProtections();
	FW_Verify();
	// secure boot succesful, jumping to app

	// disable uart
	if (HAL_UART_DeInit(uart3_handle) == HAL_OK) {
	}

	// Jump to main app
	uint32_t jump_address;
	typedef void (*Function_Pointer)(void);
	Function_Pointer p_jump_to_function;
	/* Destroy the Volatile data and CSTACK in SRAM used by Secure Boot in order to prevent any access to sensitive data
	 from the loader. */
	SRAM_Erase();
	/* Initialize address to jump */
	jump_address = *(__IO uint32_t*) ((((uint32_t) APP_REGION_ROM_START) + 0x4UL));
	p_jump_to_function = (Function_Pointer) jump_address;
	/* Initialize loader's Stack Pointer */
	__set_MSP(*(__IO uint32_t*) (APP_REGION_ROM_START));

	// try a endless loop to see if the watchdog is launched

	/* Jump into loader */
	p_jump_to_function();
	/* The point below should NOT be reached */
	return;
}
