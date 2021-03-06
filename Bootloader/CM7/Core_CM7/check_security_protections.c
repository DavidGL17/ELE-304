/**
 * @file  check_security_protections.c
 * @brief Implementation of the check_security_protections.h functions
 * @date  16 nov. 2021
 * @author David González León, Jade Gröli
 *
 */

#include "check_security_protections.h"
#include "bootloader.h"
#include "main.h"

static void Fatal_Error_Handler(void) {
	/* Lock the Options Bytes ***************************************************/
	HAL_FLASH_OB_Lock();

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	PRINTF("\r\nFatal error! Enter endless loop!\r\n");
	while (1) {
	};
}

void CheckApplyStaticProtections(void) {
	FLASH_OBProgramInitTypeDef flash_option_bytes;
	int32_t isOBChangeToApply = 0;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Clear OPTVERR bit set on virgin samples */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPERR);

	/* Unlock the Options Bytes *************************************************/
	HAL_FLASH_OB_Unlock();

	/* Get Option Bytes status for WRP AREA_A  **********/
	flash_option_bytes.WRPSector = OB_WRP_SECTOR_ALL;
	HAL_FLASHEx_OBGetConfig(&flash_option_bytes);

	/* Check/Apply RDP_Level 1. This is the minimum protection allowed */
	/* if RDP_Level 2 is already applied it's not possible to modify the OptionBytes anymore */
	if (flash_option_bytes.RDPLevel == OB_RDP_LEVEL_2) {
#ifdef WRP_PROTECT_ENABLE
      if ((flash_option_bytes.WRPStartOffset > WRP_START_ADD) || (flash_option_bytes.WRPEndOffset < WRP_END_ADD)) {
         Fatal_Error_Handler();
      }
#endif /* WRP_PROTECT_ENABLE */
	} else {
		/* Check/Set Flash configuration *******************************************/

#ifdef WRP_PROTECT_ENABLE
      /* Apply WRP setting only if expected settings are not included */
      if ((flash_option_bytes.WRPStartOffset > WRP_START_ADD) || (flash_option_bytes.WRPEndOffset < WRP_END_ADD)) {
         flash_option_bytes.WRPStartOffset = WRP_START_ADD;
         flash_option_bytes.WRPEndOffset = WRP_END_ADD;

         PRINTF("\r\nTry to apply WRP from page [%d] to [%d]\r\n", flash_option_bytes.WRPStartOffset, flash_option_bytes.WRPEndOffset);
         if (HAL_FLASHEx_OBProgram(&flash_option_bytes) != HAL_OK) {
            Fatal_Error_Handler();
         }

         isOBChangeToApply++;
      }
      PRINTF("\r\nWRP already applied from page [%d] to [%d]\r\n", flash_option_bytes.WRPStartOffset, flash_option_bytes.WRPEndOffset);
#endif /* WRP_PROTECT_ENABLE */

#ifdef SECURE_USER_PROTECT_ENABLE
      if (flash_option_bytes.SecSize != BOOT_NB_PAGE_SEC_USER_MEM) {
         /*
            We will not set the secure user memory in code,
            secure user memory area will be set in CubeProgrammer
         */
         PRINTF("\r\nSecure User Memory Area size [0x%x bytes]\r\n", flash_option_bytes.SecSize * FLASH_PAGE_SIZE);
         PRINTF("\r\nExpected setting is [0x%x bytes]\r\n", BOOT_NB_PAGE_SEC_USER_MEM * FLASH_PAGE_SIZE);
      } else {
         PRINTF("\r\nSecure User Memory Area size [0x%x bytes]\r\n", flash_option_bytes.SecSize * FLASH_PAGE_SIZE);
      }

#endif

#ifdef RDP_PROTECT_ENABLE
      /* Apply RDP setting only if expected settings are not applied */
      if (flash_option_bytes.RDPLevel != RDP_LEVEL_CONFIG) {
         flash_option_bytes.OptionType = OPTIONBYTE_RDP | OPTIONBYTE_USER;
         flash_option_bytes.RDPLevel = RDP_LEVEL_CONFIG;
         if (HAL_FLASHEx_OBProgram(&flash_option_bytes) != HAL_OK) {
            Fatal_Error_Handler();
         }

         PRINTF("\r\nSet RDP to [0x%02x], please power off and power on again.\r\n", flash_option_bytes.RDPLevel);
         isOBChangeToApply++;
      } else {
         PRINTF("\r\nRDP level set to [0x%02x]\r\n", flash_option_bytes.RDPLevel);
      }
#endif /* RDP_PROTECT_ENABLE */

		/* Generate System Reset to reload the new option byte values *************/
		/* WARNING: This means that if a protection can't be set, there will be a reset loop! */
		if (isOBChangeToApply > 0) {
			HAL_FLASH_OB_Launch();
		}
	}

	/* Lock the Options Bytes ***************************************************/
	HAL_FLASH_OB_Lock();

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	return;
}
