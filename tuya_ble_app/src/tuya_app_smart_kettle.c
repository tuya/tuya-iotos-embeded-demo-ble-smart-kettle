/**
 * @file tuya_app_smart_kettle.c
 * @author lifan
 * @brief smart kettle application source file
 * @version 1.0
 * @date 2021-07-06
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_app_smart_kettle.h"
#include "tuya_app_driver_led.h"
#include "tuya_app_driver_key.h"
#include "tuya_app_driver_ntc.h"
#include "tuya_app_driver_relay.h"
#include "tuya_app_driver_buzzer.h"
#include "tuya_ble_common.h"
#include "tuya_ble_log.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* DP ID */
#define DP_ID_BOIL              101
#define DP_ID_KEEP_WARM         102
#define DP_ID_TEMP_CUR          103
#define DP_ID_TEMP_SET          104
#define DP_ID_WATER_TYPE        105
#define DP_ID_FAULT             106
/* DP TYPE */
#define DP_TYPE_BOIL            DT_BOOL
#define DP_TYPE_KEEP_WARM       DT_BOOL
#define DP_TYPE_TEMP_CUR        DT_VALUE
#define DP_TYPE_TEMP_SET        DT_VALUE
#define DP_TYPE_WATER_TYPE      DT_ENUM
#define DP_TYPE_FAULT           DT_ENUM

/* Temperature */
#define TEMP_BOILED             97
#define TEMP_KEEP_WARM_DEFAULT  55
#define TEMP_UPPER_LIMIT        105
#define TEMP_KEEP_RANGE         3
/* Time */
#define TIME_GET_TEMP           2000        /* 2s */
#define TIME_ALLOW_CONNECT      (3*60*1000) /* 3min */

/***********************************************************
***********************typedef define***********************
***********************************************************/
/* Work mode */
typedef BYTE_T MODE_E;
#define MODE_NATURE             0x00
#define MODE_BOIL               0x01
#define MODE_KEEP_WARM1         0x02
#define MODE_KEEP_WARM2         0x03
/* Water type */
typedef BYTE_T WATER_TYPE_E;
#define WATER_TYPE_TAP          0x00
#define WATER_TYPE_PURE         0x01
/* Fault */
typedef BYTE_T FAULT_E;
#define FAULT_NORMAL            0x00
#define FAULT_LACK_WATER        0x01

/* DP data struct */
typedef struct {
    uint8_t id;
    dp_type type;
    uint8_t len;
    uint8_t value;
} DP_DATA_T;

/* Kettle struct */
typedef struct {
    MODE_E mode;
    uint8_t boil_turn;
    uint8_t temp_cur;
    uint8_t temp_set;
    uint8_t keep_warm_turn;
    WATER_TYPE_E water_type;
    FAULT_E fault;
} KETTLE_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
/* Key callback function */
void key_boil_short_press_cb_fun();
void key_keep_short_press_cb_fun();
void key_keep_long_press_cb_fun();

/* User key define */
TS02N_KEY_DEF_T user_ts02n_key_def_s = {
    .key1_pin = P_KEY_BOIL,         /* P7 */
    .key2_pin = P_KEY_KEEP,         /* P8 */
    .key1_short_press_cb = key_boil_short_press_cb_fun,
    .key2_short_press_cb = key_keep_short_press_cb_fun,
    .key1_long_press_cb = NULL,
    .key2_long_press_cb = key_keep_long_press_cb_fun,
    .key1_long_press_time = 0,      /* none */
    .key2_long_press_time = 5000,   /* 5s */
    .scan_time = 10,                /* 10ms */
};

KETTLE_T g_kettle;

/* BLE connect wait timer */
static uint32_t sg_ble_tm = 0;

/* Kettle flag */
FLAG_BIT g_kettle_flag;
    #define F_BLE_BONDING       g_kettle_flag.bit0
    #define F_WAIT_BLE_CONN     g_kettle_flag.bit1

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief get dp type
 * @param[in] dp_id: DP ID
 * @return type: DP type
 */
static uint8_t get_dp_type(uint8_t dp_id)
{
    dp_type type = 0;
    switch (dp_id) {
    case DP_ID_BOIL:
        type = DP_TYPE_BOIL;
        break;
    case DP_ID_TEMP_CUR:
        type = DP_TYPE_TEMP_CUR;
        break;
    case DP_ID_TEMP_SET:
        type = DP_TYPE_TEMP_SET;
        break;
    case DP_ID_KEEP_WARM:
        type = DP_TYPE_KEEP_WARM;
        break;
    case DP_ID_WATER_TYPE:
        type = DP_TYPE_WATER_TYPE;
        break;
    case DP_ID_FAULT:
        type = DP_TYPE_FAULT;
        break;
    default:
        break;
    }
    return type;
}

/**
 * @brief report one dp data
 * @param[in] dp_id: DP ID
 * @param[in] dp_value: DP value
 * @return none
 */
static void report_one_dp_data(uint8_t dp_id, uint8_t dp_value)
{
    DP_DATA_T dp_data_s;
    dp_data_s.id = dp_id;
    dp_data_s.type = get_dp_type(dp_id);
    dp_data_s.len = 0x01;
    dp_data_s.value = dp_value;
    tuya_ble_dp_data_report((uint8_t *)&dp_data_s, sizeof(DP_DATA_T));
}

/**
 * @brief report all dp data
 * @param[in] none
 * @return none
 */
static void report_all_dp_data(void)
{
    report_one_dp_data(DP_ID_BOIL, g_kettle.boil_turn);
    report_one_dp_data(DP_ID_KEEP_WARM, g_kettle.keep_warm_turn);
    report_one_dp_data(DP_ID_TEMP_CUR, g_kettle.temp_cur);
    report_one_dp_data(DP_ID_TEMP_SET, g_kettle.temp_set);
    report_one_dp_data(DP_ID_WATER_TYPE, g_kettle.water_type);
    report_one_dp_data(DP_ID_FAULT, g_kettle.fault);
}

/**
 * @brief set smart kettle work mode
 * @param[in] mode: 0-stop, 1-once
 * @return none
 */
static void set_work_mode(uint8_t mode)
{
    if (mode != g_kettle.mode) {
        g_kettle.mode = mode;
        TUYA_APP_LOG_DEBUG("mode: %d", g_kettle.mode);
    }
}

/**
 * @brief set boil turn on or off
 * @param[in] on_off: 0-off, 1-on
 * @return none
 */
static void set_boil_turn(uint8_t on_off)
{
    if (g_kettle.fault != FAULT_NORMAL) {
        return;
    }
    g_kettle.boil_turn = on_off;
    report_one_dp_data(DP_ID_BOIL, g_kettle.boil_turn);
    TUYA_APP_LOG_DEBUG("boil turn: %d", g_kettle.boil_turn);
    set_led_red(on_off);
}

/**
 * @brief switch boil turn
 * @param[in] none
 * @return none
 */
static void switch_boil_turn(void)
{
    if (g_kettle.boil_turn == ON) {
        set_boil_turn(OFF);
    } else {
        set_boil_turn(ON);
    }
}

/**
 * @brief set keep warm turn on or off
 * @param[in] on_off: 0-off, 1-on
 * @return none
 */
static void set_keep_warm_turn(uint8_t on_off)
{
    if (g_kettle.fault != FAULT_NORMAL) {
        return;
    }
    g_kettle.keep_warm_turn = on_off;
    report_one_dp_data(DP_ID_KEEP_WARM, g_kettle.keep_warm_turn);
    TUYA_APP_LOG_DEBUG("keep warm turn: %d", g_kettle.keep_warm_turn);
    set_led_orange(on_off);
}

/**
 * @brief switch keep warm turn
 * @param[in] none
 * @return none
 */
static void switch_keep_warm_turn(void)
{
    if (g_kettle.keep_warm_turn == ON) {
        set_keep_warm_turn(OFF);
    } else {
        set_keep_warm_turn(ON);
    }
}

/**
 * @brief set keep warm temperature
 * @param[in] temp: temperature set
 * @return none
 */
static void set_keep_warm_temp(uint8_t temp)
{
    if (g_kettle.temp_set != temp) {
        if ((temp >= 45) && (temp <= 90)) {
            g_kettle.temp_set = temp;
        }
    }
}

/**
 * @brief set water type
 * @param[in] type: water type
 * @return none
 */
static void set_water_type(WATER_TYPE_E type)
{
    g_kettle.water_type = type;
}

/**
 * @brief update fault
 * @param[in] fault: fault status
 * @return none
 */
static void update_fault(FAULT_E fault)
{
    g_kettle.fault = fault;
    report_one_dp_data(DP_ID_FAULT, g_kettle.fault);
    TUYA_APP_LOG_DEBUG("fault: %d", g_kettle.fault);
}

/**
 * @brief update ble status and data
 * @param[in] none
 * @return none
 */
static void ble_connect_status_init(void)
{
    tuya_ble_connect_status_t ble_conn_sta;

    ble_conn_sta = tuya_ble_connect_status_get();
    TUYA_APP_LOG_DEBUG("ble connect status: %d", ble_conn_sta);

    if ((ble_conn_sta == BONDING_UNCONN) ||
        (ble_conn_sta == BONDING_CONN)   ||
        (ble_conn_sta == BONDING_UNAUTH_CONN)) {
        F_BLE_BONDING = SET;
        F_WAIT_BLE_CONN = CLR;
        set_led_green_mode(LED_MODE_FIX);
    } else {
        F_BLE_BONDING = CLR;
        F_WAIT_BLE_CONN = SET;
        set_led_green_mode(LED_MODE_TWINKLE);
    }
}

/**
 * @brief try to connect ble
 * @param[in] none
 * @return none
 */
static void try_to_connect_ble(void)
{
    F_WAIT_BLE_CONN = SET;                  /* set the waiting for ble connection flag */
    set_led_green_mode(LED_MODE_TWINKLE);   /* set led green to twinkle mode */
    bls_ll_setAdvEnable(1);                 /* start advertising */
    sg_ble_tm = clock_time();               /* save current time */
}

/**
 * @brief wait ble connect
 * @param[in] none
 * @return none
 */
static void wait_ble_connect(void)
{
    if (!clock_time_exceed(sg_ble_tm, TIME_ALLOW_CONNECT*1000)) {
        return;
    }
    F_WAIT_BLE_CONN = CLR;                  /* clear the waiting for ble connection flag */
    set_led_green_mode(LED_MODE_FIX);       /* stop twinkling */
    bls_ll_setAdvEnable(0);                 /* stop advertising */
}

/**
 * @brief update ble status
 * @param[in] none
 * @return none
 */
static void update_ble_status(void)
{
    if (F_BLE_BONDING == CLR) {
        if (F_WAIT_BLE_CONN == SET) {
            wait_ble_connect();
        }
    }
}

/**
 * @brief smart kettle work in nature mode
 * @param[in] none
 * @return none
 */
static void kettle_mode_nature(void)
{
    set_led_red(OFF);
    set_led_orange(OFF);
    set_led_green_status(OFF);
    set_relay(OFF);
}

/**
 * @brief smart kettle work in boil mode
 * @param[in] none
 * @return none
 */
static void kettle_mode_boil(void)
{
    if (g_kettle.temp_cur >= TEMP_BOILED) {
        set_water_type(WATER_TYPE_PURE);
        set_boil_turn(OFF);
        set_relay(OFF);
    } else {
        set_relay(ON);
    }
}

/**
 * @brief smart kettle work in keep warm1 mode
 * @param[in] none
 * @return none
 */
static void kettle_mode_keep_warm1(void)
{
    if (g_kettle.temp_cur >= TEMP_BOILED) {
        set_water_type(WATER_TYPE_PURE);
        set_relay(OFF);
    } else {
        set_relay(ON);
    }
}

/**
 * @brief smart kettle work in keep warm2 mode
 * @param[in] none
 * @return none
 */
static void kettle_mode_keep_warm2(void)
{
    if (g_kettle.temp_cur > g_kettle.temp_set) {
        set_led_orange(ON);
        set_led_green_status(OFF);
        set_relay(OFF);
    } else if (g_kettle.temp_cur < (g_kettle.temp_set - TEMP_KEEP_RANGE)) {
        set_led_orange(ON);
        set_led_green_status(OFF);
        set_relay(ON);
    } else {
        set_led_orange(OFF);
        set_led_green_status(ON);
        set_relay(OFF);
    }
}

/**
 * @brief smart kettle stop
 * @param[in] none
 * @return none
 */
static void stop_kettle(void)
{
    set_boil_turn(OFF);
    set_keep_warm_turn(OFF);
    set_led_red(OFF);
    set_led_orange(OFF);
    set_led_green_status(OFF);
    set_relay(OFF);
}

/**
 * @brief smart kettle run
 * @param[in] none
 * @return none
 */
static void run_kettle(void)
{
    if (g_kettle.fault != FAULT_NORMAL) {
        return;
    }
    switch (g_kettle.mode) {
    case MODE_NATURE:
        kettle_mode_nature();
        break;
    case MODE_BOIL:
        kettle_mode_boil();
        break;
    case MODE_KEEP_WARM1:
        kettle_mode_keep_warm1();
        break;
    case MODE_KEEP_WARM2:
        kettle_mode_keep_warm2();
        break;
    default:
        break;
    }
}

/**
 * @brief update smart kettle mode
 * @param[in] none
 * @return none
 */
static void update_kettle_mode(void)
{
    if (g_kettle.boil_turn == ON) {
        set_work_mode(MODE_BOIL);
    } else if (g_kettle.keep_warm_turn == ON){
        if (g_kettle.water_type == WATER_TYPE_TAP) {
            set_work_mode(MODE_KEEP_WARM1);
        } else {
            set_work_mode(MODE_KEEP_WARM2);
        }
    } else {
        set_work_mode(MODE_NATURE);
    }
    run_kettle();
}

/**
 * @brief detect and handle fault event
 * @param[in] none
 * @return none
 */
static void detect_and_handle_fault_event(void)
{
    if (g_kettle.fault == FAULT_NORMAL) {
        if (g_kettle.temp_cur >= TEMP_UPPER_LIMIT) {
            update_fault(FAULT_LACK_WATER);
            set_buzzer_mode(BUZZER_MODE_FAULT);
            stop_kettle();
        }
    } else {
        if (g_kettle.temp_cur < TEMP_UPPER_LIMIT) {
            update_fault(FAULT_NORMAL);
            set_buzzer_mode(BUZZER_MODE_STOP);
            set_work_mode(MODE_NATURE);
        }
    }
}

/**
 * @brief update current temperature
 * @param[in] none
 * @return none
 */
static void update_cur_temp(void)
{
    uint8_t temp;
    static uint32_t s_get_temp_tm = 0;

    if (!clock_time_exceed(s_get_temp_tm, TIME_GET_TEMP*1000)) {
        return;
    }
    s_get_temp_tm = clock_time();

    temp = get_cur_temp();
    if (g_kettle.temp_cur != temp) {
        g_kettle.temp_cur = temp;
        report_one_dp_data(DP_ID_TEMP_CUR, g_kettle.temp_cur);
        detect_and_handle_fault_event();
    }
}

/**
 * @brief smart kettle work inboil key short press handler
 * @param[in] none
 * @return none
 */
void key_boil_short_press_cb_fun(void)
{
    if (g_kettle.fault != FAULT_NORMAL) {
        return;
    }
    switch_boil_turn();
    set_buzzer_mode(BUZZER_MODE_ONCE);
}

/**
 * @brief keep warm key short press handler
 * @param[in] none
 * @return none
 */
void key_keep_short_press_cb_fun(void)
{
    if (g_kettle.fault != FAULT_NORMAL) {
        return;
    }
    switch_keep_warm_turn();
    set_buzzer_mode(BUZZER_MODE_ONCE);
}

/**
 * @brief keep warm key long press handler
 * @param[in] none
 * @return none
 */
void key_keep_long_press_cb_fun(void)
{
    if (F_BLE_BONDING == SET) {
        return;
    }
    try_to_connect_ble();
    set_buzzer_mode(BUZZER_MODE_ONCE);
}

/**
 * @brief smart kettle init
 * @param[in] none
 * @return none
 */
void tuya_app_kettle_init(void)
{
    memset(&g_kettle, 0, sizeof(g_kettle));
    memset(&g_kettle_flag, 0, sizeof(g_kettle_flag));
    set_keep_warm_temp(TEMP_KEEP_WARM_DEFAULT);

    led_init();
    relay_init();
    buzzer_pwm_init();
    ntc_adc_init();
    ts02n_key_init(&user_ts02n_key_def_s);
    ble_connect_status_init();
}

/**
 * @brief main loop of smart kettle
 * @param[in] none
 * @return none
 */
void tuya_app_kettle_loop(void)
{
    update_ble_status();
    update_cur_temp();
    ts02n_key_loop();
    update_kettle_mode();
    update_led_green_status();
    update_buzzer_status();
}

/**
 * @brief dp data handler of smart kettle
 * @param[in] dp_data: dp data array
 * @return none
 */
void tuya_app_kettle_dp_data_handler(uint8_t *dp_data)
{
    switch (dp_data[0]) {
    case DP_ID_BOIL:
        set_boil_turn(dp_data[3]);
        *(dp_data + 3) = g_kettle.boil_turn;
        break;
    case DP_ID_KEEP_WARM:
        set_keep_warm_turn(dp_data[3]);
        *(dp_data + 3) = g_kettle.keep_warm_turn;
        break;
    case DP_ID_TEMP_SET:
        set_keep_warm_temp(dp_data[6]);
        *(dp_data + 6) = g_kettle.temp_set;
        break;
    case DP_ID_WATER_TYPE:
        set_water_type(dp_data[3]);
        *(dp_data + 3) = g_kettle.water_type;
        break;
    case DP_ID_TEMP_CUR:
    case DP_ID_FAULT:
    default:
        break;
    }
}

/**
 * @brief ble connect status change handler of smart kettle
 * @param[in] status: ble connect status
 * @return none
 */
void tuya_app_kettle_ble_connect_status_change_handler(tuya_ble_connect_status_t status)
{
    if (status == BONDING_CONN) {               /* when ble is connected */
        report_all_dp_data();                   /* report all dp information */
        if (F_WAIT_BLE_CONN == SET) {           /* when the waiting for ble connection flag is set */
            F_BLE_BONDING = SET;                /* set the ble bonding flag */
            F_WAIT_BLE_CONN = CLR;              /* clear the waiting for ble connection flag */
            set_led_green_mode(LED_MODE_FIX);   /* stop twinkling */
        }
    }
    if (status == UNBONDING_UNCONN) {           /* when ble is unbonding */
        F_BLE_BONDING = CLR;                    /* clear the ble bonding flag */
        bls_ll_setAdvEnable(0);                 /* stop advertising */
    }
}
