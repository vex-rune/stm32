/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_llcc68_interface.h
 * @brief     driver llcc68 interface header file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2023-04-15
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2023/04/15  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#ifndef DRIVER_LLCC68_INTERFACE_H
#define DRIVER_LLCC68_INTERFACE_H

#include "driver_llcc68.h"
#include "stm32f1xx_hal_gpio.h"
#include "main.h"

// CS 片选
#define CS_LOW HAL_GPIO_WritePin( lora_cs_GPIO_Port, lora_cs_Pin,GPIO_PIN_RESET);
#define CS_HIGH HAL_GPIO_WritePin( lora_cs_GPIO_Port, lora_cs_Pin,GPIO_PIN_SET);

// 复位
#define RST_LOW HAL_GPIO_WritePin( lora_rst_GPIO_Port, lora_rst_Pin,GPIO_PIN_RESET);
#define RST_HIGH HAL_GPIO_WritePin( lora_rst_GPIO_Port, lora_rst_Pin,GPIO_PIN_SET);

// 收发使能
#define RXEN_LOW HAL_GPIO_WritePin( lora_rxen_GPIO_Port, lora_rxen_Pin,GPIO_PIN_RESET);
#define RXEN_HIGH HAL_GPIO_WritePin( lora_rxen_GPIO_Port, lora_rxen_Pin,GPIO_PIN_SET);

#define TXEN_LOW HAL_GPIO_WritePin( lora_txen_GPIO_Port, lora_txen_Pin,GPIO_PIN_RESET);
#define TXEN_HIGH HAL_GPIO_WritePin( lora_txen_GPIO_Port, lora_txen_Pin,GPIO_PIN_SET);

// 读取BUSY
#define BUSY_READ HAL_GPIO_ReadPin( lora_busy_GPIO_Port, lora_busy_Pin);

#ifdef __cplusplus
extern "C" {



#endif

/**
 * @defgroup llcc68_example_driver LLCC68 示例驱动函数
 * @brief    LLCC68 示例驱动模块
 * @ingroup  llcc68_driver
 * @{
 */

/**
 * @brief LLCC68 LoRa 示例默认定义
 * @note  以下宏定义用于配置 LLCC68 LoRa 模块的默认工作参数
 */

/*============================================================================*/
/* 电源与功耗配置                                                              */
/*============================================================================*/

/**
 * @brief 稳压器工作模式：仅使用 LDO（低压差线性稳压器）
 * @note  可选模式：仅 DC-DC、仅 LDO、或 DC-DC+LDO 混合
 *        使用 LDO 可降低噪声但功耗稍高
 */
#define LLCC68_LORA_DEFAULT_REGULATOR_MODE              LLCC68_REGULATOR_MODE_DC_DC_LDO

/**
 * @brief 功率放大器占空比配置：0x02
 * @note  与 HP_MAX 配合设置输出功率等级
 */
#define LLCC68_LORA_DEFAULT_PA_CONFIG_DUTY_CYCLE        0x02

/**
 * @brief 功率放大器最大高压摆率配置：0x03
 * @note  配合 DUTY_CYCLE 设置发射功率为 +17dBm
 */
#define LLCC68_LORA_DEFAULT_PA_CONFIG_HP_MAX            0x03

/**
 * @brief 发射功率：+17dBm（50mW）
 * @note  取值范围：-9dBm 到 +22dBm
 *        更高功率 = 更远传输距离，但功耗更大
 */
#define LLCC68_LORA_DEFAULT_TX_DBM                      17

/**
 * @brief 过流保护阈值：0x38 对应 140mA
 * @note  防止功率放大器过流损坏
 *        计算公式：I_max = 0x38 * 2.5mA + 45mA ≈ 140mA
 */
#define LLCC68_LORA_DEFAULT_OCP                         0x38

/**
 * @brief 发射功率斜坡时间：10 微秒
 * @note  控制功率上升到设定值的时间
 *        较短的斜坡时间可减少前导码开销
 */
#define LLCC68_LORA_DEFAULT_RAMP_TIME                   LLCC68_RAMP_TIME_10US

/*============================================================================*/
/* LoRa 调制参数配置                                                           */
/*============================================================================*/

/**
 * @brief 扩频因子 (SF)：9
 * @note  范围：SF5-SF12，值越大传输距离越远但速率越慢
 *        SF9 = 2^9 = 512 个码片/符号，平衡距离与速率
 */
#define LLCC68_LORA_DEFAULT_SF                          LLCC68_LORA_SF_9

/**
 * @brief 信号带宽：125 kHz
 * @note  可选：125/250 kHz
 *        更大带宽 = 更高速率但更短距离、更低抗干扰性
 */
#define LLCC68_LORA_DEFAULT_BANDWIDTH                   LLCC68_LORA_BANDWIDTH_125_KHZ

/**
 * @brief 前向纠错编码率：4/5
 * @note  可选：4/5, 4/6, 4/7, 4/8
 *        更高编码率 = 更强纠错能力但更低有效数据率
 */
#define LLCC68_LORA_DEFAULT_CR                          LLCC68_LORA_CR_4_5

/**
 * @brief 低速率优化：禁用
 * @note  当 SF≥11 且 BW=125kHz 时建议启用
 *        启用后可提高低速率传输的可靠性
 */
#define LLCC68_LORA_DEFAULT_LOW_DATA_RATE_OPTIMIZE      LLCC68_BOOL_FALSE

/**
 * @brief 射频工作频率：480 MHz
 * @note  LLCC68 支持范围：150-960 MHz
 *        需根据当地法规选择合法频段
 */
#define LLCC68_LORA_DEFAULT_RF_FREQUENCY                480000000U

/*============================================================================*/
/* 数据包格式配置                                                              */
/*============================================================================*/

/**
 * @brief 前导码长度：12 个符号
 * @note  范围：6-65535 个符号
 *        更长前导码提高接收灵敏度但增加开销
 */
#define LLCC68_LORA_DEFAULT_PREAMBLE_LENGTH             12

/**
 * @brief 报头模式：显式报头
 * @note  EXPLICIT：报头包含有效负载长度和编码率
 *        IMPLICIT：报头被省略，需收发双方预先约定
 */
#define LLCC68_LORA_DEFAULT_HEADER                      LLCC68_LORA_HEADER_EXPLICIT

/**
 * @brief 数据缓冲区大小：255 字节
 * @note  LLCC68 最大支持 255 字节有效负载
 */
#define LLCC68_LORA_DEFAULT_BUFFER_SIZE                 255

/**
 * @brief CRC 校验：开启
 * @note  开启后附加 2 字节 CRC 用于数据完整性检查
 *        建议始终开启以确保数据可靠性
 */
#define LLCC68_LORA_DEFAULT_CRC_TYPE                    LLCC68_LORA_CRC_TYPE_ON

/**
 * @brief 同步字：0x3444（公共网络）
 * @note  0x3444 = 公共 LoRaWAN 网络
 *        0x1424 = 私有网络
 *        不同同步字的设备无法互相通信
 */
#define LLCC68_LORA_DEFAULT_SYNC_WORD                   0x3444U

/**
 * @brief IQ 信号反转：禁用
 * @note  LoRaWAN 下行链路需启用 (TRUE)
 *        上行链路需禁用 (FALSE)
 */
#define LLCC68_LORA_DEFAULT_INVERT_IQ                   LLCC68_BOOL_FALSE

/*============================================================================*/
/* 接收与超时配置                                                              */
/*============================================================================*/

/**
 * @brief 检测到前导码时停止定时器：禁用
 * @note  启用后，接收时检测到前导码会重置超时定时器
 *        适用于需要等待完整数据包的场景
 */
#define LLCC68_LORA_DEFAULT_STOP_TIMER_ON_PREAMBLE      LLCC68_BOOL_FALSE

/**
 * @brief 符号数超时：0（禁用）
 * @note  设置接收超时符号数，0 表示无超时
 *        非零值表示在收到指定符号数后停止接收
 */
#define LLCC68_LORA_DEFAULT_SYMB_NUM_TIMEOUT            0

/**
 * @brief 接收增益：0x94（通用增益）
 * @note  控制 LNA（低噪声放大器）增益等级
 *        更高增益提高接收灵敏度但可能增加噪声
 */
#define LLCC68_LORA_DEFAULT_RX_GAIN                     0x94

/*============================================================================*/
/* CAD (信道活动检测) 配置                                                      */
/*============================================================================*/

/**
 * @brief CAD 检测符号数：2 个符号
 * @note  范围：1-4 个符号
 *        更多符号提高检测精度但增加功耗和时间
 */
#define LLCC68_LORA_DEFAULT_CAD_SYMBOL_NUM              LLCC68_LORA_CAD_SYMBOL_NUM_2

/**
 * @brief CAD 检测峰值阈值：24
 * @note  决定检测 LoRa 前导码的灵敏度
 *        值越小越灵敏但可能产生误报
 */
#define LLCC68_LORA_DEFAULT_CAD_DET_PEAK                24

/**
 * @brief CAD 检测最小阈值：10
 * @note  与 PEAK 配合使用，决定检测下限
 *        需根据实际信道噪声环境调整
 */
#define LLCC68_LORA_DEFAULT_CAD_DET_MIN                 10

/*============================================================================*/
/* 启动与唤醒配置                                                              */
/*============================================================================*/

/**
 * @brief 启动模式：热启动
 * @note  COLD：冷启动，重新初始化所有寄存器
 *        WARM：热启动，保留部分配置快速启动
 */
#define LLCC68_LORA_DEFAULT_START_MODE                  LLCC68_START_MODE_WARM

/**
 * @brief RTC 定时唤醒：启用
 * @note  睡眠模式下启用 RTC 可在设定时间后自动唤醒
 *        适用于低功耗定时应用
 */
#define LLCC68_LORA_DEFAULT_RTC_WAKE_UP                 LLCC68_BOOL_TRUE


/**
 * @defgroup llcc68_interface_driver llcc68 interface driver function
 * @brief    llcc68 interface driver modules
 * @ingroup  llcc68_driver
 * @{
 */

/**
 * @brief  interface spi bus init
 * @return status code
 *         - 0 success
 *         - 1 spi init failed
 * @note   none
 */
uint8_t llcc68_interface_spi_init(void);

/**
 * @brief  interface spi bus deinit
 * @return status code
 *         - 0 success
 *         - 1 spi deinit failed
 * @note   none
 */
uint8_t llcc68_interface_spi_deinit(void);

/**
 * @brief      interface spi bus write read
 * @param[in]  *in_buf pointer to a input buffer
 * @param[in]  in_len input length
 * @param[out] *out_buf pointer to a output buffer
 * @param[in]  out_len output length
 * @return     status code
 *             - 0 success
 *             - 1 write read failed
 * @note       none
 */
uint8_t llcc68_interface_spi_write_read(uint8_t* in_buf, uint32_t in_len,
                                        uint8_t* out_buf, uint32_t out_len);

/**
 * @brief  interface reset gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t llcc68_interface_reset_gpio_init(void);

/**
 * @brief  interface reset gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t llcc68_interface_reset_gpio_deinit(void);

/**
 * @brief     interface reset gpio write
 * @param[in] data written data
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t llcc68_interface_reset_gpio_write(uint8_t data);

/**
 * @brief  interface busy gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t llcc68_interface_busy_gpio_init(void);

/**
 * @brief  interface busy gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t llcc68_interface_busy_gpio_deinit(void);

/**
 * @brief      interface busy gpio read
 * @param[out] *value pointer to a value buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t llcc68_interface_busy_gpio_read(uint8_t* value);

/**
 * @brief     interface delay ms
 * @param[in] ms time
 * @note      none
 */
void llcc68_interface_delay_ms(uint32_t ms);

/**
 * @brief     interface print format data
 * @param[in] fmt format data
 * @note      none
 */
void llcc68_interface_debug_print(const char* const fmt, ...);

/**
 * @brief     interface receive callback
 * @param[in] type receive callback type
 * @param[in] *buf pointer to a buffer address
 * @param[in] len buffer length
 * @note      none
 */
void llcc68_interface_receive_callback(uint16_t type, uint8_t* buf, uint16_t len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
