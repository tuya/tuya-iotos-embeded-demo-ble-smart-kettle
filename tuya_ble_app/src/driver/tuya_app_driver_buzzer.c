/**
 * @file tuya_app_driver_buzzer.c
 * @author lifan
 * @brief buzzer driver source file
 * @version 1.0
 * @date 2021-06-29
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_app_driver_buzzer.h"
#include "tuya_ble_log.h"
#include "app_config.h"
#include "gpio_8258.h"
#include "pwm.h"
#include "timer.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* PWM ID */
#define PWM_ID_BUZZER       PWM2_ID     /* PWM2 */
/* Buzzer timer */
#define BUZZER_ONCE_TIME    70          /* 70ms */

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
static uint8_t sg_buzzer_timer = OFF;   /* timer on/off */
static uint32_t sg_buzzer_tm = 0;       /* timer counter */

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief buzzer init (PWM)
 * @param[in] none
 * @return none
 */
void buzzer_pwm_init(void)
{
    pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);
	gpio_set_func(P_BUZZER, AS_PWM2_N);
    pwm_set_mode(PWM_ID_BUZZER, PWM_NORMAL_MODE);
    pwm_set_cycle_and_duty(PWM_ID_BUZZER, (uint16_t)(500 * CLOCK_SYS_CLOCK_1US), (uint16_t)(250 * CLOCK_SYS_CLOCK_1US));    /* 500us 2KHz 50% */
    gpio_write(P_BUZZER, 0);
}

/**
 * @brief set buzzer on or off
 * @param[in] b_on_off: buzzer on / buzzer off
 * @return none
 */
static void set_buzzer(bool b_on_off)
{
    static bool s_last_status = 0;
    if (s_last_status != b_on_off) {
        if (b_on_off == ON) {
        	pwm_start(PWM_ID_BUZZER);
        } else {
        	pwm_stop(PWM_ID_BUZZER);
            gpio_write(P_BUZZER, 0);
        }
        s_last_status = b_on_off;
    }
}

/**
 * @brief set buzzer mode
 * @param[in] none
 * @return none
 */
void set_buzzer_mode(BUZZER_MODE_E mode)
{
    switch (mode) {
    case BUZZER_MODE_STOP:
        set_buzzer(OFF);
        break;
    case BUZZER_MODE_ONCE:
        set_buzzer(ON);
        sg_buzzer_timer = ON;
        sg_buzzer_tm = clock_time();
        break;
    case BUZZER_MODE_FAULT:
        set_buzzer(ON);
        break;
    default:
        break;
    }
}

/**
 * @brief update buzzer status
 * @param[in] none
 * @return none
 */
void update_buzzer_status(void)
{
    if (sg_buzzer_timer == ON) {
        if (!clock_time_exceed(sg_buzzer_tm, BUZZER_ONCE_TIME*1000)) {
            return;
        }
        set_buzzer(OFF);
        sg_buzzer_timer = OFF;
    }
}
