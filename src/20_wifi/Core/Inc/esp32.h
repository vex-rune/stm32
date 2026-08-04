#ifndef __ESP32_H
#define __ESP32_H

#include "main.h"
#include "usart.h"
#include <stdint.h>

// 串口句柄，使用USART2
#define ESP_HUART       &huart2

// 响应状态枚举
typedef enum
{
    ESP_OK = 0, // 正常收到帧（IDLE触发）
    ESP_TIMEOUT = 1, // 接收超时
    ESP_ERROR = 2, // 参数错误 / 硬件错误
} ESP_StatusTypeDef;

/**
 * @brief ESP32模块完整初始化（包含握手、关闭回显、获取并打印设备信息）
 * @retval ESP_StatusTypeDef
 */
ESP_StatusTypeDef esp32_init(void);

/**
 * @brief 发送AT命令，自动计算字符串长度
 * @param cmd 以'\0'结尾的命令字符串
 * @retval 无
 */
void esp32_send_cmd(const char* cmd);

/**
 * @brief 阻塞接收串口空闲帧（HAL_UARTEx_ReceiveToIdle）
 * @param buf 接收缓冲区
 * @param len [out]实际收到字节
 * @param size 缓冲区最大容量
 * @param timeout_ms 接收超时时间
 * @retval ESP_StatusTypeDef
 */
ESP_StatusTypeDef esp32_read_resp(uint8_t buf[], uint16_t* len, uint16_t size, uint32_t timeout_ms);

/**
 * @brief 获取ESP32设备信息（版本号）
 * @param info_buf 信息缓冲区
 * @param buf_size 缓冲区大小
 * @retval ESP_OK 成功；ESP_TIMEOUT/ESP_ERROR 失败
 */
ESP_StatusTypeDef esp32_get_info(char* info_buf, uint16_t buf_size);

/**
 * @brief 关闭AT回显模式
 * @retval ESP_OK 成功；ESP_TIMEOUT/ESP_ERROR 失败
 */
ESP_StatusTypeDef esp32_disable_echo(void);

/**
 * @brief 复位ESP32模块
 * @retval ESP_OK 成功；ESP_TIMEOUT/ESP_ERROR 失败
 */
ESP_StatusTypeDef esp32_reset(void);

#endif
