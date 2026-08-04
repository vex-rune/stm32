/**
 * @file    esp32.c
 * @brief   ESP32 AT指令通信模块 - HAL中断版本
 *
 * @par 功能说明:
 *  - ESP32 WiFi模块的AT指令通信封装
 *  - 使用HAL库USART中断方式接收数据
 *  - 支持AT握手、关闭回显、获取设备信息、复位等功能
 *
 * @par 硬件连接:
 *  - STM32 PA2 (USART2_TX) -> ESP32 GPIO6 (RX)
 *  - STM32 PA3 (USART2_RX) <- ESP32 GPIO7 (TX)
 *  - GND <-> GND
 *  - ESP32 EN引脚需接3.3V使能
 *
 * @par 使用流程:
 *  1. 在CubeMX中开启USART2全局中断 (NVIC)
 *  2. 调用 MX_USART2_UART_Init() 初始化串口
 *  3. 调用 esp32_init() 完成模块初始化和握手
 *  4. 使用 esp32_send_cmd() 发送AT指令
 *  5. 使用 esp32_read_resp() 接收响应
 *
 * @par 示例代码:
 * @code
 *   // 初始化
 *   if (esp32_init() != ESP_OK) {
 *       printf("ESP32 init failed\r\n");
 *   }
 *
 *   // 发送自定义AT指令
 *   esp32_send_cmd("AT+CWMODE=1\r\n");
 *
 *   // 带超时等待响应
 *   uint8_t buf[128];
 *   uint16_t len;
 *   if (esp32_read_resp(buf, &len, sizeof(buf), 3000) == ESP_OK) {
 *       printf("Response: %s\r\n", buf);
 *   }
 * @endcode
 *
 * @par 常用AT指令速查表:
 *
 *  基础指令:
 *  | 指令              | 功能              | 响应          |
 *  |-------------------|-------------------|---------------|
 *  | AT                | 测试连接          | OK            |
 *  | AT+RST            | 重启模块          | ready         |
 *  | ATE0              | 关闭回显          | OK            |
 *  | ATE1              | 开启回显          | OK            |
 *  | AT+GMR            | 查询版本信息      | 版本号+OK     |
 *
 *  WiFi连接指令:
 *  | 指令                          | 功能                  | 示例                                  |
 *  |-------------------------------|-----------------------|---------------------------------------|
 *  | AT+CWMODE=1                   | STA模式(客户端)       | AT+CWMODE=1                           |
 *  | AT+CWMODE=2                   | AP模式(热点)          | AT+CWMODE=2                           |
 *  | AT+CWMODE=3                   | STA+AP模式            | AT+CWMODE=3                           |
 *  | AT+CWJAP="ssid","pwd"         | 连接WiFi              | AT+CWJAP="TP-LINK","123456"           |
 *  | AT+CWQAP                      | 断开WiFi连接          | OK                                    |
 *  | AT+CIFSR                      | 查询本地IP地址        | 192.168.1.100                         |
 *  | AT+CWLAP                      | 扫描周围WiFi          | 列表+OK                               |
 *
 *  TCP/UDP通信指令:
 *  | 指令                              | 功能              | 示例                                          |
 *  |-----------------------------------|-------------------|-----------------------------------------------|
 *  | AT+CIPMUX=0                       | 单连接模式        | OK                                            |
 *  | AT+CIPMUX=1                       | 多连接模式        | OK                                            |
 *  | AT+CIPSTART="TCP","ip",port       | 建立TCP连接       | AT+CIPSTART="TCP","192.168.1.1",8080          |
 *  | AT+CIPSEND=len                    | 发送数据(指定长度)| AT+CIPSEND=5                                  |
 *  | AT+CIPCLOSE                       | 关闭TCP连接       | OK                                            |
 *  | AT+CIPSERVER=1,port               | 开启TCP服务器     | AT+CIPSERVER=1,8080                         |
 *
 *  HTTP请求指令:
 *  | 指令                                      | 功能      | 示例                                                      |
 *  |-------------------------------------------|-----------|-----------------------------------------------------------|
 *  | AT+HTTPCLIENT=2,0,"url",,,1               | GET请求   | AT+HTTPCLIENT=2,0,"http://api.example.com",,,1            |
 *  | AT+HTTPCPOST="url",len                    | POST请求  | AT+HTTPCPOST="http://api.com",10                          |
 *
 * @par AT指令注意事项:
 *  1. 每条指令必须以 \r\n 结尾
 *  2. 连接WiFi需要等待 3-5 秒
 *  3. 发送数据前先执行 AT+CIPSEND=len
 *  4. 响应 "ERROR" 表示指令执行失败
 *
 * @par ESP32作为TCP服务器命令序列:
 * @code
 *   // AP模式（ESP32作为热点，客户端直接连接）
 *   const char* server_cmds[] = {
 *       "AT+CWMODE=2\r\n",                              // 设置为AP模式
 *       "AT+CWSAP=\"ESP32_AP\",\"12345678\",1,3\r\n",    // 设置热点名称、密码
 *       "AT+CIPMUX=1\r\n",                              // 开启多连接模式
 *       "AT+CIPSERVER=1,8080\r\n",                      // 启动TCP服务器，端口8080
 *       NULL
 *   };
 *
 *   // STA模式（ESP32连接现有WiFi）
 *   const char* server_sta_cmds[] = {
 *       "AT+CWMODE=1\r\n",
 *       "AT+CWJAP=\"YOUR_SSID\",\"YOUR_PASS\r\n",        // 替换为你的WiFi信息
 *       "AT+CIPMUX=1\r\n",
 *       "AT+CIPSERVER=1,8080\r\n",
 *       NULL
 *   };
 *
 *   // 使用示例：遍历发送命令
 *   for (int i = 0; server_cmds[i] != NULL; i++) {
 *       esp32_send_cmd(server_cmds[i]);
 *       HAL_Delay(100);  // 等待响应
 *   }
 *
 *   // 连接WiFi命令需要额外等待时间
 *   // AT+CWJAP 后需等待 3-5 秒直到收到 "OK"
 * @endcode
 */

#include "esp32.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief ESP32模块完整初始化
 *
 * @par 初始化流程:
 *  1. AT握手测试 - 发送"AT"等待"OK"响应
 *  2. 关闭回显模式 - 发送"ATE0"关闭命令回显
 *  3. 获取设备信息 - 发送"AT+GMR"获取版本信息并打印
 *
 * @retval ESP_OK      初始化成功
 * @retval ESP_TIMEOUT 握手超时或模块未响应
 * @retval ESP_ERROR   关闭回显或获取信息失败
 *
 * @note 必须在MX_USART2_UART_Init()之后调用
 */
ESP_StatusTypeDef esp32_init(void)
{
    uint8_t rx_buf[128] = {0};
    uint16_t rx_len = 0;

    /* ========== 步骤1: AT握手测试 ========== */
    // 清空接收缓冲区，避免残留数据干扰
    USART2_ClearBuf();

    // 发送AT测试命令
    HAL_UART_Transmit(ESP_HUART, (uint8_t*)"AT\r\n", 4, 100);

    // 等待响应：轮询检查接收缓冲区，直到超时或空闲
    uint32_t tick_start = HAL_GetTick();
    uint32_t last_rx_tick = tick_start;

    while ((HAL_GetTick() - tick_start) < 3000) // 3秒总超时
    {
        uint16_t count = USART2_GetRxCount();
        if (count > rx_len)
        {
            rx_len = count; // 更新已接收字节数
            last_rx_tick = HAL_GetTick(); // 记录最后接收时间
        }

        // 空闲检测：收到数据后100ms无新数据，认为一帧接收完成
        if (rx_len > 0 && (HAL_GetTick() - last_rx_tick) > 100)
        {
            break;
        }
    }

    // 读取接收到的数据
    USART2_ReadBuf(rx_buf, rx_len);

    // 检查是否收到OK
    if (!(rx_len > 0 && strstr((char*)rx_buf, "OK") != NULL))
    {
        return ESP_TIMEOUT; // 握手失败
    }

    /* ========== 步骤2: 关闭回显模式 ========== */
    // ATE0命令关闭AT回显，之后发送命令不会收到回显，只收到响应
    if (esp32_disable_echo() != ESP_OK)
    {
        return ESP_ERROR;
    }

    /* ========== 步骤3: 获取并打印设备信息 ========== */
    char info_buf[256] = {0};
    if (esp32_get_info(info_buf, sizeof(info_buf)) == ESP_OK)
    {
        printf("esp32 -> %s\r\n", info_buf);
    }

    return ESP_OK;
}

/**
 * @brief 发送AT命令并自动计算字符串长度
 *
 * @param cmd 以'\0'结尾的命令字符串
 *
 * @note 不会自动添加\r\n，需要调用者自行包含
 */
void esp32_send_cmd(const char* cmd)
{
    if (cmd == NULL)
        return;

    printf("esp32 -> send_cmd: [ %s ];", cmd);
    HAL_UART_Transmit(ESP_HUART, (uint8_t*)cmd, (uint16_t)strlen(cmd), 1000);
}

/**
 * @brief 读取响应（中断方式）
 *
 * @param buf        接收缓冲区
 * @param len        [out]实际接收到的字节数
 * @param size       缓冲区最大容量
 * @param timeout_ms 总等待超时时间（毫秒）
 *
 * @retval ESP_OK      成功接收到数据
 * @retval ESP_TIMEOUT 超时无数据
 * @retval ESP_ERROR   参数错误
 *
 * @note 使用空闲帧检测：收到数据后100ms无新数据认为接收完成
 */
ESP_StatusTypeDef esp32_read_resp(uint8_t buf[], uint16_t* len, uint16_t size, uint32_t timeout_ms)
{
    if (buf == NULL || len == NULL || size == 0)
    {
        if (len != NULL)
            *len = 0;
        return ESP_ERROR;
    }

    uint32_t tick_start = HAL_GetTick();
    uint32_t last_rx_tick = tick_start;
    uint16_t rx_len = 0;

    // 轮询检查接收缓冲区
    while ((HAL_GetTick() - tick_start) < timeout_ms)
    {
        uint16_t count = USART2_GetRxCount();
        if (count > rx_len)
        {
            rx_len = count;
            last_rx_tick = HAL_GetTick();
        }

        // 空闲帧检测
        if (rx_len > 0 && (HAL_GetTick() - last_rx_tick) > 100)
        {
            break;
        }
    }

    // 限制复制长度，防止缓冲区溢出
    if (rx_len > size - 1)
        rx_len = size - 1;

    USART2_ReadBuf(buf, rx_len);
    buf[rx_len] = '\0';
    *len = rx_len;

    // printf("esp32 -> recv: [ %s ];\r\n", buf);

    return (rx_len > 0) ? ESP_OK : ESP_TIMEOUT;
}

/**
 * @brief 获取ESP32设备信息（版本号）
 *
 * @param info_buf 信息缓冲区
 * @param buf_size 缓冲区大小
 *
 * @retval ESP_OK      成功
 * @retval ESP_TIMEOUT 超时
 * @retval ESP_ERROR   参数错误
 *
 * @note 发送AT+GMR命令，返回AT版本、SDK版本、编译时间等信息
 */
ESP_StatusTypeDef esp32_get_info(char* info_buf, uint16_t buf_size)
{
    if (info_buf == NULL || buf_size == 0)
        return ESP_ERROR;

    USART2_ClearBuf();
    esp32_send_cmd("AT+GMR\r\n");

    uint8_t rx_buf[256] = {0};
    uint16_t rx_len = 0;
    ESP_StatusTypeDef res = esp32_read_resp(rx_buf, &rx_len, sizeof(rx_buf), 3000);

    if (res == ESP_OK && strstr((char*)rx_buf, "OK") != NULL)
    {
        strncpy(info_buf, (char*)rx_buf, buf_size - 1);
        info_buf[buf_size - 1] = '\0';
        return ESP_OK;
    }

    return ESP_TIMEOUT;
}

/**
 * @brief 关闭AT回显模式
 *
 * @retval ESP_OK      成功
 * @retval ESP_TIMEOUT 超时
 *
 * @note 发送ATE0命令，关闭后ESP32不会回显收到的AT命令
 *       只返回命令执行结果，减少通信数据量
 */
ESP_StatusTypeDef esp32_disable_echo(void)
{
    USART2_ClearBuf();
    esp32_send_cmd("ATE0\r\n");

    uint8_t rx_buf[64] = {0};
    uint16_t rx_len = 0;
    ESP_StatusTypeDef res = esp32_read_resp(rx_buf, &rx_len, sizeof(rx_buf), 2000);
    return (res == ESP_OK && strstr((char*)rx_buf, "OK") != NULL) ? ESP_OK : ESP_TIMEOUT;
}

/**
 * @brief 复位ESP32模块
 *
 * @retval ESP_OK      成功
 * @retval ESP_TIMEOUT 超时
 *
 * @note 发送AT+RST命令复位模块，复位后模块会输出"ready"
 *       函数会等待1秒让模块完成复位
 */
ESP_StatusTypeDef esp32_reset(void)
{
    USART2_ClearBuf();
    esp32_send_cmd("AT+RST\r\n");

    uint8_t rx_buf[128] = {0};
    uint16_t rx_len = 0;
    ESP_StatusTypeDef res = esp32_read_resp(rx_buf, &rx_len, sizeof(rx_buf), 5000);
    if (res == ESP_OK && strstr((char*)rx_buf, "ready") != NULL)
    {
        HAL_Delay(1000); // 等待复位完成
        return ESP_OK;
    }
    return ESP_TIMEOUT;
}
