#include "web_server.h"

#include <stdio.h>
#include <stdlib.h>

#include "httpServer.h"

// html 的名称
// 数据存于 FLASH （.rodata 段）以节省 RAM
static const uint8_t html_name[] = "index.html";

// 一个html的欢迎页
// 数据存于 FLASH （.rodata 段）以节省 RAM
// 包含 3 个 LED 开关卡片:两个 <a> 链接直接进 GET /led.cgi
//   href 示例: /led.cgi?action=on&device=led1
static const uint8_t welcome_html[] =
    "<!DOCTYPE html>"
    "<html lang=\"zh-CN\">"
    "<head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<title>STM32 Web Server</title>"
    "<style>"
    "body{font-family:'Segoe UI',Arial,sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);margin:0;padding:0;min-height:100vh;display:flex;justify-content:center;align-items:center;color:#fff;}"
    ".container{background:rgba(255,255,255,0.1);border-radius:20px;padding:36px 44px;box-shadow:0 8px 32px rgba(0,0,0,0.2);text-align:center;border:1px solid rgba(255,255,255,0.2);max-width:780px;}"
    "h1{font-size:2em;margin:0 0 24px;letter-spacing:1px;}"
    ".grid{display:grid;grid-template-columns:repeat(3,1fr);gap:14px;margin:18px 0;}"
    ".card{background:rgba(255,255,255,0.08);border-radius:14px;padding:18px 14px;border:1px solid rgba(255,255,255,0.15);}"
    ".card h2{margin:0 0 14px;font-size:1.3em;font-weight:600;}"
    ".btns{display:flex;gap:8px;}"
    ".btn{flex:1;border:none;padding:10px 0;border-radius:10px;font-weight:bold;cursor:pointer;font-size:0.95em;color:#fff;transition:transform .08s ease,opacity .15s ease;text-decoration:none;text-align:center;display:inline-block;}"
    ".btn:active{transform:scale(0.96);}"
    ".btn.on{background:#10b981;}"
    ".btn.off{background:#ef4444;}"
    ".btn:hover{opacity:0.85;}"
    ".badge{display:inline-block;background:rgba(255,255,255,0.18);padding:6px 12px;border-radius:14px;margin:3px;font-size:0.85em;}"
    ".footer{font-size:0.85em;opacity:0.85;margin-top:18px;}"
    "@media (max-width:480px){.grid{grid-template-columns:1fr;}}"
    "</style>"
    "</head>"
    "<body>"
    "<div class=\"container\">"
    "<h1>STM32 LED 控制台</h1>"
    "<div class=\"grid\">"

    "<div class=\"card\">"
    "<h2>LED1</h2>"
    "<div class=\"btns\">"
    "<a class=\"btn on\"  href=\"javascript:void(0)\" data-url=\"/led.cgi?action=on&device=led1\">打开</a>"
    "<a class=\"btn off\" href=\"javascript:void(0)\" data-url=\"/led.cgi?action=off&device=led1\">关闭</a>"
    "</div></div>"

    "<div class=\"card\">"
    "<h2>LED2</h2>"
    "<div class=\"btns\">"
    "<a class=\"btn on\"  href=\"javascript:void(0)\" data-url=\"/led.cgi?action=on&device=led2\">打开</a>"
    "<a class=\"btn off\" href=\"javascript:void(0)\" data-url=\"/led.cgi?action=off&device=led2\">关闭</a>"
    "</div></div>"

    "<div class=\"card\">"
    "<h2>LED3</h2>"
    "<div class=\"btns\">"
    "<a class=\"btn on\"  href=\"javascript:void(0)\" data-url=\"/led.cgi?action=on&device=led3\">打开</a>"
    "<a class=\"btn off\" href=\"javascript:void(0)\" data-url=\"/led.cgi?action=off&device=led3\">关闭</a>"
    "</div></div>"

    "</div>"
    "<p class=\"footer\">"
    "<span class=\"badge\">GET</span>"
    "<span class=\"badge\">?action=on/off</span>"
    "<span class=\"badge\">&device=led1/2/3</span>"
    "</p>"
    "</div>"
    "</body>"
    "<script>"
    "(function(){"
    "  function ledSet(btn){"
    "    var url = btn.dataset.url;"
    "    if(!url) return;"
    "    if(btn.dataset.busy === '1') return;"
    "    btn.dataset.busy = '1';"
    "    var oldText = btn.textContent;"
    "    btn.textContent = '...';"
    "    fetch(url)"
    "      .then(function(r){"
    "        if(!r.ok) throw new Error('HTTP ' + r.status);"
    "        return r.text();"
    "      })"
    "      .then(function(t){"
    "        console.log('[LED] OK ' + url + ' => ' + t);"
    "        btn.style.outline = '2px solid #fff';"
    "        setTimeout(function(){ btn.style.outline=''; }, 250);"
    "      })"
    "      .catch(function(err){"
    "        var msg = (err && err.message) ? err.message : String(err);"
    "        console.warn('[LED] FAIL ' + url + ' => ' + msg + ' (raw=' + err + ')');"
    "        alert('控制失败\\nURL: ' + url + '\\n原因: ' + msg);"
    "      })"
    "      .finally(function(){"
    "        btn.textContent = oldText;"
    "        btn.dataset.busy = '0';"
    "      });"
    "  }"
    "  document.addEventListener('click', function(e){"
    "    var t = e.target.closest('a.btn');"
    "    if(!t) return;"
    "    e.preventDefault();"
    "    ledSet(t);"
    "  });"
    "})();"
    "</script>"
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
