[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver LLCC68

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/llcc68/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

LLCC68 sub-GHz 射频收发器非常适合长距离无线应用。它专为长电池寿命而设计，有源接收电流消耗仅为 4.2 mA。LLCC68 可通过高效集成功率放大器发射高达 +22 dBm 的功率。LLCC68 支持用于 LPWAN 场景的 LoRa® 调制以及用于传统场景的 (G)FSK 调制。它具有高度可配置性，能够满足工业和消费领域的不同应用需求。该器件提供与 Semtech 收发器兼容的 LoRa® 调制，符合 LoRa Alliance™ 发布的 LoRaWAN™ 规范。该射频适用于目标符合无线电法规（包括但不限于 ETSI EN 300 220、FCC CFR 47 Part 15、中国法规要求以及日本 ARIB T-108）的系统。150 MHz 至 960 MHz 的连续频率覆盖支持全球所有主要的 sub-GHz ISM 频段。LLCC68 可应用于智能电表、供应链与物流、楼宇自动化等领域。

LibDriver LLCC68 是 LibDriver 推出的 LLCC68 全功能驱动，提供无线发送、无线接收、信道活动检测（CAD）以及附加功能。LibDriver 符合 MISRA 标准。

### 目录

  - [说明](#说明)
  - [安装](#安装)
  - [使用](#使用)
    - [示例 基本发送](#示例-基本发送)
    - [示例 基本接收](#示例-基本接收)
  - [文档](#文档)
  - [贡献](#贡献)
  - [许可证](#许可证)
  - [联系我们](#联系我们)

### 说明

`/src` 包含 LibDriver LLCC68 源文件。

`/interface` 包含 LibDriver LLCC68 SPI 平台无关模板。

`/test` 包含 LibDriver LLCC68 驱动测试代码，可用于简单地测试芯片的必要功能。

`/example` 包含 LibDriver LLCC68 示例代码。

`/doc` 包含 LibDriver LLCC68 离线文档。

`/datasheet` 包含 LLCC68 数据手册。

`/project` 包含常见的 Linux 和 MCU 开发板示例代码。所有项目均使用 shell 脚本调试驱动，详细说明可在各项目的 README.md 中找到。

`/misra` 包含 LibDriver MISRA 代码扫描结果。

### 安装

参考 `/interface` SPI 平台无关模板，完成您自己平台的 SPI 驱动。

将 `/src` 目录、您平台的接口驱动以及您自己的驱动添加到您的项目中。如果想使用默认的示例驱动，请将 `/example` 目录一并添加到您的项目中。

### 使用

您可以参考 `/example` 目录中的示例来完成您自己的驱动。如果想使用默认的编程示例，使用方法如下。

#### 示例 基本发送

```C
#include "driver_llcc68_lora.h"

uint8_t (*g_gpio_irq)(void) = NULL;
uint8_t res;
static uint8_t gs_rx_done;

static void a_callback(uint16_t type, uint8_t *buf, uint16_t len)
{
    switch (type)
    {
        case LLCC68_IRQ_TX_DONE :
        {
            llcc68_interface_debug_print("llcc68: irq tx done.\n");
            

            break;
        }
        case LLCC68_IRQ_RX_DONE :
        {
            uint16_t i;
            llcc68_bool_t enable;
            float rssi;
            float snr;
            
            llcc68_interface_debug_print("llcc68: irq rx done.\n");
            
            /* 获取状态 */
            if (llcc68_lora_get_status((float *)&rssi, (float *)&snr) != 0)
            {
                return;
            }
            llcc68_interface_debug_print("llcc68: rssi is %0.1f.\n", rssi);
            llcc68_interface_debug_print("llcc68: snr is %0.2f.\n", snr);
            
            /* 检查错误 */
            if (llcc68_lora_check_packet_error(&enable) != 0)
            {
                return;
            }
            if ((enable == LLCC68_BOOL_FALSE) && len)
            {
                for (i = 0; i < len; i++)
                {
                    llcc68_interface_debug_print("%c", buf[i]);
                }
                llcc68_interface_debug_print("\n");
                gs_rx_done = 1;
            }
            
            break;
        }
        case LLCC68_IRQ_PREAMBLE_DETECTED :
        {
            llcc68_interface_debug_print("llcc68: irq preamble detected.\n");
            
            break;
        }
        case LLCC68_IRQ_SYNC_WORD_VALID :
        {
            llcc68_interface_debug_print("llcc68: irq valid sync word detected.\n");
            
            break;
        }
        case LLCC68_IRQ_HEADER_VALID :
        {
            llcc68_interface_debug_print("llcc68: irq valid header.\n");
            
            break;
        }
        case LLCC68_IRQ_HEADER_ERR :
        {
            llcc68_interface_debug_print("llcc68: irq header error.\n");
            
            break;
        }
        case LLCC68_IRQ_CRC_ERR :
        {
            llcc68_interface_debug_print("llcc68: irq crc error.\n");
            
            break;
        }
        case LLCC68_IRQ_CAD_DONE :
        {
            llcc68_interface_debug_print("llcc68: irq cad done.\n");
            
            break;
        }
        case LLCC68_IRQ_CAD_DETECTED :
        {
            llcc68_interface_debug_print("llcc68: irq cad detected.\n");
            
            break;
        }
        case LLCC68_IRQ_TIMEOUT :
        {
            llcc68_interface_debug_print("llcc68: irq timeout.\n");
            
            break;
        }
        default :
        {
            break;
        }
    }

}

/* gpio 初始化 */
res = gpio_interrupt_init();
if (res != 0)
{
    return 1;
}
g_gpio_irq = llcc68_lora_irq_handler;

/* lora 初始化 */
res = llcc68_lora_init(a_callback);
if (res != 0)
{
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;

}

/* 设置发送模式 */
res = llcc68_lora_set_send_mode();
if (res != 0)
{
    (void)llcc68_lora_deinit();
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;

}

llcc68_interface_debug_print("llcc68: send %s.\n", "123");

/* 发送数据 */
res = llcc68_lora_send((uint8_t *)"123", strlen("123"));
if (res != 0)
{
    (void)llcc68_lora_deinit();
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;

}

/* 反初始化 */
res = llcc68_lora_deinit();
if (res != 0)
{
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;

}
(void)gpio_interrupt_deinit();
g_gpio_irq = NULL;

return 0;
```

#### 示例 基本接收

```C
#include "driver_llcc68_lora.h"

uint8_t (*g_gpio_irq)(void) = NULL;
uint8_t res;
uint32_t timeout;
static uint8_t gs_rx_done;

static void a_callback(uint16_t type, uint8_t *buf, uint16_t len)
{
    switch (type)
    {
        case LLCC68_IRQ_TX_DONE :
        {
            llcc68_interface_debug_print("llcc68: irq tx done.\n");
            

            break;
        }
        case LLCC68_IRQ_RX_DONE :
        {
            uint16_t i;
            llcc68_bool_t enable;
            float rssi;
            float snr;
            
            llcc68_interface_debug_print("llcc68: irq rx done.\n");
            
            /* 获取状态 */
            if (llcc68_lora_get_status((float *)&rssi, (float *)&snr) != 0)
            {
                return 1;
            }
            llcc68_interface_debug_print("llcc68: rssi is %0.1f.\n", rssi);
            llcc68_interface_debug_print("llcc68: snr is %0.2f.\n", snr);
            
            /* 检查错误 */
            if (llcc68_lora_check_packet_error(&enable) != 0)
            {
                return 1;
            }
            if ((enable == LLCC68_BOOL_FALSE) && len)
            {
                for (i = 0; i < len; i++)
                {
                    llcc68_interface_debug_print("%c", buf[i]);
                }
                llcc68_interface_debug_print("\n");
                gs_rx_done = 1;
            }
            
            break;
        }
        case LLCC68_IRQ_PREAMBLE_DETECTED :
        {
            llcc68_interface_debug_print("llcc68: irq preamble detected.\n");
            
            break;
        }
        case LLCC68_IRQ_SYNC_WORD_VALID :
        {
            llcc68_interface_debug_print("llcc68: irq valid sync word detected.\n");
            
            break;
        }
        case LLCC68_IRQ_HEADER_VALID :
        {
            llcc68_interface_debug_print("llcc68: irq valid header.\n");
            
            break;
        }
        case LLCC68_IRQ_HEADER_ERR :
        {
            llcc68_interface_debug_print("llcc68: irq header error.\n");
            
            break;
        }
        case LLCC68_IRQ_CRC_ERR :
        {
            llcc68_interface_debug_print("llcc68: irq crc error.\n");
            
            break;
        }
        case LLCC68_IRQ_CAD_DONE :
        {
            llcc68_interface_debug_print("llcc68: irq cad done.\n");
            
            break;
        }
        case LLCC68_IRQ_CAD_DETECTED :
        {
            llcc68_interface_debug_print("llcc68: irq cad detected.\n");
            
            break;
        }
        case LLCC68_IRQ_TIMEOUT :
        {
            llcc68_interface_debug_print("llcc68: irq timeout.\n");
            
            break;
        }
        default :
        {
            break;
        }
    }

}

/* gpio 初始化 */
res = gpio_interrupt_init();
if (res != 0)
{
    return 1;
}
g_gpio_irq = llcc68_lora_irq_handler;

/* lora 初始化 */
res = llcc68_lora_init(a_callback);
if (res != 0)
{
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;

}

/* 开始接收 */
llcc68_interface_debug_print("llcc68: start receiving...\n");
gs_rx_done = 0;
timeout = 3000;

/* 启动接收 */
res = llcc68_lora_set_continuous_receive_mode();
if (res != 0)
{
    (void)llcc68_lora_deinit();
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;

}

while ((timeout != 0) && (gs_rx_done == 0))
{
    timeout--;
    llcc68_interface_delay_ms(1000);
}
if (gs_rx_done == 0)
{
    /* 接收超时 */
    llcc68_interface_debug_print("llcc68: receive timeout.\n");
    (void)llcc68_lora_deinit();
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;

}

/* 反初始化 */
res = llcc68_lora_deinit();
if (res != 0)
{
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;

}
(void)gpio_interrupt_deinit();
g_gpio_irq = NULL;

return 0;
```

### 文档

在线文档：[https://www.libdriver.com/docs/llcc68/index.html](https://www.libdriver.com/docs/llcc68/index.html)。

离线文档：`/doc/html/index.html`。

### 贡献

请参考 CONTRIBUTING.md。

### 许可证

版权所有 (c) 2015 - 至今 LibDriver 保留所有权利。



MIT 许可证（MIT）



特此免费授予任何获得本软件及相关文档文件（"软件"）副本的人处理软件的权利，包括但不限于使用、复制、修改、合并、发布、分发、再许可和/或销售软件副本的权利，并允许获得软件的人这样做，但须符合以下条件：



上述版权声明和本许可声明应包含在所有副本或实质性部分中。



本软件按"原样"提供，不提供任何形式的担保，无论是明示还是暗示，包括但不限于适销性、特定用途适用性和非侵权的担保。在任何情况下，作者或版权持有者均不对任何索赔、损害或其他责任负责，无论是在合同、侵权或其他方面的行为，由软件或软件的使用或其他交易引起或与之相关。

### 联系我们

请发送电子邮件至 lishifenging@outlook.com。