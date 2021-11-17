/**
 * @file  firmware_authentification.c
 * @brief 
 * @date  Nov 16, 2021
 * @author David González León, Jade Gröli
 *
 */

#include "main.h"
#include "bootloader.h"

#define FW_HASH_LEN              32 /* SHA256*/
#define FW_META_SIG_LEN         64 /* ECDSA P256*/
#define FW_MAGIC                'FWMA'
#define FW_META_DATA_ADD        (APP_META_DATA_ADD)
#define FW_ADD                  (APP_REGION_ROM_START)

/*
 * FW meta data for verification
 * Totoal size 128 bytes, with 20 reserved bytes not used
 */
typedef struct {
  uint32_t FWMagic;               /*!< FW Magic 'FWMA'*/
  uint32_t FwSize;                 /*!< Firmware size (bytes)*/
  uint32_t FwVersion;              /*!< Firmware version*/
  uint8_t  FwTag[FW_HASH_LEN];      /*!< Firmware Tag*/
  uint8_t  Reserved[84];          /*!< Reserved for future use: 84 extra bytes to have a header size (to be signed) of 128 bytes */
  uint8_t  MetaTag[FW_HASH_LEN];  /*!< Signature of the header message (before MetaTag)*/
  uint8_t  MetaSig[FW_META_SIG_LEN];  /*!< Signature of the header message (before MetaTag)*/
}FW_Meta_t;

int32_t FW_Verify(void);

