# Tuya IoTOS Embeded Demo Bluetooth LE Smart Kettle

[English](./README.md) | [中文](./README_zh.md)

<br>

## 简介 

本 demo 基于 [涂鸦IoT平台](https://iot.tuya.com/) 、涂鸦智能APP、IoTOS Embeded BLE SDK，使用涂鸦Bluetooth LE系列模组快速组建一个智能烧水壶demo。

<br>

## 快速上手

### 开发环境搭建 

- IDE 根据芯片原厂 SDK 要求进行安装。

- Tuya BLE SDK Demo Project 下载地址见下表所示。请参考各分支下的 `README.md` 文件完成工程导入。

  |   芯片平台   |   型号   |                           下载地址                           |
  | :----------: | :------: | :----------------------------------------------------------: |
  |    Nordic    | nrf52832 | [tuya_ble_sdk_Demo_Project_nrf52832.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_nrf52832.git) |
  |   Realtek    | RTL8762C | [tuya_ble_sdk_Demo_Project_rtl8762c.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_rtl8762c.git) |
  |    Telink    | TLSR825x | [tuya_ble_sdk_Demo_Project_tlsr8253.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_tlsr8253.git) |
  | Silicon Labs |   BG21   |                     正在规划中，敬请期待                     |
  |    Beken     | BK3431Q  | [Tuya_ble_sdk_demo_project_bk3431q.git](https://github.com/TuyaInc/Tuya_ble_sdk_demo_project_bk3431q.git) |
  |    Beken     |  BK3432  | [ tuya_ble_sdk_Demo_Project_bk3432.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_bk3432.git) |
  |   Cypress    |  Psoc63  | [tuya_ble_sdk_Demo_Project_PSoC63.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_PSoC63.git) |

<br>

### 编译烧录

- 代码修改

  1. 在 `tuya_ble_app_demo.h` 填入在 [涂鸦IoT平台](https://iot.tuya.com/) 创建的智能产品 PID。

     ```
      #define APP_PRODUCT_ID     "xxxxxxxx"
     ```
  
     将其中 `xxxxxxxx` 替换为PID。

  2. 在 `tuya_ble_app_demo.c` 填入申请的授权码（uuid 和 auth key）。

     ```
      static const char auth_key_test[] = "yyyyyyyy";
      static const char device_id_test[] = "zzzzzzzz";
     ```
  
     其中 `yyyyyyyy` 替换为 auth key， `zzzzzzzz` 替换为 uuid.

- 编译运行Demo代码

  修改好后编译代码，并下载固件至硬件运行（根据所选的芯片型号有可能还需要下载 stack 和 bootloader），观察 log 日志 ，并用第三方蓝牙调试 App（例如 IOS 下的 lightBlue）扫描确认设备有没有正常广播。

<br>

### 文件介绍
```
├── src         /* 源文件目录 */
|    ├── sdk
|    |    └── tuya_uart_common_handler.c        /* UART通用对接实现代码 */
|    ├── driver
|    |    ├── tuya_app_driver_buzzer.c          /* 蜂鸣器驱动相关 */
|    |    ├── tuya_app_driver_key.c             /* 触摸按键驱动相关 */
|    |    ├── tuya_app_driver_led.c             /* 指示灯驱动相关 */
|    |    ├── tuya_app_driver_ntc.c             /* 温度传感器驱动相关 */
|    |    └── tuya_app_driver_relay.c           /* 继电器驱动相关 */
|    ├── tuya_ble_app_demo.c                    /* 应用层入口文件 */
|    └── tuya_app_smart_kettle.c                /* 智能烧水壶应用代码 */
|
└── include     /* 头文件目录 */
     ├── sdk
     |    ├── custom_app_uart_common_handler.h  /* UART通用对接实现代码 */
     |    ├── custom_app_product_test.h         /* 自定义产测项目相关实现 */
     |    └── custom_tuya_ble_config.h          /* 应用配置文件 */
     ├── driver
     |    ├── tuya_app_driver_buzzer.h          /* 蜂鸣器驱动相关 */
     |    ├── tuya_app_driver_key.h             /* 触摸按键驱动相关 */
     |    ├── tuya_app_driver_led.h             /* 指示灯驱动相关 */
     |    ├── tuya_app_driver_ntc.h             /* 温度传感器驱动相关 */
     |    └── tuya_app_driver_relay.h           /* 继电器驱动相关 */
     ├── tuya_ble_app_demo.h                    /* 应用层入口文件 */
     ├── tuya_app_smart_kettle.h                /* 智能烧水壶应用代码 */
     └── tuya_app_common.h                      /* 应用通用定义 */
```

<br>

### 应用入口
入口文件：/tuya_ble_app/tuya_ble_app_demo.c

+ `void tuya_ble_app_init(void)` 对Tuya IoTOS Embeded Ble SDK 进行一些必要的初始化，该函数只执行一次。
+ `void app_exe()` 该函数用来执行用户应用代码，该函数循环执行。

<br>

### DP点相关 

|  函数名  | tuya_ble_dp_data_report                                      |
| :------: | :----------------------------------------------------------- |
| 函数原型 | tuya_ble_status_t tuya_ble_dp_data_report(uint8_t *p_data,uint32_t len); |
| 功能概述 | 上报dp点数据。                                               |
|   参数   | p_data [in] : dp点数据。len[in] : 数据长度，最大不能超过`TUYA_BLE_REPORT_MAX_DP_DATA_LEN`。 |
|  返回值  | TUYA_BLE_SUCCESS ：发送成功；<br/>TUYA_BLE_ERR_INVALID_PARAM：参数无效；<br/>TUYA_BLE_ERR_INVALID_STATE：当前状态不支持发送，例如蓝牙断开；<br/>TUYA_BLE_ERR_NO_MEM：内存申请失败；<br/>TUYA_BLE_ERR_INVALID_LENGTH：数据长度错误；<br/>TUYA_BLE_ERR_NO_EVENT：其他错误。 |
|   备注   | Application通过调用该函数上报 DP 点数据到 手机app。          |

参数说明：

[涂鸦IoT平台](https://iot.tuya.com/) 是以 dp 点的方式管理数据，任何设备产生的数据都需要抽象为 dp 点的形式，一个完整的dp点数据由四部分组成（具体参考 IoT 工作台上的相关介绍）：

- Dp_id： 1个字节，在开发平台注册的 dp_id 序号。


- Dp_type：1 个字节，dp点类型。

  ​	`#define DT_RAW 0`       raw类型；

  ​	`#define DT_BOOL 1`     布尔类型；

  ​	`#define DT_VALUE 2`   数值类型；（其范围在iot平台注册时指定）

  ​	`#define DT_STRING 3` 字符串类型；

  ​	`#define DT_ENUM 4 `     枚举类型；

  ​	`#define DT_BITMAP 5` 位映射类型；

- Dp_len：1 个字节或者两个字节，目前蓝牙仅支持一个字节，即单个 dp 点数据最长 255 个字节。


- Dp_data：数据，dp_len 个字节。


该 dp 点上报函数的参数 p_data 指向的数据必须以下表格式组装上报：

| Dp点1的数据 |         |        |         | ~    | Dp点n的数据 |         |        |         |
| :---------: | :-----: | :----: | :-----: | :--- | :---------: | :-----: | :----: | :-----: |
|      1      |    2    |   3    |    4    | ~    |      n      |   n+1   |  n+2   |   n+3   |
|    Dp_id    | Dp_type | Dp_len | Dp_data | ~    |    Dp_id    | Dp_type | Dp_len | Dp_data |

调用该函数时，参数 len 的最大长度为 `TUYA_BLE_REPORT_MAX_DP_DATA_LEN`（当前为255+3）。

<br>

### I/O 列表

| 外设       | I/O  | 外设       | I/O  |
| ---------- | ---- | ---------- | ---- |
| 煮沸键     | P7   | 绿色指示灯 | P6   |
| 保温键     | P8   | 蜂鸣器     | P17  |
| 红色指示灯 | P24  | 温度传感器 | ADC  |
| 橙色指示灯 | P26  | 继电器     | P14  |

<br>

## 相关文档

- [BLE SDK 说明](https://developer.tuya.com/cn/docs/iot/device-development/embedded-software-development/module-sdk-development-access/ble-chip-sdk/tuya-ble-sdk-user-guide?id=K9h5zc4e5djd9#title-17-tuya%20ble%20sdk%20callback%20event%20%E4%BB%8B%E7%BB%8D) 
- [BLE SDK Demo 说明](https://developer.tuya.com/cn/docs/iot/device-development/embedded-software-development/module-sdk-development-access/ble-chip-sdk/tuya-ble-sdk-demo-instruction-manual?id=K9gq09szmvy2o) 
- [涂鸦 Demo 中心](https://developer.tuya.com/demo) 

<br>


## 技术支持

您可以通过以下方法获得涂鸦的支持:

- [涂鸦 AI+IoT 开发者平台](https://developer.tuya.com)
- [帮助中心](https://support.tuya.com/help)
- [服务与支持](https://service.console.tuya.com)

<br>