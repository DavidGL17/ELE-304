/*
 * mdma_handler.c
 *
 *  Created on: 30 sept. 2021
 *      Author: xxpow
 */

#include "mdma_handler.h"
#include "mdma.h"

MDMA_HandleTypeDef *mdma_handle __attribute__ ((aligned(4))) __attribute__ ((section(".RAM_D3_SHM")));

uint16_t* sample_dest_ptr __attribute__ ((aligned(4))) __attribute__ ((section(".RAM_D3_SHM")));

#ifdef CORE_CM7
uint16_t sample_dest[SAMPLE_BLOCK_SIZE]  __attribute__ ((aligned(4))) __attribute__ ((section(".DTCMRAM")));

void mdma_transfer_cmplt_interupt();

bool MDMA_init() {
	HAL_MDMA_DeInit(&hmdma_mdma_channel40_sw_0);

	/* MDMA controller clock enable */
	__HAL_RCC_MDMA_CLK_ENABLE();
	/* Local variables */

	/* Configure MDMA channel MDMA_Channel0 */
	/* Configure MDMA request hmdma_mdma_channel40_sw_0 on MDMA_Channel0 */
	hmdma_mdma_channel40_sw_0.Instance = MDMA_Channel0;
	hmdma_mdma_channel40_sw_0.Init.Request = MDMA_REQUEST_SW;
	hmdma_mdma_channel40_sw_0.Init.TransferTriggerMode = MDMA_FULL_TRANSFER;
	hmdma_mdma_channel40_sw_0.Init.Priority = MDMA_PRIORITY_LOW;
	hmdma_mdma_channel40_sw_0.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
	hmdma_mdma_channel40_sw_0.Init.SourceInc = MDMA_SRC_INC_WORD;
	hmdma_mdma_channel40_sw_0.Init.DestinationInc = MDMA_DEST_INC_WORD;
	hmdma_mdma_channel40_sw_0.Init.SourceDataSize = MDMA_SRC_DATASIZE_WORD;
	hmdma_mdma_channel40_sw_0.Init.DestDataSize = MDMA_DEST_DATASIZE_WORD;
	hmdma_mdma_channel40_sw_0.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
	hmdma_mdma_channel40_sw_0.Init.BufferTransferLength = 64;
	hmdma_mdma_channel40_sw_0.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
	hmdma_mdma_channel40_sw_0.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
	hmdma_mdma_channel40_sw_0.Init.SourceBlockAddressOffset = 0;
	hmdma_mdma_channel40_sw_0.Init.DestBlockAddressOffset = 0;
	if (HAL_MDMA_Init(&hmdma_mdma_channel40_sw_0) != HAL_OK) {
		return false;
	}

	HAL_MDMA_RegisterCallback(&hmdma_mdma_channel40_sw_0, HAL_MDMA_XFER_CPLT_CB_ID, &mdma_transfer_cmplt_interupt);

	mdma_handle = &hmdma_mdma_channel40_sw_0;

	sample_dest_ptr = sample_dest;
	return true;
}

void mdma_transfer_cmplt_interupt() {

}
#else
#endif