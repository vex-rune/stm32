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

/** WiFi 连接信息（SSID / 密码），实际项目中应放到配置区 */
#define WIFI_SSID         "NYF_72"
#define WIFI_PASSWORD     "woaiwojia@2019"

/*============================================================================
 * 私有变量
 *============================================================================*/

/** WiFi 模块内部接收缓冲 */
static uint8_t s_rx_buf[WIFI_RX_BUF_SIZE];
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
* 设置 ESP32-C3 SoftAP 的配置参数
命令：
AT+CWSAP=<ssid>,<pwd>,<chl>,<ecn>[,<max conn>][,<ssid hidden>]
响应：
OK
参数
• <ssid>：字符串参数，接入点名称
• <pwd>：字符串参数，密码，范围：8 ~ 63 字节 ASCII
• <channel>：信道号
• <ecn>：加密方式，不支持 WEP
– 0: OPEN
– 2: WPA_PSK
– 3: WPA2_PSK
– 4: WPA_WPA2_PSK
• [<max conn>]：允许连入 ESP32-C3 SoftAP 的最多 station 数目，取值范围：参考 max_connection 描
述。
• [<ssid hidden>]：
– 0: 广播 SSID（默认）
– 1: 不广播 SSID
 */
void AP_Mode()
{
    // 设置 Wi-Fi 模式为 softAP。
    wifi_send_and_recv("AT+CWMODE=2\r\n", 1000, 300);

    // 使能多连接。 (当 ESP32-C3 设备作为 TCP 服务器时，必须通过AT+CIPMUX=1 命令使能多连接，因为可能有多个 TCP客户端连接到 ESP32-C3 设备。)
    wifi_send_and_recv("AT+CIPMUX=1\r\n", 1000, 300);

    // 设置 softAP AT+CWSAP="ESP32_softAP","1234567890",5,3; 5是信道号, 3是加密方式 3=WPA2_PSK
    char cwJAP[96];
    snprintf(cwJAP, sizeof(cwJAP), "AT+CWSAP=\"%s\",\"%s\",5,3\r\n", "ESP32_softAP", "1234567890");
    wifi_send_and_recv(cwJAP, 1000, 3000);

    // 等待 softAP 启动完成
    HAL_Delay(3000);

    // 查询 softAP 信息。
    esp32_send_cmd("AT+CIPAP?\r\n");
    esp32_read_resp(s_rx_buf, &s_rx_len, sizeof(s_rx_buf), 1000);

    /* 5. 解析并保存 */
    ParseQuote((char*)s_rx_buf, "CIPAP:ip:\"", s_ip_buf, sizeof(s_ip_buf));
    ParseQuote((char*)s_rx_buf, "CIPAP:gateway:\"", s_gateway_buf, sizeof(s_gateway_buf));
    ParseQuote((char*)s_rx_buf, "CIPAP:netmask:\"", s_netmask_buf, sizeof(s_netmask_buf));

    /* 6. 打印 */
    printf("wifi -> mode: AP IP: %s\r\n", s_ip_buf);
    printf("wifi -> mode: AP Gateway: %s\r\n", s_gateway_buf);
    printf("wifi -> mode: AP Netmask: %s\r\n", s_netmask_buf);

    // 建立 TCP 服务器，默认端口为 333
    wifi_send_and_recv("AT+CIPSERVER=1\r\n", 1000, 300);

    //
}


/**
* 设置 ESP32-C3 为 station + softAP 共存模式 (AP_STA)
*
* 流程：
*   1. AT+CWMODE=3         设置为 station + softAP 模式
*   2. AT+CWJAP=...        作为 station 连接路由器（用 WIFI_SSID / WIFI_PASSWORD）
*   3. AT+CWSAP=...        配置 softAP 参数（SSID / 密码 / 信道 / 加密）
*   4. AT+CIPMUX=1         开启多连接（建立 TCP 服务器前置条件）
*   5. AT+CIPSTA?          查询 station 的 IP / 网关 / 子网掩码
*   6. AT+CIPAP?           查询 softAP 的 IP / 网关 / 子网掩码
*   7. 解析并打印双网 IP 信息
*
* 响应与说明：
*   - 5/6 解析结果中前缀分别为 "CIPSTA:xxx:\" / "CIPAP:xxx:\"，
*     解析后分别存放到 s_ip_buf / s_gateway_buf / s_netmask_buf；
*     后续通过 Wifi_GetIP() 等接口拿到的是最后查询的 softAP 的 IP。
*   - 如需同时保留两套 IP 信息，可扩展为双缓冲结构。
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

    char sta_ip[32]      = {0};
    char sta_gateway[32] = {0};
    char sta_netmask[32] = {0};
    ParseQuote((char*)s_rx_buf, "CIPSTA:ip:\"",      sta_ip,      sizeof(sta_ip));
    ParseQuote((char*)s_rx_buf, "CIPSTA:gateway:\"", sta_gateway, sizeof(sta_gateway));
    ParseQuote((char*)s_rx_buf, "CIPSTA:netmask:\"", sta_netmask, sizeof(sta_netmask));

    printf("wifi -> mode: AP_STA  STA IP:      %s\r\n", sta_ip);
    printf("wifi -> mode: AP_STA  STA Gateway: %s\r\n", sta_gateway);
    printf("wifi -> mode: AP_STA  STA Netmask: %s\r\n", sta_netmask);

    /* 7. 查询 softAP 的 IP / 网关 / 子网掩码 */
    esp32_send_cmd("AT+CIPAP?\r\n");
    memset(s_rx_buf, 0, sizeof(s_rx_buf));
    esp32_read_resp(s_rx_buf, &s_rx_len, sizeof(s_rx_buf), 1000);

    ParseQuote((char*)s_rx_buf, "CIPAP:ip:\"",      s_ip_buf,      sizeof(s_ip_buf));
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
 * @brief  WiFi 完整初始化流程
 */
void Wifi_Init(Wifi_Mode mode)
{
    printf("=============== WiFi 初始化 ===============\r\n");

    // 初始化
    esp32_init();

    // 保存模式
    s_mode = mode;

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
