/**
 * @file tuya_app_driver_relay.h
 * @author lifan
 * @brief relay driver header file
 * @version 1.0
 * @date 2021-06-28
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_APP_DRIVER_RELAY_H__
#define __TUYA_APP_DRIVER_RELAY_H__

#include "tuya_ble_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief relay init
 * @param[in] none
 * @return none
 */
void relay_init(void);

/**
 * @brief set relay on or off
 * @param[in] b_on_off: relay on / relay off
 * @return none
 */
void set_relay(bool b_on_off);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_APP_DRIVER_RELAY_H__ */
