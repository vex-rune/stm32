/* LED 命令控制器头文件
 *
 * 命令表、匹配逻辑和 GPIO 操作都封装在 led.c 内，
 * 对外只暴露 ledCtrl() 一个入口。
 */
#ifndef LED_H
#define LED_H

#ifdef __cplusplus
extern "C" {
#endif

/* LED 命令控制器
 *
 * 参数：cmd - 已 '\0' 结尾的 C 字符串命令（末尾 \r \n 应由调用方去除）
 * 返回：0  = 识别并执行成功
 *      -1 = 未知命令 / 空字符串 / NULL
 *
 * 当前支持的命令（详见 led.c 中 s_led_cmds[]）：
 *   led1-on / led1-off
 *   led2-on / led2-off
 *   led-all-on / led-all-off
 *
 * 加新命令只需在 led.c 的 s_led_cmds[] 加一行，main.c 无需改动。
 */
int ledCtrl(const char *cmd);

#ifdef __cplusplus
}
#endif

#endif /* LED_H */