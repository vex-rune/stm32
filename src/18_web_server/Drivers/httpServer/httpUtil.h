/**
    @file	httpUtil.h
    @brief	Header File for HTTP Server Utilities
    @version 1.0
    @date	2014/07/15
    @par Revision
 			2014/07/15 - 1.0 Release
    @author
    \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
*/

#ifndef	__HTTPUTIL_H__
#define	__HTTPUTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "httpServer.h"
#include "httpParser.h"

/**
 * @brief   HTTP GET 请求的 CGI 统一处理函数
 * @details 由 httpServer 在收到 GET 请求时根据 URL 调用,
 *          用于从用户自定义的 CGI 回调表中查找并执行对应处理函数,
 *          将响应数据填充到 buf,并返回 HTTP 响应状态.
 * @param   uri_name   [in]  请求的 URI 名称(例如 "/", "/led")
 * @param   buf        [out] 用于存放响应数据的缓冲区
 * @param   file_len   [out] 响应数据的长度(字节)
 * @retval  HTTP_OK    处理成功
 * @retval  其他       处理失败或未找到对应 CGI
 */
uint8_t http_get_cgi_handler(uint8_t * uri_name, st_http_request * p_http_request, uint8_t * buf, uint32_t * file_len);

/**
 * @brief   HTTP POST 请求的 CGI 统一处理函数
 * @details 由 httpServer 在收到 POST 请求时根据 URL 调用,
 *          解析 POST 请求体(p_http_request)中的表单数据,
 *          并将处理结果(例如跳转响应)填充到 buf.
 * @param   uri_name         [in]  请求的 URI 名称
 * @param   p_http_request   [in]  指向已解析的 HTTP 请求结构体
 * @param   buf              [out] 用于存放响应数据的缓冲区
 * @param   file_len         [out] 响应数据的长度(字节)
 * @retval  HTTP_OK          处理成功
 * @retval  其他             处理失败或未找到对应 CGI
 */
uint8_t http_post_cgi_handler(uint8_t * uri_name, st_http_request * p_http_request, uint8_t * buf, uint32_t * file_len);

/**
 * @brief   预定义的 GET 类型 CGI 处理器(查表式)
 * @details 在 cgi_webpage / cgi_xxx 等表中按 uri_name 匹配,
 *          调用对应的 get_handler 回调,把返回的内容拷贝到 buf
 *          中,并通过 len 给出实际写入字节数.
 * @param   uri_name   [in]  请求的 URI 名称
 * @param   buf        [out] 用于存放响应数据的缓冲区
 * @param   len        [out] 响应数据的长度(字节)
 * @retval  HTTP_OK    处理成功
 * @retval  其他       未找到匹配的 CGI 或处理失败
 */
uint8_t predefined_get_cgi_processor(uint8_t * uri_name, uint8_t * buf, uint16_t * len);

/**
 * @brief   预定义的 SET 类型 CGI 处理器(查表式)
 * @details 在 cgi_webpage / cgi_xxx 等表中按 uri_name 匹配,
 *          调用对应的 set_handler 回调,把 uri 中携带的参数
 *          (例如 "?led=1")写入 buf,作为对客户端的响应或日志.
 * @param   uri_name   [in]  请求的 URI 名称(不含参数)
 * @param   uri        [in]  带参数的完整 URI 字符串
 * @param   buf        [out] 用于存放响应数据的缓冲区
 * @param   len        [out] 响应数据的长度(字节)
 * @retval  HTTP_OK    处理成功
 * @retval  其他       未找到匹配的 CGI 或处理失败
 */
uint8_t predefined_set_cgi_processor(uint8_t * uri_name, uint8_t * uri, uint8_t * buf, uint16_t * len);

#ifdef __cplusplus
}
#endif

#endif
