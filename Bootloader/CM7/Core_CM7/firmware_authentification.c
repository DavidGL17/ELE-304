/**
 * @file  firmware_authentification.c
 * @brief 
 * @date  Nov 16, 2021
 * @author David González León, Jade Gröli
 *
 */

#include "firmware_authentification.h"
#include "ecc/cmox_ecc.h"
#include "cmox_crypto.h"
#include "ecc/cmox_ecc_retvals.h"
#include "bootloader.h"
#include "ecc_pub_key.h"

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

int32_t FW_Verify(void) {
	FW_Meta_t *pFWMeta = (FW_Meta_t*) FW_META_DATA_ADD;
	uint32_t hash_status = 0;
	int32_t sig_status = -1;
	uint8_t MetaDigest[FW_HASH_LEN] = { 0 };
	size_t MetaDigestLength = FW_HASH_LEN;

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
	hash_status = cmox_hash_compute(CMOX_SHA256_ALGO, (uint8_t*) FW_META_DATA_ADD,
			sizeof(FW_Meta_t) - FW_HASH_LEN - FW_META_SIG_LEN, MetaDigest, CMOX_SHA256_SIZE, &MetaDigestLength);
	if ((hash_status == CMOX_HASH_SUCCESS) && (MetaDigestLength == FW_HASH_LEN)) {
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
		cmox_ecc_handle_t Ecc_Ctx;
		uint8_t Working_buffer[2000];
		uint32_t fault_check;
		cmox_ecc_construct(&Ecc_Ctx, CMOX_ECC256_MATH_FUNCS, Working_buffer, sizeof(Working_buffer));
		sig_status = cmox_ecdsa_verify(&Ecc_Ctx, CMOX_ECC_SECP256K1_LOWMEM, SIGN_ECC_PUB_KEY, sizeof(SIGN_ECC_PUB_KEY),
				pFWMeta->MetaTag, 32, pFWMeta->MetaSig, sizeof(pFWMeta->MetaSig), &fault_check);
	} else {
		Fatal_Error_Handler();
	}

	/* 3. Compute fw hash and compare with fw tag in meta data */
	PRINTF("\r\n Check FW Hash\r\n");
	if (sig_status == CMOX_HASH_SUCCESS) {
		PRINTF(" FW Meta data Signature check OK!\r\n");

		/* 3.1 compute the fw hash */
		hash_status = cmox_hash_compute(CMOX_SHA256_ALGO, (uint8_t*) FW_ADD, pFWMeta->FwSize, MetaDigest,
		CMOX_SHA256_SIZE, &MetaDigestLength);
		//old version : hash_status = STM32_SHA256_HASH_DigestCompute((uint8_t*) FW_ADD, pFWMeta->FwSize, &MetaDigest[0], &MetaDigestLength);
		if ((hash_status == CMOX_HASH_SUCCESS) && (MetaDigestLength == FW_HASH_LEN)) {
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
}
