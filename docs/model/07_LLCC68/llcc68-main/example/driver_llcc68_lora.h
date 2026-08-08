/**
 * 版权所有 (c) 2015 - 至今 LibDriver 保留所有权利
 * 
 * MIT 许可证
 *
 * 特此免费授予任何获得本软件及相关文档文件（"软件"）副本的人
 * 处理软件的权利，包括但不限于使用、复制、修改、合并、发布、
 * 分发、再许可和/或销售软件副本的权利，并允许获得软件的人
 * 这样做，但须符合以下条件：
 *
 * 上述版权声明和本许可声明应包含在所有副本或实质性部分中。
 *
 * 本软件按"原样"提供，不提供任何形式的担保，无论是明示还是暗示，
 * 包括但不限于适销性、特定用途适用性和非侵权的担保。在任何情况下，
 * 作者或版权持有者均不对任何索赔、损害或其他责任负责，无论是在合同、
 * 侵权或其他方面的行为，由软件或软件的使用或其他交易引起或与之相关。
 *
 * @file      driver_llcc68_lora.h
 * @brief     LLCC68 LoRa 驱动头文件
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2023-04-15
 *
 * <h3>历史记录</h3>
 * <table>
 * <tr><th>日期        <th>版本  <th>作者        <th>描述
 * <tr><td>2023/04/15  <td>1.0   <td>Shifeng Li  <td>首次上传
 * </table>
 */

#ifndef DRIVER_LLCC68_LORA_H
#define DRIVER_LLCC68_LORA_H

#include "driver_llcc68_interface.h"

#ifdef __cplusplus
extern "C"{
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
 * @brief  LLCC68 LoRa 中断处理函数
 * @return 状态码
 *         - 0 成功
 *         - 1 运行失败
 * @note   无
 */
uint8_t llcc68_lora_irq_handler(void);

/**
 * @brief     LoRa 示例初始化
 * @param[in] *callback 指向回调函数的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 初始化失败
 * @note      无
 */
uint8_t llcc68_lora_init(void (*callback)(uint16_t type, uint8_t *buf, uint16_t len));

/**
 * @brief  LoRa 示例反初始化
 * @return 状态码
 *         - 0 成功
 *         - 1 反初始化失败
 * @note   无
 */
uint8_t llcc68_lora_deinit(void);

/**
 * @brief  LoRa 示例进入连续接收模式
 * @return 状态码
 *         - 0 成功
 *         - 1 进入失败
 * @note   无
 */
uint8_t llcc68_lora_set_continuous_receive_mode(void);

/**
 * @brief     LoRa 示例进入单次接收模式
 * @param[in] us 超时时间，单位为微秒
 * @return    状态码
 *            - 0 成功
 *            - 1 进入失败
 * @note      无
 */
uint8_t llcc68_lora_set_shot_receive_mode(double us);

/**
 * @brief  LoRa 示例进入发送模式
 * @return 状态码
 *         - 0 成功
 *         - 1 进入失败
 * @note   无
 */
uint8_t llcc68_lora_set_send_mode(void);

/**
 * @brief     LoRa 示例发送 LoRa 数据
 * @param[in] *buf 指向数据缓冲区的指针
 * @param[in] len 数据长度
 * @return    状态码
 *            - 0 成功
 *            - 1 发送失败
 * @note      无
 */
uint8_t llcc68_lora_send(uint8_t *buf, uint16_t len);

/**
 * @brief      LoRa 示例运行信道活动检测 (CAD)
 * @param[out] *enable 指向使能状态缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 运行失败
 * @note       无
 */
uint8_t llcc68_lora_run_cad(llcc68_bool_t *enable);

/**
 * @brief      LoRa 示例获取状态
 * @param[out] *rssi 指向 RSSI 缓冲区的指针
 * @param[out] *snr 指向 SNR 缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取状态失败
 * @note       无
 */
uint8_t llcc68_lora_get_status(float *rssi, float *snr);

/**
 * @brief      LoRa 示例检查数据包错误
 * @param[out] *enable 指向布尔值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 检查数据包错误失败
 * @note       无
 */
uint8_t llcc68_lora_check_packet_error(llcc68_bool_t *enable);

/**
 * @brief  LoRa 示例进入睡眠模式
 * @return 状态码
 *         - 0 成功
 *         - 1 睡眠失败
 * @note   无
 */
uint8_t llcc68_lora_sleep(void);

/**
 * @brief  LoRa 示例唤醒芯片
 * @return 状态码
 *         - 0 成功
 *         - 1 唤醒失败
 * @note   无
 */
uint8_t llcc68_lora_wake_up(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
