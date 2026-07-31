/**
    @file	httpUtil.c
    @brief	HTTP Server Utilities
    @version 1.0
    @date	2014/07/15
    @par Revision
 			2014/07/15 - 1.0 Release
    @author
    \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "httpUtil.h"

// LED 控制回调:由用户在 App 层提供强符号实现
// 参数 action: 0=off, 1=on(其他值保留)
// 参数 device: 设备编号(0 表示第一个 LED,类推)
__attribute__((weak)) void _led_action(uint8_t action, uint8_t device);

uint8_t http_get_cgi_handler(uint8_t* uri_name, st_http_request* p_http_request, uint8_t* buf, uint32_t* file_len)
{
    uint8_t ret = HTTP_OK;
    uint16_t len = 0;

    if (predefined_get_cgi_processor(uri_name, buf, &len))
    {
        ;
    }
    else if (strncmp((const char*)p_http_request->URI, "/led.cgi", 8) == 0)
    {
        // 直接看 raw URI 头:避开 get_http_uri_name 中 strtok 的状态污染问题
        // URI 形如: "/led.cgi?action=1&device=0"
        uint8_t action_val = -1;   // 缺省:开
        uint8_t device_val = -1;   // 缺省:第一个设备

        // 注:本项目里 httpParser.c 的 get_http_param_value 期望传入完整 HTTP
        // 报文(含 Content-Length 与 \r\n\r\n),不能直接吃 URI。
        // 这里改用本地查找,先定位 '?' 后的查询串,再 strstr 出 key=val。
        const char * uri = (const char *)p_http_request->URI;
        const char * qmark = strchr(uri, '?');
        if (qmark != NULL) {
            const char * query = qmark + 1;  // 跳过 '?'

            // 解析 "action=1&device=0" 这种键值对
            const char * p = strstr(query, "action=");
            if (p != NULL) {
                action_val = (uint8_t)ATOI((uint8_t *)(p + 7), 10);
            }

            p = strstr(query, "device=");
            if (p != NULL) {
                device_val = (uint8_t)ATOI((uint8_t *)(p + 7), 10);
            }
        }

        if (-1 == action_val || -1 == device_val) {
          // 缺少 action 或 device 参数,返回错误
          ret = HTTP_FAILED;
        }

        // 调用用户实现的 LED 控制回调(weak 符号,允许在 App 层覆盖)
        _led_action(action_val, device_val);

        // 返回 200 + 文本响应给前端
        len = sprintf((char*)buf, "led ok\r\n  device=%u\r\n  action=%u", device_val, action_val);
    }
    else
    {
        // CGI file not found
        ret = HTTP_FAILED;
    }

    if (ret)
    {
        *file_len = len;
    }
    return ret;
}

uint8_t http_post_cgi_handler(uint8_t* uri_name, st_http_request* p_http_request, uint8_t* buf, uint32_t* file_len)
{
    uint8_t ret = HTTP_OK;
    uint16_t len = 0;
    uint8_t val = 0;

    if (predefined_set_cgi_processor(uri_name, p_http_request->URI, buf, &len))
    {
        ;
    }
    else if (strcmp((const char*)uri_name, "example.cgi") == 0)
    {
        // To do
        val = 1;
        len = sprintf((char*)buf, "%d", val);
    }
    else
    {
        // CGI file not found
        ret = HTTP_FAILED;
    }

    if (ret)
    {
        *file_len = len;
    }
    return ret;
}

uint8_t predefined_get_cgi_processor(uint8_t* uri_name, uint8_t* buf, uint16_t* len)
{
    ;
}

uint8_t predefined_set_cgi_processor(uint8_t* uri_name, uint8_t* uri, uint8_t* buf, uint16_t* en)
{
    ;
}
