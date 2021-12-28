/**
 * @file tuya_app_driver_key.c
 * @author lifan
 * @brief key driver source file
 * @version 1.0
 * @date 2021-06-29
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_app_driver_key.h"
#include "tuya_ble_log.h"
#include "tuya_ble_mem.h"
#include "gpio_8258.h"
#include "timer.h"

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
/* Key status */
typedef struct {
    uint8_t cur_code;
    uint8_t prv_code;
    uint32_t cur_time;
    uint32_t prv_time;
} TS02N_KEY_STATUS_T;

/* Key manage */
typedef struct {
    TS02N_KEY_DEF_T* ts02n_key_def_s;
    TS02N_KEY_STATUS_T ts02n_key_status_s;
} TS02N_KEY_MANAGE_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
static TS02N_KEY_MANAGE_T *sg_key_mag = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief key driver init
 * @param[in] key_def: user key define
 * @return none
 */
uint8_t ts02n_key_init(TS02N_KEY_DEF_T* key_def)
{
    /* sg_key_mag init */
    sg_key_mag = (TS02N_KEY_MANAGE_T *)tuya_ble_malloc(sizeof(TS02N_KEY_MANAGE_T));
    memset(sg_key_mag, 0, sizeof(TS02N_KEY_MANAGE_T));
    /* get user key define */
    sg_key_mag->ts02n_key_def_s = key_def;
    /* callback function check */
    if ((key_def->key1_short_press_cb == NULL) && (key_def->key2_short_press_cb == NULL) &&
        (key_def->key1_long_press_cb == NULL)  && (key_def->key2_long_press_cb == NULL)) {
        tuya_ble_free((uint8_t *)sg_key_mag);
        return KEY_INIT_ERR;
    }

    /* gpio init */
    gpio_set_func(key_def->key1_pin, AS_GPIO);
    gpio_set_input_en(key_def->key1_pin, 1);
    gpio_setup_up_down_resistor(key_def->key1_pin, PM_PIN_PULLUP_10K);
    gpio_set_func(key_def->key2_pin, AS_GPIO);
    gpio_set_input_en(key_def->key2_pin, 1);
    gpio_setup_up_down_resistor(key_def->key2_pin, PM_PIN_PULLUP_10K);

    return KEY_INIT_OK;
}

/**
 * @brief get key code
 * @param[in] none
 * @return key code
 */
static uint8_t get_key_code(void)
{
    uint8_t key_code = 0;
    /* KEY BOIL */
    if (gpio_read(sg_key_mag->ts02n_key_def_s->key1_pin) == 0) {
        key_code |= KEY1_CODE;
    } else {
        key_code &= ~KEY1_CODE;
    }
    /* KEY KEEP */
    if (gpio_read(sg_key_mag->ts02n_key_def_s->key2_pin) == 0) {
        key_code |= KEY2_CODE;
    } else {
        key_code &= ~KEY2_CODE;
    }
    return key_code;
}

/**
 * @brief key driver
 * @param[in] time_inc: time increment
 * @return none
 */
static void update_key_status(uint32_t time_inc)
{
    uint8_t key_code;

    key_code = get_key_code();
    sg_key_mag->ts02n_key_status_s.prv_time = sg_key_mag->ts02n_key_status_s.cur_time;
    sg_key_mag->ts02n_key_status_s.cur_time += time_inc;

    if (key_code != sg_key_mag->ts02n_key_status_s.cur_code) {
        sg_key_mag->ts02n_key_status_s.prv_code = sg_key_mag->ts02n_key_status_s.cur_code;
        sg_key_mag->ts02n_key_status_s.cur_code = key_code;
        sg_key_mag->ts02n_key_status_s.prv_time = sg_key_mag->ts02n_key_status_s.cur_time;
        sg_key_mag->ts02n_key_status_s.cur_time = 0;
    } else {
        sg_key_mag->ts02n_key_status_s.prv_code = sg_key_mag->ts02n_key_status_s.cur_code;
    }
}

/**
 * @brief is key press over time
 * @param[in] key_code: key code used for judgment
 * @param[in] press_time: press time used for judgment
 * @return 0-false 1-true
 */
static uint8_t is_key_press_over_time(uint8_t key_code, uint32_t press_time)
{
    if (sg_key_mag->ts02n_key_status_s.cur_code == key_code) {
        if ((sg_key_mag->ts02n_key_status_s.cur_time >= press_time) &&
            (sg_key_mag->ts02n_key_status_s.prv_time < press_time)) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief is key from release to release less time
 * @param[in] key_code: key code used for judgment
 * @param[in] press_time: press time used for judgment
 * @return 0-false 1-true
 */
static uint8_t is_key_release_to_release_less_time(uint8_t key_code, uint32_t press_time)
{
    if ((sg_key_mag->ts02n_key_status_s.prv_code == key_code) &&
        (sg_key_mag->ts02n_key_status_s.cur_code != key_code)) {
        if ((sg_key_mag->ts02n_key_status_s.prv_time >= KEY_PRESS_SHORT_TIME) &&
            (sg_key_mag->ts02n_key_status_s.prv_time < press_time)) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief detect and handle key event
 * @param[in] none
 * @return none
 */
static void detect_and_handle_key_event(void)
{
    /* key1 handler */
    if (sg_key_mag->ts02n_key_def_s->key1_long_press_cb != NULL) {
        /* long press */
        if (is_key_press_over_time(KEY1_CODE, sg_key_mag->ts02n_key_def_s->key1_long_press_time)) {
            sg_key_mag->ts02n_key_def_s->key1_long_press_cb();
            TUYA_APP_LOG_DEBUG("key1 is long pressed");
        }
        /* short press */
        if (sg_key_mag->ts02n_key_def_s->key1_short_press_cb != NULL) {
            if (is_key_release_to_release_less_time(KEY1_CODE, sg_key_mag->ts02n_key_def_s->key1_long_press_time)) {
                sg_key_mag->ts02n_key_def_s->key1_short_press_cb();
                TUYA_APP_LOG_DEBUG("key1 is pressed");
            }
        }
    } else {
        /* short press */
        if (sg_key_mag->ts02n_key_def_s->key1_short_press_cb != NULL) {
            if (is_key_press_over_time(KEY1_CODE, KEY_PRESS_SHORT_TIME)) {
                sg_key_mag->ts02n_key_def_s->key1_short_press_cb();
                TUYA_APP_LOG_DEBUG("key1 is pressed");
            }
        }
    }
    /* key2 handler */
    if (sg_key_mag->ts02n_key_def_s->key2_long_press_cb != NULL) {
        /* long press */
        if (is_key_press_over_time(KEY2_CODE, sg_key_mag->ts02n_key_def_s->key2_long_press_time)) {
            sg_key_mag->ts02n_key_def_s->key2_long_press_cb();
            TUYA_APP_LOG_DEBUG("key2 is long pressed");
        }
        /* short press */
        if (sg_key_mag->ts02n_key_def_s->key2_short_press_cb != NULL) {
            if (is_key_release_to_release_less_time(KEY2_CODE, sg_key_mag->ts02n_key_def_s->key2_long_press_time)) {
                sg_key_mag->ts02n_key_def_s->key2_short_press_cb();
                TUYA_APP_LOG_DEBUG("key2 is pressed");
            }
        }
    } else {
        /* short press */
        if (sg_key_mag->ts02n_key_def_s->key2_short_press_cb != NULL) {
            if (is_key_press_over_time(KEY2_CODE, KEY_PRESS_SHORT_TIME)) {
                sg_key_mag->ts02n_key_def_s->key2_short_press_cb();
                TUYA_APP_LOG_DEBUG("key2 is pressed");
            }
        }
    }
}

/**
 * @brief ts02n key loop
 * @param[in] none
 * @return none
 */
void ts02n_key_loop(void)
{
    static uint32_t s_key_scan_tm = 0;

	if (!clock_time_exceed(s_key_scan_tm, (sg_key_mag->ts02n_key_def_s->scan_time)*1000)) {
		return;
	}
	s_key_scan_tm = clock_time();
    update_key_status(sg_key_mag->ts02n_key_def_s->scan_time);
    detect_and_handle_key_event();
}
