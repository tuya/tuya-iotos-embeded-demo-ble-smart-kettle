/**
 * @file tuya_app_driver_led.h
 * @author lifan
 * @brief led driver header file
 * @version 1.0
 * @date 2021-06-28
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_APP_DRIVER_LED_H__
#define __TUYA_APP_DRIVER_LED_H__

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
/* LED mode */
typedef BYTE_T LED_MODE_E;
#define LED_MODE_FIX        0x00
#define LED_MODE_TWINKLE    0x01

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief led init
 * @param[in] none
 * @return none
 */
void led_init(void);

/**
 * @brief set red led on or off
 * @param[in] b_on_off: led on / led off
 * @return none
 */
void set_led_red(bool b_on_off);

/**
 * @brief set orange led on or off
 * @param[in] b_on_off: led on / led off
 * @return none
 */
void set_led_orange(bool b_on_off);

/**
 * @brief set green led on or off
 * @param[in] mode: led mode
 * @return none
 */
void set_led_green_mode(LED_MODE_E mode);

/**
 * @brief set green led status
 * @param[in] status: led status
 * @return none
 */
void set_led_green_status(uint8_t status);

/**
 * @brief update led green status
 * @param[in] none
 * @return none
 */
void update_led_green_status(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_APP_DRIVER_LED_H__ */
