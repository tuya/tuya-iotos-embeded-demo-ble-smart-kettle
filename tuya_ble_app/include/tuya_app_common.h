/**
 * @file tuya_app_common.h
 * @author lifan
 * @brief app common header file
 * @version 1.0
 * @date 2021-06-28
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_APP_COMMON_H__
#define __TUYA_APP_COMMON_H__

#include "tuya_ble_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/
/* STATUS */
#define OFF                 0
#define ON                  (!OFF)

/* FLAG */
#define CLR                 0
#define SET                 (!CLR)

/* PIN */
#define P_KEY_BOIL          GPIO_PC3    /* P7 */
#define P_KEY_KEEP          GPIO_PC2    /* P8 */
#define P_NTC               GPIO_PB6    /* ADC */
#define P_LED_RED           GPIO_PB5    /* P24 */
#define P_LED_ORANGE        GPIO_PB4    /* P26 */
#define P_LED_GREEN         GPIO_PD2    /* P6 */
#define P_RELAY             GPIO_PD3    /* P14 */
#define P_BUZZER            GPIO_PD4    /* P17 */

/***********************************************************
***********************typedef define***********************
***********************************************************/
/* TYPE */
typedef unsigned char BYTE_T;
typedef struct {
    uint8_t bit0 : 1;
    uint8_t bit1 : 1;
    uint8_t bit2 : 1;
    uint8_t bit3 : 1;
    uint8_t bit4 : 1;
    uint8_t bit5 : 1;
    uint8_t bit6 : 1;
} FLAG_BIT;

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_APP_COMMON_H__ */
