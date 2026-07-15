#ifndef LED_SWICH_H
#define LED_SWICH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "gpio.h"

/* LED 模式枚举：key4 循环切换 */
typedef enum {
    LED_MODE_ALL_ON    = 0,   /* 全亮 */
    LED_MODE_WATERFALL = 1,   /* 流水灯（依次点亮再循环） */
    LED_MODE_BLINK     = 2,   /* 全部一起闪烁 */
    LED_MODE_MARQUEE   = 3,   /* 跑马灯（只有一个亮，循环移动） */
    LED_MODE_ALL_OFF   = 4,   /* 全灭 */
    LED_MODE_MAX
} LedMode_t;

/* 初始化（关闭所有 LED，进入默认模式） */
void LedSwich_Init(void);

/* 主循环中周期性调用，用于推进流水/闪烁/跑马灯状态 */
void LedSwich_Process(void);

/* 4 个按键回调（外部中断中调用，已做防抖） */
void LedSwich_OnKey1Pressed(void);
void LedSwich_OnKey2Pressed(void);
void LedSwich_OnKey3Pressed(void);
void LedSwich_OnKey4Pressed(void);

#ifdef __cplusplus
}
#endif

#endif /* LED_SWICH_H */