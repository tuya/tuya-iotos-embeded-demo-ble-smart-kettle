/**
 * @file tuya_app_driver_relay.c
 * @author lifan
 * @brief ralay driver source file
 * @version 1.0
 * @date 2021-06-29
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_app_driver_relay.h"
#include "tuya_app_common.h"
#include "gpio_8258.h"

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
void relay_init(void)
{
	gpio_set_func(P_RELAY, AS_GPIO);
	gpio_set_output_en(P_RELAY, 1);
	gpio_write(P_RELAY, 0);
}

/**
 * @brief set relay on or off
 * @param[in] b_on_off: relay on / relay off
 * @return none
 */
void set_relay(bool b_on_off)
{
    static bool s_last_status = 0;
    if (s_last_status != b_on_off) {
        if (b_on_off == ON) {
        	gpio_write(P_RELAY, 1);
        } else {
        	gpio_write(P_RELAY, 0);
        }
        s_last_status = b_on_off;
    }
}
