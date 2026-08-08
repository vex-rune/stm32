/**
 * 版权所有 (c) 2015 - 至今 LibDriver 保留所有权利
 *
 * MIT 许可证 (MIT)
 *
 * 特此免费授予任何获得本软件及相关文档文件（以下简称"软件"）副本的人，
 * 不受限制地处理本软件，包括但不限于使用、复制、修改、合并、发布、分发、
 * 再授权和/或销售软件副本的权利，并允许接受本软件的人员在符合以下条件
 * 的情况下这样做：
 *
 * 上述版权声明和本许可声明应包含在软件的所有副本或重要部分中。
 *
 * 本软件按"原样"提供，不提供任何形式的明示或暗示担保，包括但不限于
 * 适销性、特定用途适用性和非侵权性的担保。在任何情况下，作者或版权
 * 持有人均不对任何索赔、损害或其他责任负责，无论是在合同、侵权或其他
 * 行为中，由软件或软件的使用或其他交易引起、产生或与之相关。
 *
 * @file      driver_llcc68.h
 * @brief     llcc68 驱动头文件
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2023-04-15
 *
 * <h3>修订历史</h3>
 * <table>
 * <tr><th>日期        <th>版本    <th>作者         <th>说明
 * <tr><td>2023/04/15  <td>1.0     <td>Shifeng Li  <td>首次上传
 * </table>
 */

#ifndef DRIVER_LLCC68_H
#define DRIVER_LLCC68_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup llcc68_driver llcc68 驱动函数
 * @brief    llcc68 驱动模块
 * @{
 */

/**
 * @addtogroup llcc68_command_driver
 * @{
 */

/**
 * @brief llcc68 布尔枚举定义
 */
typedef enum
{
    LLCC68_BOOL_FALSE = 0x00,        /**< 禁用功能 */
    LLCC68_BOOL_TRUE  = 0x01,        /**< 启用功能 */
} llcc68_bool_t;

/**
 * @brief llcc68 启动模式枚举定义
 */
typedef enum
{
    LLCC68_START_MODE_COLD = 0x00,        /**< 冷启动模式 */
    LLCC68_START_MODE_WARM = 0x01,        /**< 热启动模式 */
} llcc68_start_mode_t;

/**
 * @brief llcc68 时钟源枚举定义
 */
typedef enum
{
    LLCC68_CLOCK_SOURCE_RC_13M     = 0x00,        /**< 内部 RC 13MHz 振荡器 */
    LLCC68_CLOCK_SOURCE_XTAL_32MHZ = 0x01,        /**< 外部晶振 32MHz */
} llcc68_clock_source_t;

/**
 * @brief llcc68 稳压器模式枚举定义
 */
typedef enum
{
    LLCC68_REGULATOR_MODE_ONLY_LDO  = 0x00,        /**< 所有模式下仅使用 LDO */
    LLCC68_REGULATOR_MODE_DC_DC_LDO = 0x01,        /**< 在 standby_xosc、fs、rx 和 rx 模式下使用 DC_DC + LDO */
} llcc68_regulator_mode_t;

/**
 * @brief llcc68 校准枚举定义
 */
typedef enum
{
    LLCC68_CALIBRATION_RC64K      = (1 << 0),        /**< rc64k 校准 */
    LLCC68_CALIBRATION_RC13M      = (1 << 1),        /**< rc13M 校准 */
    LLCC68_CALIBRATION_PLL        = (1 << 2),        /**< pll 校准 */
    LLCC68_CALIBRATION_ADC_PULSE  = (1 << 3),        /**< adc 脉冲校准 */
    LLCC68_CALIBRATION_ADC_BULK_N = (1 << 4),        /**< adc 批量 N 校准 */
    LLCC68_CALIBRATION_ADC_BULK_P = (1 << 5),        /**< adc 批量 P 校准 */
    LLCC68_CALIBRATION_IMAGE      = (1 << 6),        /**< 镜像校准 */
} llcc68_calibration_t;

/**
 * @brief llcc68 中断枚举定义
 */
typedef enum
{
    LLCC68_IRQ_TX_DONE           = (1 << 0),        /**< 数据包发送完成 */
    LLCC68_IRQ_RX_DONE           = (1 << 1),        /**< 数据包接收完成 */
    LLCC68_IRQ_PREAMBLE_DETECTED = (1 << 2),        /**< 检测到前导码 */
    LLCC68_IRQ_SYNC_WORD_VALID   = (1 << 3),        /**< 检测到有效同步字 */
    LLCC68_IRQ_HEADER_VALID      = (1 << 4),        /**< 数据包头有效 */
    LLCC68_IRQ_HEADER_ERR        = (1 << 5),        /**< 数据包头错误 */
    LLCC68_IRQ_CRC_ERR           = (1 << 6),        /**< CRC 校验错误 */
    LLCC68_IRQ_CAD_DONE          = (1 << 7),        /**< 信道活动检测完成 */
    LLCC68_IRQ_CAD_DETECTED      = (1 << 8),        /**< 信道活动检测到信号 */
    LLCC68_IRQ_TIMEOUT           = (1 << 9),        /**< 超时 */
} llcc68_irq_t;

/**
 * @brief llcc68 接收发送回退模式枚举定义
 */
typedef enum
{
    LLCC68_RX_TX_FALLBACK_MODE_FS         = 0x40,        /**< 收发完成后进入频率合成 (FS) 模式 */
    LLCC68_RX_TX_FALLBACK_MODE_STDBY_XOSC = 0x30,        /**< 收发完成后进入 standby_xosc 模式 */
    LLCC68_RX_TX_FALLBACK_MODE_STDBY_RC   = 0x20,        /**< 收发完成后进入 standby_rc 模式 */
} llcc68_rx_tx_fallback_mode_t;

/**
 * @brief llcc68 TCXO 电压枚举定义
 */
typedef enum
{
    LLCC68_TCXO_VOLTAGE_1P6V = 0x00,        /**< dio3 输出 1.6 V 为 TCXO 供电 */
    LLCC68_TCXO_VOLTAGE_1P7V = 0x01,        /**< dio3 输出 1.7 V 为 TCXO 供电 */
    LLCC68_TCXO_VOLTAGE_1P8V = 0x02,        /**< dio3 输出 1.8 V 为 TCXO 供电 */
    LLCC68_TCXO_VOLTAGE_2P2V = 0x03,        /**< dio3 输出 2.2 V 为 TCXO 供电 */
    LLCC68_TCXO_VOLTAGE_2P4V = 0x04,        /**< dio3 输出 2.4 V 为 TCXO 供电 */
    LLCC68_TCXO_VOLTAGE_2P7V = 0x05,        /**< dio3 输出 2.7 V 为 TCXO 供电 */
    LLCC68_TCXO_VOLTAGE_3P0V = 0x06,        /**< dio3 输出 3.0 V 为 TCXO 供电 */
    LLCC68_TCXO_VOLTAGE_3P3V = 0x07,        /**< dio3 输出 3.3 V 为 TCXO 供电 */
} llcc68_tcxo_voltage_t;

/**
 * @brief llcc68 数据包类型枚举定义
 */
typedef enum
{
    LLCC68_PACKET_TYPE_GFSK = 0x00,        /**< GFSK 调制模式 */
    LLCC68_PACKET_TYPE_LORA = 0x01,        /**< LoRa 调制模式 */
} llcc68_packet_type_t;

/**
 * @brief llcc68 发射斜率时间枚举定义
 */
typedef enum
{
    LLCC68_RAMP_TIME_10US   = 0x00,        /**< 10 微秒 */
    LLCC68_RAMP_TIME_20US   = 0x01,        /**< 20 微秒 */
    LLCC68_RAMP_TIME_40US   = 0x02,        /**< 40 微秒 */
    LLCC68_RAMP_TIME_80US   = 0x03,        /**< 80 微秒 */
    LLCC68_RAMP_TIME_200US  = 0x04,        /**< 200 微秒 */
    LLCC68_RAMP_TIME_800US  = 0x05,        /**< 800 微秒 */
    LLCC68_RAMP_TIME_1700US = 0x06,        /**< 1700 微秒 */
    LLCC68_RAMP_TIME_3400US = 0x07,        /**< 3400 微秒 */
} llcc68_ramp_time_t;

/**
 * @brief llcc68 GFSK 脉冲整形枚举定义
 */
typedef enum
{
    LLCC68_GFSK_PULSE_SHAPE_NO_FILTER       = 0x00,        /**< 不使用滤波器 */
    LLCC68_GFSK_PULSE_SHAPE_GAUSSIAN_BT_0P3 = 0x08,        /**< 高斯 BT 0.3 滤波器 */
    LLCC68_GFSK_PULSE_SHAPE_GAUSSIAN_BT_0P5 = 0x09,        /**< 高斯 BT 0.5 滤波器 */
    LLCC68_GFSK_PULSE_SHAPE_GAUSSIAN_BT_0P7 = 0x0A,        /**< 高斯 BT 0.7 滤波器 */
    LLCC68_GFSK_PULSE_SHAPE_GAUSSIAN_BT_1   = 0x0B,        /**< 高斯 BT 1.0 滤波器 */
} llcc68_gfsk_pulse_shape_t;

/**
 * @brief llcc68 GFSK 带宽枚举定义
 */
typedef enum
{
    LLCC68_GFSK_BANDWIDTH_4P8_KHZ   = 0x1F,        /**< 4.8 kHz */
    LLCC68_GFSK_BANDWIDTH_5P8_KHZ   = 0x17,        /**< 5.8 kHz */
    LLCC68_GFSK_BANDWIDTH_7P3_KHZ   = 0x0F,        /**< 7.3 kHz */
    LLCC68_GFSK_BANDWIDTH_9P7_KHZ   = 0x1E,        /**< 9.7 kHz */
    LLCC68_GFSK_BANDWIDTH_11P7_KHZ  = 0x16,        /**< 11.7 kHz */
    LLCC68_GFSK_BANDWIDTH_14P6_KHZ  = 0x0E,        /**< 14.6 kHz */
    LLCC68_GFSK_BANDWIDTH_19P5_KHZ  = 0x1D,        /**< 19.5 kHz */
    LLCC68_GFSK_BANDWIDTH_23P4_KHZ  = 0x15,        /**< 23.4 kHz */
    LLCC68_GFSK_BANDWIDTH_29P3_KHZ  = 0x0D,        /**< 29.3 kHz */
    LLCC68_GFSK_BANDWIDTH_39_KHZ    = 0x1C,        /**< 39 kHz */
    LLCC68_GFSK_BANDWIDTH_46P9_KHZ  = 0x14,        /**< 46.9 kHz */
    LLCC68_GFSK_BANDWIDTH_58P6_KHZ  = 0x0C,        /**< 58.6 kHz */
    LLCC68_GFSK_BANDWIDTH_78P2_KHZ  = 0x1B,        /**< 78.2 kHz */
    LLCC68_GFSK_BANDWIDTH_93P8_KHZ  = 0x13,        /**< 93.8 kHz */
    LLCC68_GFSK_BANDWIDTH_117P3_KHZ = 0x0B,        /**< 117.3 kHz */
    LLCC68_GFSK_BANDWIDTH_156P2_KHZ = 0x1A,        /**< 156.2 kHz */
    LLCC68_GFSK_BANDWIDTH_187P2_KHZ = 0x12,        /**< 187.2 kHz */
    LLCC68_GFSK_BANDWIDTH_232P3_KHZ = 0x0A,        /**< 232.3 kHz */
    LLCC68_GFSK_BANDWIDTH_312_KHZ   = 0x19,        /**< 312 kHz */
    LLCC68_GFSK_BANDWIDTH_373P6_KHZ = 0x11,        /**< 373.6 kHz */
    LLCC68_GFSK_BANDWIDTH_467_KHZ   = 0x09,        /**< 467 kHz */
} llcc68_gfsk_bandwidth_t;

/**
 * @brief llcc68 LoRa 扩频因子枚举定义
 */
typedef enum
{
    LLCC68_LORA_SF_5  = 0x05,        /**< 扩频因子 5 */
    LLCC68_LORA_SF_6  = 0x06,        /**< 扩频因子 6 */
    LLCC68_LORA_SF_7  = 0x07,        /**< 扩频因子 7 */
    LLCC68_LORA_SF_8  = 0x08,        /**< 扩频因子 8 */
    LLCC68_LORA_SF_9  = 0x09,        /**< 扩频因子 9 */
    LLCC68_LORA_SF_10 = 0x0A,        /**< 扩频因子 10 */
    LLCC68_LORA_SF_11 = 0x0B,        /**< 扩频因子 11 */
} llcc68_lora_sf_t;

/**
 * @brief llcc68 LoRa 带宽枚举定义
 */
typedef enum
{
    LLCC68_LORA_BANDWIDTH_125_KHZ   = 0x04,        /**< 125 kHz */
    LLCC68_LORA_BANDWIDTH_250_KHZ   = 0x05,        /**< 250 kHz */
    LLCC68_LORA_BANDWIDTH_500_KHZ   = 0x06,        /**< 500 kHz */
} llcc68_lora_bandwidth_t;

/**
 * @brief llcc68 LoRa 编码率枚举定义
 */
typedef enum
{
    LLCC68_LORA_CR_4_5 = 0x01,        /**< 编码率 4/5 */
    LLCC68_LORA_CR_4_6 = 0x02,        /**< 编码率 4/6 */
    LLCC68_LORA_CR_4_7 = 0x03,        /**< 编码率 4/7 */
    LLCC68_LORA_CR_4_8 = 0x04,        /**< 编码率 4/8 */
} llcc68_lora_cr_t;

/**
 * @brief llcc68 GFSK 前导码检测器长度枚举定义
 */
typedef enum
{
    LLCC68_GFSK_PREAMBLE_DETECTOR_LENGTH_OFF     = 0x00,        /**< 前导码检测器关闭 */
    LLCC68_GFSK_PREAMBLE_DETECTOR_LENGTH_8_BITS  = 0x04,        /**< 前导码检测器长度 8 位 */
    LLCC68_GFSK_PREAMBLE_DETECTOR_LENGTH_16_BITS = 0x05,        /**< 前导码检测器长度 16 位 */
    LLCC68_GFSK_PREAMBLE_DETECTOR_LENGTH_24_BITS = 0x06,        /**< 前导码检测器长度 24 位 */
    LLCC68_GFSK_PREAMBLE_DETECTOR_LENGTH_32_BITS = 0x07,        /**< 前导码检测器长度 32 位 */
} llcc68_gfsk_preamble_detector_length_t;

/**
 * @brief llcc68 GFSK 地址过滤枚举定义
 */
typedef enum
{
    LLCC68_GFSK_ADDR_FILTER_DISABLE                       = 0x00,        /**< 禁用地址过滤 */
    LLCC68_GFSK_ADDR_FILTER_ACTIVATED_NODE_ADDR           = 0x01,        /**< 启用节点地址过滤 */
    LLCC68_GFSK_ADDR_FILTER_ACTIVATED_NODE_BROADCAST_ADDR = 0x02,        /**< 启用节点地址及广播地址过滤 */
} llcc68_gfsk_addr_filter_t;

/**
 * @brief llcc68 GFSK 数据包类型枚举定义
 */
typedef enum
{
    LLCC68_GFSK_PACKET_TYPE_KNOWN_LENGTH  = 0x00,        /**< 收发双方已知数据包长度，数据包中不含负载长度字段 */
    LLCC68_GFSK_PACKET_TYPE_VARIABLE_SIZE = 0x01,        /**< 可变长度数据包，负载的第一个字节表示数据包大小 */
} llcc68_gfsk_packet_type_t;

/**
 * @brief llcc68 GFSK CRC 类型枚举定义
 */
typedef enum
{
    LLCC68_GFSK_CRC_TYPE_OFF        = 0x01,        /**< 不使用 CRC */
    LLCC68_GFSK_CRC_TYPE_1_BYTE     = 0x00,        /**< 对 1 字节进行 CRC 计算 */
    LLCC68_GFSK_CRC_TYPE_2_BYTE     = 0x02,        /**< 对 2 字节进行 CRC 计算 */
    LLCC68_GFSK_CRC_TYPE_1_BYTE_INV = 0x04,        /**< 对 1 字节进行 CRC 计算并取反 */
    LLCC68_GFSK_CRC_TYPE_2_BYTE_INV = 0x06,        /**< 对 2 字节进行 CRC 计算并取反 */
} llcc68_gfsk_crc_type_t;

/**
 * @brief llcc68 LoRa 数据包头枚举定义
 */
typedef enum
{
    LLCC68_LORA_HEADER_EXPLICIT = 0x00,        /**< 显式数据包头 */
    LLCC68_LORA_HEADER_IMPLICIT = 0x01,        /**< 隐式数据包头 */
} llcc68_lora_header_t;

/**
 * @brief llcc68 LoRa CRC 类型枚举定义
 */
typedef enum
{
    LLCC68_LORA_CRC_TYPE_OFF = 0x00,        /**< 关闭 CRC */
    LLCC68_LORA_CRC_TYPE_ON  = 0x01,        /**< 开启 CRC */
} llcc68_lora_crc_type_t;

/**
 * @brief llcc68 LoRa 信道活动检测符号数量枚举定义
 */
typedef enum
{
    LLCC68_LORA_CAD_SYMBOL_NUM_1  = 0x00,        /**< 1 个符号 */
    LLCC68_LORA_CAD_SYMBOL_NUM_2  = 0x01,        /**< 2 个符号 */
    LLCC68_LORA_CAD_SYMBOL_NUM_4  = 0x02,        /**< 4 个符号 */
    LLCC68_LORA_CAD_SYMBOL_NUM_8  = 0x03,        /**< 8 个符号 */
    LLCC68_LORA_CAD_SYMBOL_NUM_16 = 0x04,        /**< 16 个符号 */
} llcc68_lora_cad_symbol_num_t;

/**
 * @brief llcc68 LoRa 信道活动检测退出模式枚举定义
 */
typedef enum
{
    LLCC68_LORA_CAD_EXIT_MODE_ONLY = 0x00,        /**< 检测完成后无论信道活动如何，芯片都返回 STBY_RC 模式 */
    LLCC68_LORA_CAD_EXIT_MODE_RX   = 0x01,        /**< 芯片执行 CAD 操作，若检测到活动则保持 RX 状态，
                                                       直到接收到数据包或计时器达到由 cad timeout * 15.625 us 定义的时长 */
} llcc68_lora_cad_exit_mode_t;

/**
 * @brief llcc68 FSK 接收状态枚举定义
 */
typedef enum
{
    LLCC68_FSK_RX_STATUS_PREAMBLE_ERR = (1 << 7),        /**< 前导码错误 */
    LLCC68_FSK_RX_STATUS_SYNC_ERR     = (1 << 6),        /**< 同步字错误 */
    LLCC68_FSK_RX_STATUS_ADDR_ERR     = (1 << 5),        /**< 地址错误 */
    LLCC68_FSK_RX_STATUS_CRC_ERR      = (1 << 4),        /**< CRC 校验错误 */
    LLCC68_FSK_RX_STATUS_LENGTH_ERR   = (1 << 3),        /**< 长度错误 */
    LLCC68_FSK_RX_STATUS_ABORT_ERR    = (1 << 2),        /**< 中止错误 */
    LLCC68_FSK_RX_STATUS_PKT_RECEIVED = (1 << 1),        /**< 数据包已接收 */
    LLCC68_FSK_RX_STATUS_PKT_SEND     = (1 << 0),        /**< 数据包已发送 */
} llcc68_fsk_rx_status_t;

/**
 * @brief llcc68 操作错误枚举定义
 */
typedef enum
{
    LLCC68_OP_ERROR_PA_RAMP_ERR     = (1 << 8),        /**< PA 斜坡失败 */
    LLCC68_OP_ERROR_PLL_LOCK_ERR    = (1 << 6),        /**< PLL 锁定失败 */
    LLCC68_OP_ERROR_XOSC_START_ERR  = (1 << 5),        /**< XOSC 启动失败 */
    LLCC68_OP_ERROR_IMG_CALIB_ERR   = (1 << 4),        /**< 镜像校准失败 */
    LLCC68_OP_ERROR_ADC_CALIB_ERR   = (1 << 3),        /**< ADC 校准失败 */
    LLCC68_OP_ERROR_PLL_CALIB_ERR   = (1 << 2),        /**< PLL 校准失败 */
    LLCC68_OP_ERROR_RC13M_CALIB_ERR = (1 << 1),        /**< RC13M 校准失败 */
    LLCC68_OP_ERROR_RC64K_CALIB_ERR = (1 << 0),        /**< RC64K 校准失败 */
} llcc68_op_error_t;

/**
 * @brief llcc68 句柄结构定义
 */
typedef struct llcc68_handle_s
{
    uint8_t (*reset_gpio_init)(void);                                     /**< 指向 reset_gpio_init 函数的指针 */
    uint8_t (*reset_gpio_deinit)(void);                                   /**< 指向 reset_gpio_deinit 函数的指针 */
    uint8_t (*reset_gpio_write)(uint8_t value);                           /**< 指向 reset_gpio_write 函数的指针 */
    uint8_t (*busy_gpio_init)(void);                                      /**< 指向 busy_gpio_init 函数的指针 */
    uint8_t (*busy_gpio_deinit)(void);                                    /**< 指向 busy_gpio_deinit 函数的指针 */
    uint8_t (*busy_gpio_read)(uint8_t *value);                            /**< 指向 busy_gpio_read 函数的指针 */
    uint8_t (*spi_init)(void);                                            /**< 指向 spi_init 函数的指针 */
    uint8_t (*spi_deinit)(void);                                          /**< 指向 spi_deinit 函数的指针 */
    uint8_t (*spi_write_read)(uint8_t *in_buf, uint32_t in_len,
                              uint8_t *out_buf, uint32_t out_len);        /**< 指向 spi_write_read 函数的指针 */
    void (*delay_ms)(uint32_t ms);                                        /**< 指向 delay_ms 函数的指针 */
    void (*debug_print)(const char *const fmt, ...);                      /**< 指向 debug_print 函数的指针 */
    void (*receive_callback)(uint16_t type,
                             uint8_t *buf, uint16_t len);                 /**< 指向 receive_callback 函数的指针 */
    uint8_t inited;                                                       /**< 初始化标志 */
    uint8_t tx_done;                                                      /**< 发送完成标志 */
    uint8_t cad_done;                                                     /**< 信道活动检测完成标志 */
    uint8_t cad_detected;                                                 /**< 信道活动检测到信号标志 */
    uint8_t crc_error;                                                    /**< CRC 校验错误标志 */
    uint8_t timeout;                                                      /**< 超时标志 */
    uint8_t buf[384];                                                     /**< 内部缓冲区 */
    uint8_t receive_buf[256];                                             /**< 接收缓冲区 */
} llcc68_handle_t;

/**
 * @brief llcc68 信息结构定义
 */
typedef struct llcc68_info_s
{
    char chip_name[32];                /**< 芯片名称 */
    char manufacturer_name[32];        /**< 制造商名称 */
    char interface[8];                 /**< 芯片接口名称 */
    float supply_voltage_min_v;        /**< 芯片最小供电电压 */
    float supply_voltage_max_v;        /**< 芯片最大供电电压 */
    float max_current_ma;              /**< 芯片最大工作电流 */
    float temperature_min;             /**< 芯片最低工作温度 */
    float temperature_max;             /**< 芯片最高工作温度 */
    uint32_t driver_version;           /**< 驱动版本 */
} llcc68_info_t;

/**
 * @}
 */

/**
 * @defgroup llcc68_link_driver llcc68 链接驱动函数
 * @brief    llcc68 链接驱动模块
 * @ingroup  llcc68_driver
 * @{
 */

/**
 * @brief     初始化 llcc68_handle_t 结构
 * @param[in] HANDLE 指向 llcc68 句柄结构的指针
 * @param[in] STRUCTURE llcc68_handle_t 结构
 * @note      无
 */
#define DRIVER_LLCC68_LINK_INIT(HANDLE, STRUCTURE)                memset(HANDLE, 0, sizeof(STRUCTURE))

/**
 * @brief     链接 spi_init 函数
 * @param[in] HANDLE 指向 llcc68 句柄结构的指针
 * @param[in] FUC 指向 spi_init 函数的地址
 * @note      无
 */
#define DRIVER_LLCC68_LINK_SPI_INIT(HANDLE, FUC)                  (HANDLE)->spi_init = FUC

/**
 * @brief     链接 spi_deinit 函数
 * @param[in] HANDLE 指向 llcc68 句柄结构的指针
 * @param[in] FUC 指向 spi_deinit 函数的地址
 * @note      无
 */
#define DRIVER_LLCC68_LINK_SPI_DEINIT(HANDLE, FUC)                (HANDLE)->spi_deinit = FUC

/**
 * @brief     链接 spi_write_read 函数
 * @param[in] HANDLE 指向 llcc68 句柄结构的指针
 * @param[in] FUC 指向 spi_write_read 函数的地址
 * @note      无
 */
#define DRIVER_LLCC68_LINK_SPI_WRITE_READ(HANDLE, FUC)            (HANDLE)->spi_write_read = FUC

/**
 * @brief     链接 reset_gpio_init 函数
 * @param[in] HANDLE 指向 llcc68 句柄结构的指针
 * @param[in] FUC 指向 reset_gpio_init 函数的地址
 * @note      无
 */
#define DRIVER_LLCC68_LINK_RESET_GPIO_INIT(HANDLE, FUC)           (HANDLE)->reset_gpio_init = FUC

/**
 * @brief     链接 reset_gpio_deinit 函数
 * @param[in] HANDLE 指向 llcc68 句柄结构的指针
 * @param[in] FUC 指向 reset_gpio_deinit 函数的地址
 * @note      无
 */
#define DRIVER_LLCC68_LINK_RESET_GPIO_DEINIT(HANDLE, FUC)         (HANDLE)->reset_gpio_deinit = FUC

/**
 * @brief     链接 reset_gpio_write 函数
 * @param[in] HANDLE 指向 llcc68 句柄结构的指针
 * @param[in] FUC 指向 reset_gpio_write 函数的地址
 * @note      无
 */
#define DRIVER_LLCC68_LINK_RESET_GPIO_WRITE(HANDLE, FUC)          (HANDLE)->reset_gpio_write = FUC

/**
 * @brief     链接 busy_gpio_init 函数
 * @param[in] HANDLE 指向 llcc68 句柄结构的指针
 * @param[in] FUC 指向 busy_gpio_init 函数的地址
 * @note      无
 */
#define DRIVER_LLCC68_LINK_BUSY_GPIO_INIT(HANDLE, FUC)            (HANDLE)->busy_gpio_init = FUC

/**
 * @brief     链接 busy_gpio_deinit 函数
 * @param[in] HANDLE 指向 llcc68 句柄结构的指针
 * @param[in] FUC 指向 busy_gpio_deinit 函数的地址
 * @note      无
 */
#define DRIVER_LLCC68_LINK_BUSY_GPIO_DEINIT(HANDLE, FUC)          (HANDLE)->busy_gpio_deinit = FUC

/**
 * @brief     链接 busy_gpio_read 函数
 * @param[in] HANDLE 指向 llcc68 句柄结构的指针
 * @param[in] FUC 指向 busy_gpio_read 函数的地址
 * @note      无
 */
#define DRIVER_LLCC68_LINK_BUSY_GPIO_READ(HANDLE, FUC)            (HANDLE)->busy_gpio_read = FUC

/**
 * @brief     链接 delay_ms 函数
 * @param[in] HANDLE 指向 llcc68 句柄结构的指针
 * @param[in] FUC 指向 delay_ms 函数的地址
 * @note      无
 */
#define DRIVER_LLCC68_LINK_DELAY_MS(HANDLE, FUC)                  (HANDLE)->delay_ms = FUC

/**
 * @brief     链接 debug_print 函数
 * @param[in] HANDLE 指向 llcc68 句柄结构的指针
 * @param[in] FUC 指向 debug_print 函数的地址
 * @note      无
 */
#define DRIVER_LLCC68_LINK_DEBUG_PRINT(HANDLE, FUC)               (HANDLE)->debug_print = FUC

/**
 * @brief     链接 receive_callback 函数
 * @param[in] HANDLE 指向 llcc68 句柄结构的指针
 * @param[in] FUC 指向 receive_callback 函数的地址
 * @note      无
 */
#define DRIVER_LLCC68_LINK_RECEIVE_CALLBACK(HANDLE, FUC)          (HANDLE)->receive_callback = FUC

/**
 * @}
 */

/**
 * @defgroup llcc68_command_driver llcc68 命令驱动函数
 * @brief    llcc68 命令驱动模块
 * @ingroup  llcc68_driver
 * @{
 */

/**
 * @brief      获取芯片信息
 * @param[out] *info 指向 llcc68 信息结构的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 handle 为空
 * @note       无
 */
uint8_t llcc68_info(llcc68_info_t *info);

/**
 * @brief     中断处理函数
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 运行失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 * @note      无
 */
uint8_t llcc68_irq_handler(llcc68_handle_t *handle);

/**
 * @brief     初始化芯片
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 spi 初始化失败
 *            - 2 handle 为空
 *            - 3 链接函数为空
 *            - 4 reset gpio 初始化失败
 *            - 5 busy gpio 初始化失败
 *            - 6 复位芯片失败
 * @note      无
 */
uint8_t llcc68_init(llcc68_handle_t *handle);

/**
 * @brief     关闭芯片
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 spi 去初始化失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 *            - 5 掉电失败
 *            - 6 busy gpio 去初始化失败
 *            - 7 reset gpio 去初始化失败
 * @note      无
 */
uint8_t llcc68_deinit(llcc68_handle_t *handle);

/**
 * @brief     发送 LoRa 数据
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] standby_src 时钟源
 * @param[in] preamble_length 前导码长度
 * @param[in] header_type 数据包头类型
 * @param[in] crc_type CRC 类型
 * @param[in] invert_iq_enable 布尔值，是否反转 IQ
 * @param[in] *buf 指向数据缓冲区的指针
 * @param[in] len 数据长度
 * @param[in] us 超时时间
 * @return    状态码
 *            - 0 成功
 *            - 1 发送失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 *            - 5 发送超时
 *            - 6 中断超时
 *            - 7 未知结果
 * @note      无
 */
uint8_t llcc68_lora_transmit(llcc68_handle_t *handle, llcc68_clock_source_t standby_src,
                             uint16_t preamble_length, llcc68_lora_header_t header_type,
                             llcc68_lora_crc_type_t crc_type, llcc68_bool_t invert_iq_enable,
                             uint8_t *buf, uint16_t len, uint32_t us);

/**
 * @brief      执行信道活动检测
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *enable 指向使能缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 lora cad 失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 *             - 5 cad 超时
 * @note       无
 */
uint8_t llcc68_lora_cad(llcc68_handle_t *handle, llcc68_bool_t *enable);

/**
 * @brief     进入单次接收模式
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] us 接收超时时间
 * @return    状态码
 *            - 0 成功
 *            - 1 lora 单次接收失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_single_receive(llcc68_handle_t *handle, double us);

/**
 * @brief     进入连续接收模式
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 lora 连续接收失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_continuous_receive(llcc68_handle_t *handle);

/**
 * @brief     写入寄存器
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] reg 寄存器地址
 * @param[in] *buf 指向数据缓冲区的指针
 * @param[in] len 数据长度
 * @return    状态码
 *            - 0 成功
 *            - 1 写寄存器失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_write_register(llcc68_handle_t *handle, uint16_t reg, uint8_t *buf, uint16_t len);

/**
 * @brief      读取寄存器
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[in]  reg 寄存器地址
 * @param[out] *buf 指向数据缓冲区的指针
 * @param[in]  len 数据长度
 * @return     状态码
 *             - 0 成功
 *             - 1 读寄存器失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_read_register(llcc68_handle_t *handle, uint16_t reg, uint8_t *buf, uint16_t len);

/**
 * @brief     写入缓冲区
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] offset 缓冲区偏移
 * @param[in] *buf 指向数据缓冲区的指针
 * @param[in] len 数据长度
 * @return    状态码
 *            - 0 成功
 *            - 1 写缓冲区失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_write_buffer(llcc68_handle_t *handle, uint8_t offset, uint8_t *buf, uint16_t len);

/**
 * @brief      读取缓冲区
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[in]  offset 缓冲区偏移
 * @param[out] *buf 指向数据缓冲区的指针
 * @param[in]  len 数据长度
 * @return     状态码
 *             - 0 成功
 *             - 1 读缓冲区失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_read_buffer(llcc68_handle_t *handle, uint8_t offset, uint8_t *buf, uint16_t len);

/**
 * @brief      检查数据包错误
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *enable 指向布尔缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 * @note       无
 */
uint8_t llcc68_check_packet_error(llcc68_handle_t *handle, llcc68_bool_t *enable);

/**
 * @brief     进入睡眠模式
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] mode 启动模式
 * @param[in] rtc_wake_up_enable 布尔值，是否启用 RTC 唤醒
 * @return    状态码
 *            - 0 成功
 *            - 1 设置睡眠失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_sleep(llcc68_handle_t *handle, llcc68_start_mode_t mode, llcc68_bool_t rtc_wake_up_enable);

/**
 * @brief     进入待机模式
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] src 时钟源
 * @return    状态码
 *            - 0 成功
 *            - 1 设置待机失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_standby(llcc68_handle_t *handle, llcc68_clock_source_t src);

/**
 * @brief     进入频率合成模式
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 设置频率合成失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_frequency_synthesis(llcc68_handle_t *handle);

/**
 * @brief     进入发送模式
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] timeout 发送超时时间
 * @return    状态码
 *            - 0 成功
 *            - 1 设置发送失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      0x000000 表示无超时，单次发送模式
 */
uint8_t llcc68_set_tx(llcc68_handle_t *handle, uint32_t timeout);

/**
 * @brief     进入接收模式
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] timeout 接收超时时间
 * @return    状态码
 *            - 0 成功
 *            - 1 设置接收失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      0x000000 表示无超时，单次接收模式
 *           0xFFFFFF 表示连续接收模式
 */
uint8_t llcc68_set_rx(llcc68_handle_t *handle, uint32_t timeout);

/**
 * @brief      将超时时间转换为寄存器原始数据
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[in]  us 超时时间
 * @param[out] *reg 指向寄存器原始数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 * @note       无
 */
uint8_t llcc68_timeout_convert_to_register(llcc68_handle_t *handle, double us, uint32_t *reg);

/**
 * @brief      将寄存器原始数据转换为超时时间
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[in]  reg 寄存器原始数据
 * @param[out] *us 指向 us 时间缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 * @note       无
 */
uint8_t llcc68_timeout_convert_to_data(llcc68_handle_t *handle, uint32_t reg, double *us);

/**
 * @brief     在前导码上停止定时器
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] enable 布尔值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置前导码停止定时失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_stop_timer_on_preamble(llcc68_handle_t *handle, llcc68_bool_t enable);

/**
 * @brief     设置接收占空比
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] rx_period 接收周期
 * @param[in] sleep_period 睡眠周期
 * @return    状态码
 *            - 0 成功
 *            - 1 设置接收占空比失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      t_preamble + t_header <= 2 * rx_period + sleep_period
 */
uint8_t llcc68_set_rx_duty_cycle(llcc68_handle_t *handle, uint32_t rx_period, uint32_t sleep_period);

/**
 * @brief     执行信道活动检测
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 CAD 失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_cad(llcc68_handle_t *handle);

/**
 * @brief     进入发射连续波模式
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 设置发射连续波失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_tx_continuous_wave(llcc68_handle_t *handle);

/**
 * @brief     进入发射无限前导码模式
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 设置发射无限前导码失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_tx_infinite_preamble(llcc68_handle_t *handle);

/**
 * @brief     设置稳压器模式
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] mode 稳压器模式
 * @return    状态码
 *            - 0 成功
 *            - 1 设置稳压器模式失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_regulator_mode(llcc68_handle_t *handle, llcc68_regulator_mode_t mode);

/**
 * @brief     设置校准参数
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] settings 校准参数
 * @return    状态码
 *            - 0 成功
 *            - 1 设置校准失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_calibration(llcc68_handle_t *handle, uint8_t settings);

/**
 * @brief     设置镜像校准频率
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] freq1 镜像频率 1
 * @param[in] freq2 镜像频率 2
 * @return    状态码
 *            - 0 成功
 *            - 1 设置镜像校准失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_calibration_image(llcc68_handle_t *handle, uint8_t freq1, uint8_t freq2);

/**
 * @brief     设置 PA 配置
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] pa_duty_cycle PA 占空比
 * @param[in] hp_max 最大功率
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 PA 配置失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_pa_config(llcc68_handle_t *handle, uint8_t pa_duty_cycle, uint8_t hp_max);

/**
 * @brief     设置收发回退模式
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] mode 收发回退模式
 * @return    状态码
 *            - 0 成功
 *            - 1 设置收发回退模式失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_rx_tx_fallback_mode(llcc68_handle_t *handle, llcc68_rx_tx_fallback_mode_t mode);

/**
 * @brief     设置 dio 中断参数
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] irq_mask 中断屏蔽位
 * @param[in] dio1_mask dio1 屏蔽位
 * @param[in] dio2_mask dio2 屏蔽位
 * @param[in] dio3_mask dio3 屏蔽位
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 dio 中断参数失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_dio_irq_params(llcc68_handle_t *handle, uint16_t irq_mask, uint16_t dio1_mask,
                                  uint16_t dio2_mask, uint16_t dio3_mask);

/**
 * @brief      获取中断状态
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *status 指向状态缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取中断状态失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_irq_status(llcc68_handle_t *handle, uint16_t *status);

/**
 * @brief     清除中断状态
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] clear_irq_param 中断屏蔽位
 * @return    状态码
 *            - 0 成功
 *            - 1 清除中断状态失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_clear_irq_status(llcc68_handle_t *handle, uint16_t clear_irq_param);

/**
 * @brief     设置 dio2 作为 RF 开关控制
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] enable 布尔值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 dio2 作为 RF 开关控制失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_dio2_as_rf_switch_ctrl(llcc68_handle_t *handle, llcc68_bool_t enable);

/**
 * @brief     设置 dio3 作为 TCXO 控制
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] voltage TCXO 电压
 * @param[in] delay TCXO 控制延时
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 dio3 作为 TCXO 控制失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_dio3_as_tcxo_ctrl(llcc68_handle_t *handle, llcc68_tcxo_voltage_t voltage, uint32_t delay);

/**
 * @brief      将频率转换为寄存器原始数据
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[in]  freq 频率
 * @param[out] *reg 指向寄存器原始数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 * @note       无
 */
uint8_t llcc68_frequency_convert_to_register(llcc68_handle_t *handle, uint32_t freq, uint32_t *reg);

/**
 * @brief      将寄存器原始数据转换为频率
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[in]  reg 寄存器原始数据
 * @param[out] *freq 指向频率缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 * @note       无
 */
uint8_t llcc68_frequency_convert_to_data(llcc68_handle_t *handle, uint32_t reg, uint32_t *freq);

/**
 * @brief     设置 RF 频率
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] reg RF 频率寄存器数据
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 RF 频率失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_rf_frequency(llcc68_handle_t *handle, uint32_t reg);

/**
 * @brief     设置数据包类型
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] type 数据包类型
 * @return    状态码
 *            - 0 成功
 *            - 1 设置数据包类型失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_packet_type(llcc68_handle_t *handle, llcc68_packet_type_t type);

/**
 * @brief      获取数据包类型
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *type 指向数据包类型缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取数据包类型失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_packet_type(llcc68_handle_t *handle, llcc68_packet_type_t *type);

/**
 * @brief     设置发射参数
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] dbm RF 功率
 * @param[in] t 斜率时间
 * @return    状态码
 *            - 0 成功
 *            - 1 设置发射参数失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_tx_params(llcc68_handle_t *handle, int8_t dbm, llcc68_ramp_time_t t);

/**
 * @brief     设置 GFSK 模式的调制参数
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] br 比特率
 * @param[in] shape 脉冲整形
 * @param[in] bw 带宽
 * @param[in] fdev 频率偏移
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 gfsk 调制参数失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_gfsk_modulation_params(llcc68_handle_t *handle, uint32_t br, llcc68_gfsk_pulse_shape_t shape,
                                          llcc68_gfsk_bandwidth_t bw, uint32_t fdev);

/**
 * @brief      将比特率转换为寄存器原始数据
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[in]  br 比特率
 * @param[out] *reg 指向寄存器原始数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 * @note       无
 */
uint8_t llcc68_gfsk_bit_rate_convert_to_register(llcc68_handle_t *handle, uint32_t br, uint32_t *reg);

/**
 * @brief      将寄存器原始数据转换为比特率
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[in]  reg 寄存器原始数据
 * @param[out] *br 指向比特率缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 * @note       无
 */
uint8_t llcc68_gfsk_bit_rate_convert_to_data(llcc68_handle_t *handle, uint32_t reg, uint32_t *br);

/**
 * @brief      将频率偏移转换为寄存器原始数据
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[in]  freq 频率偏移
 * @param[out] *reg 指向寄存器原始数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 * @note       无
 */
uint8_t llcc68_gfsk_frequency_deviation_convert_to_register(llcc68_handle_t *handle, uint32_t freq, uint32_t *reg);

/**
 * @brief      将寄存器原始数据转换为频率偏移
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[in]  reg 寄存器原始数据
 * @param[out] *freq 指向频率偏移缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 * @note       无
 */
uint8_t llcc68_gfsk_frequency_deviation_convert_to_data(llcc68_handle_t *handle, uint32_t reg, uint32_t *freq);

/**
 * @brief     设置 LoRa 模式的调制参数
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] sf 扩频因子
 * @param[in] bw 带宽
 * @param[in] low_data_rate_optimize_enable 布尔值，是否启用低数据速率优化
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 lora 调制参数失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_lora_modulation_params(llcc68_handle_t *handle, llcc68_lora_sf_t sf, llcc68_lora_bandwidth_t bw,
                                          llcc68_lora_cr_t cr, llcc68_bool_t low_data_rate_optimize_enable);

/**
 * @brief     设置 GFSK 模式的数据包参数
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] preamble_length 前导码长度
 * @param[in] detector_length 前导码检测器长度
 * @param[in] sync_word_length 同步字长度
 * @param[in] filter 地址过滤
 * @param[in] packet_type 数据包类型
 * @param[in] payload_length 负载长度
 * @param[in] crc_type CRC 类型
 * @param[in] whitening_enable 布尔值，是否启用白化
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 gfsk 数据包参数失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 *            - 5 同步字长度超过 0x40
 * @note      无
 */
uint8_t llcc68_set_gfsk_packet_params(llcc68_handle_t *handle, uint16_t preamble_length,
                                      llcc68_gfsk_preamble_detector_length_t detector_length,
                                      uint8_t sync_word_length, llcc68_gfsk_addr_filter_t filter,
                                      llcc68_gfsk_packet_type_t packet_type, uint8_t payload_length,
                                      llcc68_gfsk_crc_type_t crc_type, llcc68_bool_t whitening_enable);

/**
 * @brief     设置 LoRa 模式的数据包参数
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] preamble_length 前导码长度
 * @param[in] header_type 数据包头类型
 * @param[in] payload_length 负载长度
 * @param[in] crc_type CRC 类型
 * @param[in] invert_iq_enable 布尔值，是否反转 IQ
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 lora 数据包参数失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_lora_packet_params(llcc68_handle_t *handle, uint16_t preamble_length,
                                      llcc68_lora_header_t header_type, uint8_t payload_length,
                                      llcc68_lora_crc_type_t crc_type, llcc68_bool_t invert_iq_enable);

/**
 * @brief     设置 CAD 参数
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] num CAD 符号数量
 * @param[in] cad_det_peak CAD 检测峰值
 * @param[in] cad_det_min 最小 CAD 检测峰值
 * @param[in] mode CAD 退出模式
 * @param[in] timeout CAD 超时
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 CAD 参数失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_cad_params(llcc68_handle_t *handle, llcc68_lora_cad_symbol_num_t num,
                              uint8_t cad_det_peak, uint8_t cad_det_min, llcc68_lora_cad_exit_mode_t mode,
                              uint32_t timeout);

/**
 * @brief     设置缓冲区基地址
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] tx_base_addr 发送缓冲区基地址
 * @param[in] rx_base_addr 接收缓冲区基地址
 * @return    状态码
 *            - 0 成功
 *            - 1 设置缓冲区基地址失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_buffer_base_address(llcc68_handle_t *handle, uint8_t tx_base_addr, uint8_t rx_base_addr);

/**
 * @brief     设置 LoRa 符号数量超时
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] symb_num 符号数量
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 lora 符号超时失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_lora_symb_num_timeout(llcc68_handle_t *handle, uint8_t symb_num);

/**
 * @brief      获取状态
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *status 指向状态缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取状态失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_status(llcc68_handle_t *handle, uint8_t *status);

/**
 * @brief      获取接收缓冲区状态
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *payload_length_rx 指向接收负载长度缓冲区的指针
 * @param[out] *rx_start_buffer_pointer 指向接收起始指针缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取接收缓冲区状态失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_rx_buffer_status(llcc68_handle_t *handle, uint8_t *payload_length_rx, uint8_t *rx_start_buffer_pointer);

/**
 * @brief      获取 GFSK 模式的数据包状态
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *rx_status 指向接收状态缓冲区的指针
 * @param[out] *rssi_sync_raw 指向 rssi 同步原始值缓冲区的指针
 * @param[out] *rssi_avg_raw 指向 rssi 平均原始值缓冲区的指针
 * @param[out] *rssi_sync 指向 rssi 同步值缓冲区的指针
 * @param[out] *rssi_avg 指向 rssi 平均值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 gfsk 数据包状态失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_gfsk_packet_status(llcc68_handle_t *handle, uint8_t *rx_status, uint8_t *rssi_sync_raw,
                                      uint8_t *rssi_avg_raw, float *rssi_sync, float *rssi_avg);

/**
 * @brief      获取 LoRa 模式的数据包状态
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *rssi_pkt_raw 指向 rssi 数据包原始值缓冲区的指针
 * @param[out] *snr_pkt_raw 指向 snr 数据包原始值缓冲区的指针
 * @param[out] *signal_rssi_pkt_raw 指向信号 rssi 数据包原始值缓冲区的指针
 * @param[out] *rssi_pkt 指向 rssi 数据包缓冲区的指针
 * @param[out] *snr_pkt 指向 snr 数据包缓冲区的指针
 * @param[out] *signal_rssi_pkt 指向信号 rssi 数据包缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 lora 数据包状态失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_lora_packet_status(llcc68_handle_t *handle, uint8_t *rssi_pkt_raw, int8_t *snr_pkt_raw,
                                      uint8_t *signal_rssi_pkt_raw, float *rssi_pkt, float *snr_pkt, float *signal_rssi_pkt);

/**
 * @brief      获取瞬时 RSSI
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *rssi_inst_raw 指向 rssi 瞬时原始值缓冲区的指针
 * @param[out] *rssi_inst 指向 rssi 瞬时值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取瞬时 RSSI 失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_instantaneous_rssi(llcc68_handle_t *handle, uint8_t *rssi_inst_raw, float *rssi_inst);

/**
 * @brief      获取统计信息
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *pkt_received 指向数据包已接收计数缓冲区的指针
 * @param[out] *pkt_crc_error 指向数据包 CRC 错误缓冲区的指针
 * @param[out] *pkt_length_header_error 指向数据包长度头错误缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取统计信息失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_stats(llcc68_handle_t *handle, uint16_t *pkt_received, uint16_t *pkt_crc_error, uint16_t *pkt_length_header_error);

/**
 * @brief     重置统计信息
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] pkt_received 已接收数据包计数器屏蔽位
 * @param[in] pkt_crc_error 数据包 CRC 错误屏蔽位
 * @param[in] pkt_length_header_error 数据包长度头错误屏蔽位
 * @return    状态码
 *            - 0 成功
 *            - 1 重置统计信息失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_reset_stats(llcc68_handle_t *handle, uint16_t pkt_received, uint16_t pkt_crc_error, uint16_t pkt_length_header_error);

/**
 * @brief      获取设备错误
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *op_error 指向操作错误缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取设备错误失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_device_errors(llcc68_handle_t *handle, uint16_t *op_error);

/**
 * @brief     清除设备错误
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 清除设备错误失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_clear_device_errors(llcc68_handle_t *handle);

/**
 * @}
 */

/**
 * @defgroup llcc68_register_driver llcc68 寄存器驱动函数
 * @brief    llcc68 寄存器驱动模块
 * @ingroup  llcc68_driver
 * @{
 */

/**
 * @brief     设置 FSK 模式下的白化初始值
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] value 设置值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 fsk 白化初始值失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_fsk_whitening_initial_value(llcc68_handle_t *handle, uint16_t value);

/**
 * @brief      获取 FSK 模式下的白化初始值
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *value 指向数值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 fsk 白化初始值失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_fsk_whitening_initial_value(llcc68_handle_t *handle, uint16_t *value);

/**
 * @brief     设置 FSK 模式下的 CRC 初始值
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] value 设置值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 fsk CRC 初始值失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_fsk_crc_initical_value(llcc68_handle_t *handle, uint16_t value);

/**
 * @brief      获取 FSK 模式下的 CRC 初始值
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *value 指向数值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 fsk CRC 初始值失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_fsk_crc_initical_value(llcc68_handle_t *handle, uint16_t *value);

/**
 * @brief     设置 FSK 模式下的 CRC 多项式值
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] value 设置值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 fsk CRC 多项式值失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_fsk_crc_polynomial_value(llcc68_handle_t *handle, uint16_t value);

/**
 * @brief      获取 FSK 模式下的 CRC 多项式值
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *value 指向数值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 fsk CRC 多项式值失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_fsk_crc_polynomial_value(llcc68_handle_t *handle, uint16_t *value);

/**
 * @brief     设置 FSK 模式下的同步字
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] *sync_word 指向同步字缓冲区的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 fsk 同步字失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_fsk_sync_word(llcc68_handle_t *handle, uint8_t sync_word[8]);

/**
 * @brief      获取 FSK 模式下的同步字
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *sync_word 指向同步字缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 fsk 同步字失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_fsk_sync_word(llcc68_handle_t *handle, uint8_t sync_word[8]);

/**
 * @brief     设置 FSK 模式下的节点地址
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] addr 节点地址
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 fsk 节点地址失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_fsk_node_address(llcc68_handle_t *handle, uint8_t addr);

/**
 * @brief      获取 FSK 模式下的节点地址
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *addr 指向节点地址缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 fsk 节点地址失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_fsk_node_address(llcc68_handle_t *handle, uint8_t *addr);

/**
 * @brief     设置 FSK 模式下的广播地址
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] addr 广播地址
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 fsk 广播地址失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_fsk_broadcast_address(llcc68_handle_t *handle, uint8_t addr);

/**
 * @brief      获取 FSK 模式下的广播地址
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *addr 指向广播地址缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 fsk 广播地址失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_fsk_broadcast_address(llcc68_handle_t *handle, uint8_t *addr);

/**
 * @brief     设置 IQ 极性
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] setup 设置
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 IQ 极性失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_iq_polarity(llcc68_handle_t *handle, uint8_t setup);

/**
 * @brief      获取 IQ 极性
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *setup 指向设置缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 IQ 极性失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_iq_polarity(llcc68_handle_t *handle, uint8_t *setup);

/**
 * @brief     设置 LoRa 同步字
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] sync_word 同步字
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 lora 同步字失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_lora_sync_word(llcc68_handle_t *handle, uint16_t sync_word);

/**
 * @brief      获取 LoRa 同步字
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *sync_word 指向同步字缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 lora 同步字失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_lora_sync_word(llcc68_handle_t *handle, uint16_t *sync_word);

/**
 * @brief      获取随机数
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *r 指向随机数缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取随机数失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_random_number(llcc68_handle_t *handle, uint32_t *r);

/**
 * @brief     设置发射调制
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] modulation 发射调制
 * @return    状态码
 *            - 0 成功
 *            - 1 设置发射调制失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_tx_modulation(llcc68_handle_t *handle, uint8_t modulation);

/**
 * @brief      获取发射调制
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *modulation 指向发射调制缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取发射调制失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_tx_modulation(llcc68_handle_t *handle, uint8_t *modulation);

/**
 * @brief     设置接收增益
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] gain 接收增益
 * @return    状态码
 *            - 0 成功
 *            - 1 设置接收增益失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_rx_gain(llcc68_handle_t *handle, uint8_t gain);

/**
 * @brief      获取接收增益
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *gain 指向接收增益缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取接收增益失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_rx_gain(llcc68_handle_t *handle, uint8_t *gain);

/**
 * @brief     设置发射钳位配置
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] config 发射钳位配置
 * @return    状态码
 *            - 0 成功
 *            - 1 设置发射钳位配置失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_tx_clamp_config(llcc68_handle_t *handle, uint8_t config);

/**
 * @brief      获取发射钳位配置
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *config 指向发射钳位配置缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取发射钳位配置失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_tx_clamp_config(llcc68_handle_t *handle, uint8_t *config);

/**
 * @brief     设置 OCP
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] ocp 过流保护级别
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 ocp 失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_ocp(llcc68_handle_t *handle, uint8_t ocp);

/**
 * @brief      获取 OCP
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *ocp 指向过流保护级别缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 ocp 失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_ocp(llcc68_handle_t *handle, uint8_t *ocp);

/**
 * @brief     设置 RTC 控制
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] control RTC 控制
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 RTC 控制失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_rtc_control(llcc68_handle_t *handle, uint8_t control);

/**
 * @brief      获取 RTC 控制
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *control 指向 RTC 控制缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 RTC 控制失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_rtc_control(llcc68_handle_t *handle, uint8_t *control);

/**
 * @brief     设置 XTA 修调
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] trim XTA 修调
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 XTA 修调失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_xta_trim(llcc68_handle_t *handle, uint8_t trim);

/**
 * @brief      获取 XTA 修调
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *trim 指向 XTA 修调缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 XTA 修调失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_xta_trim(llcc68_handle_t *handle, uint8_t *trim);

/**
 * @brief     设置 XTB 修调
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] trim XTB 修调
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 XTB 修调失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_xtb_trim(llcc68_handle_t *handle, uint8_t trim);

/**
 * @brief      获取 XTB 修调
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *trim 指向 XTB 修调缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 XTB 修调失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_xtb_trim(llcc68_handle_t *handle, uint8_t *trim);

/**
 * @brief     设置 dio3 输出
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] control dio3 输出控制
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 dio3 输出控制失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_dio3_output_control(llcc68_handle_t *handle, uint8_t control);

/**
 * @brief      获取 dio3 输出
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *control 指向 dio3 输出控制缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 dio3 输出控制失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_dio3_output_control(llcc68_handle_t *handle, uint8_t *control);

/**
 * @brief     设置事件屏蔽
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] mask 事件屏蔽
 * @return    状态码
 *            - 0 成功
 *            - 1 设置事件屏蔽失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_event_mask(llcc68_handle_t *handle, uint8_t mask);

/**
 * @brief      获取事件屏蔽
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *mask 指向事件屏蔽缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取事件屏蔽失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_event_mask(llcc68_handle_t *handle, uint8_t *mask);

/**
 * @brief     设置 dio 输出使能
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] enable 使能设置
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 dio 输出使能失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_dio_output_enable(llcc68_handle_t *handle, uint8_t enable);

/**
 * @brief      获取 dio 输出使能
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *enable 指向使能设置缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 dio 输出使能失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_dio_output_enable(llcc68_handle_t *handle, uint8_t *enable);

/**
 * @brief     设置 dio 输入使能
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] enable 使能设置
 * @return    状态码
 *            - 0 成功
 *            - 1 设置 dio 输入使能失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_dio_input_enable(llcc68_handle_t *handle, uint8_t enable);

/**
 * @brief      获取 dio 输入使能
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *enable 指向使能设置缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取 dio 输入使能失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_dio_input_enable(llcc68_handle_t *handle, uint8_t *enable);

/**
 * @brief     设置上拉控制
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] control 上拉控制
 * @return    状态码
 *            - 0 成功
 *            - 1 设置上拉控制失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_pull_up_control(llcc68_handle_t *handle, uint8_t control);

/**
 * @brief      获取上拉控制
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *control 指向上拉控制缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取上拉控制失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_pull_up_control(llcc68_handle_t *handle, uint8_t *control);

/**
 * @brief     设置下拉控制
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] control 下拉控制
 * @return    状态码
 *            - 0 成功
 *            - 1 设置下拉控制失败
 *            - 2 handle 为空
 *            - 3 handle 未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_pull_down_control(llcc68_handle_t *handle, uint8_t control);

/**
 * @brief      获取下拉控制
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *control 指向下拉控制缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取下拉控制失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_pull_down_control(llcc68_handle_t *handle, uint8_t *control);

/**
 * @}
 */

/**
 * @defgroup llcc68_extern_driver llcc68 外部驱动函数
 * @brief    llcc68 外部驱动模块
 * @ingroup  llcc68_driver
 * @{
 */

/**
 * @brief      读写寄存器
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[in]  *in_buf 指向输入缓冲区的指针
 * @param[in]  in_len 输入长度
 * @param[out] *out_buf 指向输出缓冲区的指针
 * @param[in]  out_len 输出长度
 * @return     状态码
 *             - 0 成功
 *             - 1 读写失败
 *             - 2 handle 为空
 *             - 3 handle 未初始化
 * @note       无
 */
uint8_t llcc68_write_read_reg(llcc68_handle_t *handle, uint8_t *in_buf, uint32_t in_len,
                              uint8_t *out_buf, uint32_t out_len);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
