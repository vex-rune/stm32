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

/*============================================================================*/
/* LLCC68 硬件接口控制宏                                                       */
/*============================================================================*/
/* 以下宏将 LLCC68 所需的所有硬件控制信号(SPI 片选、复位、射频收发切换、BUSY  */
/* 状态读取)统一封装为 GPIO 操作,目的是屏蔽底层硬件差异,使上层驱动只需关注    */
/* 业务逻辑而无需关心具体引脚和寄存器。LLCC68 与 SX126x 系列类似,采用 SPI 总线  */
/* 通信 + 多个辅助控制引脚的架构:                                              */
/*                                                                            */
/*     MCU 主机                    LLCC68 从机                                 */
/*   +-----------+   CS    ---->   +----------+                                */
/*   |           |   SCK   ---->   |          |                                */
/*   |   SPI     |   MOSI  ---->   |  LLCC68  | <---->  射频前端                */
/*   |           |   MISO  <----   |          |                                */
/*   +-----------+   RST   ---->   |          |                                */
/*                 BUSY   <----    |          |                                */
/*                 RXEN   ---->   |          |                                */
/*                 TXEN   ---->   +----------+                                */
/*                                                                            */
/* 注意:所有宏末尾带分号(;),调用时不能再加分号,否则会出现 "; ;" 双分号错误。    */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* SPI 片选(CS / NSS)控制                                                    */
/*----------------------------------------------------------------------------*/
/**
 * @brief  SPI 片选拉低:选中 LLCC68,启动一次 SPI 通信事务
 * @note   为什么需要这样操作:
 *         - SPI 总线是"片选+时钟"协议,多个从机可共享 SCK/MOSI/MISO
 *         - CS 拉低 (低电平有效)告知 LLCC68"现在开始和你通信",其余从机忽略总线
 *         - 不操作 CS 直接收发将导致总线冲突,所有从机同时响应,数据全部错乱
 *         - CS 必须先于 SCK 第一个时钟边沿之前拉低,并在最后一个时钟边沿之后保持
 *           一定时间再拉高(满足 tCS_high ≥ 100ns 的时序要求)
 *
 *         什么情况需要这样操作:
 *         - 任何 SPI 读写操作(寄存器写、寄存器读、固件烧录、状态查询等)之前
 *         - 必须在调用 HAL_SPI_Transmit / HAL_SPI_Receive 之前执行
 *         - 与 CS_HIGH 配对使用,成对出现在每次 SPI 事务的起止处
 *
 *         注意事项:
 *         - 不要在 CS 拉低后立即拉高,需等待 SPI 传输完全结束
 *         - 若 SPI 传输中途异常退出,务必拉高 CS 防止总线被永久占用
 */
#define CS_LOW HAL_GPIO_WritePin( lora_cs_GPIO_Port, lora_cs_Pin,GPIO_PIN_RESET);

/**
 * @brief  SPI 片选拉高:释放 LLCC68,结束当前 SPI 通信事务
 * @note   为什么需要这样操作:
 *         - LLCC68 内部根据 CS 上升沿判定"一次命令结束",开始解析此前收到的字节
 *         - 长期不释放 CS 会让 LLCC68 持续等待更多字节,无法进入命令处理流程
 *         - 释放 CS 后,MISO 引脚进入高阻态,避免与其他从机冲突
 *
 *         什么情况需要这样操作:
 *         - 每次 SPI 读写完成后必须执行,告知从机本次通信结束
 *         - SPI 传输失败(超时/错误)时也必须执行,防止死锁
 *         - 多任务/中断环境下,即使暂时不收发也要拉高 CS 释放总线
 *
 *         注意事项:
 *         - 若 CS 未及时拉高,LLCC68 会一直处于等待状态,BUSY 引脚保持高电平
 *         - 同一函数内有多个 return 路径时,每个 return 前都应确保 CS_HIGH
 */
#define CS_HIGH HAL_GPIO_WritePin( lora_cs_GPIO_Port, lora_cs_Pin,GPIO_PIN_SET);

/*----------------------------------------------------------------------------*/
/* 硬件复位(RESET)控制                                                       */
/*----------------------------------------------------------------------------*/
/**
 * @brief  硬件复位触发:拉低 RESET 引脚,强制 LLCC68 复位
 * @note   为什么需要这样操作:
 *         - 拉低 RESET ≥ 100us 后再拉高,可对 LLCC68 进行完整的硬件复位
 *         - 复位后所有寄存器恢复默认状态,固件上下文丢失,需重新配置
 *         - 硬件复位是"救命手段":当 LLCC68 进入异常状态(命令无响应、BUSY
 *           长时间拉高、通信完全失效)时,软件复位无法恢复,只能硬件复位
 *
 *         什么情况需要这样操作:
 *         - 系统上电初始化时(部分应用需要延迟一段时间再拉高,确保电源稳定)
 *         - 软件复位(llcc68_reset)无效时的最终恢复手段
 *         - 固件升级前后(确保从干净状态启动)
 *         - 通信长时间卡死、SPI 返回错误码且 BUSY 持续为高时
 *
 *         注意事项:
 *         - RESET 引脚具有内部上拉,默认处于高电平(非复位状态)
 *         - 拉低时间必须 ≥ 100us,推荐 1ms 以保证稳定
 *         - 复位后必须重新调用 llcc68_init() 等初始化序列
 */
#define RST_LOW HAL_GPIO_WritePin( lora_rst_GPIO_Port, lora_rst_Pin,GPIO_PIN_RESET);

/**
 * @brief  释放复位:拉高 RESET 引脚,使 LLCC68 退出复位状态
 * @note   为什么需要这样操作:
 *         - 拉低 RESET 期间 LLCC68 处于复位状态,SPI 通信被忽略
 *         - 必须先拉低一段时间(>100us)再拉高,才能产生有效的复位脉冲
 *         - 拉高后 LLCC68 开始内部引导(BUSY 会拉高),约几 ms 后 BUSY 拉低
 *           此时才能开始正常的 SPI 通信
 *
 *         什么情况需要这样操作:
 *         - 与 RST_LOW 配对使用,组成完整的复位脉冲
 *         - 通常流程:RST_LOW -> 延时 1~10ms -> RST_HIGH -> 等待 BUSY 变低
 *
 *         注意事项:
 *         - 拉高后不要立即操作 SPI,需先轮询 BUSY_READ 等待 BUSY 拉低
 *         - 若 BUSY 长时间不拉低(>100ms),说明芯片异常,需检查电源与硬件连接
 */
#define RST_HIGH HAL_GPIO_WritePin( lora_rst_GPIO_Port, lora_rst_Pin,GPIO_PIN_SET);

/*----------------------------------------------------------------------------*/
/* 射频收发通道切换(RXEN / TXEN)控制                                          */
/*----------------------------------------------------------------------------*/
/**
 * @brief  接收通道关闭(RXEN 拉低)
 * @note   为什么需要这样操作:
 *         - 外部射频前端(RF Switch / FEM)需要 MCU 控制信号来切换收发通道
 *         - RXEN/TXEN 是"互斥"信号:同一时刻只能有一个为高电平(或组合方式
 *           由具体硬件原理图决定),避免发射功率泄漏到接收机烧毁 LNA
 *         - RXEN 拉低表示关闭接收通道,使能发射通道(配合 TXEN_HIGH)
 *
 *         什么情况需要这样操作:
 *         - 准备发射数据前,必须先关闭 RX,打开 TX
 *         - 在 lora_send() 入口处与 TXEN_HIGH 配合使用
 *
 *         注意事项:
 *         - 不同硬件方案 RXEN 极性可能相反,需参考原理图调整宏实现
 *         - TX/RX 切换后建议延时几十微秒,等待射频开关稳定后再发送 SPI 命令
 */
#define RXEN_LOW HAL_GPIO_WritePin( lora_rxen_GPIO_Port, lora_rxen_Pin,GPIO_PIN_RESET);

/**
 * @brief  接收通道使能(RXEN 拉高)
 * @note   为什么需要这样操作:
 *         - 拉高 RXEN 使外部射频开关切换到接收通路,信号从天线进入 LLCC68
 *         - 在进入连续接收或单次接收前必须执行,否则接收不到任何数据
 *
 *         什么情况需要这样操作:
 *         - 调用 llcc68_continuous_receive() / llcc68_single_receive() 之前
 *         - 从发送模式切回接收模式时
 *
 *         注意事项:
 *         - 必须确保 TXEN 处于关闭状态,避免收发冲突
 *         - 切换后需延时等待射频开关稳定(典型 20~100us)
 */
#define RXEN_HIGH HAL_GPIO_WritePin( lora_rxen_GPIO_Port, lora_rxen_Pin,GPIO_PIN_SET);

/**
 * @brief  发射通道关闭(TXEN 拉低)
 * @note   为什么需要这样操作:
 *         - 关闭发射通道,准备进入接收状态
 *         - 与 RXEN_HIGH 配对使用,完成 RX/TX 状态切换
 *
 *         什么情况需要这样操作:
 *         - 从发送完成切回接收模式时
 *         - 接收数据前确保 TX 已关闭,防止 PA 残余信号干扰接收
 */
#define TXEN_LOW HAL_GPIO_WritePin( lora_txen_GPIO_Port, lora_txen_Pin,GPIO_PIN_RESET);

/**
 * @brief  发射通道使能(TXEN 拉高)
 * @note   为什么需要这样操作:
 *         - 拉高 TXEN 使外部射频开关切换到发射通路,PA 连接到天线
 *         - 在发送数据前必须执行,否则信号无法到达天线
 *
 *         什么情况需要这样操作:
 *         - 调用 llcc68_lora_send() 之前(参见 lora.c 的 lora_send 函数)
 *         - 与 RXEN_LOW 配对使用,完成收发切换
 *
 *         注意事项:
 *         - TX 完成后应及时切换回 RX 模式(关闭 TXEN,使能 RXEN)
 *         - 长时间使能 TXEN 会消耗额外电流
 *         - 切换后需延时等待射频开关稳定
 */
#define TXEN_HIGH HAL_GPIO_WritePin( lora_txen_GPIO_Port, lora_txen_Pin,GPIO_PIN_SET);

/*----------------------------------------------------------------------------*/
/* BUSY 状态读取                                                              */
/*----------------------------------------------------------------------------*/
/**
 * @brief  读取 BUSY 引脚电平,获取 LLCC68 内部状态
 * @note   为什么需要这样操作:
 *         - LLCC68 通过 BUSY 引脚主动通知 MCU 其当前工作状态
 *         - BUSY = 1(高电平):芯片正在处理上一条命令,无法接受新命令
 *           此时 MCU 必须等待,否则新命令会被丢弃或导致通信错误
 *         - BUSY = 0(低电平):芯片空闲,可以发送下一条 SPI 命令
 *         - LLCC68 的所有 SPI 命令(读寄存器、写寄存器、SetTx、SetRx 等)前
 *           都必须先确认 BUSY 为 0,这是芯片通信协议的核心约束
 *
 *         什么情况需要这样操作:
 *         - 任何 SPI 通信发起前,必须轮询 BUSY 直到为低
 *         - 复位(RST_HIGH)后必须等待 BUSY 拉低才能继续操作
 *         - 长命令(如 SetTx 参数较多、固件烧录)执行后需轮询 BUSY 确认完成
 *         - 接收/发送过程中也需关注 BUSY 变化,作为状态切换依据
 *
 *         注意事项:
 *         - BUSY_READ 是表达式,使用前不要加括号调用,如:while(BUSY_READ);
 *         - 建议加上超时机制(轮询超时则复位芯片),避免死循环
 *         - BUSY 长时间为高(>100ms)通常表示芯片异常,需考虑硬件复位
 */
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
