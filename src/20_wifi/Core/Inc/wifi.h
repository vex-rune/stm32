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
 * @brief  WiFi 完整初始化入口
 *
 * @par 流程:
 *   1. esp32_init()                ESP32 模块初始化（AT 握手 / 关闭回显 / 获取设备信息）
 *   2. 根据 mode 分发到对应的模式配置函数：
 *      - STA     -> STA_Mode()      作为 station 连接路由器
 *      - AP      -> AP_Mode()       作为 softAP 发射热点
 *      - AP_STA  -> AP_STA_Mode()   station + softAP 双模共存
 *   3. 初始化完成后，可通过 Wifi_GetIP() / Wifi_GetGateway() / Wifi_GetNetmask()
 *      获取解析后的网络信息
 *
 * @param  mode : 选择 WiFi 工作模式（STA / AP / AP_STA）
 *
 * @note
 *   - 整个流程耗时较长（数秒 ~ 十数秒），内部包含 WiFi 连接 / 热点启动等待。
 *   - 本函数默认在 main 初始化阶段调用一次，不支持运行时热切换工作模式；
 *     如需切换，请复位 ESP32 后重新调用本函数。
 *   - 必须在 HAL / USART2 初始化完成之后调用。
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
 * @brief  启动 ESP32-C3 TCP 服务器
 *
 * @par 流程:
 *   1. AT+CIPMUX=1       开启多连接（TCP 服务器的前置条件）
 *   2. AT+CIPSERVER=1,333
 *                        启动 TCP 服务器，默认端口 333
 *   3. 等待服务器启动完成
 *
 * @note
 *   - 调用前需确保已调用 Wifi_Init() 完成 ESP32 初始化与模式设置。
 *   - 必须在多连接模式下使用：AP / AP_STA 模式中已默认开启多连接；
 *     STA 模式下如需作为服务器，请先手动发送 AT+CIPMUX=1。
 *   - 服务器启动后，可通过 Wifi_SendData() / Wifi_ReadData() 收发数据。
 */
void Wifi_StartServer(void);

/**
 * @brief  向指定 TCP 连接发送数据
 *
 * @par 流程:
 *   1. AT+CIPSEND=<id>,<len>    告诉 ESP32 准备发送 <len> 字节到连接 <id>
 *   2. 等待 ESP32 返回 ">"       提示符
 *   3. UART 发送 <len> 字节原始数据
 *   4. 等待 "SEND OK" 响应
 *
 * @param  id  : 连接 ID（多连接模式下由 ESP32 分配，范围 0~4）
 * @param  data : 待发送数据缓冲区
 * @param  len  : 待发送字节数
 *
 * @note
 *   - 必须在多连接模式（AT+CIPMUX=1）下调用。
 *   - <id> 必须对应一个已建立的 TCP 连接，否则 ESP32 返回 ERROR。
 *   - 调用前需确保 Wifi_Init() 与 Wifi_StartServer() 已执行完成。
 */
void Wifi_SendData(uint8_t id, uint8_t* data, uint16_t len);

/**
 * @brief  读取 ESP32 主动上报的接收数据
 *
 * @par 帧格式 (AT+CIPDINFO=1):
 *   [\r\n \t]* +IPD,<id>,<len>,"<remote_ip>",<remote_port>:<data>
 *
 * @par 说明:
 *   某些 ESP32 固件版本会在 +IPD 帧之前输出若干 \r\n / 空格 / Tab 作为分隔，
 *   本函数会在帧头检测和 sscanf 解析时自动跳过这些前导空白。
 *
 * @param[out] buf          : 数据接收缓冲区（必须 >= 256 字节）
 * @param[in]  buf_capacity : buf 数组的最大容量（字节数）
 * @param[out] id           : 接收到的数据来自哪个连接（0~4）
 * @param[out] port         : 对端的端口号
 * @param[out] ip           : 对端的 IP 地址（字符串，必须 >= 32 字节）
 *
 * @retval >0 : 实际解析出的 data 字节数（已截断到不超过 buf_capacity）
 * @retval  0 : 当前无数据 / 帧不是 +IPD / 解析失败 / 参数错误
 *
 * @note
 *   - 必须在多连接模式（AT+CIPMUX=1）下调用。
 *   - 必须在 Wifi_StartServer() 之前发过 AT+CIPDINFO=1，否则帧不含 IP/Port。
 *   - 应在主循环中周期性轮询调用，以避免 ESP32 内部接收缓冲溢出。
 *   - buf 必须 >= 256 字节、ip 必须 >= 32 字节：函数内部采用固定正则
 *     解析 +IPD 帧，data 字段硬编码限制为 255 字符，IP 字段硬编码限制
 *     为 31 字符，传入缓冲区小于此限制会越界。
 *   - buf_capacity 仅用于参数检查与返回值截断，无法限制 sscanf 实际写入，
 *     真正能保护 buf 不越界的是固定正则在 sscanf 层面的 255 字符硬限制。
 */
uint16_t Wifi_ReadData(uint8_t* buf, uint16_t buf_capacity,
                       uint8_t* id, uint16_t* port, uint8_t* ip
);


#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H */
