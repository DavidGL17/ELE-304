/**
 * @file  firmware_authentification.h
 * @brief 
 * @date  Nov 16, 2021
 * @author David González León, Jade Gröli
 *
 */
#ifndef FIRMWARE_AUTHENTIFICATION_H_
#define FIRMWARE_AUTHENTIFICATION_H_

#include "firmware_authentification.h"

void Fatal_Error_Handler(void) {
	PRINTF("\r\nFatal error! Enter endless loop!\r\n");
	while (1) {
	};
}

static void print_buffer(char *name, uint8_t *buf, uint32_t size) {
	int i;
	if (name != NULL) {
		PRINTF("\r\n*** %s *** \r\n", name);
	} else {
		PRINTF("\r\n");
	}
	for (i = 0; i < size; i++) {
		PRINTF("%02x", buf[i]);
		if ((i + 1) % 8 == 0)
			PRINTF("  ");
		if ((i + 1) % 16 == 0)
			PRINTF("\r\n");
	}
	PRINTF("\r\n");
}

/**
 * @brief  SHA256 HASH digest compute example.
 * @param  InputMessage: pointer to input message to be hashed.
 * @param  InputMessageLength: input data message length in byte.
 * @param  MessageDigest: pointer to output parameter that will handle message digest
 * @param  MessageDigestLength: pointer to output digest length.
 * @retval error status: can be HASH_SUCCESS if success or one of
 *         HASH_ERR_BAD_PARAMETER, HASH_ERR_BAD_CONTEXT,
 *         HASH_ERR_BAD_OPERATION if error occured.
 */
int32_t STM32_SHA256_HASH_DigestCompute(uint8_t *InputMessage, uint32_t InputMessageLength, uint8_t *MessageDigest, int32_t *MessageDigestLength) {
	SHA256ctx_stt P_pSHA256ctx;
	uint32_t error_status = HASH_SUCCESS;

	/* Set the size of the desired hash digest */
	P_pSHA256ctx.mTagSize = CRL_SHA256_SIZE;

	/* Set flag field to default value */
	P_pSHA256ctx.mFlags = E_HASH_DEFAULT;

	error_status = SHA256_Init(&P_pSHA256ctx);

	/* check for initialization errors */
	if (error_status == HASH_SUCCESS) {
		/* Add data to be hashed */
		error_status = SHA256_Append(&P_pSHA256ctx, InputMessage, InputMessageLength);

		if (error_status == HASH_SUCCESS) {
			/* retrieve */
			error_status = SHA256_Finish(&P_pSHA256ctx, MessageDigest, MessageDigestLength);
		}
	}

	return error_status;
}

int32_t FW_Verify(void) {
	FW_Meta_t *pFWMeta = (FW_Meta_t*) FW_META_DATA_ADD;
	uint32_t hash_status = 0;
	int32_t sig_status = -1;
	uint8_t MetaDigest[FW_HASH_LEN] = { 0 };
	int32_t MetaDigestLength = FW_HASH_LEN;

	/* enable CRC to allow cryptolib to work */
	__CRC_CLK_ENABLE();

	PRINTF("\r\nStart APP FW Verification...\r\n");
	PRINTF("FW Meta data @0x%08x:\r\n", FW_META_DATA_ADD);
	PRINTF("FW Magic: 0x%08x\r\n", pFWMeta->FWMagic);
	PRINTF("FW Size: 0x%08x\r\n", pFWMeta->FwSize);
	PRINTF("FW Version: 0x%08x\r\n", pFWMeta->FwVersion);
	print_buffer("FW HASH", pFWMeta->FwTag, FW_HASH_LEN);
	print_buffer("META DATA HASH", pFWMeta->MetaTag, FW_HASH_LEN);
	print_buffer("META DATA SIGNATURE", pFWMeta->MetaSig, FW_META_SIG_LEN);

	/* 1. Check the magic number of fw meta data */
	PRINTF("\r\n Check FW Magic\r\n");
	if (pFWMeta->FWMagic != FW_MAGIC) {
		Fatal_Error_Handler();
	}
	PRINTF(" FW Meta data Magic check OK!\r\n");

	/* 2. Verify fw meta data */
	PRINTF("\r\n Check FW Meta data\r\n");
	/* 2.1 Compute meta data hash */
	PRINTF("\r\n Check FW Meta data hash\r\n");
	hash_status = STM32_SHA256_HASH_DigestCompute((uint8_t*) pFWMeta, sizeof(FW_Meta_t) - FW_HASH_LEN - FW_META_SIG_LEN, &MetaDigest[0], &MetaDigestLength);
	if ((hash_status == HASH_SUCCESS) && (MetaDigestLength == FW_HASH_LEN)) {
		/* 2.2 Compare meta data hash with the stored hash */
		int i;
		print_buffer("Computed META DATA HASH", MetaDigest, FW_HASH_LEN);

		for (i = 0; i < FW_HASH_LEN; i++) {
			if (pFWMeta->MetaTag[i] != MetaDigest[i]) {
				Fatal_Error_Handler();
			}
		}
		PRINTF(" FW Meta data Hash check OK!\r\n");

		/* 2.3 Verify meta data signature*/
		print_buffer("META HASH", MetaDigest, MetaDigestLength);
		PRINTF("\r\n Check FW Meta data signature\r\n");
		sig_status = SignatureVerify(&pFWMeta->MetaSig[0], &pFWMeta->MetaTag[0], MetaDigestLength);
	} else {
		Fatal_Error_Handler();
	}

	/* 3. Compute fw hash and compare with fw tag in meta data */
	PRINTF("\r\n Check FW Hash\r\n");
	if (sig_status == SIGNATURE_VALID) {
		PRINTF(" FW Meta data Signature check OK!\r\n");

		/* 3.1 compute the fw hash */
		hash_status = STM32_SHA256_HASH_DigestCompute((uint8_t*) FW_ADD, pFWMeta->FwSize, &MetaDigest[0], &MetaDigestLength);
		if ((hash_status == HASH_SUCCESS) && (MetaDigestLength == FW_HASH_LEN)) {
			int i;
			print_buffer("FW HASH", MetaDigest, MetaDigestLength);
			/* 3.2 Compare fw hash with fw tag */
			for (i = 0; i < FW_HASH_LEN; i++) {
				if (pFWMeta->FwTag[i] != MetaDigest[i]) {
					Fatal_Error_Handler();
				}
			}
			PRINTF(" FW Hash check OK!\r\n");
		} else {
			Fatal_Error_Handler();
		}
	}

	PRINTF("\r\n\r\n");
	return sig_status;

	ERROR: sig_status = -1;
	Fatal_Error_Handler();

	return sig_status;
}

#endif /* FIRMWARE_AUTHENTIFICATION_H_ */
