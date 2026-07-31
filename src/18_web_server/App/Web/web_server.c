#include "web_server.h"

#include <stdio.h>
#include <stdlib.h>

#include "httpServer.h"

// html 的名称
// 数据存于 FLASH （.rodata 段）以节省 RAM
static const uint8_t html_name[] = "index.html";

// 一个html的欢迎页
// 数据存于 FLASH （.rodata 段）以节省 RAM
static const uint8_t welcome_html[] =
    "<!DOCTYPE html>"
    "<html lang=\"zh-CN\">"
    "<head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<title>STM32 Web Server</title>"
    "<style>"
    "body{font-family:'Segoe UI',Arial,sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);margin:0;padding:0;min-height:100vh;display:flex;justify-content:center;align-items:center;color:#fff;}"
    ".container{background:rgba(255,255,255,0.1);border-radius:20px;padding:40px 60px;box-shadow:0 8px 32px rgba(0,0,0,0.2);text-align:center;border:1px solid rgba(255,255,255,0.2);}"
    "h1{font-size:2.5em;margin:0 0 20px;letter-spacing:2px;}"
    "p{font-size:1.1em;line-height:1.6;margin:10px 0;}"
    ".badge{display:inline-block;background:rgba(255,255,255,0.2);padding:8px 18px;border-radius:20px;margin:5px;font-size:0.95em;}"
    ".heart{color:#ff6b6b;font-size:1.4em;}"
    "</style>"
    "</head>"
    "<body>"
    "<div class=\"container\">"
    "<h1>Welcome to STM32 Web Server</h1>"
    "<p><span class=\"heart\">&#10084;</span> 嵌入式 Web 服务器已成功运行 <span class=\"heart\">&#10084;</span></p>"
    "<p>基于 W5500 以太网芯片 + HTTP Server 实现</p>"
    "<div style=\"margin-top:20px;\">"
    "<span class=\"badge\">STM32F103ZE</span>"
    "<span class=\"badge\">W5500</span>"
    "<span class=\"badge\">SPI</span>"
    "<span class=\"badge\">TCP/IP</span>"
    "</div>"
    "<p style=\"margin-top:30px;font-size:0.9em;opacity:0.8;\">Hello from STM32!</p>"
    "</div>"
    "</body>"
    "</html>";


// 缓存区
uint8_t tx_buf[4096] = {0};
// 缓存区大小
uint8_t rx_buf[4096] = {0};
// socket 个数
uint8_t socket_count = 8;
// socket 列表
uint8_t socket_list[8] = {0, 1, 2, 3, 4, 5, 6, 7};

// 初始化 web 服务器
void WebServer_Init(void)
{
    printf("[WebServer] init ...\r\n");

    // 初始化 服务
    httpServer_init(tx_buf, rx_buf, socket_count, socket_list);
    printf("[WebServer] init done\r\n");


    printf("[WebServer] register html ...\r\n");
    // 注册html页面的内容
    // 强转 (uint8_t *) 去掉 const，是因为 reg_httpServer_webContent 原型为 uint8_t *
    // 但函数内部仅读不改（strlen + 保存指针），数据存在 FLASH 是安全的
    reg_httpServer_webContent((uint8_t*)html_name, (uint8_t*)welcome_html);
    printf("[WebServer] register html done\r\n");
}


void WebServer_Start(void)
{
    httpServer_run(0);
}
