/**
 * @file  firmware_authentification.h
 * @brief 
 * @date  Nov 16, 2021
 * @author David González León, Jade Gröli
 *
 */
#ifndef FIRMWARE_AUTHENTIFICATION_H_
#define FIRMWARE_AUTHENTIFICATION_H_

#include "main.h"

#define FW_HASH_LEN              32 /* SHA256*/
#define FW_META_SIG_LEN         64 /* ECDSA P256*/
#define FW_MAGIC    0x46574d41 //ascii de FWMA
#define FW_LIMIT 0x08020200
#define APP_META_DATA_ADD FW_LIMIT
#define FW_ADD (APP_META_DATA_ADD + 0x200)
#define FW_META_DATA_ADD (APP_META_DATA_ADD)

/*
 * FW meta data for verification
 * Totoal size 128 bytes, with 20 reserved bytes not used
 */
typedef struct {
	uint32_t FWMagic; /*!< FW Magic 'FWMA'*/
	uint32_t FwSize; /*!< Firmware size (bytes)*/
	uint32_t FwVersion; /*!< Firmware version*/
	uint8_t FwTag[FW_HASH_LEN]; /*!< Firmware Tag*/
	uint8_t Reserved[84]; /*!< Reserved for future use: 84 extra bytes to have a header size (to be signed) of 128 bytes */
	uint8_t MetaTag[FW_HASH_LEN]; /*!< Signature of the header message (before MetaTag)*/
	uint8_t MetaSig[FW_META_SIG_LEN]; /*!< Signature of the header message (before MetaTag)*/
} FW_Meta_t;

int32_t FW_Verify(void);

#endif /* FIRMWARE_AUTHENTIFICATION_H_ */
