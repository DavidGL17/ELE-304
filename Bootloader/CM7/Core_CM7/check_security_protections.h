/**
 * @file  check_security_protections.h
 * @brief Defines functions that help control and enable the static protections of the device
 * @date  16 nov. 2021
 * @author David González León, Jade Gröli
 *
 */
#ifndef CHECK_SECURITY_PROTECTIONS_H_
#define CHECK_SECURITY_PROTECTIONS_H_

/*Uncomment any of these 2 defines to enable write protection or read protection*/
//#define WRP_PROTECT_ENABLE
//#define RDP_PROTECT_ENABLE
/**
 * @brief  Check and if not applied apply the Static security  protections to
 *         critical sections in Flash: RDP, WRP. Static security protections
 *         those protections not impacted by a Reset. They are set using the Option Bytes
 *         When the device is locked (RDP Level2), these protections cannot be changed anymore
 * @param  None
 * @note   If security setting apply fails, enter Error Handler
 */
void CheckApplyStaticProtections(void);

#endif /* CHECK_SECURITY_PROTECTIONS_H_ */
