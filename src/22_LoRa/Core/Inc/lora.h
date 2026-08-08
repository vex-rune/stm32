#ifndef  __LORA_H
#define  __LORA_H

#include <stdint.h>

/*======================================================================*/
/*                  顶层 API（main 直接调用）                           */
/*======================================================================*/

/* LoRa 初始化（SPI / 寄存器全配置） */
uint8_t lora_init(void);

/* 发送一包 LoRa 数据 */
void lora_send(uint8_t* buf, uint16_t len);

/* 进入连续接收模式（拉 RXEN、切 LoRa RX） */
uint8_t lora_receive_mode(void);

/*======================================================================*/
/*                  分步式接收 API（main 按顺序调用）                   */
/*======================================================================*/

/* 查询：芯片是否收到完整一包？ 1=收到 / 0=未收到 */
uint8_t lora_check_rx(void);

/* 从 FIFO 读出一包数据；返回 0=成功 / 1=失败 */
uint8_t lora_read_packet(uint8_t* buf, uint16_t* len);

/* 读取最近一包的 RSSI/SNR（dBm / dB） */
void lora_get_rssi_snr(float* rssi, float* snr);

/* IRQ 收尾：清标志 + 重新进入 RX */
void lora_resume_rx(void);

#endif