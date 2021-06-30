# Tuya IoTOS Embeded Demo BLE Smart Kettle

[English](./README.md) | [中文](./README_zh.md) 

<br>

## Introduction 

This demo is based on Tuya Smart Cloud Platform, Tuya Smart APP, IoTOS Embeded Ble SDK, using Tuya BLE series modules quickly build a smart kettle demo.

<br>


## Quick start 

### Development environment build 

- IDE to be installed according to the original chip SDK requirements.
- The Tuya BLE SDK Demo Project can be downloaded from the table below. Please refer to the `README.md` file under each branch to complete the project import.

|   Platform   |  Model   |                       Download Address                       |
| :----------: | :------: | :----------------------------------------------------------: |
|    Nordic    | nrf52832 | [tuya_ble_sdk_Demo_Project_nrf52832.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_nrf52832.git) |
|   Realtek    | RTL8762C | [tuya_ble_sdk_Demo_Project_rtl8762c.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_rtl8762c.git) |
|    Telink    | TLSR825x | [tuya_ble_sdk_Demo_Project_tlsr8253.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_tlsr8253.git) |
| Silicon Labs |   BG21   |              In the planning stages, stay tuned              |
|    Beken     | BK3431Q  | [Tuya_ble_sdk_demo_project_bk3431q.git](https://github.com/TuyaInc/Tuya_ble_sdk_demo_project_bk3431q.git) |
|    Beken     |  BK3432  | [ tuya_ble_sdk_Demo_Project_bk3432.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_bk3432.git) |
|   Cypress    |  Psoc63  | [tuya_ble_sdk_Demo_Project_PSoC63.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_PSoC63.git) |

<br>

### Compile and burn

- code modifications

  1. Fill in `tuya_ble_app_demo.h` with the PID of the smart product created in the Tuya IoT workbench.

     ```
      #define APP_PRODUCT_ID "xxxxxxxx"
     ```
  
     Replace `xxxxxxxx` with the PID.

  2. Fill in `tuya_ble_app_demo.c` with the requested authorization code (uuid and auth key).

     ```
      static const char auth_key_test[] = "yyyyyyyy";
      static const char device_id_test[] = "zzzzzzzz";
     ```
  
     where `yyyyyyyy` is replaced with auth key and `zzzzzzzz` is replaced with uuid.

- Compile and run the demo code

  Compile the code and download the firmware to the hardware (you may also need to download the stack and bootloader depending on the chip model you have chosen), watch the logs and use a third party Bluetooth debugging app (e.g. lightBlue for IOS) to scan the device to make sure it is broadcasting properly.

<br>

### File description 

```
├── src         /* source file */
|    ├── sdk
|    |    └── tuya_uart_common_handler.c        /* UART common handler */
|    ├── driver
|    |    ├── tuya_app_driver_buzzer.c          /* buzzer driver related */
|    |    ├── tuya_app_driver_key.c             /* key driver related */
|    |    ├── tuya_app_driver_led.c             /* led driver related */
|    |    ├── tuya_app_driver_ntc.c             /* NTC driver related */
|    |    └── tuya_app_driver_relay.c           /* relay driver related */
|    ├── tuya_ble_app_demo.c                    /* application entry file */
|    └── tuya_app_smart_kettle.c                /* smart kettle application */
|
├── include     /* header file */
     ├── sdk
     |    ├── custom_app_uart_common_handler.h  /* UART common handler */
     |    ├── custom_app_product_test.h         /* custom product test related */
     |    └── custom_tuya_ble_config.h          /* application config file */
     ├── driver
     |    ├── tuya_app_driver_buzzer.h          /* buzzer driver related */
     |    ├── tuya_app_driver_key.h             /* key driver related */
     |    ├── tuya_app_driver_led.h             /* led driver related */
     |    ├── tuya_app_driver_ntc.h             /* NTC driver related */
     |    └── tuya_app_driver_relay.c           /* relay driver related */
     ├── tuya_ble_app_demo.h                    /* application entry file */
     ├── tuya_app_smart_kettle.h                /* smart kettle application */
     └── tuya_app_common.h                      /* application common define */
```

<br>

### Application entry

Entry file: /tuya_ble_app/tuya_ble_app_demo.c

- `void tuya_ble_app_init(void)` Performs some necessary initialization of the Tuya IoTOS Embeded Ble SDK, this function is executed only once.
- `void app_exe()` This function is used to execute the user application code, this function executes in a loop.

<br>

### DP point related

|   function name    | tuya_ble_dp_data_report                                      |
| :----------------: | :----------------------------------------------------------- |
| Function prototype | tuya_ble_status_t tuya_ble_dp_data_report(uint8_t *p_data,uint32_t len); |
| Function overview  | Report dp point data.                                        |
|     Parameters     | p_data [in] : The dp point data. len[in] : The length of the data, the maximum cannot exceed `TUYA_BLE_REPORT_MAX_DP_DATA_LEN`. |
|    Return value    | TUYA_BLE_SUCCESS : send success;<br/>TUYA_BLE_ERR_INVALID_PARAM : invalid parameter;<br/>TUYA_BLE_ERR_INVALID_STATE : current status does not support send, e.g. Bluetooth disconnect;<br/>TUYA_BLE _ERR_NO_MEM: memory request failed;<br/>TUYA_BLE_ERR_INVALID_LENGTH: data length error;<br/>TUYA_BLE_ERR_NO_EVENT: other error. |
|      Remarks       | The Application reports the DP point data to the phone app by calling this function. |

Parameter description.

The Tuya Cloud Platform manages data in the form of dp points. Any data generated by the device needs to be abstracted as dp points. A complete dp point data consists of four parts (refer to the relevant introduction on the IoT workbench for details).

- Dp_id: 1 byte, the dp_id serial number registered with the development platform.

- Dp_type: 1 byte, the dp point type.

  ​	   `#define DT_RAW    0`  raw type;

  ​	   `#define DT_BOOL   1`  boolean type;

  ​	   `#define DT_VALUE  2`  numeric type; (whose range is specified at the time of iot platform registration)

  ​	   `#define DT_STRING 3`  string type;

  ​	   `#define DT_ENUM   4`  enumeration type;

  ​	   `#define DT_BITMAP 5`  bitmap type;

- Dp_len: 1 byte or two bytes, currently Bluetooth only supports one byte, i.e. up to 255 bytes of data for a single dp point.

- Dp_data: data, dp_len bytes.

The data referred to by the parameter p_data of this dp point reporting function must be assembled and reported in the following table format.

| Data of Dp point 1 |         |        |         |  ~   | Data of Dp point n |         |        |         |
| :----------------: | :-----: | :----: | :-----: | :--: | :----------------: | :-----: | :----: | :-----: |
|         1          |    2    |   3    |   4~    |  ~   |         n          |   n+1   |  n+2   |  n+3~   |
|       Dp_id        | Dp_type | Dp_len | Dp_data |  ~   |       Dp_id        | Dp_type | Dp_len | Dp_data |

When this function is called, the maximum length of the argument len is `TUYA_BLE_REPORT_MAX_DP_DATA_LEN` (currently 255+3).

<br>

### I/O list

| Peripheral    | I/O  | Peripheral | I/O  |
| ------------- | ---- | ---------- | ---- |
| Boil key      | P7   | Green led  | P6   |
| Keep warm key | P8   | Buzzer     | P17  |
| Red led       | P24  | NTC        | ADC  |
| Orange led    | P26  | Relay      | P14  |

<br>

## Related documentation 

+ [BLE SDK Description](https://developer.tuya.com/cn/docs/iot/device-development/embedded-software-development/module-sdk-development-access/ble-chip-sdk/tuya-ble-sdk-user-guide?id=K9h5zc4e5djd9#title-17-tuya%20ble%20sdk%20callback%20event%20%E4%BB%8B%E7%BB%8D) 
+ [BLE SDK Demo Description](https://developer.tuya.com/cn/docs/iot/device-development/embedded-software-development/module-sdk-development-access/ble-chip-sdk/tuya-ble-sdk-demo-instruction-manual?id=K9gq09szmvy2o) 
+ [Tuya Demo Center](https://developer.tuya.com/demo)  

<br>

## Technical support 

You can get support for Tuya in the following ways:

+ Developer Center：https://developer.tuya.com
+ Help Center：https://support.tuya.com/help
+ Technical Support Work Order Center：https://service.console.tuya.com 

<br>