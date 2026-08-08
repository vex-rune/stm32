
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
 * @file      driver_llcc68.c
 * @brief     LLCC68 驱动源文件
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

#include "../Inc/driver_llcc68.h"
#include <math.h>

/**
 * @brief 芯片信息定义
 */
#define CHIP_NAME                 "Semtech LLCC68"        /**< 芯片名称 */
#define MANUFACTURER_NAME         "Semtech"               /**< 制造商名称 */
#define SUPPLY_VOLTAGE_MIN        1.8f                    /**< 芯片最小供电电压 */
#define SUPPLY_VOLTAGE_MAX        3.7f                    /**< 芯片最大供电电压 */
#define MAX_CURRENT               118.0f                  /**< 芯片最大电流 */
#define TEMPERATURE_MIN           -40.0f                  /**< 芯片最小工作温度 */
#define TEMPERATURE_MAX           85.0f                   /**< 芯片最大工作温度 */
#define DRIVER_VERSION            1000                    /**< 驱动版本 */

/**
 * @brief 芯片命令定义
 */
#define LLCC68_COMMAND_SET_SLEEP                         0x84        /**< 设置睡眠命令 */
#define LLCC68_COMMAND_SET_STANDBY                       0x80        /**< 设置待机命令 */
#define LLCC68_COMMAND_SET_FS                            0xC1        /**< 设置 FS 命令 */
#define LLCC68_COMMAND_SET_TX                            0x83        /**< 设置发送命令 */
#define LLCC68_COMMAND_SET_RX                            0x82        /**< 设置接收命令 */
#define LLCC68_COMMAND_STOP_TIMER_ON_PREAMBLE            0x9F        /**< 前导码停止定时器命令 */
#define LLCC68_COMMAND_SET_RX_DUTY_CYCLE                 0x94        /**< 设置接收占空比命令 */
#define LLCC68_COMMAND_SET_CAD                           0xC5        /**< 设置 CAD 命令 */
#define LLCC68_COMMAND_SET_TX_CONTINUOUS_WAVE            0xD1        /**< 设置发送连续波命令 */
#define LLCC68_COMMAND_SET_TX_INFINITE_PREAMBLE          0xD2        /**< 设置发送无限前导码命令 */
#define LLCC68_COMMAND_SET_REGULATOR_MODE                0x96        /**< 设置稳压器模式命令 */
#define LLCC68_COMMAND_SET_CALIBRATE                     0x89        /**< 设置校准命令 */
#define LLCC68_COMMAND_SET_CALIBRATE_IMAGE               0x98        /**< 设置镜像校准命令 */
#define LLCC68_COMMAND_SET_PA_CONFIG                     0x95        /**< 设置 PA 配置命令 */
#define LLCC68_COMMAND_SET_RX_TX_FALLBACK_MODE           0x93        /**< 设置收发回退模式命令 */
#define LLCC68_COMMAND_WRITE_REGISTER                    0x0D        /**< 写寄存器命令 */
#define LLCC68_COMMAND_READ_REGISTER                     0x1D        /**< 读寄存器命令 */
#define LLCC68_COMMAND_WRITE_BUFFER                      0x0E        /**< 写缓冲区命令 */
#define LLCC68_COMMAND_READ_BUFFER                       0x1E        /**< 读缓冲区命令 */
#define LLCC68_COMMAND_SET_DIO_IRQ_PARAMS                0x08        /**< 设置 DIO 中断参数命令 */
#define LLCC68_COMMAND_GET_IRQ_STATUS                    0x12        /**< 获取中断状态命令 */
#define LLCC68_COMMAND_CLEAR_IRQ_STATUS                  0x02        /**< 清除中断状态命令 */
#define LLCC68_COMMAND_SET_DIO2_AS_RF_SWITCH_CTRL        0x9D        /**< 设置 DIO2 为射频开关控制命令 */
#define LLCC68_COMMAND_SET_DIO3_AS_TCXO_CTRL             0x97        /**< 设置 DIO3 为 TCXO 控制命令 */
#define LLCC68_COMMAND_SET_RF_FREQUENCY                  0x86        /**< 设置射频频率命令 */
#define LLCC68_COMMAND_SET_PACKET_TYPE                   0x8A        /**< 设置数据包类型命令 */
#define LLCC68_COMMAND_GET_PACKET_TYPE                   0x11        /**< 获取数据包类型命令 */
#define LLCC68_COMMAND_SET_TX_PARAMS                     0x8E        /**< 设置发送参数命令 */
#define LLCC68_COMMAND_SET_MODULATION_PARAMS             0x8B        /**< 设置调制参数命令 */
#define LLCC68_COMMAND_SET_PACKET_PARAMS                 0x8C        /**< 设置数据包参数命令 */
#define LLCC68_COMMAND_SET_CAD_PARAMS                    0x88        /**< 设置 CAD 参数命令 */
#define LLCC68_COMMAND_SET_BUFFER_BASE_ADDRESS           0x8F        /**< 设置缓冲区基地址命令 */
#define LLCC68_COMMAND_SET_LORA_SYMB_NUM_TIMEOUT         0xA0        /**< 设置 LoRa 符号数超时命令 */
#define LLCC68_COMMAND_GET_STATUS                        0xC0        /**< 获取状态命令 */
#define LLCC68_COMMAND_GET_RSSI_LNST                     0x15        /**< 获取瞬时 RSSI 命令 */
#define LLCC68_COMMAND_GET_RX_BUFFER_STATUS              0x13        /**< 获取接收缓冲区状态命令 */
#define LLCC68_COMMAND_GET_PACKET_STATUS                 0x14        /**< 获取数据包状态命令 */
#define LLCC68_COMMAND_GET_DEVICE_ERRORS                 0x17        /**< 获取设备错误命令 */
#define LLCC68_COMMAND_CLEAR_DEVICE_ERRORS               0x07        /**< 清除设备错误命令 */
#define LLCC68_COMMAND_GET_STATS                         0x10        /**< 获取统计命令 */
#define LLCC68_COMMAND_RESET_STATS                       0x00        /**< 重置统计命令 */

/**
 * @brief 芯片寄存器定义
 */
#define LLCC68_REG_DIOX_OUTPUT_ENABLE                    0x0580      /**< DIOx 输出使能寄存器 */
#define LLCC68_REG_DIOX_INPUT_ENABLE                     0x0583      /**< DIOx 输入使能寄存器 */
#define LLCC68_REG_DIOX_PULL_UP_CONTROL                  0x0584      /**< DIOx 上拉控制寄存器 */
#define LLCC68_REG_DIOX_PULL_DOWN_CONTROL                0x0585      /**< DIOx 下拉控制寄存器 */
#define LLCC68_REG_WHITENING_INIT_VALUE_MSB              0x06B8      /**< 白化初始值高字节寄存器 */
#define LLCC68_REG_WHITENING_INIT_VALUE_LSB              0x06B9      /**< 白化初始值低字节寄存器 */
#define LLCC68_REG_CRC_INIT_VALUE_MSB                    0x06BC      /**< CRC 初始值高字节寄存器 */
#define LLCC68_REG_CRC_INIT_VALUE_LSB                    0x06BD      /**< CRC 初始值低字节寄存器 */
#define LLCC68_REG_CRC_POLYNOMIAL_VALUE_MSB              0x06BE      /**< CRC 多项式值高字节寄存器 */
#define LLCC68_REG_CRC_POLYNOMIAL_VALUE_LSB              0x06BF      /**< CRC 多项式值低字节寄存器 */
#define LLCC68_REG_SYNC_WORD_0                           0x06C0      /**< FSK 模式同步字第 1 字节寄存器 */
#define LLCC68_REG_SYNC_WORD_1                           0x06C1      /**< FSK 模式同步字第 2 字节寄存器 */
#define LLCC68_REG_SYNC_WORD_2                           0x06C2      /**< FSK 模式同步字第 3 字节寄存器 */
#define LLCC68_REG_SYNC_WORD_3                           0x06C3      /**< FSK 模式同步字第 4 字节寄存器 */
#define LLCC68_REG_SYNC_WORD_4                           0x06C4      /**< FSK 模式同步字第 5 字节寄存器 */
#define LLCC68_REG_SYNC_WORD_5                           0x06C5      /**< FSK 模式同步字第 6 字节寄存器 */
#define LLCC68_REG_SYNC_WORD_6                           0x06C6      /**< FSK 模式同步字第 7 字节寄存器 */
#define LLCC68_REG_SYNC_WORD_7                           0x06C7      /**< FSK 模式同步字第 8 字节寄存器 */
#define LLCC68_REG_NODE_ADDRESS                          0x06CD      /**< 节点地址寄存器 */
#define LLCC68_REG_BROADCAST_ADDRESS                     0x06CE      /**< 广播地址寄存器 */
#define LLCC68_REG_IQ_POLARITY_SETUP                     0x0736      /**< IQ 极性设置寄存器 */
#define LLCC68_REG_LORA_SYNC_WORD_MSB                    0x0740      /**< LoRa 同步字高字节寄存器 */
#define LLCC68_REG_LORA_SYNC_WORD_LSB                    0x0741      /**< LoRa 同步字低字节寄存器 */
#define LLCC68_REG_RANDOM_NUMBER_GEN_0                   0x0819      /**< 随机数生成器 0 寄存器 */
#define LLCC68_REG_RANDOM_NUMBER_GEN_1                   0x081A      /**< 随机数生成器 1 寄存器 */
#define LLCC68_REG_RANDOM_NUMBER_GEN_2                   0x081B      /**< 随机数生成器 2 寄存器 */
#define LLCC68_REG_RANDOM_NUMBER_GEN_3                   0x081C      /**< 随机数生成器 3 寄存器 */
#define LLCC68_REG_TX_MODULATION                         0x0889      /**< 发送调制寄存器 */
#define LLCC68_REG_RX_GAIN                               0x08AC      /**< 接收增益寄存器 */
#define LLCC68_REG_TX_CLAMP_CONFIG                       0x08D8      /**< 发送钳位配置寄存器 */
#define LLCC68_REG_OCP_CONFIGURATION                     0x08E7      /**< 过流保护配置寄存器 */
#define LLCC68_REG_RTC_CONTROL                           0x0902      /**< RTC 控制寄存器 */
#define LLCC68_REG_XTA_TRIM                              0x0911      /**< XTA 微调寄存器 */
#define LLCC68_REG_XTB_TRIM                              0x0912      /**< XTB 微调寄存器 */
#define LLCC68_REG_DIO3_OUTPUT_CONTROL                   0x0920      /**< DIO3 输出电压控制寄存器 */
#define LLCC68_REG_EVENT_MASK                            0x0944      /**< 事件掩码寄存器 */

/**
 * @brief      读取字节数据
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[in]  reg 寄存器地址
 * @param[out] *buf 指向数据缓冲区的指针
 * @param[in]  len 数据长度
 * @return     状态码
 *             - 0 成功
 *             - 1 SPI 读取失败
 * @note       无
 */
static uint8_t a_llcc68_spi_read(llcc68_handle_t* handle, uint8_t reg, uint8_t* buf, uint16_t len)
{
    if (handle->spi_write_read(&reg, 1, buf, len) != 0) /* spi 读取 */
    {
        return 1; /* 返回错误 */
    }
    else
    {
        return 0; /* 成功返回 0 */
    }
}

/**
 * @brief     write bytes
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] reg register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 spi write failed
 *            - 2 len is over 383
 * @note      none
 */
static uint8_t a_llcc68_spi_write(llcc68_handle_t* handle, uint8_t reg, uint8_t* buf, uint16_t len)
{
    if (len > (384 - 1)) /* check the length */
    {
        handle->debug_print("llcc68: len is over 383.\n"); /* len is over 383 */

        return 2; /* 返回错误 */
    }

    memset(handle->buf, 0, sizeof(uint8_t) * 384); /* clear the buffer */
    handle->buf[0] = reg; /* set the reg */
    memcpy(&handle->buf[1], buf, len); /* copy the buffer */
    if (handle->spi_write_read(handle->buf, len + 1, NULL, 0) != 0) /* spi write */
    {
        return 1; /* 返回错误 */
    }
    else
    {
        return 0; /* success return 0 */
    }
}

/**
 * @brief      read register bytes
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[in]  reg register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 spi read register failed
 *             - 2 len is over 383
 * @note       none
 */
static uint8_t a_llcc68_spi_read_register(llcc68_handle_t* handle, uint16_t reg, uint8_t* buf, uint16_t len)
{
    uint8_t reg_buf[3];

    if (len > (384 - 1)) /* check the length */
    {
        handle->debug_print("llcc68: len is over 383.\n"); /* len is over 383 */

        return 2; /* 返回错误 */
    }

    memset(handle->buf, 0, sizeof(uint8_t) * 384); /* clear the buffer */
    reg_buf[0] = LLCC68_COMMAND_READ_REGISTER; /* set the command */
    reg_buf[1] = (reg >> 8) & 0xFF; /* set msb */
    reg_buf[2] = (reg >> 0) & 0xFF; /* set lsb */
    if (handle->spi_write_read((uint8_t*)reg_buf, 3, handle->buf, len + 1) != 0) /* spi read */
    {
        return 1; /* 返回错误 */
    }
    else
    {
        memcpy(buf, handle->buf + 1, len); /* copy the data */

        return 0; /* success return 0 */
    }
}

/**
 * @brief     写入寄存器字节数据
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] reg 寄存器地址
 * @param[in] *buf 指向数据缓冲区的指针
 * @param[in] len 数据长度
 * @return    状态码
 *            - 0 成功
 *            - 1 SPI 写入寄存器失败
 *            - 2 长度超过 381
 * @note      无
 */
static uint8_t a_llcc68_spi_write_register(llcc68_handle_t* handle, uint16_t reg, uint8_t* buf, uint16_t len)
{
    if (len > (384 - 3)) /* 检查长度 */
    {
        handle->debug_print("llcc68: len is over 381.\n"); /* 长度超过 381 */

        return 2; /* 返回错误 */
    }

    memset(handle->buf, 0, sizeof(uint8_t) * 384); /* 清空缓冲区 */
    handle->buf[0] = LLCC68_COMMAND_WRITE_REGISTER; /* 设置命令 */
    handle->buf[1] = (reg >> 8) & 0xFF; /* 设置寄存器高字节 */
    handle->buf[2] = (reg >> 0) & 0xFF; /* 设置寄存器低字节 */
    memcpy(&handle->buf[3], buf, len); /* 复制数据到缓冲区 */
    if (handle->spi_write_read(handle->buf, len + 3, NULL, 0) != 0) /* spi 写入 */
    {
        return 1; /* 返回错误 */
    }
    else
    {
        return 0; /* 成功返回 0 */
    }
}

/**
 * @brief     写入缓冲区
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] offset 缓冲区偏移量
 * @param[in] *buf 指向数据缓冲区的指针
 * @param[in] len 数据长度
 * @return    状态码
 *            - 0 成功
 *            - 1 SPI 写入缓冲区失败
 *            - 2 长度超过 382
 * @note      无
 */
static uint8_t a_llcc68_spi_write_buffer(llcc68_handle_t* handle, uint8_t offset, uint8_t* buf, uint16_t len)
{
    if (len > (384 - 2)) /* 检查长度 */
    {
        handle->debug_print("llcc68: len is over 382.\n"); /* 长度超过 382 */

        return 2; /* 返回错误 */
    }

    memset(handle->buf, 0, sizeof(uint8_t) * 384); /* 清空缓冲区 */
    handle->buf[0] = LLCC68_COMMAND_WRITE_BUFFER; /* 设置命令 */
    handle->buf[1] = offset; /* 设置偏移量 */
    memcpy(&handle->buf[2], buf, len); /* 复制数据到缓冲区 */
    if (handle->spi_write_read(handle->buf, len + 2, NULL, 0) != 0) /* spi 写入 */
    {
        return 1; /* 返回错误 */
    }
    else
    {
        return 0; /* 成功返回 0 */
    }
}

/**
 * @brief      read buffer
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[in]  offset buffer offset
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 spi read buffer failed
 *             - 2 len is over 383
 * @note      none
 */
static uint8_t a_llcc68_spi_read_buffer(llcc68_handle_t* handle, uint8_t offset, uint8_t* buf, uint16_t len)
{
    uint8_t reg_buf[2];

    if (len > (384 - 1)) /* check the length */
    {
        handle->debug_print("llcc68: len is over 383.\n"); /* len is over 383 */

        return 2; /* 返回错误 */
    }

    memset(handle->buf, 0, sizeof(uint8_t) * 384); /* clear the buffer */
    reg_buf[0] = LLCC68_COMMAND_READ_BUFFER; /* set the command */
    reg_buf[1] = offset; /* set msb */
    if (handle->spi_write_read((uint8_t*)reg_buf, 2, handle->buf, len + 1) != 0) /* spi write */
    {
        return 1; /* 返回错误 */
    }
    else
    {
        memcpy(buf, handle->buf + 1, len); /* copy the data */

        return 0; /* success return 0 */
    }
}

/**
 * @brief     check command busy
 * @param[in] *handle pointer to an llcc68 handle structure
 * @return    status code
 *            - 0 idle
 *            - 1 busy or error
 * @note      none
 */
static uint8_t a_llcc68_check_busy(llcc68_handle_t* handle)
{
    uint8_t level;
    uint8_t timeout;

    timeout = 100; /* set max 100 */

    while (1) /* loop */
    {
        if (handle->busy_gpio_read((uint8_t*)&level) != 0) /* read busy gpio */
        {
            return 1; /* 返回错误 */
        }
        else
        {
            if (level == 0) /* check level */
            {
                return 0; /* success return 0 */
            }
            else if ((level == 1) && (timeout != 0)) /* check level and timeout */
            {
                handle->delay_ms(10); /* delay 10 ms */
                timeout--; /* timeout-- */
                if (timeout != 0) /* check timeout */
                {
                    continue; /* continue */
                }
                else
                {
                    return 1; /* 返回错误 */
                }
            }
            else
            {
                return 1; /* 返回错误 */
            }
        }
    }
}

/**
 * @brief     irq handler
 * @param[in] *handle pointer to an llcc68 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t llcc68_irq_handler(llcc68_handle_t* handle)
{
    uint8_t res;
    uint8_t buf[3];
    uint16_t status;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    memset(buf, 0, sizeof(uint8_t) * 3); /* 清空缓冲区 */
    res = a_llcc68_spi_read(handle, LLCC68_COMMAND_GET_IRQ_STATUS, (uint8_t*)buf, 3); /* 读取命令 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: get irq status failed.\n"); /* 获取中断状态失败 */

        return 1; /* 返回错误 */
    }
    status = ((uint16_t)buf[1] << 8) | buf[2]; /* 设置状态 */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_CLEAR_IRQ_STATUS, (uint8_t*)&buf[1], 2); /* 写入命令 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: clear irq status failed.\n"); /* 清除中断状态失败 */

        return 1; /* 返回错误 */
    }

    handle->crc_error = 0; /* 清除 CRC 错误 */
    if ((status & LLCC68_IRQ_PREAMBLE_DETECTED) != 0) /* if preamble detected */
    {
        if (handle->receive_callback != NULL) /* if receive callback */
        {
            handle->receive_callback(LLCC68_IRQ_PREAMBLE_DETECTED, NULL, 0); /* run callback */
        }
    }
    if ((status & LLCC68_IRQ_SYNC_WORD_VALID) != 0) /* if valid sync word detected */
    {
        if (handle->receive_callback != NULL) /* if receive callback */
        {
            handle->receive_callback(LLCC68_IRQ_SYNC_WORD_VALID, NULL, 0); /* run callback */
        }
    }
    if ((status & LLCC68_IRQ_HEADER_VALID) != 0) /* if valid header */
    {
        if (handle->receive_callback != NULL) /* if receive callback */
        {
            handle->receive_callback(LLCC68_IRQ_HEADER_VALID, NULL, 0); /* run callback */
        }
    }
    if ((status & LLCC68_IRQ_HEADER_ERR) != 0) /* if header error */
    {
        if (handle->receive_callback != NULL) /* if receive callback */
        {
            handle->receive_callback(LLCC68_IRQ_HEADER_ERR, NULL, 0); /* run callback */
        }
    }
    if ((status & LLCC68_IRQ_CRC_ERR) != 0) /* if crc error */
    {
        if (handle->receive_callback != NULL) /* if receive callback */
        {
            handle->receive_callback(LLCC68_IRQ_CRC_ERR, NULL, 0); /* run callback */
        }
        handle->crc_error = 1; /* set crc error */
    }
    if ((status & LLCC68_IRQ_CAD_DONE) != 0) /* if cad done */
    {
        if (handle->receive_callback != NULL) /* if receive callback */
        {
            handle->receive_callback(LLCC68_IRQ_CAD_DONE, NULL, 0); /* run callback */
        }
        handle->cad_done = 1; /* set cad done */
    }
    if ((status & LLCC68_IRQ_CAD_DETECTED) != 0) /* if cad detected */
    {
        if (handle->receive_callback != NULL) /* if receive callback */
        {
            handle->receive_callback(LLCC68_IRQ_CAD_DETECTED, NULL, 0); /* run callback */
        }
        handle->cad_detected = 1; /* set detected */
    }
    if ((status & LLCC68_IRQ_TIMEOUT) != 0) /* if timeout */
    {
        uint8_t control;
        uint8_t mask;

        control = 0x00;
        res = a_llcc68_spi_write_register(handle, LLCC68_REG_RTC_CONTROL, (uint8_t*)&control, 1); /* 写寄存器 */
        if (res != 0) /* 检查结果 */
        {
            handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

            return 1; /* 返回错误 */
        }

        /* read mask */
        res = a_llcc68_spi_read_register(handle, LLCC68_REG_EVENT_MASK, (uint8_t*)&mask, 1); /* 读寄存器 */
        if (res != 0) /* 检查结果 */
        {
            handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

            return 1; /* 返回错误 */
        }
        mask |= 0x02; /* 设置掩码 */
        res = a_llcc68_spi_write_register(handle, LLCC68_REG_EVENT_MASK, (uint8_t*)&mask, 1); /* 写寄存器 */
        if (res != 0) /* 检查结果 */
        {
            handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

            return 1; /* 返回错误 */
        }

        if (handle->receive_callback != NULL) /* if receive callback */
        {
            handle->receive_callback(LLCC68_IRQ_TIMEOUT, NULL, 0); /* run callback */
        }
        handle->timeout = 1; /* flag timeout */
    }
    if ((status & LLCC68_IRQ_TX_DONE) != 0) /* if tx done */
    {
        if (handle->receive_callback != NULL) /* if receive callback */
        {
            handle->receive_callback(LLCC68_IRQ_TX_DONE, NULL, 0); /* run callback */
        }
        handle->tx_done = 1; /* flag tx done */
    }
    if ((status & LLCC68_IRQ_RX_DONE) != 0) /* if rx done */
    {
        uint8_t payload_length_rx;
        uint8_t rx_start_buffer_pointer;

        memset(buf, 0, sizeof(uint8_t) * 3); /* clear the buffer */
        res = a_llcc68_spi_read(handle, LLCC68_COMMAND_GET_RX_BUFFER_STATUS, (uint8_t*)buf, 3); /* read command */
        if (res != 0) /* 检查结果 */
        {
            handle->debug_print("llcc68: get rx buffer status failed.\n"); /* get rx buffer status failed */

            return 1; /* 返回错误 */
        }
        payload_length_rx = buf[1]; /* set status */
        rx_start_buffer_pointer = buf[2]; /* set status */

        res = a_llcc68_spi_read_buffer(handle, rx_start_buffer_pointer, handle->receive_buf,
                                       payload_length_rx); /* read buffer */
        if (res != 0) /* 检查结果 */
        {
            handle->debug_print("llcc68: read buffer failed.\n"); /* 读取缓冲区失败 */

            return 1; /* 返回错误 */
        }
        if (handle->receive_callback != NULL) /* if receive callback */
        {
            if (handle->crc_error == 0) /* check crc error */
            {
                handle->receive_callback(LLCC68_IRQ_RX_DONE, handle->receive_buf, payload_length_rx); /* run callback */
            }
            else
            {
                handle->receive_callback(LLCC68_IRQ_RX_DONE, NULL, 0); /* run callback */
            }
        }
    }

    return 0; /* success return 0 */
}

/**
 * @brief     初始化芯片
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 SPI 初始化失败
 *            - 2 句柄为空
 *            - 3 链接函数为空
 *            - 4 复位 GPIO 初始化失败
 *            - 5 忙状态 GPIO 初始化失败
 *            - 6 复位芯片失败
 * @note      无
 */
uint8_t llcc68_init(llcc68_handle_t* handle)
{
    uint8_t buf[1];
    uint8_t prev;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->debug_print == NULL) /* 检查 debug_print */
    {
        return 3; /* 返回错误 */
    }
    if (handle->spi_init == NULL) /* check spi_init */
    {
        handle->debug_print("llcc68: spi_init is null.\n"); /* spi_init is null */

        return 3; /* 返回错误 */
    }
    if (handle->spi_deinit == NULL) /* check spi_deinit */
    {
        handle->debug_print("llcc68: spi_deinit is null.\n"); /* spi_deinit is null */

        return 3; /* 返回错误 */
    }
    if (handle->spi_write_read == NULL) /* 检查 spi_write_read */
    {
        handle->debug_print("llcc68: spi_write_read is null.\n"); /* spi_write_read 为空 */

        return 3; /* 返回错误 */
    }
    if (handle->reset_gpio_init == NULL) /* 检查 reset_gpio_init */
    {
        handle->debug_print("llcc68: reset_gpio_init is null.\n"); /* reset_gpio_init 为空 */

        return 3; /* 返回错误 */
    }
    if (handle->reset_gpio_deinit == NULL) /* 检查 reset_gpio_deinit */
    {
        handle->debug_print("llcc68: reset_gpio_deinit is null.\n"); /* reset_gpio_deinit 为空 */

        return 3; /* 返回错误 */
    }
    if (handle->reset_gpio_write == NULL) /* 检查 reset_gpio_write */
    {
        handle->debug_print("llcc68: reset_gpio_write is null.\n"); /* reset_gpio_write 为空 */

        return 3; /* 返回错误 */
    }
    if (handle->busy_gpio_init == NULL) /* 检查 busy_gpio_init */
    {
        handle->debug_print("llcc68: busy_gpio_init is null.\n"); /* busy_gpio_init 为空 */

        return 3; /* 返回错误 */
    }
    if (handle->busy_gpio_deinit == NULL) /* 检查 busy_gpio_deinit */
    {
        handle->debug_print("llcc68: busy_gpio_deinit is null.\n"); /* busy_gpio_deinit 为空 */

        return 3; /* 返回错误 */
    }
    if (handle->busy_gpio_read == NULL) /* 检查 busy_gpio_read */
    {
        handle->debug_print("llcc68: busy_gpio_read is null.\n"); /* busy_gpio_read 为空 */

        return 3; /* 返回错误 */
    }
    if (handle->delay_ms == NULL) /* 检查 delay_ms */
    {
        handle->debug_print("llcc68: delay_ms is null.\n"); /* delay_ms 为空 */

        return 3; /* 返回错误 */
    }
    if (handle->receive_callback == NULL) /* 检查 receive_callback */
    {
        handle->debug_print("llcc68: receive_callback is null.\n"); /* receive_callback 为空 */

        return 3; /* 返回错误 */
    }

    if (handle->spi_init() != 0) /* SPI 初始化 */
    {
        handle->debug_print("llcc68: spi initialization failed.\n"); /* SPI 初始化失败 */

        return 1; /* 返回错误 */
    }
    if (handle->reset_gpio_init() != 0) /* 复位 GPIO 初始化 */
    {
        handle->debug_print("llcc68: reset gpio initialization failed.\n"); /* 复位 GPIO 初始化失败 */
        (void)handle->spi_deinit(); /* SPI 反初始化 */

        return 4; /* 返回错误 */
    }
    if (handle->busy_gpio_init() != 0) /* 忙状态 GPIO 初始化 */
    {
        handle->debug_print("llcc68: busy gpio initialization failed.\n"); /* 忙状态 GPIO 初始化失败 */
        (void)handle->spi_deinit(); /* SPI 反初始化 */
        (void)handle->reset_gpio_deinit(); /* 复位 GPIO 反初始化 */

        return 5; /* 返回错误 */
    }

    if (handle->reset_gpio_write(1) != 0) /* 设置高电平 */
    {
        handle->debug_print("llcc68: reset chip failed.\n"); /* 芯片复位失败 */
        (void)handle->spi_deinit(); /* SPI 反初始化 */
        (void)handle->reset_gpio_deinit(); /* 复位 GPIO 反初始化 */
        (void)handle->busy_gpio_deinit(); /* 忙状态 GPIO 反初始化 */

        return 6; /* 返回错误 */
    }
    handle->delay_ms(5); /* 延时 5 毫秒 */
    if (handle->reset_gpio_write(0) != 0) /* 设置低电平 */
    {
        handle->debug_print("llcc68: reset chip failed.\n"); /* 芯片复位失败 */
        (void)handle->spi_deinit(); /* SPI 反初始化 */
        (void)handle->reset_gpio_deinit(); /* 复位 GPIO 反初始化 */
        (void)handle->busy_gpio_deinit(); /* 忙状态 GPIO 反初始化 */

        return 6; /* 返回错误 */
    }
    handle->delay_ms(10); /* 延时 10 毫秒 */
    if (handle->reset_gpio_write(1) != 0) /* 设置高电平 */
    {
        handle->debug_print("llcc68: reset chip failed.\n"); /* 芯片复位失败 */
        (void)handle->spi_deinit(); /* SPI 反初始化 */
        (void)handle->reset_gpio_deinit(); /* 复位 GPIO 反初始化 */
        (void)handle->busy_gpio_deinit(); /* 忙状态 GPIO 反初始化 */

        return 6; /* 返回错误 */
    }
    handle->delay_ms(5); /* 延时 5 毫秒 */

    if (a_llcc68_spi_read(handle, LLCC68_COMMAND_GET_STATUS, (uint8_t*)buf, 1) != 0) /* 读取命令 */
    {
        handle->debug_print("llcc68: get status failed.\n"); /* 获取状态失败 */
        (void)handle->spi_deinit(); /* SPI 反初始化 */
        (void)handle->reset_gpio_deinit(); /* 复位 GPIO 反初始化 */
        (void)handle->busy_gpio_deinit(); /* 忙状态 GPIO 反初始化 */

        return 6; /* 返回错误 */
    }
    prev = 0x00;
    if (a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_STANDBY, (uint8_t*)&prev, 1) != 0) /* 写入命令 */
    {
        handle->debug_print("llcc68: set standby failed.\n"); /* 设置待机模式失败 */
        (void)handle->spi_deinit(); /* SPI 反初始化 */
        (void)handle->reset_gpio_deinit(); /* 复位 GPIO 反初始化 */
        (void)handle->busy_gpio_deinit(); /* 忙状态 GPIO 反初始化 */

        return 6; /* 返回错误 */
    }
    handle->inited = 1; /* 标记初始化完成 */

    return 0; /* 成功返回 0 */
}

/**
 * @brief     关闭芯片
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 SPI 反初始化失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 芯片忙
 *            - 5 关机失败
 *            - 6 忙状态 GPIO 反初始化失败
 *            - 7 复位 GPIO 反初始化失败
 * @note      无
 */
uint8_t llcc68_deinit(llcc68_handle_t* handle)
{
    uint8_t res, prev;

    if (handle == NULL) /* 检查句柄 */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    prev = 0x00; /* 设置关机 */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_SLEEP, (uint8_t*)&prev, 1); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: power down failed.\n"); /* power down failed */

        return 5; /* 返回错误 */
    }

    res = handle->busy_gpio_deinit(); /* 忙状态 GPIO 反初始化 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: busy gpio deinit failed.\n"); /* busy gpio deinit failed */

        return 6; /* 返回错误 */
    }
    res = handle->reset_gpio_deinit(); /* 复位 GPIO 反初始化 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: reset gpio deinit failed.\n"); /* reset gpio deinit failed */

        return 7; /* 返回错误 */
    }
    res = handle->spi_deinit(); /* spi deinit */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: spi deinit failed.\n"); /* SPI 反初始化失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     进入单接收模式
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] us 接收超时时间(微秒)
 * @return    状态码
 *            - 0 成功
 *            - 1 LoRa 单接收失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 芯片繁忙
 * @note      无
 */
uint8_t llcc68_single_receive(llcc68_handle_t* handle, double us)
{
    uint8_t res;
    uint8_t buf[3];
    uint16_t clear_irq_param;
    uint32_t timeout;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    clear_irq_param = 0x03FF; /* 设置掩码 */
    buf[0] = (clear_irq_param >> 8) & 0xFF; /* set param */
    buf[1] = (clear_irq_param >> 0) & 0xFF; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_CLEAR_IRQ_STATUS, (uint8_t*)buf, 2); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: clear irq status failed.\n"); /* 清除中断状态失败 */

        return 1; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    timeout = (uint32_t)(us / 15.625); /* convert real data to register data */
    buf[0] = (timeout >> 16) & 0xFF; /* 位 23 : 16 */
    buf[1] = (timeout >> 8) & 0xFF; /* 位15:8 */
    buf[2] = (timeout >> 0) & 0xFF; /* bit 7 : 0 */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_RX, (uint8_t*)buf, 3); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set rx failed.\n"); /* 设置接收模式失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     进入连续接收模式
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 LoRa 连续接收失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 芯片繁忙
 * @note      无
 */
uint8_t llcc68_continuous_receive(llcc68_handle_t* handle)
{
    uint8_t res;
    uint16_t clear_irq_param;
    uint8_t buf[3];

    if (handle == NULL) /* 检查句柄 */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* 芯片繁忙 */

        return 4; /* 返回错误 */
    }

    clear_irq_param = 0x03FF; /* 设置掩码 */
    buf[0] = (clear_irq_param >> 8) & 0xFF; /* 设置参数 */
    buf[1] = (clear_irq_param >> 0) & 0xFF; /* 设置参数 */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_CLEAR_IRQ_STATUS, (uint8_t*)buf, 2); /* 写入命令 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: clear irq status failed.\n"); /* 清除中断状态失败 */

        return 1; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* 芯片繁忙 */

        return 4; /* 返回错误 */
    }

    buf[0] = 0xFF; /* 位 23 : 16 */
    buf[1] = 0xFF; /* 位 15 : 8 */
    buf[2] = 0xFF; /* 位 7 : 0 */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_RX, (uint8_t*)buf, 3); /* 写入命令 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set rx failed.\n"); /* 设置接收模式失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* 成功返回 0 */
}

/**
 * @brief      运行信道活动检测(CAD)
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *enable 指向使能标志缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 LoRa CAD 失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 *             - 4 芯片繁忙
 *             - 5 CAD 超时
 * @note       无
 */
uint8_t llcc68_lora_cad(llcc68_handle_t* handle, llcc68_bool_t* enable)
{
    uint8_t res;
    uint16_t clear_irq_param;
    uint8_t buf[2];
    uint16_t timeout;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    clear_irq_param = 0x03FF; /* 设置掩码 */
    buf[0] = (clear_irq_param >> 8) & 0xFF; /* set param */
    buf[1] = (clear_irq_param >> 0) & 0xFF; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_CLEAR_IRQ_STATUS, (uint8_t*)buf, 2); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: clear irq status failed.\n"); /* 清除中断状态失败 */

        return 1; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    handle->cad_done = 0; /* clear cad done */
    handle->cad_detected = 0; /* clear cad done */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_CAD, NULL, 0); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set cad failed.\n"); /* set cad failed */

        return 1; /* 返回错误 */
    }

    timeout = 10000; /*  set timeout */
    while ((timeout != 0) && (handle->cad_done == 0)) /* wait */
    {
        handle->delay_ms(1); /* 延时1毫秒 */
        timeout--; /* timeout-- */
    }
    if (timeout == 0) /* check timeout */
    {
        handle->debug_print("llcc68: cad timeout.\n"); /* cad timeout */

        return 5; /* 返回错误 */
    }
    if (handle->cad_detected == 1) /* set cad */
    {
        *enable = LLCC68_BOOL_TRUE; /* enable */
    }
    else
    {
        *enable = LLCC68_BOOL_FALSE; /* 禁用 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      检查数据包错误
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *enable 指向布尔值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t llcc68_check_packet_error(llcc68_handle_t* handle, llcc68_bool_t* enable)
{
    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    *enable = (llcc68_bool_t)(handle->crc_error); /* check error */

    return 0; /* success return 0 */
}

/**
 * @brief     发送 LoRa 数据
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] standby_src 时钟源
 * @param[in] preamble_length 前导码长度
 * @param[in] header_type 报头类型
 * @param[in] crc_type CRC 类型
 * @param[in] invert_iq_enable 是否反转 IQ 的布尔值
 * @param[in] *buf 指向数据缓冲区的指针
 * @param[in] len 数据长度
 * @param[in] us 超时时间(微秒)
 * @return    状态码
 *            - 0 成功
 *            - 1 发送失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 芯片繁忙
 *            - 5 发送超时
 *            - 6 中断超时
 *            - 7 未知结果
 * @note      无
 */
uint8_t llcc68_lora_transmit(llcc68_handle_t* handle, llcc68_clock_source_t standby_src,
                             uint16_t preamble_length, llcc68_lora_header_t header_type,
                             llcc68_lora_crc_type_t crc_type, llcc68_bool_t invert_iq_enable,
                             uint8_t* buf, uint16_t len, uint32_t us)
{
    uint8_t res;
    uint8_t prev;
    uint8_t buffer[6];
    uint16_t clear_irq_param;
    uint32_t ms;
    uint32_t reg;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    clear_irq_param = 0x03FF; /* 设置掩码 */
    buffer[0] = (clear_irq_param >> 8) & 0xFF; /* 设置参数 */
    buffer[1] = (clear_irq_param >> 0) & 0xFF; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_CLEAR_IRQ_STATUS, (uint8_t*)buffer, 2); /* 写命令 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: clear irq status failed.\n"); /* 清除中断状态失败 */

        return 1; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    prev = standby_src; /* 设置参数 */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_STANDBY, (uint8_t*)&prev, 1); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set standby failed.\n"); /* set standby failed */

        return 1; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buffer[0] = (preamble_length >> 8) & 0xFF; /* set param */
    buffer[1] = (preamble_length >> 0) & 0xFF; /* 设置参数 */
    buffer[2] = header_type; /* 设置参数 */
    buffer[3] = (uint8_t)len; /* set param */
    buffer[4] = crc_type; /* set param */
    buffer[5] = invert_iq_enable; /* 设置参数 */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_PACKET_PARAMS, (uint8_t*)buffer, 6); /* 写命令 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set lora modulation params failed.\n"); /* set lora modulation params failed */

        return 1; /* 返回错误 */
    }

    if (invert_iq_enable == LLCC68_BOOL_FALSE) /* not invert iq */
    {
        uint8_t setup;

        res = a_llcc68_check_busy(handle); /* 检查忙状态 */
        if (res != 0) /* 检查结果 */
        {
            handle->debug_print("llcc68: chip is busy.\n"); /* 芯片忙 */

            return 4; /* 返回错误 */
        }

        res = a_llcc68_spi_read_register(handle, LLCC68_REG_IQ_POLARITY_SETUP, (uint8_t*)&setup, 1); /* 读寄存器 */
        if (res != 0) /* 检查结果 */
        {
            handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

            return 1; /* 返回错误 */
        }

        setup |= (1 << 2); /* set bit 2 */
        res = a_llcc68_spi_write_register(handle, LLCC68_REG_IQ_POLARITY_SETUP, (uint8_t*)&setup, 1); /* 读寄存器 */
        if (res != 0) /* 检查结果 */
        {
            handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

            return 1; /* 返回错误 */
        }
    }
    else
    {
        uint8_t setup;

        res = a_llcc68_check_busy(handle); /* 检查忙状态 */
        if (res != 0) /* 检查结果 */
        {
            handle->debug_print("llcc68: chip is busy.\n"); /* 芯片忙 */

            return 4; /* 返回错误 */
        }

        res = a_llcc68_spi_read_register(handle, LLCC68_REG_IQ_POLARITY_SETUP, (uint8_t*)&setup, 1); /* 读寄存器 */
        if (res != 0) /* 检查结果 */
        {
            handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

            return 1; /* 返回错误 */
        }

        setup &= ~(1 << 2); /* 清除位2 */
        res = a_llcc68_spi_write_register(handle, LLCC68_REG_IQ_POLARITY_SETUP, (uint8_t*)&setup, 1); /* 读寄存器 */
        if (res != 0) /* 检查结果 */
        {
            handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

            return 1; /* 返回错误 */
        }
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    if (a_llcc68_spi_write_buffer(handle, 0x00, buf, len) != 0) /* write buffer */
    {
        handle->debug_print("llcc68: write buffer failed.\n"); /* write buffer failed */

        return 1; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    reg = (uint32_t)(us / 15.625); /* convert the timeout */
    buffer[0] = (reg >> 16) & 0xFF; /* 位23:16 */
    buffer[1] = (reg >> 8) & 0xFF; /* 位15:8 */
    buffer[2] = (reg >> 0) & 0xFF; /* 位7:0 */
    handle->tx_done = 0; /* 标志为0 */
    handle->timeout = 0; /* 标志为0 */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_TX, (uint8_t*)buffer, 3); /* 写命令 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set tx failed.\n"); /* 设置发送模式失败 */

        return 1; /* 返回错误 */
    }
    ms = us / 1000 + 10000; /* set timeout */
    while ((ms != 0) && (handle->tx_done == 0) && (handle->timeout == 0)) /* 检查超时 */
    {
        handle->delay_ms(1); /* 延时1毫秒 */
        ms--; /* 毫秒减1 */
    }
    if ((ms != 0) && (handle->tx_done == 1)) /* check the result */
    {
        return 0; /* 成功返回0 */
    }
    else if ((ms == 0) && (handle->tx_done == 0)) /* 检查结果 */
    {
        handle->debug_print("llcc68: send timeout.\n"); /* send timeout */

        return 5; /* 返回错误 */
    }
    else if (handle->timeout == 1) /* check the result */
    {
        handle->debug_print("llcc68: irq timeout.\n"); /* 中断超时 */

        return 6; /* 返回错误 */
    }
    else
    {
        handle->debug_print("llcc68: unknown result.\n"); /* 未知结果 */

        return 7; /* 返回错误 */
    }
}

/**
 * @brief     写寄存器
 * @param[in] *handle 指向llcc68句柄结构的指针
 * @param[in] reg 寄存器地址
 * @param[in] *buf 指向数据缓冲区的指针
 * @param[in] len 数据长度
 * @return    状态码
 *            - 0 成功
 *            - 1 写寄存器失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_write_register(llcc68_handle_t* handle, uint16_t reg, uint8_t* buf, uint16_t len)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    if (a_llcc68_spi_write_register(handle, reg, buf, len) != 0) /* 写寄存器 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      read the register
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[in]  reg register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 read register failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_read_register(llcc68_handle_t* handle, uint16_t reg, uint8_t* buf, uint16_t len)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    if (a_llcc68_spi_read_register(handle, reg, buf, len) != 0) /* 读寄存器 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     write the buffer
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] offset buffer offset
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 write buffer failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_write_buffer(llcc68_handle_t* handle, uint8_t offset, uint8_t* buf, uint16_t len)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    if (a_llcc68_spi_write_buffer(handle, offset, buf, len) != 0) /* write buffer */
    {
        handle->debug_print("llcc68: write buffer failed.\n"); /* 写缓冲区失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      read the buffer
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[in]  offset buffer offset
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 read buffer failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_read_buffer(llcc68_handle_t* handle, uint8_t offset, uint8_t* buf, uint16_t len)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    if (a_llcc68_spi_read_buffer(handle, offset, buf, len) != 0) /* read buffer */
    {
        handle->debug_print("llcc68: read buffer failed.\n"); /* 读缓冲区失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     进入睡眠模式
 * @param[in] *handle 指向llcc68句柄结构的指针
 * @param[in] mode 启动模式
 * @param[in] rtc_wake_up_enable 布尔值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置睡眠模式失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_sleep(llcc68_handle_t* handle, llcc68_start_mode_t mode, llcc68_bool_t rtc_wake_up_enable)
{
    uint8_t res;
    uint8_t prev;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    prev = (uint8_t)((mode << 2) | (rtc_wake_up_enable << 0)); /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_SLEEP, (uint8_t*)&prev, 1); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set sleep failed.\n"); /* set sleep failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     enter to the standby mode
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] src clock source
 * @return    status code
 *            - 0 success
 *            - 1 set standby failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_standby(llcc68_handle_t* handle, llcc68_clock_source_t src)
{
    uint8_t res;
    uint8_t prev;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    prev = src; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_STANDBY, (uint8_t*)&prev, 1); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set standby failed.\n"); /* set standby failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     enter to the frequency synthesis mode
 * @param[in] *handle pointer to an llcc68 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 set frequency synthesis failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_frequency_synthesis(llcc68_handle_t* handle)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_FS, NULL, 0); /* 写命令 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set fs failed.\n"); /* set fs failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     enter to the tx mode
 * @param[in] *handle 指向llcc68句柄结构的指针
 * @param[in] timeout 发送超时时间
 * @return    状态码
 *            - 0 成功
 *            - 1 设置发送模式失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 芯片忙
 * @note      0x000000 表示禁用超时，发送单次模式
 */
uint8_t llcc68_set_tx(llcc68_handle_t* handle, uint32_t timeout)
{
    uint8_t res;
    uint8_t buf[3];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = (timeout >> 16) & 0xFF; /* 位 23 : 16 */
    buf[1] = (timeout >> 8) & 0xFF; /* 位15:8 */
    buf[2] = (timeout >> 0) & 0xFF; /* bit 7 : 0 */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_TX, (uint8_t*)buf, 3); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set tx failed.\n"); /* set tx failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     进入接收模式
 * @param[in] *handle 指向llcc68句柄结构的指针
 * @param[in] timeout 接收超时时间
 * @return    状态码
 *            - 0 成功
 *            - 1 设置接收模式失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 芯片忙
 * @note      0x000000 表示禁用超时，接收单次模式
 *            0xFFFFFF 表示接收连续模式
 */
uint8_t llcc68_set_rx(llcc68_handle_t* handle, uint32_t timeout)
{
    uint8_t res;
    uint8_t buf[3];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = (timeout >> 16) & 0xFF; /* 位 23 : 16 */
    buf[1] = (timeout >> 8) & 0xFF; /* 位15:8 */
    buf[2] = (timeout >> 0) & 0xFF; /* bit 7 : 0 */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_RX, (uint8_t*)buf, 3); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set rx failed.\n"); /* 设置接收模式失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      将超时时间转换为寄存器原始数据
 * @param[in]  *handle 指向llcc68句柄结构的指针
 * @param[in]  us 超时时间（微秒）
 * @param[out] *reg 指向寄存器原始数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t llcc68_timeout_convert_to_register(llcc68_handle_t* handle, double us, uint32_t* reg)
{
    if (handle == NULL) /* 检查句柄 */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    *reg = (uint32_t)(us / 15.625); /* 将实际数据转换为寄存器数据 */

    return 0; /* 成功返回0 */
}

/**
 * @brief      将寄存器原始数据转换为超时时间
 * @param[in]  *handle 指向llcc68句柄结构的指针
 * @param[in]  reg 寄存器原始数据
 * @param[out] *us 指向微秒缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t llcc68_timeout_convert_to_data(llcc68_handle_t* handle, uint32_t reg, double* us)
{
    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    *us = (double)(reg) * 15.625; /* convert raw data to real data */

    return 0; /* success return 0 */
}

/**
 * @brief     在前导码上停止定时器
 * @param[in] *handle 指向llcc68句柄结构的指针
 * @param[in] enable 布尔值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置前导码停止定时器失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_stop_timer_on_preamble(llcc68_handle_t* handle, llcc68_bool_t enable)
{
    uint8_t res;
    uint8_t prev;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    prev = enable; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_STOP_TIMER_ON_PREAMBLE, (uint8_t*)&prev, 1); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set stop timer on preamble failed.\n"); /* 设置前导码停止定时器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the rx duty cycle
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] rx_period rx period
 * @param[in] sleep_period sleep period
 * @return    status code
 *            - 0 success
 *            - 1 set rx duty cycle failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      t_preamble + t_header <= 2 * rx_period + sleep_period
 */
uint8_t llcc68_set_rx_duty_cycle(llcc68_handle_t* handle, uint32_t rx_period, uint32_t sleep_period)
{
    uint8_t res;
    uint8_t buf[6];

    if (handle == NULL) /* 检查句柄 */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* 芯片忙 */

        return 4; /* 返回错误 */
    }

    buf[0] = (rx_period >> 16) & 0xFF; /* 位 23 : 16 */
    buf[1] = (rx_period >> 8) & 0xFF; /* 位 15 : 8 */
    buf[2] = (rx_period >> 0) & 0xFF; /* 位 7 : 0 */
    buf[3] = (sleep_period >> 16) & 0xFF; /* 位 23 : 16 */
    buf[4] = (sleep_period >> 8) & 0xFF; /* 位 15 : 8 */
    buf[5] = (sleep_period >> 0) & 0xFF; /* 位 7 : 0 */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_RX_DUTY_CYCLE, (uint8_t*)buf, 6); /* 写命令 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set rx duty cycle failed.\n"); /* 设置接收占空比失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* 成功返回0 */
}

/**
 * @brief     run the cad
 * @param[in] *handle pointer to an llcc68 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 set cad failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_cad(llcc68_handle_t* handle)
{
    uint8_t res;

    if (handle == NULL) /* 检查句柄 */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* 芯片忙 */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_CAD, NULL, 0); /* 写命令 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set cad failed.\n"); /* 设置CAD失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* 成功返回0 */
}

/**
 * @brief     enter to the tx continuous wave mode
 * @param[in] *handle pointer to an llcc68 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 set tx continuous wave failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_tx_continuous_wave(llcc68_handle_t* handle)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_TX_CONTINUOUS_WAVE, NULL, 0); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set tx continuous wave failed.\n"); /* set tx continuous wave failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     enter to the tx infinite preamble mode
 * @param[in] *handle pointer to an llcc68 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 set tx infinite preamble failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_tx_infinite_preamble(llcc68_handle_t* handle)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_TX_INFINITE_PREAMBLE, NULL, 0); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set tx infinite preamble failed.\n"); /* set tx infinite preamble failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     设置稳压器模式
 * @param[in] *handle 指向llcc68句柄结构的指针
 * @param[in] mode 稳压器模式
 * @return    状态码
 *            - 0 成功
 *            - 1 设置稳压器模式失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_regulator_mode(llcc68_handle_t* handle, llcc68_regulator_mode_t mode)
{
    uint8_t res;
    uint8_t prev;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    prev = mode; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_REGULATOR_MODE, (uint8_t*)&prev, 1); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set regulator mode failed.\n"); /* set regulator mode failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the calibration settings
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] settings calibration param
 * @return    status code
 *            - 0 success
 *            - 1 set calibration failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_calibration(llcc68_handle_t* handle, uint8_t settings)
{
    uint8_t res;
    uint8_t prev;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    prev = settings; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_CALIBRATE, (uint8_t*)&prev, 1); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set calibration failed.\n"); /* 设置校准失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the calibration image frequency
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] freq1 image frequency 1
 * @param[in] freq2 image frequency 2
 * @return    status code
 *            - 0 success
 *            - 1 set calibration image failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_calibration_image(llcc68_handle_t* handle, uint8_t freq1, uint8_t freq2)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = freq1; /* set param */
    buf[1] = freq2; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_CALIBRATE_IMAGE, (uint8_t*)buf, 2); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set calibration image failed.\n"); /* set calibration image failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the pa config
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] pa_duty_cycle pa duty cycle
 * @param[in] hp_max max power
 * @return    status code
 *            - 0 success
 *            - 1 set the pa config failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_pa_config(llcc68_handle_t* handle, uint8_t pa_duty_cycle, uint8_t hp_max)
{
    uint8_t res;
    uint8_t buf[4];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = pa_duty_cycle; /* set param */
    buf[1] = hp_max; /* set param */
    buf[2] = 0x00; /* set param */
    buf[3] = 0x01; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_PA_CONFIG, (uint8_t*)buf, 4); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set pa config failed.\n"); /* set pa config failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the rx tx fallback mode
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] mode rx tx fallback mode
 * @return    status code
 *            - 0 success
 *            - 1 set rx tx fallback mode failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_rx_tx_fallback_mode(llcc68_handle_t* handle, llcc68_rx_tx_fallback_mode_t mode)
{
    uint8_t res;
    uint8_t prev;

    if (handle == NULL) /* 检查句柄 */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* 芯片忙 */

        return 4; /* 返回错误 */
    }

    prev = mode; /* 设置参数 */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_RX_TX_FALLBACK_MODE, (uint8_t*)&prev, 1); /* 写命令 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set rx tx fallback mode failed.\n"); /* 设置收发回退模式失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* 成功返回0 */
}

/**
 * @brief     设置DIO中断请求参数
 * @param[in] *handle 指向llcc68句柄结构的指针
 * @param[in] irq_mask 中断请求掩码
 * @param[in] dio1_mask DIO1掩码
 * @param[in] dio2_mask DIO2掩码
 * @param[in] dio3_mask DIO3掩码
 * @return    状态码
 *            - 0 success
 *            - 1 set dio irq params failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_dio_irq_params(llcc68_handle_t* handle, uint16_t irq_mask, uint16_t dio1_mask,
                                  uint16_t dio2_mask, uint16_t dio3_mask)
{
    uint8_t res;
    uint8_t buf[8];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = (irq_mask >> 8) & 0xFF; /* set param */
    buf[1] = (irq_mask >> 0) & 0xFF; /* set param */
    buf[2] = (dio1_mask >> 8) & 0xFF; /* set param */
    buf[3] = (dio1_mask >> 0) & 0xFF; /* set param */
    buf[4] = (dio2_mask >> 8) & 0xFF; /* set param */
    buf[5] = (dio2_mask >> 0) & 0xFF; /* set param */
    buf[6] = (dio3_mask >> 8) & 0xFF; /* set param */
    buf[7] = (dio3_mask >> 0) & 0xFF; /* set param */

    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_DIO_IRQ_PARAMS, (uint8_t*)buf, 8); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set dio irq params failed.\n"); /* set dio irq params failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the irq status
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *status pointer to a status buffer
 * @return     status code
 *             - 0 success
 *             - 1 get irq status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_irq_status(llcc68_handle_t* handle, uint16_t* status)
{
    uint8_t res;
    uint8_t buf[3];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read(handle, LLCC68_COMMAND_GET_IRQ_STATUS, (uint8_t*)buf, 3); /* read command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: get irq status failed.\n"); /* get irq status failed */

        return 1; /* 返回错误 */
    }
    *status = ((uint16_t)buf[1] << 8) | buf[2]; /* set status */

    return 0; /* success return 0 */
}

/**
 * @brief     clear the irq status
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] clear_irq_param irq mask
 * @return    status code
 *            - 0 success
 *            - 1 clear irq status failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_clear_irq_status(llcc68_handle_t* handle, uint16_t clear_irq_param)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = (clear_irq_param >> 8) & 0xFF; /* set param */
    buf[1] = (clear_irq_param >> 0) & 0xFF; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_CLEAR_IRQ_STATUS, (uint8_t*)buf, 2); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: clear irq status failed.\n"); /* 清除中断状态失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set dio2 as rf switch ctrl
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set dio2 as rf switch ctrl failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_dio2_as_rf_switch_ctrl(llcc68_handle_t* handle, llcc68_bool_t enable)
{
    uint8_t res;
    uint8_t prev;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    prev = enable; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_DIO2_AS_RF_SWITCH_CTRL, (uint8_t*)&prev, 1); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set dio2 as rf switch ctrl failed.\n"); /* set dio2 as rf switch ctrl failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set dio3 as tcxo ctrl
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] voltage tcxo voltage
 * @param[in] delay tcxo ctrl delay
 * @return    status code
 *            - 0 success
 *            - 1 set dio3 as tcxo ctrl failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_dio3_as_tcxo_ctrl(llcc68_handle_t* handle, llcc68_tcxo_voltage_t voltage, uint32_t delay)
{
    uint8_t res;
    uint8_t buf[4];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = voltage; /* set param */
    buf[1] = (delay >> 16) & 0xFF; /* set param */
    buf[2] = (delay >> 8) & 0xFF; /* set param */
    buf[3] = (delay >> 0) & 0xFF; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_DIO3_AS_TCXO_CTRL, (uint8_t*)buf, 4); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set dio3 as tcxo ctrl status failed.\n"); /* 设置DIO3作为TCXO控制状态失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      将频率转换为寄存器原始数据
 * @param[in]  *handle 指向llcc68句柄结构的指针
 * @param[in]  freq 频率
 * @param[out] *reg 指向寄存器原始数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t llcc68_frequency_convert_to_register(llcc68_handle_t* handle, uint32_t freq, uint32_t* reg)
{
    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    *reg = (uint32_t)(powf(2.0f, 25.0f) / (32 * powf(10.f, 6.0f)) * freq); /* convert real data to register data */

    return 0; /* success return 0 */
}

/**
 * @brief      将寄存器原始数据转换为频率
 * @param[in]  *handle 指向llcc68句柄结构的指针
 * @param[in]  reg 寄存器原始数据
 * @param[out] *freq 指向频率缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t llcc68_frequency_convert_to_data(llcc68_handle_t* handle, uint32_t reg, uint32_t* freq)
{
    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    *freq = (uint32_t)(32 * powf(10.f, 6.0f) / powf(2.0f, 25.0f) * reg); /* convert real data to register data */

    return 0; /* success return 0 */
}

/**
 * @brief     set the rf frequency
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] reg rf frequency register data
 * @return    status code
 *            - 0 success
 *            - 1 set rf frequency failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_rf_frequency(llcc68_handle_t* handle, uint32_t reg)
{
    uint8_t res;
    uint8_t buf[4];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = (reg >> 24) & 0xFF; /* set param */
    buf[1] = (reg >> 16) & 0xFF; /* set param */
    buf[2] = (reg >> 8) & 0xFF; /* set param */
    buf[3] = (reg >> 0) & 0xFF; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_RF_FREQUENCY, (uint8_t*)buf, 4); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set rf frequency failed.\n"); /* set rf frequency failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     设置数据包类型
 * @param[in] *handle 指向 llcc68 句柄结构的指针
 * @param[in] type 数据包类型
 * @return    状态码
 *            - 0 成功
 *            - 1 设置数据包类型失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_packet_type(llcc68_handle_t* handle, llcc68_packet_type_t type)
{
    uint8_t res;
    uint8_t prev;

    if (handle == NULL) /* 检查句柄 */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* 芯片忙 */

        return 4; /* 返回错误 */
    }

    prev = type; /* 设置参数 */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_PACKET_TYPE, (uint8_t*)&prev, 1); /* 写命令 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set packet type failed.\n"); /* 设置数据包类型失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* 成功返回 0 */
}

/**
 * @brief      get the packet type
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *type pointer to a packet type buffer
 * @return     status code
 *             - 0 success
 *             - 1 get packet type failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_packet_type(llcc68_handle_t* handle, llcc68_packet_type_t* type)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read(handle, LLCC68_COMMAND_GET_PACKET_TYPE, (uint8_t*)buf, 2); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: get packet type failed.\n"); /* get packet type failed */

        return 1; /* 返回错误 */
    }
    *type = (llcc68_packet_type_t)(buf[1]); /* get type */

    return 0; /* success return 0 */
}

/**
 * @brief     set the tx params
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] dbm rf power
 * @param[in] t ramp time
 * @return    status code
 *            - 0 success
 *            - 1 set tx params failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_tx_params(llcc68_handle_t* handle, int8_t dbm, llcc68_ramp_time_t t)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = dbm; /* set param */
    buf[1] = t; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_TX_PARAMS, (uint8_t*)buf, 2); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set tx params failed.\n"); /* set tx params failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the modulation params in GFSK mode
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] br bit rate
 * @param[in] shape pulse shape
 * @param[in] bw bandwidth
 * @param[in] fdev frequency deviation
 * @return    status code
 *            - 0 success
 *            - 1 set gfsk modulation params failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_gfsk_modulation_params(llcc68_handle_t* handle, uint32_t br, llcc68_gfsk_pulse_shape_t shape,
                                          llcc68_gfsk_bandwidth_t bw, uint32_t fdev)
{
    uint8_t res;
    uint8_t buf[8];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = (br >> 16) & 0xFF; /* set param */
    buf[1] = (br >> 8) & 0xFF; /* set param */
    buf[2] = (br >> 0) & 0xFF; /* set param */
    buf[3] = shape; /* set param */
    buf[4] = bw; /* set param */
    buf[5] = (fdev >> 16) & 0xFF; /* set param */
    buf[6] = (fdev >> 8) & 0xFF; /* set param */
    buf[7] = (fdev >> 0) & 0xFF; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_MODULATION_PARAMS, (uint8_t*)buf, 8); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set gfsk modulation params failed.\n"); /* set gfsk modulation params failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      convert the bit rate to the register raw data
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[in]  br bit rate
 * @param[out] *reg pointer to a register raw buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t llcc68_gfsk_bit_rate_convert_to_register(llcc68_handle_t* handle, uint32_t br, uint32_t* reg)
{
    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    *reg = (uint32_t)(32 * (32 * powf(10.f, 6.0f))) / br; /* convert real data to register data */

    return 0; /* success return 0 */
}

/**
 * @brief      convert the register raw data to the bit rate
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[in]  reg register raw data
 * @param[out] *br pointer to a bit rate buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t llcc68_gfsk_bit_rate_convert_to_data(llcc68_handle_t* handle, uint32_t reg, uint32_t* br)
{
    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    *br = (uint32_t)(32 * 32 * powf(10.f, 6.0f) / reg); /* convert real data to register data */

    return 0; /* success return 0 */
}

/**
 * @brief      convert the frequency deviation to the register raw data
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[in]  freq frequency deviation
 * @param[out] *reg pointer to a register raw buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t llcc68_gfsk_frequency_deviation_convert_to_register(llcc68_handle_t* handle, uint32_t freq, uint32_t* reg)
{
    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    *reg = (uint32_t)(powf(2.0f, 25.0f) * freq / (32 * powf(10.f, 6.0f))); /* convert real data to register data */

    return 0; /* success return 0 */
}

/**
 * @brief      将寄存器原始数据转换为频率偏移
 * @param[in]  *handle 指向llcc68句柄结构的指针
 * @param[in]  reg 寄存器原始数据
 * @param[out] *freq 指向频率偏移缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t llcc68_gfsk_frequency_deviation_convert_to_data(llcc68_handle_t* handle, uint32_t reg, uint32_t* freq)
{
    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    *freq = (uint32_t)(32 * powf(10.f, 6.0f) / powf(2.0f, 25.0f) * reg); /* convert real data to register data */

    return 0; /* success return 0 */
}

/**
 * @brief     在LoRa模式下设置调制参数
 * @param[in] *handle 指向llcc68句柄结构的指针
 * @param[in] sf 扩频因子
 * @param[in] bw 带宽
 * @param[in] cr 编码率
 * @param[in] low_data_rate_optimize_enable 布尔值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置LoRa调制参数失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 芯片忙
 * @note      无
 */
uint8_t llcc68_set_lora_modulation_params(llcc68_handle_t* handle, llcc68_lora_sf_t sf, llcc68_lora_bandwidth_t bw,
                                          llcc68_lora_cr_t cr, llcc68_bool_t low_data_rate_optimize_enable)
{
    uint8_t res;
    uint8_t buf[4];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = sf; /* set param */
    buf[1] = bw; /* set param */
    buf[2] = cr; /* set param */
    buf[3] = low_data_rate_optimize_enable; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_MODULATION_PARAMS, (uint8_t*)buf, 4); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set lora modulation params failed.\n"); /* 设置LoRa调制参数失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     在GFSK模式下设置数据包参数
 * @param[in] *handle 指向llcc68句柄结构的指针
 * @param[in] preamble_length 前导码长度
 * @param[in] detector_length 前导码检测器长度
 * @param[in] sync_word_length 同步字长度
 * @param[in] filter 地址过滤器
 * @param[in] packet_type 数据包类型
 * @param[in] payload_length 有效载荷长度
 * @param[in] crc_type CRC类型
 * @param[in] whitening_enable 布尔值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置GFSK数据包参数失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 芯片忙
 *            - 5 同步字长度超过0x40
 * @note      无
 */
uint8_t llcc68_set_gfsk_packet_params(llcc68_handle_t* handle, uint16_t preamble_length,
                                      llcc68_gfsk_preamble_detector_length_t detector_length,
                                      uint8_t sync_word_length, llcc68_gfsk_addr_filter_t filter,
                                      llcc68_gfsk_packet_type_t packet_type, uint8_t payload_length,
                                      llcc68_gfsk_crc_type_t crc_type, llcc68_bool_t whitening_enable)
{
    uint8_t res;
    uint8_t buf[9];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }
    if (sync_word_length > 0x40) /* check sync word length */
    {
        handle->debug_print("llcc68: sync word length is over 0x40.\n"); /* chip is busy */

        return 5; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = (preamble_length >> 8) & 0xFF; /* set param */
    buf[1] = (preamble_length >> 0) & 0xFF; /* set param */
    buf[2] = detector_length; /* set param */
    buf[3] = sync_word_length; /* set param */
    buf[4] = filter; /* set param */
    buf[5] = packet_type; /* set param */
    buf[6] = payload_length; /* set param */
    buf[7] = crc_type; /* set param */
    buf[8] = whitening_enable; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_PACKET_PARAMS, (uint8_t*)buf, 9); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set gfsk modulation params failed.\n"); /* set gfsk modulation params failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the packet params in LoRa mode
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] preamble_length preamble length
 * @param[in] header_type header type
 * @param[in] payload_length length of payload
 * @param[in] crc_type crc type
 * @param[in] invert_iq_enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set lora packet params failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_lora_packet_params(llcc68_handle_t* handle, uint16_t preamble_length,
                                      llcc68_lora_header_t header_type, uint8_t payload_length,
                                      llcc68_lora_crc_type_t crc_type, llcc68_bool_t invert_iq_enable)
{
    uint8_t res;
    uint8_t buf[6];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = (preamble_length >> 8) & 0xFF; /* set param */
    buf[1] = (preamble_length >> 0) & 0xFF; /* set param */
    buf[2] = header_type; /* set param */
    buf[3] = payload_length; /* set param */
    buf[4] = crc_type; /* set param */
    buf[5] = invert_iq_enable; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_PACKET_PARAMS, (uint8_t*)buf, 6); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set lora modulation params failed.\n"); /* set lora modulation params failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the cad params
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] num cad symbol number
 * @param[in] cad_det_peak cad detection peak
 * @param[in] cad_det_min min cad detection peak
 * @param[in] mode cad exit mode
 * @param[in] timeout cad timeout
 * @return    status code
 *            - 0 success
 *            - 1 set cad params failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_cad_params(llcc68_handle_t* handle, llcc68_lora_cad_symbol_num_t num,
                              uint8_t cad_det_peak, uint8_t cad_det_min, llcc68_lora_cad_exit_mode_t mode,
                              uint32_t timeout)
{
    uint8_t res;
    uint8_t buf[7];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = num; /* set param */
    buf[1] = cad_det_peak; /* set param */
    buf[2] = cad_det_min; /* set param */
    buf[3] = mode; /* set param */
    buf[4] = (timeout >> 16) & 0xFF; /* set param */
    buf[5] = (timeout >> 8) & 0xFF; /* set param */
    buf[6] = (timeout >> 0) & 0xFF; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_CAD_PARAMS, (uint8_t*)buf, 7); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set cad params failed.\n"); /* set cad params failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the buffer base address
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] tx_base_addr tx base address
 * @param[in] rx_base_addr rx base address
 * @return    status code
 *            - 0 success
 *            - 1 set buffer base address failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_buffer_base_address(llcc68_handle_t* handle, uint8_t tx_base_addr, uint8_t rx_base_addr)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* 芯片忙 */

        return 4; /* 返回错误 */
    }

    buf[0] = tx_base_addr; /* set param */
    buf[1] = rx_base_addr; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_BUFFER_BASE_ADDRESS, (uint8_t*)buf, 2); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set buffer base address failed.\n"); /* set buffer base address failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the lora symbol number timeout
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] symb_num symbol number
 * @return    status code
 *            - 0 success
 *            - 1 set lora symb num timeout failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_lora_symb_num_timeout(llcc68_handle_t* handle, uint8_t symb_num)
{
    uint8_t res;
    uint8_t prev;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    prev = symb_num; /* set param */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_SET_LORA_SYMB_NUM_TIMEOUT, (uint8_t*)&prev, 1); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: set lora symb num timeout failed.\n"); /* set lora symb num timeout failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      获取状态
 * @param[in]  *handle 指向 llcc68 句柄结构的指针
 * @param[out] *status 指向状态缓冲区的指针
 * @return     status code
 *             - 0 成功
 *             - 1 获取状态失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 *             - 4 芯片忙
 * @note       无
 */
uint8_t llcc68_get_status(llcc68_handle_t* handle, uint8_t* status)
{
    uint8_t res;
    uint8_t buf[1];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    memset(buf, 0, sizeof(uint8_t) * 1); /* clear the buffer */
    res = a_llcc68_spi_read(handle, LLCC68_COMMAND_GET_STATUS, (uint8_t*)buf, 1); /* read command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: get status failed.\n"); /* get status failed */

        return 1; /* 返回错误 */
    }
    *status = buf[0]; /* set status */

    return 0; /* success return 0 */
}

/**
 * @brief      get the rx buffer status
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *payload_length_rx pointer to an rx payload length buffer
 * @param[out] *rx_start_buffer_pointer pointer to an rx start pointer buffer
 * @return     status code
 *             - 0 success
 *             - 1 get rx buffer status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_rx_buffer_status(llcc68_handle_t* handle, uint8_t* payload_length_rx,
                                    uint8_t* rx_start_buffer_pointer)
{
    uint8_t res;
    uint8_t buf[3];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read(handle, LLCC68_COMMAND_GET_RX_BUFFER_STATUS, (uint8_t*)buf, 3); /* read command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: get rx buffer status failed.\n"); /* get rx buffer status failed */

        return 1; /* 返回错误 */
    }
    *payload_length_rx = buf[1]; /* set status */
    *rx_start_buffer_pointer = buf[2]; /* set status */

    return 0; /* success return 0 */
}

/**
 * @brief      get the packet status in GFSK mode
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *rx_status pointer to an rx status buffer
 * @param[out] *rssi_sync_raw pointer to an rssi sync raw buffer
 * @param[out] *rssi_avg_raw pointer to an rssi avg raw buffer
 * @param[out] *rssi_sync pointer to an rssi sync buffer
 * @param[out] *rssi_avg pointer to an rssi avg buffer
 * @return     status code
 *             - 0 success
 *             - 1 get gfsk packet status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_gfsk_packet_status(llcc68_handle_t* handle, uint8_t* rx_status, uint8_t* rssi_sync_raw,
                                      uint8_t* rssi_avg_raw, float* rssi_sync, float* rssi_avg)
{
    uint8_t res;
    uint8_t buf[4];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read(handle, LLCC68_COMMAND_GET_PACKET_STATUS, (uint8_t*)buf, 4); /* read command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: get packet status failed.\n"); /* get packet status failed */

        return 1; /* 返回错误 */
    }
    *rx_status = buf[1]; /* set status */
    *rssi_sync_raw = buf[2]; /* set status */
    *rssi_avg_raw = buf[3]; /* set status */
    *rssi_sync = -(float)(*rssi_sync_raw) / 2.0f; /* set status */
    *rssi_avg = -(float)(*rssi_avg_raw) / 2.0f; /* set status */

    return 0; /* success return 0 */
}

/**
 * @brief      get the packet status in LoRa mode
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *rssi_pkt_raw pointer to an rssi packet raw buffer
 * @param[out] *snr_pkt_raw pointer to an snr packet raw buffer
 * @param[out] *signal_rssi_pkt_raw pointer to a signal rssi packet raw buffer
 * @param[out] *rssi_pkt pointer to an rssi packet buffer
 * @param[out] *snr_pkt pointer to an snr packet buffer
 * @param[out] *signal_rssi_pkt pointer to a signal rssi packet buffer
 * @return     status code
 *             - 0 success
 *             - 1 get lora packet status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_lora_packet_status(llcc68_handle_t* handle, uint8_t* rssi_pkt_raw, int8_t* snr_pkt_raw,
                                      uint8_t* signal_rssi_pkt_raw, float* rssi_pkt, float* snr_pkt,
                                      float* signal_rssi_pkt)
{
    uint8_t res;
    uint8_t buf[4];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read(handle, LLCC68_COMMAND_GET_PACKET_STATUS, (uint8_t*)buf, 4); /* read command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: get packet status failed.\n"); /* get packet status failed */

        return 1; /* 返回错误 */
    }
    *rssi_pkt_raw = buf[1]; /* set status */
    *snr_pkt_raw = (int8_t)buf[2]; /* set status */
    *signal_rssi_pkt_raw = buf[3]; /* set status */
    *rssi_pkt = -(float)(*rssi_pkt_raw) / 2.0f; /* set status */
    *snr_pkt = (float)(*snr_pkt_raw) / 4.0f; /* set status */
    *signal_rssi_pkt = -(float)(*signal_rssi_pkt_raw) / 2.0f; /* set status */

    return 0; /* success return 0 */
}

/**
 * @brief      get the instantaneous rssi
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *rssi_inst_raw pointer to an rssi instantaneous raw buffer
 * @param[out] *rssi_inst pointer to an rssi instantaneous buffer
 * @return     status code
 *             - 0 success
 *             - 1 get instantaneous rssi failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_instantaneous_rssi(llcc68_handle_t* handle, uint8_t* rssi_inst_raw, float* rssi_inst)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read(handle, LLCC68_COMMAND_GET_RSSI_LNST, (uint8_t*)buf, 2); /* read command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: get instantaneous rssi failed.\n"); /* get instantaneous rssi failed */

        return 1; /* 返回错误 */
    }
    *rssi_inst_raw = buf[1]; /* set status */
    *rssi_inst = -(float)(*rssi_inst_raw) / 2.0f; /* set status */

    return 0; /* success return 0 */
}

/**
 * @brief      get the stats
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *pkt_received pointer to a packet received counter buffer
 * @param[out] *pkt_crc_error pointer to a packet crc error buffer
 * @param[out] *pkt_length_header_error pointer to a packet length header error buffer
 * @return     status code
 *             - 0 success
 *             - 1 get stats failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_stats(llcc68_handle_t* handle, uint16_t* pkt_received, uint16_t* pkt_crc_error,
                         uint16_t* pkt_length_header_error)
{
    uint8_t res;
    uint8_t buf[7];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read(handle, LLCC68_COMMAND_GET_STATS, (uint8_t*)buf, 7); /* read command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: get stats failed.\n"); /* get stats failed */

        return 1; /* 返回错误 */
    }
    *pkt_received = (uint16_t)(((uint16_t)buf[1] << 8) | buf[2]); /* set status */
    *pkt_crc_error = (uint16_t)(((uint16_t)buf[3] << 8) | buf[4]); /* set status */
    *pkt_length_header_error = (uint16_t)(((uint16_t)buf[5] << 8) | buf[6]); /* set status */

    return 0; /* success return 0 */
}

/**
 * @brief     reset the stats
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] pkt_received received packet counter mask
 * @param[in] pkt_crc_error packet crc error mask
 * @param[in] pkt_length_header_error packet length header error mask
 * @return    status code
 *            - 0 success
 *            - 1 reset stats failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_reset_stats(llcc68_handle_t* handle, uint16_t pkt_received, uint16_t pkt_crc_error,
                           uint16_t pkt_length_header_error)
{
    uint8_t res;
    uint8_t buf[6];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = (pkt_received >> 8) & 0xFF; /* set status */
    buf[1] = (pkt_received >> 0) & 0xFF; /* set status */
    buf[2] = (pkt_crc_error >> 8) & 0xFF; /* set status */
    buf[3] = (pkt_crc_error >> 0) & 0xFF; /* set status */
    buf[4] = (pkt_length_header_error >> 8) & 0xFF; /* set status */
    buf[5] = (pkt_length_header_error >> 0) & 0xFF; /* set status */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_RESET_STATS, (uint8_t*)buf, 6); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: reset stats failed.\n"); /* reset stats failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the device errors
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *op_error pointer to an op error buffer
 * @return     status code
 *             - 0 success
 *             - 1 get device errors failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_device_errors(llcc68_handle_t* handle, uint16_t* op_error)
{
    uint8_t res;
    uint8_t buf[3];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read(handle, LLCC68_COMMAND_GET_DEVICE_ERRORS, (uint8_t*)buf, 3); /* read command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: get device errors failed.\n"); /* get device errors failed */

        return 1; /* 返回错误 */
    }
    *op_error = (uint16_t)(((uint16_t)buf[1] << 8) | buf[2]); /* set status */

    return 0; /* success return 0 */
}

/**
 * @brief     clear the device errors
 * @param[in] *handle pointer to an llcc68 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 clear device errors failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_clear_device_errors(llcc68_handle_t* handle)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = 0x00; /* set status */
    buf[1] = 0x00; /* set status */
    res = a_llcc68_spi_write(handle, LLCC68_COMMAND_CLEAR_DEVICE_ERRORS, (uint8_t*)buf, 2); /* write command */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: clear device errors failed.\n"); /* clear device errors failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the whitening initial value in FSK mode
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] value set value
 * @return    status code
 *            - 0 success
 *            - 1 set fsk whitening initial value failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_fsk_whitening_initial_value(llcc68_handle_t* handle, uint16_t value)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = (value >> 8) & 0xFF; /* set msb */
    buf[1] = (value >> 0) & 0xFF; /* set lsb */
    res = a_llcc68_spi_write_register(handle, LLCC68_REG_WHITENING_INIT_VALUE_MSB, (uint8_t*)buf, 2); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the whitening initial value in FSK mode
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *value pointer to a value buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fsk whitening initial value failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_fsk_whitening_initial_value(llcc68_handle_t* handle, uint16_t* value)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_WHITENING_INIT_VALUE_MSB, (uint8_t*)buf, 2); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }
    *value = (uint16_t)((uint16_t)buf[0] << 8 | buf[1]); /* set value */

    return 0; /* success return 0 */
}

/**
 * @brief     set the crc initial value in FSK mode
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] value set value
 * @return    status code
 *            - 0 success
 *            - 1 set fsk crc initial value failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_fsk_crc_initical_value(llcc68_handle_t* handle, uint16_t value)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* 芯片忙 */

        return 4; /* 返回错误 */
    }

    buf[0] = (value >> 8) & 0xFF; /* set msb */
    buf[1] = (value >> 0) & 0xFF; /* set lsb */
    res = a_llcc68_spi_write_register(handle, LLCC68_REG_CRC_INIT_VALUE_MSB, (uint8_t*)buf, 2); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the crc initical value in FSK mode
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *value pointer to a value buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fsk crc initical value failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_fsk_crc_initical_value(llcc68_handle_t* handle, uint16_t* value)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* 芯片忙 */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_CRC_INIT_VALUE_MSB, (uint8_t*)buf, 2); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }
    *value = (uint16_t)((uint16_t)buf[0] << 8 | buf[1]); /* set value */

    return 0; /* success return 0 */
}

/**
 * @brief     set the crc polynomial value in FSK mode
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] value set value
 * @return    status code
 *            - 0 success
 *            - 1 set fsk crc polynomial value failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_fsk_crc_polynomial_value(llcc68_handle_t* handle, uint16_t value)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    buf[0] = (value >> 8) & 0xFF; /* set msb */
    buf[1] = (value >> 0) & 0xFF; /* set lsb */
    res = a_llcc68_spi_write_register(handle, LLCC68_REG_CRC_POLYNOMIAL_VALUE_MSB, (uint8_t*)buf, 2); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the crc polynomial value in FSK mode
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *value pointer to a value buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fsk crc polynomial value failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_fsk_crc_polynomial_value(llcc68_handle_t* handle, uint16_t* value)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_CRC_POLYNOMIAL_VALUE_MSB, (uint8_t*)buf, 2); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }
    *value = (uint16_t)((uint16_t)buf[0] << 8 | buf[1]); /* set value */

    return 0; /* success return 0 */
}

/**
 * @brief     set the sync word in FSK mode
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] *sync_word pointer to sync word buffer
 * @return    status code
 *            - 0 success
 *            - 1 set fsk sync word failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_fsk_sync_word(llcc68_handle_t* handle, uint8_t sync_word[8])
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_SYNC_WORD_0, (uint8_t*)sync_word, 8); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the sync word in FSK mode
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *sync_word pointer to sync word buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fsk sync word failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_fsk_sync_word(llcc68_handle_t* handle, uint8_t sync_word[8])
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_SYNC_WORD_0, (uint8_t*)sync_word, 8); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the node address in FSK mode
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] addr node address
 * @return    status code
 *            - 0 success
 *            - 1 set fsk node address failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_fsk_node_address(llcc68_handle_t* handle, uint8_t addr)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_NODE_ADDRESS, (uint8_t*)&addr, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the node address in FSK mode
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *addr pointer to a node address buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fsk node address failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_fsk_node_address(llcc68_handle_t* handle, uint8_t* addr)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_NODE_ADDRESS, (uint8_t*)addr, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the broadcast address in FSK mode
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] addr broadcast address
 * @return    status code
 *            - 0 success
 *            - 1 set fsk broadcast address failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_fsk_broadcast_address(llcc68_handle_t* handle, uint8_t addr)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_BROADCAST_ADDRESS, (uint8_t*)&addr, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the broadcast address in FSK mode
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *addr pointer to a broadcast address buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fsk broadcast address failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_fsk_broadcast_address(llcc68_handle_t* handle, uint8_t* addr)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_BROADCAST_ADDRESS, (uint8_t*)addr, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the iq polarity
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] setup settings
 * @return    status code
 *            - 0 success
 *            - 1 set iq polarity failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_iq_polarity(llcc68_handle_t* handle, uint8_t setup)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_IQ_POLARITY_SETUP, (uint8_t*)&setup, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the iq polarity
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *setup pointer to a settings buffer
 * @return     status code
 *             - 0 success
 *             - 1 get iq polarity failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_iq_polarity(llcc68_handle_t* handle, uint8_t* setup)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* 芯片忙 */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_IQ_POLARITY_SETUP, (uint8_t*)setup, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the lora sync word
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] sync_word sync word
 * @return    status code
 *            - 0 success
 *            - 1 set lora sync word failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_lora_sync_word(llcc68_handle_t* handle, uint16_t sync_word)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* 芯片忙 */

        return 4; /* 返回错误 */
    }

    buf[0] = (sync_word >> 8) & 0xFF; /* set msb */
    buf[1] = (sync_word >> 0) & 0xFF; /* set lsb */
    res = a_llcc68_spi_write_register(handle, LLCC68_REG_LORA_SYNC_WORD_MSB, (uint8_t*)buf, 2); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the lora sync word
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *sync_word pointer to an sync word buffer
 * @return     status code
 *             - 0 success
 *             - 1 get lora sync word failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_lora_sync_word(llcc68_handle_t* handle, uint16_t* sync_word)
{
    uint8_t res;
    uint8_t buf[2];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_LORA_SYNC_WORD_MSB, (uint8_t*)buf, 2); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }
    *sync_word = (uint16_t)((uint16_t)buf[0] << 8 | buf[1]); /* set value */

    return 0; /* success return 0 */
}

/**
 * @brief      get the random number
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *r pointer to a random number buffer
 * @return     status code
 *             - 0 success
 *             - 1 get random number failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_random_number(llcc68_handle_t* handle, uint32_t* r)
{
    uint8_t res;
    uint8_t buf[4];

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* 芯片忙 */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_RANDOM_NUMBER_GEN_0, (uint8_t*)buf, 4); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }
    *r = (uint32_t)((uint32_t)buf[0] << 24 | (uint32_t)buf[1] << 16 |
        (uint32_t)buf[2] << 8 | buf[3]); /* set rand */

    return 0; /* success return 0 */
}

/**
 * @brief     set the tx modulation
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] modulation tx modulation
 * @return    status code
 *            - 0 success
 *            - 1 set tx modulation failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_tx_modulation(llcc68_handle_t* handle, uint8_t modulation)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_TX_MODULATION, (uint8_t*)&modulation, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the tx modulation
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *modulation pointer to a tx modulation buffer
 * @return     status code
 *             - 0 success
 *             - 1 get tx modulation failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_tx_modulation(llcc68_handle_t* handle, uint8_t* modulation)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_TX_MODULATION, (uint8_t*)modulation, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the rx gain
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] gain rx gain
 * @return    status code
 *            - 0 success
 *            - 1 set rx gain failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_rx_gain(llcc68_handle_t* handle, uint8_t gain)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_RX_GAIN, (uint8_t*)&gain, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the rx gain
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *gain pointer to an rx gain buffer
 * @return     status code
 *             - 0 success
 *             - 1 get rx gain failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_rx_gain(llcc68_handle_t* handle, uint8_t* gain)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_RX_GAIN, (uint8_t*)gain, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the tx clamp config
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] config tx clamp config
 * @return    status code
 *            - 0 success
 *            - 1 set tx clamp config failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_tx_clamp_config(llcc68_handle_t* handle, uint8_t config)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_TX_CLAMP_CONFIG, (uint8_t*)&config, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the tx clamp config
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *config pointer to a tx clamp config buffer
 * @return     status code
 *             - 0 success
 *             - 1 get tx clamp config failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_tx_clamp_config(llcc68_handle_t* handle, uint8_t* config)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_TX_CLAMP_CONFIG, (uint8_t*)config, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the ocp
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] ocp over current protection level
 * @return    status code
 *            - 0 success
 *            - 1 set ocp failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_ocp(llcc68_handle_t* handle, uint8_t ocp)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* 芯片忙 */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_OCP_CONFIGURATION, (uint8_t*)&ocp, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the ocp
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *ocp pointer to an over current protection level buffer
 * @return     status code
 *             - 0 success
 *             - 1 get ocp failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_ocp(llcc68_handle_t* handle, uint8_t* ocp)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_OCP_CONFIGURATION, (uint8_t*)ocp, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the rtc control
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] control rtc control
 * @return    status code
 *            - 0 success
 *            - 1 set rtc control failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_rtc_control(llcc68_handle_t* handle, uint8_t control)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_RTC_CONTROL, (uint8_t*)&control, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the rtc control
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *control pointer to an rtc control buffer
 * @return     status code
 *             - 0 success
 *             - 1 get rtc control failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_rtc_control(llcc68_handle_t* handle, uint8_t* control)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_RTC_CONTROL, (uint8_t*)control, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the xta trim
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] trim xta trim
 * @return    status code
 *            - 0 success
 *            - 1 set xta trim failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_xta_trim(llcc68_handle_t* handle, uint8_t trim)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_XTA_TRIM, (uint8_t*)&trim, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the xta trim
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *trim pointer to an xta trim buffer
 * @return     status code
 *             - 0 success
 *             - 1 get xta trim failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_xta_trim(llcc68_handle_t* handle, uint8_t* trim)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_XTA_TRIM, (uint8_t*)trim, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the xtb trim
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] trim xtb trim
 * @return    status code
 *            - 0 success
 *            - 1 set xtb trim failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_xtb_trim(llcc68_handle_t* handle, uint8_t trim)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_XTB_TRIM, (uint8_t*)&trim, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the xtb trim
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *trim pointer to an xtb trim buffer
 * @return     status code
 *             - 0 success
 *             - 1 get xtb trim failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_xtb_trim(llcc68_handle_t* handle, uint8_t* trim)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_XTB_TRIM, (uint8_t*)trim, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the dio3 output
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] control dio3 output control
 * @return    status code
 *            - 0 success
 *            - 1 set dio3 output control failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_dio3_output_control(llcc68_handle_t* handle, uint8_t control)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_DIO3_OUTPUT_CONTROL, (uint8_t*)&control, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the dio3 output
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *control pointer to a dio3 output control buffer
 * @return     status code
 *             - 0 success
 *             - 1 get dio3 output control failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_dio3_output_control(llcc68_handle_t* handle, uint8_t* control)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_DIO3_OUTPUT_CONTROL, (uint8_t*)control, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the event mask
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] mask event mask
 * @return    status code
 *            - 0 success
 *            - 1 set event mask failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_event_mask(llcc68_handle_t* handle, uint8_t mask)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_EVENT_MASK, (uint8_t*)&mask, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the event mask
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *mask pointer to an event mask buffer
 * @return     status code
 *             - 0 success
 *             - 1 get event mask failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_event_mask(llcc68_handle_t* handle, uint8_t* mask)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_EVENT_MASK, (uint8_t*)mask, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the dio output enable
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] enable enable settings
 * @return    status code
 *            - 0 success
 *            - 1 set dio output enable failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_dio_output_enable(llcc68_handle_t* handle, uint8_t enable)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_DIOX_OUTPUT_ENABLE, (uint8_t*)&enable, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the dio output enable
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *enable pointer to an enable settings buffer
 * @return     status code
 *             - 0 success
 *             - 1 get dio output enable failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_dio_output_enable(llcc68_handle_t* handle, uint8_t* enable)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_DIOX_OUTPUT_ENABLE, (uint8_t*)enable, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the dio input enable
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] enable enable settings
 * @return    status code
 *            - 0 success
 *            - 1 set dio input enable failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_dio_input_enable(llcc68_handle_t* handle, uint8_t enable)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_DIOX_INPUT_ENABLE, (uint8_t*)&enable, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the dio input enable
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *enable pointer to an enable settings buffer
 * @return     status code
 *             - 0 success
 *             - 1 get dio input enable failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_dio_input_enable(llcc68_handle_t* handle, uint8_t* enable)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_DIOX_INPUT_ENABLE, (uint8_t*)enable, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the pull up control
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] control pull up control
 * @return    status code
 *            - 0 success
 *            - 1 set pull up control failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_pull_up_control(llcc68_handle_t* handle, uint8_t control)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_DIOX_PULL_UP_CONTROL, (uint8_t*)&control, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the pull up control
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *control pointer to a pull up control buffer
 * @return     status code
 *             - 0 success
 *             - 1 get pull up control failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_pull_up_control(llcc68_handle_t* handle, uint8_t* control)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_DIOX_PULL_UP_CONTROL, (uint8_t*)control, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief     set the pull down control
 * @param[in] *handle pointer to an llcc68 handle structure
 * @param[in] control pull down control
 * @return    status code
 *            - 0 success
 *            - 1 set pull down control failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t llcc68_set_pull_down_control(llcc68_handle_t* handle, uint8_t control)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_write_register(handle, LLCC68_REG_DIOX_PULL_DOWN_CONTROL, (uint8_t*)&control, 1); /* 写寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: write register failed.\n"); /* write register failed */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get the pull down control
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[out] *control pointer to a pull down control buffer
 * @return     status code
 *             - 0 success
 *             - 1 get pull down control failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t llcc68_get_pull_down_control(llcc68_handle_t* handle, uint8_t* control)
{
    uint8_t res;

    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    res = a_llcc68_check_busy(handle); /* 检查忙状态 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: chip is busy.\n"); /* chip is busy */

        return 4; /* 返回错误 */
    }

    res = a_llcc68_spi_read_register(handle, LLCC68_REG_DIOX_PULL_DOWN_CONTROL, (uint8_t*)control, 1); /* 读寄存器 */
    if (res != 0) /* 检查结果 */
    {
        handle->debug_print("llcc68: read register failed.\n"); /* 读寄存器失败 */

        return 1; /* 返回错误 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      write and read register
 * @param[in]  *handle pointer to an llcc68 handle structure
 * @param[in]  *in_buf pointer to an input buffer
 * @param[in]  in_len input length
 * @param[out] *out_buf pointer to an output buffer
 * @param[in]  out_len output length
 * @return     status code
 *             - 0 success
 *             - 1 write read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t llcc68_write_read_reg(llcc68_handle_t* handle, uint8_t* in_buf, uint32_t in_len,
                              uint8_t* out_buf, uint32_t out_len)
{
    if (handle == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }
    if (handle->inited != 1) /* 检查句柄初始化 */
    {
        return 3; /* 返回错误 */
    }

    if (handle->spi_write_read(in_buf, in_len, out_buf, out_len) != 0) /* spi read */
    {
        return 1; /* 返回错误 */
    }
    else
    {
        return 0; /* success return 0 */
    }
}

/**
 * @brief      get chip's information
 * @param[out] *info pointer to an llcc68 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t llcc68_info(llcc68_info_t* info)
{
    if (info == NULL) /* check handle */
    {
        return 2; /* 返回错误 */
    }

    memset(info, 0, sizeof(llcc68_info_t)); /* initialize llcc68 info structure */
    strncpy(info->chip_name, CHIP_NAME, 32); /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32); /* copy manufacturer name */
    strncpy(info->interface, "SPI", 8); /* copy interface name */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN; /* set minimal supply voltage */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX; /* set maximum supply voltage */
    info->max_current_ma = MAX_CURRENT; /* set maximum current */
    info->temperature_max = TEMPERATURE_MAX; /* set minimal temperature */
    info->temperature_min = TEMPERATURE_MIN; /* set maximum temperature */
    info->driver_version = DRIVER_VERSION; /* set driver version */

    return 0; /* success return 0 */
}
