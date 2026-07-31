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

uint8_t http_get_cgi_handler(uint8_t * uri_name, st_http_request * p_http_request, uint8_t * buf, uint32_t * file_len) {
    uint8_t ret = HTTP_OK;
    uint16_t len = 0;

    if (predefined_get_cgi_processor(uri_name, buf, &len)) {
        ;
    } else if (strcmp((const char *)uri_name, "example.cgi") == 0) {
        // =======================================================================
        // 示例：解析 GET 请求中 ?action=1 形式的查询参数
        //   浏览器访问:  GET /example.cgi?action=1 HTTP/1.1
        //   p_http_request->URI 实际存放的是整行原始报文:
        //       "GET /example.cgi?action=1 HTTP/1.1"
        //   于是我们手动从 '?' 起截出 query,再按 key=val 解析。
        // =======================================================================
        int action = -1;                                  // 默认 -1 表示未提供
        const char * query = (const char *)p_http_request->URI;
        char * qmark = strchr(query, '?');                // 定位 '?'
        if (qmark != NULL) {
            qmark++;                                      // 跳过 '?' 指向参数起咰
            // 查找 "action=" 子串(仅在最前面处理一个参数的场景)
            char * p = strstr(qmark, "action=");
            if (p != NULL) {
                p += strlen("action=");                   // 指向 value 起点
                action = (int)ATOI((uint8_t *)p, 10);     // 转十进制
            }
        }

        // 根据 action 的取值拼装响应内容
        switch (action) {
        case 1:
            len = sprintf((char *)buf, "Action=1: ON");
            break;
        case 0:
            len = sprintf((char *)buf, "Action=0: OFF");
            break;
        default:
            len = sprintf((char *)buf, "Action not specified");
            break;
        }
    } else {
        // CGI file not found
        ret = HTTP_FAILED;
    }

    if (ret) {
        *file_len = len;
    }
    return ret;
}

uint8_t http_post_cgi_handler(uint8_t * uri_name, st_http_request * p_http_request, uint8_t * buf, uint32_t * file_len) {
    uint8_t ret = HTTP_OK;
    uint16_t len = 0;
    uint8_t val = 0;

    if (predefined_set_cgi_processor(uri_name, p_http_request->URI, buf, &len)) {
        ;
    } else if (strcmp((const char *)uri_name, "example.cgi") == 0) {
        // To do
        val = 1;
        len = sprintf((char *)buf, "%d", val);
    } else {
        // CGI file not found
        ret = HTTP_FAILED;
    }

    if (ret)	{
        *file_len = len;
    }
    return ret;
}

uint8_t predefined_get_cgi_processor(uint8_t * uri_name, uint8_t * buf, uint16_t * len) {
    ;
}

uint8_t predefined_set_cgi_processor(uint8_t * uri_name, uint8_t * uri, uint8_t * buf, uint16_t * en) {
    ;
}
