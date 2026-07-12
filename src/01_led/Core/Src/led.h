#ifndef LED_H
#define LED_H
#include "gpio.h"

// 红led引脚
#define LED_RED_PIN GPIO_PIN_0
// 绿led引脚
#define LED_GREEN_PIN GPIO_PIN_1
// 蓝led引脚
#define LED_BLUE_PIN GPIO_PIN_2

// 初始化引脚
void led_init(void);

// 开始流水灯
void led_start(void);

// 停止流水灯
void led_stop(void);

#endif // LED_H
