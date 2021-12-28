/**
 * @file tuya_app_driver_led.c
 * @author lifan
 * @brief led driver source file
 * @version 1.0
 * @date 2021-06-28
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_app_driver_led.h"
#include "tuya_ble_log.h"
#include "gpio_8258.h"
#include "timer.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* LED time */
#define LED_TWINKLE_TIME  200  /* 0.2s */

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
/* LED green */
LED_MODE_E g_led_green_mode = LED_MODE_FIX;
uint8_t g_led_green_status = OFF;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief led init
 * @param[in] none
 * @return none
 */
void led_init(void)
{
	gpio_set_func(P_LED_RED, AS_GPIO);
    gpio_set_output_en(P_LED_RED, 1);
    gpio_write(P_LED_RED, 1);
    gpio_set_func(P_LED_ORANGE, AS_GPIO);
    gpio_set_output_en(P_LED_ORANGE, 1);
    gpio_write(P_LED_ORANGE, 1);
    gpio_set_output_en(P_LED_GREEN, 1);
	gpio_set_func(P_LED_GREEN, AS_GPIO);
    gpio_write(P_LED_GREEN, 1);
}

/**
 * @brief set red led on or off
 * @param[in] b_on_off: led on / led off
 * @return none
 */
void set_led_red(bool b_on_off)
{
    static bool s_last_status = 0;
    if (s_last_status != b_on_off) {
        if (b_on_off == ON) {
        	gpio_write(P_LED_RED, 0);
        } else {
        	gpio_write(P_LED_RED, 1);
        }
        s_last_status = b_on_off;
    }
}

/**
 * @brief set orange led on or off
 * @param[in] b_on_off: led on / led off
 * @return none
 */
void set_led_orange(bool b_on_off)
{
    static bool s_last_status = 0;
    if (s_last_status != b_on_off) {
        if (b_on_off == ON) {
        	gpio_write(P_LED_ORANGE, 0);
        } else {
        	gpio_write(P_LED_ORANGE, 1);
        }
        s_last_status = b_on_off;
    }
}

/**
 * @brief set green led on or off
 * @param[in] b_on_off: led on / led off
 * @return none
 */
static void set_led_green(bool b_on_off)
{
    static bool s_last_status = 0;

    if (s_last_status != b_on_off) {
        if (b_on_off == ON) {
        	gpio_write(P_LED_GREEN, 0);
        } else {
        	gpio_write(P_LED_GREEN, 1);
        }
        s_last_status = b_on_off;
    }
}

/**
 * @brief set green led on or off
 * @param[in] mode: led mode
 * @return none
 */
void set_led_green_mode(LED_MODE_E mode)
{
    g_led_green_mode = mode;
}

/**
 * @brief set green led status
 * @param[in] status: led status
 * @return none
 */
void set_led_green_status(uint8_t status)
{
    g_led_green_status = status;
}

/**
 * @brief update led green status
 * @param[in] none
 * @return none
 */
void update_led_green_status(void)
{
    static bool s_status = 0;
    static uint32_t s_twinkle_tm = 0;

    switch (g_led_green_mode) {
    case LED_MODE_FIX:
        set_led_green(g_led_green_status);
        s_twinkle_tm = 0;
        break;
    case LED_MODE_TWINKLE:
        if (!clock_time_exceed(s_twinkle_tm, LED_TWINKLE_TIME*1000)) {
            break;
        }
        s_twinkle_tm = clock_time();
        s_status = !s_status;
        set_led_green(s_status);
        break;
    default:
        break;
    }
}
