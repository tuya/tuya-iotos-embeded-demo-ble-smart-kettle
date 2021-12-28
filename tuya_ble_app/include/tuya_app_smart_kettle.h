/**
 * @file tuya_app_smart_kettle.h
 * @author lifan
 * @brief smart kettle application header file
 * @version 1.0
 * @date 2021-06-28
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_APP_SMART_KETTLE_H__
#define __TUYA_APP_SMART_KETTLE_H__

#include "tuya_app_common.h"

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
 * @brief smart kettle init
 * @param[in] none
 * @return none
 */
void tuya_app_kettle_init(void);

/**
 * @brief main loop of smart kettle
 * @param[in] none
 * @return none
 */
void tuya_app_kettle_loop(void);

/**
 * @brief dp data handler of smart kettle
 * @param[in] dp_data: dp data array
 * @return none
 */
void tuya_app_kettle_dp_data_handler(uint8_t *dp_data);

/**
 * @brief ble connect status change handler of smart kettle
 * @param[in] status: ble connect status
 * @return none
 */
void tuya_app_kettle_ble_connect_status_change_handler(tuya_ble_connect_status_t status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_APP_SMART_KETTLE_H__ */
