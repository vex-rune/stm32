#include  "lora.h"

#include "spi.h"

#include <string.h>

#include "driver_llcc68.h"
#include "driver_llcc68_interface.h"

/* 定义全局结构体对象, 剧本 */
static llcc68_handle_t gs_handle;

// 初始化
uint8_t lora_init(void)
{
    printf(">>>>>>>>>>>>>>>> llcc68: 初始化. <<<<<<<<<<<<<<<<<<<<<<\r\n");

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
    DRIVER_LLCC68_LINK_RECEIVE_CALLBACK(&gs_handle, llcc68_interface_receive_callback);

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
    res = llcc68_set_pa_config(&gs_handle, LLCC68_LORA_DEFAULT_PA_CONFIG_DUTY_CYCLE,
                               LLCC68_LORA_DEFAULT_PA_CONFIG_HP_MAX);
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


    // 暂时没用到
    // /* 设置 DIO 中断 */
    // res = llcc68_set_dio_irq_params(&gs_handle, 0x03FF, 0x03FF, 0x0000, 0x0000);
    // if (res != 0)
    // {
    //     llcc68_interface_debug_print("llcc68: 设置 DIO 中断参数失败.\n");
    //     (void)llcc68_deinit(&gs_handle);
    //
    //     return 1;
    // }
    //
    // /* 清除中断状态 */
    // res = llcc68_clear_irq_status(&gs_handle, 0x03FF);
    // if (res != 0)
    // {
    //     llcc68_interface_debug_print("llcc68: 清除中断状态失败.\n");
    //     (void)llcc68_deinit(&gs_handle);
    //
    //     return 1;
    // }

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

    /* 转换频率 国内: 470-510 Mhz*/
    res = llcc68_frequency_convert_to_register(&gs_handle, LLCC68_LORA_DEFAULT_RF_FREQUENCY, (uint32_t*)&reg);
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


    ///////////////////////////////////////////////
    /// 解决了硬件可能出现的问题, 请看 15.1 Modulation Quality with 500 kHz LoRa® Bandwidth
    /* 获取发送调制 */
    res = llcc68_get_tx_modulation(&gs_handle, (uint8_t*)&modulation);
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
    ///////////////////////////////////////////////

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
    res = llcc68_get_tx_clamp_config(&gs_handle, (uint8_t*)&config);
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

    printf(">>>>>>>>>>>>>>>>>>>>>> llcc68: lora init done. <<<<<<<<<<<<<<<<<<<<<<<<<< \r\n");

    return 0;
}

/**
 * @brief  内部：进入发送模式（设置 IRQ + 清状态）
 * @note   仅供 lora_send() 调用，不暴露到 .h
 */
static uint8_t lora_set_send_mode_internal(void)
{
    /* 设置 DIO 中断 */
    if (llcc68_set_dio_irq_params(
        &gs_handle, LLCC68_IRQ_TX_DONE | LLCC68_IRQ_TIMEOUT | LLCC68_IRQ_CAD_DONE | LLCC68_IRQ_CAD_DETECTED,
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
 * @brief  内部：调用底层 llcc68_lora_transmit 发送
 * @note   仅供 lora_send() 调用
 */
static uint8_t lora_send_internal(uint8_t* buf, uint16_t len)
{
    /* 发送数据 */
    if (llcc68_lora_transmit(&gs_handle, LLCC68_CLOCK_SOURCE_XTAL_32MHZ,
                             LLCC68_LORA_DEFAULT_PREAMBLE_LENGTH, LLCC68_LORA_DEFAULT_HEADER,
                             LLCC68_LORA_DEFAULT_CRC_TYPE, LLCC68_LORA_DEFAULT_INVERT_IQ,
                             (uint8_t*)buf, len, 0) != 0)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief  发送一包 LoRa 数据
 */
void lora_send(uint8_t* buf, uint16_t len)
{
    /* 切射频开关到 TX 模式 */
    TXEN_HIGH;
    RXEN_LOW;
    llcc68_interface_delay_ms(5);

    /* 进入发送模式 + 发送 */
    lora_set_send_mode_internal();
    lora_send_internal(buf, len);
}

// 进入连续接受模式
uint8_t lora_receive_mode(void)
{
    uint8_t setup;

    /* 切射频开关到 RX 模式 */
    RXEN_HIGH;
    TXEN_LOW;
    llcc68_interface_delay_ms(5);

    /* 设置 DIO 中断 */
    if (llcc68_set_dio_irq_params(
        &gs_handle,
        LLCC68_IRQ_RX_DONE | LLCC68_IRQ_TIMEOUT | LLCC68_IRQ_CRC_ERR | LLCC68_IRQ_CAD_DONE | LLCC68_IRQ_CAD_DETECTED,
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
    if (llcc68_get_iq_polarity(&gs_handle, (uint8_t*)&setup) != 0)
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

/*======================================================================*/
/*                  分步式接收 API（main 按顺序调用）*/
/*======================================================================*/

/**
 * @brief  查询芯片 IRQ 状态，返回是否收到完整一包
 * @retval 1 收到 / 0 未收到 / 出错
 */
uint8_t lora_check_rx(void)
{
    uint16_t irq = 0;
    if (llcc68_get_irq_status(&gs_handle, &irq) != 0)
        return 0;
    return (irq & LLCC68_IRQ_RX_DONE) ? 1 : 0;
}

/**
 * @brief  从 RX FIFO 读取一包数据
 * @param[out] buf  接收缓冲区
 * @param[out] len  实际字节数
 * @retval 0 成功 / 1 失败
 */
uint8_t lora_read_packet(uint8_t *buf, uint16_t *len)
{
    uint8_t payload_len = 0, start_ptr = 0;
    uint8_t payload[255];

    if (llcc68_get_rx_buffer_status(&gs_handle, &payload_len, &start_ptr) != 0)
        return 1;
    if (payload_len == 0 || payload_len > sizeof(payload))
        return 1;
    if (llcc68_read_buffer(&gs_handle, start_ptr, payload, payload_len) != 0)
        return 1;

    memcpy(buf, payload, payload_len);
    *len = payload_len;
    return 0;
}

/**
 * @brief  读取最近一包的 RSSI / SNR
 * @param[out] rssi 信号强度 dBm
 * @param[out] snr  信噪比 dB
 */
void lora_get_rssi_snr(float *rssi, float *snr)
{
    uint8_t rssi_raw = 0, sig_raw = 0;
    int8_t  snr_raw  = 0;
    float   r = 0.0f, s = 0.0f;
    llcc68_get_lora_packet_status(&gs_handle,
        &rssi_raw, &snr_raw, &sig_raw, &r, &s, NULL);
    if (rssi) *rssi = r;
    if (snr)  *snr  = s;
}

/**
 * @brief  处理 IRQ 收尾：清标志 + 重新进入 RX
 * @note   收到包 / 超时 / CRC错 / 报头错 时调用
 */
void lora_resume_rx(void)
{
    uint16_t irq = 0;
    llcc68_get_irq_status(&gs_handle, &irq);

    if (irq & (LLCC68_IRQ_TIMEOUT | LLCC68_IRQ_CRC_ERR
             | LLCC68_IRQ_HEADER_ERR | LLCC68_IRQ_RX_DONE))
    {
        llcc68_clear_irq_status(&gs_handle, 0x03FFU);
        llcc68_continuous_receive(&gs_handle);
    }
}