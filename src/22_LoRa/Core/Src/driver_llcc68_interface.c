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
 * @file      driver_llcc68_interface_template.c
 * @brief     driver llcc68 interface template source file
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

#include "driver_llcc68_interface.h"

#include <stdint.h>
#include "driver_llcc68.h"
#include "spi.h"
#include "stdarg.h"

/**
 * @brief  interface spi bus init
 * @return status code
 *         - 0 success
 *         - 1 spi init failed
 * @note   none
 */
uint8_t llcc68_interface_spi_init(void)
{
    // see: main.c -> MX_SPI1_Init, 需要注释掉
    return HAL_SPI_Init(&hspi1) == HAL_OK ? 0 : 1;
}

/**
 * @brief  interface spi bus deinit
 * @return status code
 *         - 0 success
 *         - 1 spi deinit failed
 * @note   none
 */
uint8_t llcc68_interface_spi_deinit(void)
{
    return HAL_SPI_DeInit(&hspi1) == HAL_OK ? 0 : 1;
}

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
                                        uint8_t* out_buf, uint32_t out_len)
{
    // 片选使能开始
    CS_LOW;
    if (in_len > 0)
    {
        // 发送
        HAL_StatusTypeDef transmit_State = HAL_SPI_Transmit(&hspi1, in_buf, in_len, 1000);
        if (transmit_State != HAL_OK)
        {
            CS_HIGH;
            return 1;
        }
    }


    if (out_len > 0)
    {
        // 接收
        HAL_StatusTypeDef receive_State = HAL_SPI_Receive(&hspi1, out_buf, out_len, 1000);

        if (receive_State != HAL_OK)
        {
            CS_HIGH;
            return 1;
        }
    }


    // 片选使能结束
    CS_HIGH;

    return 0;
}

/**
 * @brief  interface reset gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t llcc68_interface_reset_gpio_init(void)
{
    // main 中统一调用
    return 0;
}

/**
 * @brief  interface reset gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t llcc68_interface_reset_gpio_deinit(void)
{
    // main 中统一调用
    return 0;
}

/**
 * @brief     interface reset gpio write
 * @param[in] data written data
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t llcc68_interface_reset_gpio_write(uint8_t data)
{
    if (data == 0)
    {
        RST_LOW;
    }
    else
    {
        RST_HIGH;
    }
    return 0;
}

/**
 * @brief  interface busy gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t llcc68_interface_busy_gpio_init(void)
{
    return 0;
}

/**
 * @brief  interface busy gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t llcc68_interface_busy_gpio_deinit(void)
{
    return 0;
}

/**
 * @brief      interface busy gpio read
 * @param[out] *value pointer to a value buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t llcc68_interface_busy_gpio_read(uint8_t* value)
{
    *value = (uint8_t)BUSY_READ;
    return 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms time
 * @note      none
 */
void llcc68_interface_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt format data
 * @note      none
 */
void llcc68_interface_debug_print(const char* const fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

/**
 * @brief     interface receive callback
 * @param[in] type receive callback type
 * @param[in] *buf pointer to a buffer address
 * @param[in] len buffer length
 * @note      none
 */
void llcc68_interface_receive_callback(uint16_t type, uint8_t* buf, uint16_t len)
{
    switch (type)
    {
        // =======================================================================
        // 发送完成中断 (TX Done, 0x0001, bit0)
        // 触发条件：MCU 调用 llcc68_set_tx() 启动发送后，芯片完成整个 LoRa 物理帧
        //           的空中发送（包含前导码 Preamble + 同步字 Sync Word + SFD 帧起
        //           始分隔符 + Header + Payload + CRC 校验码）后自动置位。
        // 典型应用：发射机模式下，在该中断里可以切回 STDBY 模式以节省功耗，或
        //           立即切到 RX 模式进入接收态以实现收发联动。
        // 配合使用：驱动层在收到该事件后会调用 user callback，用户可在其中释放
        //           发送缓冲区、点 LEDs 指示发送完成、统计发送成功率等。
        // =======================================================================
    case LLCC68_IRQ_TX_DONE:
        {
            llcc68_interface_debug_print("llcc68: irq tx done.\n");

            break;
        }
        // =======================================================================
        // 接收完成中断 (RX Done, 0x0002, bit1)
        // 触发条件：芯片在 RX 模式下成功接收一个完整的 LoRa 物理帧，且 CRC 校验
        //           通过（即 Payload 末端的 CRC 与本地计算结果一致）。芯片会自动
        //           将接收到的 Payload 数据存入内部 RX FIFO，MCU 通过 SPI 读取。
        // 典型应用：调用 llcc68_get_rx_buffer() / llcc68_get_packet_status() 取出
        //           有效数据、RSSI、SNR 等信息做业务处理（上行上报、命令解析）。
        // 与 CRC_ERR 区别：RX_DONE 表示一帧“业务上”有效；CRC_ERR 则表示检测到
        //           一帧但校验失败，FIFO 中的内容被丢弃，不应使用。
        // =======================================================================
    case LLCC68_IRQ_RX_DONE:
        {
            llcc68_interface_debug_print("llcc68: irq rx done.\n");

            break;
        }
        // =======================================================================
        // 前导码检测中断 (Preamble Detected, 0x0004, bit2)
        // 触发条件：在 RX 模式下，芯片检测到空中存在与本地配置匹配的 Preamble
        //           前导码 chirp 序列（典型长度 8~12 个符号）。注意这只是“可能
        //           有信号”，并不保证后续 Sync Word / Header / Payload 一定能解
        //           调成功。
        // 典型应用：
        //   1) 用作信道粗略占用指示，唤醒上层业务做预处理；
        //   2) 用于测量前导码长度、信道质量统计；
        //   3) 在一些低功耗协议中，前导码检测可作为“被叫唤醒”信号。
        // 是否上报可通过 llcc68_set_dio_irq_params() 配置。
        // =======================================================================
    case LLCC68_IRQ_PREAMBLE_DETECTED:
        {
            llcc68_interface_debug_print("llcc68: irq preamble detected.\n");

            break;
        }
        // =======================================================================
        // 同步字有效中断 (Sync Word Valid, 0x0008, bit3)
        // 触发条件：硬件在 Preamble 之后正确解调出 1~2 字节的 Sync Word，且与
        //           本地配置的 SyncWord（私有网常用 0x12，LoRaWAN 公网 0x34）完
        //           全匹配。匹配是网络隔离的最重要手段：不匹配的帧芯片在硬件层
        //           就直接丢弃，不会上报到 MCU，极大降低非本网信号对 MCU 的打扰。
        // 典型应用：可用于“打底”唤醒通知，比 RX_DONE 早，作为业务准备信号。
        // =======================================================================
    case LLCC68_IRQ_SYNC_WORD_VALID:
        {
            llcc68_interface_debug_print("llcc68: irq valid sync word detected.\n");

            break;
        }
        // =======================================================================
        // 显式包头有效中断 (Header Valid, 0x0010, bit4)
        // 触发条件：LoRa 处于“显式头模式”（Explicit Header Mode，默认）时，芯片
        //           成功解析出 Header 字段（Header 含：Payload 长度、使用的编码
        //           率 CR、是否使能 CRC）。只有 Header 解析正确后，芯片才会进一
        //           步按 Header 中声明的长度去收 Payload。
        // 注意事项：
        //   1) 若切换为“隐式头模式”（Implicit Header），本 IRQ 永远不会触发；
        //   2) Header 错误则触发 LLCC68_IRQ_HEADER_ERR。
        // 典型应用：用户可以信任 Header 中的 payload 长度来设置读取字节数。
        // =======================================================================
    case LLCC68_IRQ_HEADER_VALID:
        {
            llcc68_interface_debug_print("llcc68: irq valid header.\n");

            break;
        }
        // =======================================================================
        // 显式包头错误中断 (Header Error, 0x0020, bit5)
        // 触发条件：显式头模式下，Header 内容校验失败或格式异常。常见原因包括：
        //           1) 空中干扰过强，Header 比特出错；
        //           2) 收发两端配置不匹配（如对端用 Implicit Header 而本端用
        //              Explicit Header，或两端的 Coding Rate 设置不一致等）。
        // 后续动作：当前帧被直接丢弃，芯片自动返回 RX 等待下一帧；FIFO 内容无
        //           效，不要读取业务数据。
        // 典型应用：在用户层统计 Header 错误率，用于评估链路质量或排查参数配置
        //           一致性问题。
        // =======================================================================
    case LLCC68_IRQ_HEADER_ERR:
        {
            llcc68_interface_debug_print("llcc68: irq header error.\n");

            break;
        }
        // =======================================================================
        // CRC 校验错误中断 (CRC Error, 0x0040, bit6)
        // 触发条件：Payload 末端的硬件 CRC 与本地计算结果不一致。说明数据包虽
        //           被捕获，但因空中噪声、距离过远、多径衰落等原因产生了无法通
        //           过 FEC（前向纠错，依赖 CR 设置）纠正的误码。
        // 后续动作：当前帧被丢弃，FIFO 中的数据不应使用。
        // 典型应用：用户层统计 CRC 错误率，是衡量链路质量/距离裕量的重要指标；
        //           持续出现 CRC_ERR 可考虑提高 CR（如 CR4）、提高 SF（如 SF12）、
        //           降低 BW（如 125kHz）以提升灵敏度。
        // =======================================================================
    case LLCC68_IRQ_CRC_ERR:
        {
            llcc68_interface_debug_print("llcc68: irq crc error.\n");

            break;
        }
        // =======================================================================
        // CAD（信道活动检测）完成中断 (CAD Done, 0x0080, bit7)
        // 触发条件：MCU 调用 llcc68_set_cad() 启动一次信道活动检测过程，过程结
        //           束后无论是否检测到信道活动都会置位。
        // 关于 CAD：芯片会在短时间内扫描当前信道是否存在 LoRa Preamble，扫描
        //           结束后立即回到 STDBY 模式，非常省电，是 LBT（Listen Before
        //           Talk，先听后说）协议的基础。
        // 典型应用：必须与 LLCC68_IRQ_CAD_DETECTED 配合使用——
        //           仅 CAD_DONE 不代表“有信号”，需要再读 CAD_DETECTED 状态。
        // =======================================================================
    case LLCC68_IRQ_CAD_DONE:
        {
            llcc68_interface_debug_print("llcc68: irq cad done.\n");

            break;
        }
        // =======================================================================
        // CAD 检测到信道活动中断 (CAD Detected, 0x0100, bit8)
        // 触发条件：CAD 扫描过程中，芯片检测到与本地配置相符的 LoRa Preamble，
        //           意味着当前信道正被其他 LoRa 设备占用。
        // 典型应用：
        //   1) 随机退避后重试发送，避免冲突（CSMA-CA 思路）；
        //   2) 与 LLCC68_IRQ_CAD_DONE 组合判断：只有同时存在两个标志才表示
        //      “本轮 CAD 完成且信道忙”；只 CAD_DONE 单独出现则表示“信道空闲”。
        //   3) 在一些以环境监测为目的的嗅探节点中，CAD_DETECTED 可作为“网络
        //      活动”统计指标。
        // =======================================================================
    case LLCC68_IRQ_CAD_DETECTED:
        {
            llcc68_interface_debug_print("llcc68: irq cad detected.\n");

            break;
        }
        // =======================================================================
        // 接收超时中断 (Timeout, 0x0200, bit9)
        // 触发条件：MCU 调用 llcc68_set_rx() 时通过 timeout 参数设置了“有限超
        //           时接收”，在超时时间内芯片没有收到任何有效帧（既没有完整的
        //           RX_DONE 也没有 Header/CRC 错误等），则超时后置位 Timeout
        //           并自动回到 STDBY 模式。
        // 典型应用：
        //   1) 用于需要“低功耗轮询”业务的设备：只在 timeout 周期内短暂打开
        //      接收，其余时间休眠；
        //   2) 与 PREAMBLE_DETECTED 等组合判断“是否有干扰但解不出帧”的环境。
        // 注意：timeout = 0 表示持续接收，永不超时（不会触发该 IRQ）。
        // =======================================================================
    case LLCC68_IRQ_TIMEOUT:
        {
            llcc68_interface_debug_print("llcc68: irq timeout.\n");

            break;
        }
        // =======================================================================
        // 未知 / 未识别的 IRQ 类型
        // 触发条件：传入的 type 字段不匹配任何已定义的 LLCC68_IRQ_* 枚举，可能
        //           原因：驱动/固件版本不一致、I/O 拼装异常、误触发。
        // 处理建议：开发阶段应打印原始 type 值便于排查；正式产品中可选择忽略
        //           并清除对应 IRQ 标志。
        // =======================================================================
    default:
        {
            llcc68_interface_debug_print("llcc68: unknown code.\n");

            break;
        }
    }
}
