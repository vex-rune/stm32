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
 * @file      driver_llcc68_lora.c
 * @brief     LLCC68 LoRa 驱动源文件
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

#include "driver_llcc68_lora.h"

static llcc68_handle_t gs_handle;        /**< llcc68 句柄 */

/**
 * @brief  LLCC68 LoRa 中断处理函数
 * @return 状态码
 *         - 0 成功
 *         - 1 运行失败
 * @note   无
 */
uint8_t llcc68_lora_irq_handler(void)
{
    if (llcc68_irq_handler(&gs_handle) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief     LoRa 示例初始化
 * @param[in] *callback 指向回调函数的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 初始化失败
 * @note      无
 */
uint8_t llcc68_lora_init(void (*callback)(uint16_t type, uint8_t *buf, uint16_t len))
{
    uint8_t res;
    uint32_t reg;
    uint8_t modulation;
    uint8_t config;
    
    /* 链接接口函数 */
    DRIVER_LLCC68_LINK_INIT(&gs_handle, llcc68_handle_t);
    DRIVER_LLCC68_LINK_SPI_INIT(&gs_handle, llcc68_interface_spi_init);
    DRIVER_LLCC68_LINK_SPI_DEINIT(&gs_handle, llcc68_interface_spi_deinit);
    DRIVER_LLCC68_LINK_SPI_WRITE_READ(&gs_handle, llcc68_interface_spi_write_read);
    DRIVER_LLCC68_LINK_RESET_GPIO_INIT(&gs_handle, llcc68_interface_reset_gpio_init);
    DRIVER_LLCC68_LINK_RESET_GPIO_DEINIT(&gs_handle, llcc68_interface_reset_gpio_deinit);
    DRIVER_LLCC68_LINK_RESET_GPIO_WRITE(&gs_handle, llcc68_interface_reset_gpio_write);
    DRIVER_LLCC68_LINK_BUSY_GPIO_INIT(&gs_handle, llcc68_interface_busy_gpio_init);
    DRIVER_LLCC68_LINK_BUSY_GPIO_DEINIT(&gs_handle, llcc68_interface_busy_gpio_deinit);
    DRIVER_LLCC68_LINK_BUSY_GPIO_READ(&gs_handle, llcc68_interface_busy_gpio_read);
    DRIVER_LLCC68_LINK_DELAY_MS(&gs_handle, llcc68_interface_delay_ms);
    DRIVER_LLCC68_LINK_DEBUG_PRINT(&gs_handle, llcc68_interface_debug_print);
    DRIVER_LLCC68_LINK_RECEIVE_CALLBACK(&gs_handle, callback);
    
    /* 初始化 llcc68 */
    res = llcc68_init(&gs_handle);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 初始化失败.\n");
       
        return 1;
    }
    
    /* 进入待机模式 */
    res = llcc68_set_standby(&gs_handle, LLCC68_CLOCK_SOURCE_XTAL_32MHZ);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置待机模式失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置前导码停止定时器 */
    res = llcc68_set_stop_timer_on_preamble(&gs_handle, LLCC68_LORA_DEFAULT_STOP_TIMER_ON_PREAMBLE);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置前导码停止定时器失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置稳压器模式 */
    res = llcc68_set_regulator_mode(&gs_handle, LLCC68_LORA_DEFAULT_REGULATOR_MODE);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置稳压器模式失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置 PA 配置 */
    res = llcc68_set_pa_config(&gs_handle, LLCC68_LORA_DEFAULT_PA_CONFIG_DUTY_CYCLE, LLCC68_LORA_DEFAULT_PA_CONFIG_HP_MAX);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置 PA 配置失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 进入 STDBY XOSC 模式 */
    res = llcc68_set_rx_tx_fallback_mode(&gs_handle, LLCC68_RX_TX_FALLBACK_MODE_STDBY_XOSC);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置收发回退模式失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置 DIO 中断 */
    res = llcc68_set_dio_irq_params(&gs_handle, 0x03FF, 0x03FF, 0x0000, 0x0000);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置 DIO 中断参数失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 清除中断状态 */
    res = llcc68_clear_irq_status(&gs_handle, 0x03FF);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 清除中断状态失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置 LoRa 模式 */
    res = llcc68_set_packet_type(&gs_handle, LLCC68_PACKET_TYPE_LORA);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置数据包类型失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置发送参数 */
    res = llcc68_set_tx_params(&gs_handle, LLCC68_LORA_DEFAULT_TX_DBM, LLCC68_LORA_DEFAULT_RAMP_TIME);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置发送参数失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置 LoRa 调制参数 */
    res = llcc68_set_lora_modulation_params(&gs_handle, LLCC68_LORA_DEFAULT_SF, LLCC68_LORA_DEFAULT_BANDWIDTH, 
                                            LLCC68_LORA_DEFAULT_CR, LLCC68_LORA_DEFAULT_LOW_DATA_RATE_OPTIMIZE);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置 LoRa 调制参数失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 转换频率 */
    res = llcc68_frequency_convert_to_register(&gs_handle, LLCC68_LORA_DEFAULT_RF_FREQUENCY, (uint32_t *)&reg);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 转换到寄存器失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置频率 */
    res = llcc68_set_rf_frequency(&gs_handle, reg);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置射频频率失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置基地址 */
    res = llcc68_set_buffer_base_address(&gs_handle, 0x00, 0x00);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置缓冲区基地址失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置 LoRa 符号数超时 */
    res = llcc68_set_lora_symb_num_timeout(&gs_handle, LLCC68_LORA_DEFAULT_SYMB_NUM_TIMEOUT);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置 LoRa 符号数超时失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 重置统计 */
    res = llcc68_reset_stats(&gs_handle, 0x0000, 0x0000, 0x0000);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 重置统计失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 清除设备错误 */
    res = llcc68_clear_device_errors(&gs_handle);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 清除设备错误失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置 LoRa 同步字 */
    res = llcc68_set_lora_sync_word(&gs_handle, LLCC68_LORA_DEFAULT_SYNC_WORD);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置 LoRa 同步字失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 获取发送调制 */
    res = llcc68_get_tx_modulation(&gs_handle, (uint8_t *)&modulation);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 获取发送调制失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    modulation |= 0x04;
    
    /* 设置发送调制 */
    res = llcc68_set_tx_modulation(&gs_handle, modulation);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置发送调制失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置接收增益 */
    res = llcc68_set_rx_gain(&gs_handle, LLCC68_LORA_DEFAULT_RX_GAIN);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置接收增益失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置过流保护 */
    res = llcc68_set_ocp(&gs_handle, LLCC68_LORA_DEFAULT_OCP);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置过流保护失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 获取发送钳位配置 */
    res = llcc68_get_tx_clamp_config(&gs_handle, (uint8_t *)&config);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 获取发送钳位配置失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    config |= 0x1E;
    
    /* 设置发送钳位配置 */
    res = llcc68_set_tx_clamp_config(&gs_handle, config);
    if (res != 0)
    {
        llcc68_interface_debug_print("llcc68: 设置发送钳位配置失败.\n");
        (void)llcc68_deinit(&gs_handle);
        
        return 1;
    }
    
    return 0;
}

/**
 * @brief  LoRa 示例反初始化
 * @return 状态码
 *         - 0 成功
 *         - 1 反初始化失败
 * @note   无
 */
uint8_t llcc68_lora_deinit(void)
{
    if (llcc68_deinit(&gs_handle) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  LoRa 示例进入睡眠模式
 * @return 状态码
 *         - 0 成功
 *         - 1 睡眠失败
 * @note   无
 */
uint8_t llcc68_lora_sleep(void)
{
    if (llcc68_set_sleep(&gs_handle, LLCC68_LORA_DEFAULT_START_MODE, LLCC68_LORA_DEFAULT_RTC_WAKE_UP) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  LoRa 示例唤醒芯片
 * @return 状态码
 *         - 0 成功
 *         - 1 唤醒失败
 * @note   无
 */
uint8_t llcc68_lora_wake_up(void)
{
    uint8_t status;
    
    if (llcc68_get_status(&gs_handle, (uint8_t *)&status) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  LoRa 示例进入连续接收模式
 * @return 状态码
 *         - 0 成功
 *         - 1 进入失败
 * @note   无
 */
uint8_t llcc68_lora_set_continuous_receive_mode(void)
{
    uint8_t setup;
    
    /* 设置 DIO 中断 */
    if (llcc68_set_dio_irq_params(&gs_handle, LLCC68_IRQ_RX_DONE | LLCC68_IRQ_TIMEOUT | LLCC68_IRQ_CRC_ERR | LLCC68_IRQ_CAD_DONE | LLCC68_IRQ_CAD_DETECTED,
                                  LLCC68_IRQ_RX_DONE | LLCC68_IRQ_TIMEOUT | LLCC68_IRQ_CRC_ERR | LLCC68_IRQ_CAD_DONE | LLCC68_IRQ_CAD_DETECTED,
                                  0x0000, 0x0000) != 0)
    {
        return 1;
    }
    
    /* 清除中断状态 */
    if (llcc68_clear_irq_status(&gs_handle, 0x03FFU) != 0)
    {
        return 1;
    }
    
    /* 设置 LoRa 数据包参数 */
    if (llcc68_set_lora_packet_params(&gs_handle, LLCC68_LORA_DEFAULT_PREAMBLE_LENGTH,
                                      LLCC68_LORA_DEFAULT_HEADER, LLCC68_LORA_DEFAULT_BUFFER_SIZE,
                                      LLCC68_LORA_DEFAULT_CRC_TYPE, LLCC68_LORA_DEFAULT_INVERT_IQ) != 0)
    {
        return 1;
    }
    
    /* 获取 IQ 极性 */
    if (llcc68_get_iq_polarity(&gs_handle, (uint8_t *)&setup) != 0)
    {
        return 1;
    }
    
#if LLCC68_LORA_DEFAULT_INVERT_IQ == LLCC68_BOOL_FALSE
    setup |= 1 << 2;
#else
    setup &= ~(1 << 2);
#endif
    
    /* 设置 IQ 极性 */
    if (llcc68_set_iq_polarity(&gs_handle, setup) != 0)
    {
        return 1;
    }
    
    /* 开始接收 */
    if (llcc68_continuous_receive(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     LoRa 示例进入单次接收模式
 * @param[in] us 超时时间，单位为微秒
 * @return    状态码
 *            - 0 成功
 *            - 1 进入失败
 * @note      无
 */
uint8_t llcc68_lora_set_shot_receive_mode(double us)
{
    uint8_t setup;
    
    /* 设置 DIO 中断 */
    if (llcc68_set_dio_irq_params(&gs_handle, LLCC68_IRQ_RX_DONE | LLCC68_IRQ_TIMEOUT | LLCC68_IRQ_CRC_ERR | LLCC68_IRQ_CAD_DONE | LLCC68_IRQ_CAD_DETECTED,
                                  LLCC68_IRQ_RX_DONE | LLCC68_IRQ_TIMEOUT | LLCC68_IRQ_CRC_ERR | LLCC68_IRQ_CAD_DONE | LLCC68_IRQ_CAD_DETECTED,
                                  0x0000, 0x0000) != 0)
    {
        return 1;
    }
    
    /* 清除中断状态 */
    if (llcc68_clear_irq_status(&gs_handle, 0x03FFU) != 0)
    {
        return 1;
    }
    
    /* 设置 LoRa 数据包参数 */
    if (llcc68_set_lora_packet_params(&gs_handle, LLCC68_LORA_DEFAULT_PREAMBLE_LENGTH,
                                      LLCC68_LORA_DEFAULT_HEADER, LLCC68_LORA_DEFAULT_BUFFER_SIZE,
                                      LLCC68_LORA_DEFAULT_CRC_TYPE, LLCC68_LORA_DEFAULT_INVERT_IQ) != 0)
    {
        return 1;
    }
    
    /* 获取 IQ 极性 */
    if (llcc68_get_iq_polarity(&gs_handle, (uint8_t *)&setup) != 0)
    {
        return 1;
    }
    
#if LLCC68_LORA_DEFAULT_INVERT_IQ == LLCC68_BOOL_FALSE
    setup |= 1 << 2;
#else
    setup &= ~(1 << 2);
#endif
    
    /* 设置 IQ 极性 */
    if (llcc68_set_iq_polarity(&gs_handle, setup) != 0)
    {
        return 1;
    }
    
    /* 开始接收 */
    if (llcc68_single_receive(&gs_handle, us) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  LoRa 示例进入发送模式
 * @return 状态码
 *         - 0 成功
 *         - 1 进入失败
 * @note   无
 */
uint8_t llcc68_lora_set_send_mode(void)
{
    /* 设置 DIO 中断 */
    if (llcc68_set_dio_irq_params(&gs_handle, LLCC68_IRQ_TX_DONE | LLCC68_IRQ_TIMEOUT | LLCC68_IRQ_CAD_DONE | LLCC68_IRQ_CAD_DETECTED,
                                  LLCC68_IRQ_TX_DONE | LLCC68_IRQ_TIMEOUT | LLCC68_IRQ_CAD_DONE | LLCC68_IRQ_CAD_DETECTED,
                                  0x0000, 0x0000) != 0)
    {
        return 1;
    }
    
    /* 清除中断状态 */
    if (llcc68_clear_irq_status(&gs_handle, 0x03FFU) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     LoRa 示例发送 LoRa 数据
 * @param[in] *buf 指向数据缓冲区的指针
 * @param[in] len 数据长度
 * @return    状态码
 *            - 0 成功
 *            - 1 发送失败
 * @note      无
 */
uint8_t llcc68_lora_send(uint8_t *buf, uint16_t len)
{
    /* 发送数据 */
    if (llcc68_lora_transmit(&gs_handle, LLCC68_CLOCK_SOURCE_XTAL_32MHZ,
                             LLCC68_LORA_DEFAULT_PREAMBLE_LENGTH, LLCC68_LORA_DEFAULT_HEADER,
                             LLCC68_LORA_DEFAULT_CRC_TYPE, LLCC68_LORA_DEFAULT_INVERT_IQ,
                            (uint8_t *)buf, len, 0) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      LoRa 示例运行信道活动检测 (CAD)
 * @param[out] *enable 指向使能状态缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 运行失败
 * @note       无
 */
uint8_t llcc68_lora_run_cad(llcc68_bool_t *enable)
{
    /* 设置 CAD 参数 */
    if (llcc68_set_cad_params(&gs_handle, LLCC68_LORA_DEFAULT_CAD_SYMBOL_NUM,
                              LLCC68_LORA_DEFAULT_CAD_DET_PEAK, LLCC68_LORA_DEFAULT_CAD_DET_MIN,
                              LLCC68_LORA_CAD_EXIT_MODE_ONLY, 0) != 0)
    {
        return 1;
    }
    
    /* 运行 CAD */
    if (llcc68_lora_cad(&gs_handle, enable) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      LoRa 示例获取状态
 * @param[out] *rssi 指向 RSSI 缓冲区的指针
 * @param[out] *snr 指向 SNR 缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取状态失败
 * @note       无
 */
uint8_t llcc68_lora_get_status(float *rssi, float *snr)
{
    uint8_t rssi_pkt_raw;
    int8_t snr_pkt_raw;
    uint8_t signal_rssi_pkt_raw;
    float signal_rssi_pkt;
    
    /* 获取状态 */
    if (llcc68_get_lora_packet_status(&gs_handle, (uint8_t *)&rssi_pkt_raw, (int8_t *)&snr_pkt_raw,
                                     (uint8_t *)&signal_rssi_pkt_raw, (float *)rssi, (float *)snr, (float *)&signal_rssi_pkt) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      LoRa 示例检查数据包错误
 * @param[out] *enable 指向布尔值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 检查数据包错误失败
 * @note       无
 */
uint8_t llcc68_lora_check_packet_error(llcc68_bool_t *enable)
{
    /* 检查错误 */
    if (llcc68_check_packet_error(&gs_handle, enable) != 0)
    {
        return 1;
    }

    return 0;
}
