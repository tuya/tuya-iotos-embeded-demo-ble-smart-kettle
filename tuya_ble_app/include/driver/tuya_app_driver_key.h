/**
 * @file tuya_app_driver_key.h
 * @author lifan
 * @brief key driver header file
 * @version 1.0
 * @date 2021-06-28
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_APP_DRIVER_KEY_H__
#define __TUYA_APP_DRIVER_KEY_H__

#include "tuya_ble_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/
/* Key code */
#define KEY1_CODE               0x01
#define KEY2_CODE               0x02
/* Key press time */
#define KEY_PRESS_SHORT_TIME    50
/* Key init result */
#define KEY_INIT_OK             0
#define KEY_INIT_ERR            (!KEY_INIT_OK)

/***********************************************************
***********************typedef define***********************
***********************************************************/
/* Key define */
typedef void(* KEY_CALLBACK)();
typedef struct {                        /* user define */
    uint16_t key1_pin;                  /* ts02n CH1 pin */
    uint16_t key2_pin;                  /* ts02n CH2 pin */
    KEY_CALLBACK key1_short_press_cb;   /* key1 short press callback function */
    KEY_CALLBACK key2_short_press_cb;   /* key2 short press callback function */
    KEY_CALLBACK key1_long_press_cb;    /* key1 long press callback function */
    KEY_CALLBACK key2_long_press_cb;    /* key2 long press callback function */
    uint32_t key1_long_press_time;      /* key1 long press time set (ms) */
    uint32_t key2_long_press_time;      /* key2 long press time set (ms) */
    uint32_t scan_time;                 /* Scan cycle set (ms) */
} TS02N_KEY_DEF_T;

/***********************************************************
***********************variable define**********************
***********************************************************/


/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief key driver init
 * @param[in] key_def: user key define
 * @return none
 */
uint8_t ts02n_key_init(TS02N_KEY_DEF_T* user_key_def);

/**
 * @brief ts02n key loop
 * @param[in] none
 * @return none
 */
void ts02n_key_loop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_APP_DRIVER_KEY_H__ */
