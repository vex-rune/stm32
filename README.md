# stm32

## 软件
下载 STM32 相关的软件


[KEIL](https://www.keil.com/download/product/)
[KEIL](https://www.keil.com/demo/eval/arm.htm#/DOWNLOAD)

[STM32CubeIDE](
https://www.st.com.cn/zh/development-tools/stm32cubeide.html#tools-software8)

[ST-LINK/V2](
https://www.st.com.cn/zh/development-tools/stsw-link009.html#tools-software)

[ST-LINK Utility](https://www.st.com/en/development-tools/stsw-link004.html)


## 标准库
- STM32F10x 标准库

[STM32F10x 标准库](https://www.st.com/en/embedded-software/stsw-stm32054.html)


# VS Code 的使用
- `CMake Tools` 插件
- `STM32` 插件
- `clangd` 插件



# 日记

目前到了 LoRa, 还没有完成, 待续...


# src 目录说明

`src/` 目录按数字顺序组织 STM32 学习项目，每个子目录对应一个独立课题。

## 目录结构

```
src/
├── 01_led_swich                  # GPIO 输入输出 - LED 与拨码开关
├── 02_usart_interrupt            # USART 中断接收
├── 03_i2c                        # I2C 总线 - 读写 AT24C02 EEPROM
├── 04_timer                      # 定时器（基础 / 通用 / SysTick）
├── 05_breathing_led_duty         # 呼吸灯 - PWM 占空比渐变
├── 06_bg_lcd                     # 段码 LCD 显示
├── 07_pwm_input                  # PWM 输入捕获 - 测频率/占空比
├── 08_pwm_super                  # 高级 PWM（互补输出 / 死区）
├── 09_dma_rom_ram                # DMA：Flash → RAM
├── 10_dma_ram_com                # DMA：RAM ↔ USART
├── 11_adc_RV                     # 单通道 ADC - 读取电位器
├── 12_adc_mul                    # 多通道 ADC + DMA
├── 13_spi_w25q32                 # SPI 总线 - 读写 W25Q32 Flash
├── 14_fsmc_is62wv51216           # FSMC 总线 - 扩展 SRAM IS62WV51216
├── 15_lcd                        # FSMC 驱动 TFT LCD
├── 16_can_mode_silentI_loopback  # CAN 总线 - 静默/回环模式
├── 17_ethernet_test              # 以太网 - W5500 TCP/UDP 测试
├── 18_web_server                 # 以太网 - W5500 Web 服务器
├── 19_esp32c3_at                 # ESP32-C3 AT 指令集入门
├── 20_wifi                       # ESP32 WiFi 联网（AT）
├── 21_bluetooth                  # ESP32 蓝牙（AT）
├── 22_LoRa                       # LLCC68 LoRa 模块 + LibDriver
├── 23_power_control_sleep        # 电源控制 - 睡眠模式
├── 24_power_control_stop         # 电源控制 - 停机模式
├── 25_power_control_standby      # 电源控制 - 待机模式
└── 26_bkp                        # 备份寄存器（BKP）+ RTC
```

## 子目录典型结构

每个项目目录内部通常包含：

```
XX_topic/
├── Core/           # HAL 驱动（CubeMX 生成）
│   ├── Src/        # main.c / 外设初始化
│   └── Inc/        # main.h / 外设头文件
├── Drivers/        # STM32 HAL 库 / BSP
├── cmake/          # CMake 工具链脚本
├── XX_topic.ioc    # STM32CubeMX 工程文件
├── CMakeLists.txt  # CMake 构建脚本
├── STM32F103xx_FLASH.ld   # 链接脚本
├── startup_stm32f103xe.s  # 启动文件
├── *.md            # 学习笔记
└── README.md       # 项目说明
```

## 学习主题分类

### 基础外设（GPIO / 通信）
- `01_led_swich` - GPIO 输入输出
- `02_usart_interrupt` - USART 中断
- `03_i2c` - I2C 总线
- `13_spi_w25q32` - SPI 总线
- `16_can_mode_silentI_loopback` - CAN 总线

### 定时器与 PWM
- `04_timer` - 定时器基础
- `05_breathing_led_duty` - PWM 呼吸灯
- `06_bg_lcd` - 段码 LCD（定时器驱动）
- `07_pwm_input` - PWM 输入捕获
- `08_pwm_super` - 高级 PWM

### DMA / ADC
- `09_dma_rom_ram` - DMA 基础
- `10_dma_ram_com` - DMA + USART
- `11_adc_RV` - ADC 单通道
- `12_adc_mul` - ADC 多通道 + DMA

### 存储与显示
- `13_spi_w25q32` - SPI Flash
- `14_fsmc_is62wv51216` - FSMC 扩展 SRAM
- `15_lcd` - FSMC 驱动 TFT LCD

### 无线与网络
- `17_ethernet_test` - W5500 TCP/UDP
- `18_web_server` - W5500 Web 服务器
- `19_esp32c3_at` - ESP32 AT 指令
- `20_wifi` - WiFi 联网
- `21_bluetooth` - 蓝牙
- `22_LoRa` - LoRa 远程通信

### 电源管理
- `23_power_control_sleep` - 睡眠模式
- `24_power_control_stop` - 停机模式
- `25_power_control_standby` - 待机模式
- `26_bkp` - 备份寄存器（掉电保持）

## 当前进度

最近在做的：**22_LoRa（LLCC68）** 与 **23~26（电源管理与备份寄存器）**。