#ifndef LED_H
#define LED_H
#include "gpio.h"

// 初始化引脚
void led_init(void);

// 开始流水灯
void led_start(void);

// 停止流水灯
void led_stop(void);


// 单独开灯
void led_on_by_index( int index);

// 单独关灯
void led_off_by_index(int index);

#endif // LED_H
