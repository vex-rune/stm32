/* LED 命令控制器实现
 *
 * 把命令表、匹配逻辑和 GPIO 操作都封在这里，
 * 对外只暴露 ledCtrl() 一个入口。
 */
#include "led.h"
#include "gpio.h"              /* led1_GPIO_Port / led1_Pin 等宏由 gpio.h 提供 */
#include "stm32f1xx_hal.h"     /* HAL_GPIO_WritePin / GPIO_TypeDef 等 */
#include <string.h>            /* strcmp */

/* ---------- 私有类型与命令表 ---------- */

/* 单条命令：字符串 → (目标 LED id, 期望状态)
 * led_id = 0xFF 表示同时控制所有 LED */
typedef struct
{
  const char* cmd;
  uint8_t led_id;
  uint8_t on; /* 1=点亮，0=熄灭 */
} led_cmd_t;

/* 命令表：加新命令只要在这里加一行 */
static const led_cmd_t s_led_cmds[] = {
  {"led1-on", 1, 0},
  {"led1-off", 1, 1},
  {"led2-on", 2, 0},
  {"led2-off", 2, 1},
  {"led-all-on", 0xFF, 0},
  {"led-all-off", 0xFF, 1},
};
#define LED_CMD_COUNT  (sizeof(s_led_cmds) / sizeof(s_led_cmds[0]))

/* ---------- 私有 helper ---------- */

/* 单个 LED 输出：id ∈ {1, 2}；on=1 拉高点亮 */
static void led_set_one(uint8_t id, uint8_t on)
{
  GPIO_TypeDef* port;
  uint16_t pin;
  switch (id)
  {
  case 1: port = led1_GPIO_Port;
    pin = led1_Pin;
    break;
  case 2: port = led2_GPIO_Port;
    pin = led2_Pin;
    break;
  default: return;
  }
  HAL_GPIO_WritePin(port, pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* ---------- 公开入口 ---------- */

/* 命令控制器入口：成功返回 0，未知命令返回 -1 */
int ledCtrl(const char* cmd)
{
  if (cmd == NULL || cmd[0] == '\0') return -1;

  for (uint32_t i = 0; i < LED_CMD_COUNT; i++)
  {
    if (strcmp(cmd, s_led_cmds[i].cmd) != 0) continue;

    if (s_led_cmds[i].led_id == 0xFF)
    {
      /* 全控：同时驱动所有 LED */
      led_set_one(1, s_led_cmds[i].on);
      led_set_one(2, s_led_cmds[i].on);
    }
    else
    {
      led_set_one(s_led_cmds[i].led_id, s_led_cmds[i].on);
    }
    return 0;
  }
  return -1;
}
