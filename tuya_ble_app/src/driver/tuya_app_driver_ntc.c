/**
 * @file tuya_app_driver_ntc.c
 * @author lifan
 * @brief ntc(B3950/100K) driver source file
 * @version 1.0
 * @date 2021-06-29
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_app_driver_ntc.h"
#include "tuya_app_common.h"
#include "tuya_ble_log.h"
#include "gpio_8258.h"
#include "adc.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* Temperature-voltage array related */
#define TEMP_ARRAY_MIN_VALUE    0
#define TEMP_ARRAY_SIZE         120

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
/* Voltage value corresponding to NTC(B3950/100K) temperature */
const uint16_t vol_data_of_temp[TEMP_ARRAY_SIZE] = {
     190,  199,  209,  219,  229,  240,  251,  263,  275,  288,  301,  314,  328,  342,  357,  372,  388,  404,  420,  437, /* 0 ~ 19 */
     455,  473,  491,  510,  530,  549,  570,  591,  612,  634,  656,  679,  702,  725,  749,  774,  799,  824,  849,  875, /* 20 ~ 39 */
     902,  928,  955,  982, 1010, 1038, 1066, 1094, 1123, 1152, 1181, 1210, 1239, 1268, 1298, 1327, 1357, 1386, 1416, 1446, /* 40 ~ 59 */
    1475, 1505, 1535, 1564, 1593, 1623, 1652, 1681, 1710, 1738, 1767, 1795, 1823, 1851, 1878, 1906, 1933, 1959, 1986, 2012, /* 60 ~ 79 */
    2038, 2063, 2088, 2113, 2138, 2162, 2185, 2209, 2232, 2255, 2277, 2299, 2320, 2342, 2362, 2383, 2403, 2423, 2442, 2461, /* 80 ~ 99 */
    2480, 2498, 2516, 2534, 2551, 2568, 2584, 2600, 2616, 2632, 2647, 2662, 2676, 2690, 2704, 2718, 2731, 2744, 2757, 2769  /* 100 ~ 119 */
};

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief ntc adc init
 * @param[in] none
 * @return none
 */
void ntc_adc_init(void)
{
	adc_init();
	adc_base_init(P_NTC);
	adc_power_on_sar_adc(1);
}

/**
 * @brief is voltage value between data[num1] and data[num2]
 * @param[in] value: voltage value
 * @param[in] num1: smaller voltage value's number
 * @param[in] num2: larger voltage value's number
 * @return none
 */
static uint8_t is_vol_value_between(uint16_t value, uint8_t num1, uint8_t num2)
{
    if ((value >= vol_data_of_temp[num1]) && (value <= vol_data_of_temp[num2])) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief get closer number
 * @param[in] value: voltage value
 * @param[in] num1: smaller voltage value's number
 * @param[in] num2: larger voltage value's number
 * @return none
 */
static uint8_t get_closer_num(uint16_t value, uint8_t num1, uint8_t num2)
{
    if ((value - vol_data_of_temp[num1]) < (vol_data_of_temp[num2] - value)) {
        return num1;
    } else {
        return num2;
    }
}

/**
 * @brief transform voltage value to temperature value
 * @param[in] value: voltage value
 * @return temp: temprature value
 */
static uint8_t transform_vol_to_temp(uint16_t vol_value)
{
    uint8_t comp_num;
    uint8_t min = 0;
    uint8_t max = TEMP_ARRAY_SIZE - 1;
    uint8_t temp = 0;

    if (vol_value <= vol_data_of_temp[min]) {
        return TEMP_ARRAY_MIN_VALUE;
    }
    if (vol_value >= vol_data_of_temp[max]) {
        return (TEMP_ARRAY_MIN_VALUE + TEMP_ARRAY_SIZE - 1);
    }
    while (1) {
        comp_num = (max + min) / 2;
        if (vol_value == vol_data_of_temp[comp_num]) {
            temp = comp_num + TEMP_ARRAY_MIN_VALUE;
            break;
        } else if (vol_value < vol_data_of_temp[comp_num]) {
            if (is_vol_value_between(vol_value, comp_num-1, comp_num)) {
                temp = get_closer_num(vol_value, comp_num-1, comp_num) + TEMP_ARRAY_MIN_VALUE;
                break;
            } else {
                max = comp_num;
            }
        } else {
            if (is_vol_value_between(vol_value, comp_num, comp_num+1)) {
                temp = get_closer_num(vol_value, comp_num, comp_num+1) + TEMP_ARRAY_MIN_VALUE;
                break;
            } else {
                min = comp_num;
            }
        }
    }
    return temp;
}

/**
 * @brief get current temperature
 * @param[in] none
 * @return ntc_temp: temperature value
 */
uint8_t get_cur_temp(void)
{
    uint8_t ntc_temp;
    uint16_t ntc_vol_value;
    /* adc init */
    ntc_adc_init();
    /* get adc result (voltage value -mV) */
    ntc_vol_value = (uint16_t)adc_sample_and_get_result();
    TUYA_APP_LOG_DEBUG("voltage: %d", ntc_vol_value);
    /* get temp value */
    ntc_temp = transform_vol_to_temp(ntc_vol_value);
    TUYA_APP_LOG_DEBUG("temperature: %d", ntc_temp);
    return ntc_temp;
}
