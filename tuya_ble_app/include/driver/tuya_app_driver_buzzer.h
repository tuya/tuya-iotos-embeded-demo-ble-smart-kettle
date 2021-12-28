/**
 * @file tuya_app_driver_buzzer.h
 * @author lifan
 * @brief buzzer driver header file
 * @version 1.0
 * @date 2021-06-28
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_APP_DRIVER_BUZZER_H__
#define __TUYA_APP_DRIVER_BUZZER_H__

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
/* Buzzer mode */
typedef BYTE_T BUZZER_MODE_E;
#define BUZZER_MODE_STOP    0x00
#define BUZZER_MODE_ONCE    0x01
#define BUZZER_MODE_FAULT   0x02

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief buzzer init (PWM)
 * @param[in] none
 * @return none
 */
void buzzer_pwm_init(void);

/**
 * @brief set buzzer mode
 * @param[in] none
 * @return none
 */
void set_buzzer_mode(BUZZER_MODE_E mode);

/**
 * @brief update buzzer status
 * @param[in] none
 * @return none
 */
void update_buzzer_status(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_APP_DRIVER_BUZZER_H__ */
