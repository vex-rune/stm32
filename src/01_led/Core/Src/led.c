#include "led.h"

#include "main.h"
#include "stm32f1xx_hal_gpio.h"

/**
 * 初始化 LED
 * 默认是停止的
 */
void led_init(void)
{
    HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(led3_GPIO_Port, led3_Pin, GPIO_PIN_SET);
}

/**
 * 流水等
 */
void led_running(void)
{
    HAL_GPIO_TogglePin(led1_GPIO_Port, led1_Pin);
    HAL_Delay(500);
    HAL_GPIO_TogglePin(led2_GPIO_Port, led2_Pin);
    HAL_Delay(500);
    HAL_GPIO_TogglePin(led3_GPIO_Port, led3_Pin);
    HAL_Delay(500);
}

/**
 * 停止所有 LED
 */
void led_stop(void)
{
    HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(led3_GPIO_Port, led3_Pin, GPIO_PIN_SET);
}

