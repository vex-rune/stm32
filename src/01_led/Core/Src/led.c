/*
 * led 流水灯操作
 */

#include "led.h"
#include "gpio.h"


// led 引脚数组
 uint16_t leds[] = {
    LED_RED_PIN,
    LED_GREEN_PIN,
    LED_BLUE_PIN,
};

// led 流水灯控制顺序 1 -> 2 -> 3 -> 2 -> 1
uint16_t led_order[] = {
    LED_RED_PIN,
    LED_GREEN_PIN,
    LED_BLUE_PIN,
    LED_GREEN_PIN
};

// led初始化
void led_init(void)
{
    // 初始化led为灭
    led_stop();
}

// led流水灯开始
void led_start(void)
{
    // led_order 按照顺序流水灯
    for (int i = 0; i < 4; i++)
    {
        HAL_GPIO_TogglePin(GPIOA, led_order[i]);
        HAL_GPIO_TogglePin(GPIOA, led_order[i]);
        HAL_Delay(500);
    }
    

}


// led流水灯停止
void led_stop(void)
{
    // 初始化led为灭
    for (int i = 0; i < 3; i++)
    {
        HAL_GPIO_WritePin(GPIOA, leds[i], GPIO_PIN_RESET);
    }
}





