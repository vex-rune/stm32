#ifndef __WIFI_H
#define __WIFI_H

#ifdef __cplusplus
extern "C" {



#endif

#include <stdint.h>

typedef enum
{
    STA,
    AP,
    AP_STA
} Wifi_Mode;

/**
 * @brief  WiFi 完整初始化流程
 *
 * 执行步骤：
 *   1. 查询 WiFi 初始化状态 (AT+CWINIT?)
 *   2. 设置 WiFi 模式为 station (AT+CWMODE=1)
 *   3. 连接路由器 (AT+CWJAP="SSID","PWD")
 *   4. 查询 IP/网关/子网掩码 (AT+CIPSTA?)
 *   5. 解析 AT+CIPSTA 响应，打印 IP/Gateway/Netmask
 *
 * @note 必须在 esp32_init() 之后调用。
 */
void Wifi_Init(Wifi_Mode mode);

/**
 * @brief  获取解析后的 IP 地址字符串
 * @retval 指向内部缓冲区的以 '\0' 结尾的字符串（调用前需先执行 Wifi_Init）
 */
const char* Wifi_GetIP(void);

/**
 * @brief  获取解析后的网关地址字符串
 * @retval 指向内部缓冲区的以 '\0' 结尾的字符串（调用前需先执行 Wifi_Init）
 */
const char* Wifi_GetGateway(void);

/**
 * @brief  获取解析后的子网掩码字符串
 * @retval 指向内部缓冲区的以 '\0' 结尾的字符串（调用前需先执行 Wifi_Init）
 */
const char* Wifi_GetNetmask(void);

/**
 * 启动服务器
 */
void Wifi_StartServer(void);

/**
 * 发送数据 (包含连接ID)
 */
void Wifi_SendData(uint8_t id, uint8_t* data, uint16_t len);

/**
 * 读取数据 (包含了连接id, 对端的端口号, 对端的ip)
 */
uint16_t Wifi_ReadData(uint8_t* buf, uint16_t len,
                       uint8_t* id, uint16_t* port, uint8_t* ip
);


#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H */
