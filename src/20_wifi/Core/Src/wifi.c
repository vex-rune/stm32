/**
 * @file    wifi.c
 * @brief   WiFi 应用层模块 - 封装 ESP32 AT 指令的 WiFi 初始化与信息解析
 *
 * @par 依赖:
 *  - esp32.h  : 提供 esp32_send_cmd / esp32_read_resp
 *  - HAL 库   : 提供 HAL_Delay
 */

#include "wifi.h"
#include "esp32.h"

#include <stdio.h>
#include <string.h>

/*============================================================================
 * 宏定义
 *============================================================================*/

/** 模块内部接收缓冲区大小 */
#define WIFI_RX_BUF_SIZE  256
#define WIFI_RX_MAX_BUF_SIZE  1024

/** WiFi 连接信息（SSID / 密码），实际项目中应放到配置区 */
#define WIFI_SSID         "NYF_72"
#define WIFI_PASSWORD     "woaiwojia@2019"

/*============================================================================
 * 私有变量
 *============================================================================*/

/** WiFi 模块内部接收缓冲 */
static uint8_t s_rx_buf[WIFI_RX_BUF_SIZE];
static uint8_t d_rx_buf[WIFI_RX_MAX_BUF_SIZE];
static uint16_t s_rx_len = 0;

/** 解析得到的网络信息 */
static char s_ip_buf[32] = {0};
static char s_gateway_buf[32] = {0};
static char s_netmask_buf[32] = {0};

Wifi_Mode s_mode = 0;

/*============================================================================
 * 私有函数
 *============================================================================*/

/**
 * @brief  从 AT 响应中解析指定前缀后双引号内的内容
 * @param  buf    : 待解析的源字符串
 * @param  prefix : 目标前缀字符串 (如 "CIPSTA:ip:\"")
 * @param  out    : 输出缓冲区
 * @param  out_sz : 输出缓冲区大小
 * @retval 1 解析成功, 0 解析失败
 */
static uint8_t ParseQuote(const char* buf, const char* prefix, char* out, uint16_t out_sz)
{
    if (buf == NULL || prefix == NULL || out == NULL || out_sz == 0)
        return 0;

    const char* s = strstr(buf, prefix);
    if (!s) return 0;

    s += strlen(prefix);

    const char* e = strstr(s, "\"");
    if (!e) return 0;

    uint16_t len = (uint16_t)(e - s);
    if (len >= out_sz)
        len = out_sz - 1;

    memcpy(out, s, len);
    out[len] = '\0';
    return 1;
}

/**
 * @brief  发送 AT 命令并接收响应（带小延时）
 */
static void wifi_send_and_recv(const char* cmd, uint32_t timeout_ms, uint32_t post_delay_ms)
{
    esp32_send_cmd(cmd);
    memset(s_rx_buf, 0, sizeof(s_rx_buf));
    esp32_read_resp(s_rx_buf, &s_rx_len, sizeof(s_rx_buf), timeout_ms);
    if (post_delay_ms > 0)
        HAL_Delay(post_delay_ms);
}

/**
 * @brief  配置 ESP32-C3 为 station 模式 (STA)
 *
 * @par 流程:
 *   1. AT+CWINIT?            查询 WiFi 初始化状态
 *   2. AT+CWMODE=1           设置 WiFi 模式为 station
 *   3. AT+CWJAP="SSID","PWD" 连接指定路由器（使用 WIFI_SSID / WIFI_PASSWORD）
 *   4. AT+CIPSTA?            查询 station 的 IP / 网关 / 子网掩码
 *   5. 解析 AT+CIPSTA 响应，结果保存到 s_ip_buf / s_gateway_buf / s_netmask_buf
 *   6. 打印 station 网络信息
 *
 * @note
 *   - 连接 WiFi 后需等待 3s 左右让 DHCP 分配完成。
 *   - 解析后的 IP / Gateway / Netmask 可通过 Wifi_GetIP() / Wifi_GetGateway() / Wifi_GetNetmask() 获取。
 *   - 必须在 esp32_init() 之后调用。
 */
void STA_Mode()
{
    /* 1. 查询 WiFi 初始化状态 */
    wifi_send_and_recv("AT+CWINIT?\r\n", 1000, 300);

    /* 2. 设置 WiFi 模式为 station */
    wifi_send_and_recv("AT+CWMODE=1\r\n", 1000, 300);

    /* 3. 连接路由器 */
    char cwJAP[96];
    snprintf(cwJAP, sizeof(cwJAP), "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASSWORD);
    wifi_send_and_recv(cwJAP, 1000, 3000);

    /* 4. 查询 IP / 网关 / 子网掩码 */
    esp32_send_cmd("AT+CIPSTA?\r\n");
    memset(s_rx_buf, 0, sizeof(s_rx_buf));
    esp32_read_resp(s_rx_buf, &s_rx_len, sizeof(s_rx_buf), 1000);

    /* 5. 解析并保存 */
    ParseQuote((char*)s_rx_buf, "CIPSTA:ip:\"", s_ip_buf, sizeof(s_ip_buf));
    ParseQuote((char*)s_rx_buf, "CIPSTA:gateway:\"", s_gateway_buf, sizeof(s_gateway_buf));
    ParseQuote((char*)s_rx_buf, "CIPSTA:netmask:\"", s_netmask_buf, sizeof(s_netmask_buf));

    /* 6. 打印 */
    printf("wifi -> mode: STA IP: %s\r\n", s_ip_buf);
    printf("wifi -> mode: STA Gateway: %s\r\n", s_gateway_buf);
    printf("wifi -> mode: STA Netmask: %s\r\n", s_netmask_buf);
}


/**
 * @brief  配置 ESP32-C3 为 softAP 模式 (AP)
 *
 * @par 流程:
 *   1. AT+CWMODE=2               设置 WiFi 模式为 softAP
 *   2. AT+CIPMUX=1               开启多连接（建立 TCP 服务器的前置条件）
 *   3. AT+CWSAP=...              配置 softAP 参数（SSID / 密码 / 信道 / 加密方式）
 *   4. 延时 3s                    等待 softAP 启动完成
 *   5. AT+CIPAP?                 查询 softAP 的 IP / 网关 / 子网掩码
 *   6. 解析 AT+CIPAP 响应，结果保存到 s_ip_buf / s_gateway_buf / s_netmask_buf
 *   7. AT+CIPSERVER=1            启动 TCP 服务器（默认端口 333）
 *   8. 打印 softAP 网络信息
 *
 * @par AT+CWSAP 参数说明:
 *  | 参数           | 本实现取值 | 说明                          |
 *  |----------------|------------|-------------------------------|
 *  | <ssid>         | ESP32_softAP | 接入点名称                   |
 *  | <pwd>          | 1234567890   | 密码（8~63 字节 ASCII）     |
 *  | <chl>          | 5            | 信道号                       |
 *  | <ecn>          | 3            | 加密方式：3 = WPA2_PSK       |
 *  | [<max conn>]  | 缺省          | 允许连入的最大 station 数     |
 *  | [<ssid hidden>]| 缺省          | 0 = 广播 SSID（默认）         |
 *
 * @note
 *   - 必须在 esp32_init() 之后调用。
 *   - 解析后的 IP / Gateway / Netmask 可通过 Wifi_GetIP() / Wifi_GetGateway() / Wifi_GetNetmask() 获取。
 */
void AP_Mode()
{
    /* 1. 设置 Wi-Fi 模式为 softAP */
    wifi_send_and_recv("AT+CWMODE=2\r\n", 1000, 300);

    /* 2. 使能多连接（TCP 服务器前置条件） */
    wifi_send_and_recv("AT+CIPMUX=1\r\n", 1000, 300);

    /* 3. 设置 softAP 参数：SSID=ESP32_softAP, PWD=1234567890, 信道=5, 加密=WPA2_PSK */
    char cwJAP[96];
    snprintf(cwJAP, sizeof(cwJAP), "AT+CWSAP=\"%s\",\"%s\",5,3\r\n", "ESP32_softAP", "1234567890");
    wifi_send_and_recv(cwJAP, 1000, 3000);

    /* 4. 等待 softAP 启动完成 */
    HAL_Delay(3000);

    /* 5. 查询 softAP 的 IP / 网关 / 子网掩码 */
    esp32_send_cmd("AT+CIPAP?\r\n");
    esp32_read_resp(s_rx_buf, &s_rx_len, sizeof(s_rx_buf), 1000);

    /* 6. 解析并保存 */
    ParseQuote((char*)s_rx_buf, "CIPAP:ip:\"", s_ip_buf, sizeof(s_ip_buf));
    ParseQuote((char*)s_rx_buf, "CIPAP:gateway:\"", s_gateway_buf, sizeof(s_gateway_buf));
    ParseQuote((char*)s_rx_buf, "CIPAP:netmask:\"", s_netmask_buf, sizeof(s_netmask_buf));

    /* 7. 打印 */
    printf("wifi -> mode: AP IP: %s\r\n", s_ip_buf);
    printf("wifi -> mode: AP Gateway: %s\r\n", s_gateway_buf);
    printf("wifi -> mode: AP Netmask: %s\r\n", s_netmask_buf);

    /* 8. 建立 TCP 服务器，默认端口为 333 */
    wifi_send_and_recv("AT+CIPSERVER=1\r\n", 1000, 300);
}


/**
 * @brief  配置 ESP32-C3 为 station + softAP 共存模式 (AP_STA)
 *
 * @par 流程:
 *   1. AT+CWMODE=3            设置为 station + softAP 模式
 *   2. AT+CWJAP=...           作为 station 连接路由器（使用 WIFI_SSID / WIFI_PASSWORD）
 *   3. AT+CWSAP=...           配置 softAP 参数（SSID / 密码 / 信道 / 加密 / 最大连接数 / SSID 广播）
 *   4. 延时 3s                 等待 softAP 启动完成
 *   5. AT+CIPMUX=1            开启多连接（建立 TCP 服务器的前置条件）
 *   6. AT+CIPSTA?             查询 station 的 IP / 网关 / 子网掩码
 *   7. AT+CIPAP?              查询 softAP 的 IP / 网关 / 子网掩码
 *   8. 解析并打印双网 IP 信息
 *
 * @par AT+CWSAP 参数说明:
 *  | 参数           | 本实现取值 | 说明                          |
 *  |----------------|------------|-------------------------------|
 *  | <ssid>         | ESP32_softAP | 接入点名称                   |
 *  | <pwd>          | 1234567890   | 密码（8~63 字节 ASCII）     |
 *  | <chl>          | 5            | 信道号                       |
 *  | <ecn>          | 3            | 加密方式：3 = WPA2_PSK       |
 *  | <max conn>     | 4            | 允许连入的最大 station 数     |
 *  | <ssid hidden>  | 0            | 0 = 广播 SSID                 |
 *
 * @note
 *   - station 端 IP 解析后保存到局部变量，不进入全局缓冲；softAP 端 IP 解析后保存到
 *     s_ip_buf / s_gateway_buf / s_netmask_buf。
 *   - 后续通过 Wifi_GetIP() 等接口拿到的是 softAP 的 IP。
 *   - 如需同时保留两套 IP 信息，可扩展为双缓冲结构。
 *   - 必须在 esp32_init() 之后调用。
 */
void AP_STA_Mode()
{
    /* 1. 设置 Wi-Fi 模式为 station + softAP (AT+CWMODE=3) */
    wifi_send_and_recv("AT+CWMODE=3\r\n", 1000, 300);

    /* 2. 作为 station 连接路由器 */
    char cwJAP[96];
    snprintf(cwJAP, sizeof(cwJAP), "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASSWORD);
    wifi_send_and_recv(cwJAP, 1000, 3000);

    /* 3. 设置 softAP 参数
     *    5  : 信道号
     *    3  : WPA2_PSK 加密
     *    4  : 最大允许 4 个 station 接入
     *    0  : 广播 SSID
     */
    char cwsap[96];
    snprintf(cwsap, sizeof(cwsap), "AT+CWSAP=\"%s\",\"%s\",5,3,4,0\r\n",
             "ESP32_softAP", "1234567890");
    wifi_send_and_recv(cwsap, 1000, 3000);

    /* 4. 等待 softAP 启动完成 */
    HAL_Delay(3000);

    /* 5. 开启多连接（建立 TCP 服务器前置条件） */
    wifi_send_and_recv("AT+CIPMUX=1\r\n", 1000, 300);

    /* 6. 查询 station 的 IP / 网关 / 子网掩码 */
    esp32_send_cmd("AT+CIPSTA?\r\n");
    memset(s_rx_buf, 0, sizeof(s_rx_buf));
    esp32_read_resp(s_rx_buf, &s_rx_len, sizeof(s_rx_buf), 1000);

    char sta_ip[32] = {0};
    char sta_gateway[32] = {0};
    char sta_netmask[32] = {0};
    ParseQuote((char*)s_rx_buf, "CIPSTA:ip:\"", sta_ip, sizeof(sta_ip));
    ParseQuote((char*)s_rx_buf, "CIPSTA:gateway:\"", sta_gateway, sizeof(sta_gateway));
    ParseQuote((char*)s_rx_buf, "CIPSTA:netmask:\"", sta_netmask, sizeof(sta_netmask));

    printf("wifi -> mode: AP_STA  STA IP:      %s\r\n", sta_ip);
    printf("wifi -> mode: AP_STA  STA Gateway: %s\r\n", sta_gateway);
    printf("wifi -> mode: AP_STA  STA Netmask: %s\r\n", sta_netmask);

    /* 7. 查询 softAP 的 IP / 网关 / 子网掩码 */
    esp32_send_cmd("AT+CIPAP?\r\n");
    memset(s_rx_buf, 0, sizeof(s_rx_buf));
    esp32_read_resp(s_rx_buf, &s_rx_len, sizeof(s_rx_buf), 1000);

    ParseQuote((char*)s_rx_buf, "CIPAP:ip:\"", s_ip_buf, sizeof(s_ip_buf));
    ParseQuote((char*)s_rx_buf, "CIPAP:gateway:\"", s_gateway_buf, sizeof(s_gateway_buf));
    ParseQuote((char*)s_rx_buf, "CIPAP:netmask:\"", s_netmask_buf, sizeof(s_netmask_buf));

    printf("wifi -> mode: AP_STA  AP  IP:      %s\r\n", s_ip_buf);
    printf("wifi -> mode: AP_STA  AP  Gateway: %s\r\n", s_gateway_buf);
    printf("wifi -> mode: AP_STA  AP  Netmask: %s\r\n", s_netmask_buf);
}


/*============================================================================
 * 公有函数
 *============================================================================*/

/**
 * @brief  WiFi 完整初始化入口
 *
 * @par 流程:
 *   1. esp32_init()                ESP32 模块初始化（AT 握手 / 关闭回显 / 获取设备信息）
 *   2. s_mode = mode                保存当前工作模式，供后续接口查询
 *   3. 根据 mode 调用对应的模式配置函数：
 *      - STA     -> STA_Mode()
 *      - AP      -> AP_Mode()
 *      - AP_STA  -> AP_STA_Mode()
 *   4. 模式配置完成后，可通过 Wifi_GetIP() / Wifi_GetGateway() / Wifi_GetNetmask()
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
void Wifi_Init(Wifi_Mode mode)
{
    printf("=============== WiFi 初始化 ===============\r\n");

    /* 1. ESP32 模块初始化（AT 握手 / 关闭回显 / 获取设备信息） */
    esp32_init();

    /* 2. 保存当前工作模式 */
    s_mode = mode;

    /* 3. 根据 mode 分发到对应的模式配置函数 */
    switch (mode)
    {
    case STA:
        STA_Mode();
        break;
    case AP:
        AP_Mode();
        break;
    case AP_STA:
        AP_STA_Mode();
        break;
    }

    /* 4. 初始化完成，可通过 Get 接口读取网络信息 */
    printf("=============== WiFi 初始化完成 ===============\r\n");
}

const char* Wifi_GetIP(void)
{
    return s_ip_buf;
}

const char* Wifi_GetGateway(void)
{
    return s_gateway_buf;
}

const char* Wifi_GetNetmask(void)
{
    return s_netmask_buf;
}

/**
 * @brief  启动 ESP32-C3 TCP 服务器
 *
 * @par 流程:
 *   1. AT+CIPMUX=1       开启多连接（多连接是 TCP 服务器的前置条件）
 *   2. AT+CIPSERVER=1,<port>
 *                        启动 TCP 服务器，<port> 为服务器端口
 *   3. 设置IPD消息详情
*                      AT+CIPDINFO=1 时, 返回数据会包含 IPD 消息详情 格式如下: "+IPD,<length>,<"remote_ip">"
 *
 * @note
 *   - 默认端口为 333（与 AP_Mode() 中使用的端口保持一致）。
 *   - 调用前需确保已调用 Wifi_Init() 完成 ESP32 初始化与模式设置。
 *   - 必须在多连接模式下使用：AP / AP_STA 模式中已默认开启多连接；
 *     STA 模式下如需作为服务器，请先手动发送 AT+CIPMUX=1。
 *   - 服务器启动后，可通过 Wifi_SendData() / Wifi_ReadData() 收发数据。
 */
void Wifi_StartServer(void)
{
    /* 1. 开启多连接（TCP 服务器前置条件） */
    wifi_send_and_recv("AT+CIPMUX=1\r\n", 1000, 300);

    /* 2. 启动 TCP 服务器，默认端口 333, 这里设置为 8080 */
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+CIPSERVER=1,%d\r\n", 8080);
    wifi_send_and_recv(cmd, 1000, 300);

    /* 3. 设置IPD消息详情 */
    wifi_send_and_recv("AT+CIPDINFO=1\r\n", 1000, 300);
}


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
void Wifi_SendData(uint8_t id, uint8_t* data, uint16_t len)
{
    // 1. 告诉 ESP32 准备发送 <len> 字节到连接 <id>
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d,%d\r\n", id, len);
    wifi_send_and_recv(cmd, 1000, 300);

    // 2. 进入数据发送模式
    esp32_send_cmd((char*)data);

    // 3. 结束
    esp32_send_cmd("+++");
}


/**
 * @brief  跳过字符串开头的空白字符
 * @param  str : 输入字符串
 * @retval 跳过空白后的字符串指针（如果原串为空或仅含空白，返回末尾 '\0' 位置）
 */
static const char* SkipWhitespace(const char* str)
{
    if (str == NULL) return NULL;
    while (*str == '\r' || *str == '\n' || *str == ' ' || *str == '\t')
    {
        str++;
    }
    return str;
}

/**
 * @brief  判断字符串（跳过前导空白后）是否以 "+IPD," 开头
 *
 * @note
 *   ESP32 在多连接模式下上报数据帧时，某些固件版本会在 +IPD 帧之前
 *   输出多个 CR/LF（如 "\\r\\n\\r\\n\\r\\n+IPD,..."）作为分隔，
 *   本函数会自动跳过这些前导空白后再进行帧头判断。
 *
 * @param  str : 待检测字符串
 * @retval 1 是 +IPD 帧, 0 不是
 */
static uint8_t IsIpdStart(const char* str)
{
    printf("[IsIpdStart: %s]\r\n", str);
    if (str == NULL) return 0;
    return (strncmp(SkipWhitespace(str), "+IPD,", 5) == 0) ? 1 : 0;
}


/**
 * @brief  +IPD 帧的 sscanf 解析模板（固定正则）
 *
 * @par 格式:
 *   +IPD,%d,%d,"%31[0-9.]",%d:%255[^\r\n]
 *   含义:  +IPD,<id>,<len>,"<remote_ip>",<remote_port>:<data>
 *
 * @par 限制:
 *   - %31[0-9.]    : IP 最多 31 字符（调用者 ip 缓冲区 >= 32 字节）
 *   - %255[^\r\n]  : data 最多 255 字符（调用者 buf 容量必须 >= 256 字节）
 */
static const char s_ipd_pattern[] = "+IPD,%d,%d,\"%31[0-9.]\",%d:%255[^\r\n]";


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
 * @param[out] buf          : 数据接收缓冲区（由调用者分配）
 * @param[in]  buf_capacity : buf 数组的最大容量（字节数）
 * @param[out] id           : 接收到的数据来自哪个连接（0~4）
 * @param[out] port         : 对端的端口号
 * @param[out] ip           : 对端的 IP 地址（字符串，调用者需保证 >= 32 字节）
 *
 * @retval >0 : 实际解析出的 data 字节数（已截断到不超过 buf_capacity）
 * @retval  0 : 当前无数据 / 帧不是 +IPD / 解析失败 / 参数错误
 *
 * @note
 *   - 必须在多连接模式（AT+CIPMUX=1）下调用。
 *   - 必须在 Wifi_StartServer() 之前发过 AT+CIPDINFO=1，否则帧不含 IP/Port。
 *   - 应在主循环中周期性轮询调用，以避免 ESP32 内部接收缓冲溢出。
 *   - buf 必须 >= 256 字节：本函数采用固定正则 s_ipd_pattern，data 字段
 *     硬编码限制为 255 字符 + '\0'，传入 buf 小于 256 字节会越界。
 *   - ip 必须 >= 32 字节：IP 字段硬编码限制为 31 字符 + '\0'。
 *   - buf_capacity 仅用于参数检查与返回值截断，无法限制 sscanf 的实际写入
 *     （sscanf 受限于固定正则的 %255 硬限制），因此 buf 容量 >= 256 是硬性约束。
 *   - 底层接收缓冲 d_rx_buf 为 1024 字节（WIFI_RX_MAX_BUF_SIZE），
 *     +IPD 帧中的 <data> 长度不得超过约 1000 字节（剩下的作为帧头），
 *     否则会在 esp32_read_resp() 阶段被截断。
 */
uint16_t Wifi_ReadData(uint8_t* buf, uint16_t buf_capacity,
                       uint8_t* id, uint16_t* port, uint8_t* ip
)
{
    /* ====================================================================
     * 0. 参数合法性检查
     * ====================================================================
     * 任何指针为 NULL 或 buf_capacity=0 都无法承载任何数据，直接返回 0。
     * 避免后续 sscanf / strncpy 访问空指针导致 HardFault。
     */
    if (buf == NULL || buf_capacity == 0 || id == NULL || port == NULL || ip == NULL)
    {
        return 0;
    }

    /* ====================================================================
     * 局部变量定义
     * ====================================================================
     * - rx_len   : esp32_read_resp 实际接收到的字节数（输出参数）
     * - id_tmp   : sscanf 要求 int* 接收 %d，所以用 int 中转，
     *              最后再截断到 uint8_t（ESP32 连接 ID 实际范围 0~4）
     * - data_len : 帧中 <len> 字段的声明值
     * - port_tmp : sscanf 要求 int* 接收 %d，用 int 中转后再转 uint16_t
     * - ip_tmp   : sscanf 要求 char*，固定 32 字节
     *              （包含末尾 '\0'，对应 s_ipd_pattern 中的 %31[0-9.]）
     * - ipd_pos  : 指向 +IPD 帧在缓冲区中的位置（用于处理多帧混合的情况）
     */
    uint16_t rx_len = 0;
    int id_tmp = 0;
    int data_len = 0;
    int port_tmp = 0;
    char ip_tmp[32] = {0};
    const char* ipd_pos = NULL;

    /* ====================================================================
     * 1. 从串口接收一帧 ESP32 上报的数据
     * ====================================================================
     * d_rx_buf 是模块级静态缓冲区（1024 字节，WIFI_RX_MAX_BUF_SIZE），
     * 调用前先清零，避免上一次残留的数据干扰本次解析。
     * esp32_read_resp 是阻塞接收，1000ms 超时；超时 / 硬件错误都返回 0。
     */
    memset(d_rx_buf, 0, sizeof(d_rx_buf)); // 清空 d_rx_buf，防上次残留
    if (esp32_read_resp(d_rx_buf, &rx_len, sizeof(d_rx_buf), 1000) != ESP_OK) // 阻塞 1000ms 接收一帧
    {
        return 0;
    }

    // printf( "[ WIFI ___: %s ]\r\n", d_rx_buf);

    /* ====================================================================
     * 2. 在接收到的数据中搜索 +IPD 帧
     * ====================================================================
     * ESP32 可能会在一个缓冲区中返回多个响应（如 OK、+CIPSTA 等），
     * 需要在缓冲区中搜索 "+IPD," 字符串来定位数据帧的开始位置。
     * 如果找不到 +IPD 帧，说明当前没有数据需要处理。
     */
    ipd_pos = strstr((const char*)d_rx_buf, "+IPD,");
    if (ipd_pos == NULL)
    {
        return 0;
    }

    /* ====================================================================
     * 3. 解析 +IPD 帧
     * ====================================================================
     * 帧格式: +IPD,<id>,<len>,"<remote_ip>",<remote_port>:<data>
     * sscanf 会按 s_ipd_pattern 模板依次提取 5 个字段：
     *   - %d          -> &id_tmp      连接 ID
     *   - %d          -> &data_len    data 字段声明的字节数
     *   - %31[0-9.]   -> ip_tmp       对端 IP（最多 31 字符）
     *   - %d          -> &port_tmp    对端端口
     *   - %255[^\r\n] -> (char*)buf   实际负载数据（最多 255 字符）
     *
     * 使用 ipd_pos 作为解析起点，确保即使在混合帧中也能正确解析。
     * 成功匹配必须恰好为 5 个字段，否则视为帧格式错误。
     */
    if (sscanf(ipd_pos, s_ipd_pattern,
               &id_tmp, &data_len, ip_tmp, &port_tmp, (char*)buf) != 5)
    {
        return 0;
    }

    /* ====================================================================
     * 4. 填充输出参数（int -> uint8_t / uint16_t 类型截断）
     * ====================================================================
     * - ESP32 连接 ID 实际范围 0~4，截断到 uint8_t 安全
     * - 端口号实际范围 0~65535，截断到 uint16_t 安全
     * - IP 字符串通过 strncpy 安全拷贝（即使 ip_tmp 没填满也会补 '\0'）
     * - 最后再补一个 '\0' 强制终止，防御 sscanf 边界异常
     */
    *id = (uint8_t)id_tmp; // int -> uint8_t，截断高位
    *port = (uint16_t)port_tmp; // int -> uint16_t，截断高位
    strncpy((char*)ip, ip_tmp, sizeof(ip_tmp) - 1); // 安全拷贝 IP 字符串（最多 31 字符）
    ((char*)ip)[sizeof(ip_tmp) - 1] = '\0'; // 强制补 '\0'，保证字符串终止

    /* ====================================================================
     * 5. 限制返回长度在 [0, buf_capacity] 范围内
     * ====================================================================
     * - sscanf 异常时 data_len 可能为负数（需防御）
     * - sscanf 写入 buf 的实际字节数可能超过调用者传入的 buf_capacity 上限
     * - 双重边界保护，确保返回值永远在安全区间内
     */
    if (data_len < 0) data_len = 0; // 防御负数（sscanf 解析异常时可能为负）
    if (data_len > (int)buf_capacity) data_len = (int)buf_capacity; // 防御超出调用者 buf 容量

    /* 返回实际接收的 data 字节数（调用者据此判断 buf 是否被填满） */
    return (uint16_t)data_len;
}
