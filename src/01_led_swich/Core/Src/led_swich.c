#include "led_swich.h"
#include "stm32f1xx_hal.h"

/* 默认模式：全灭 */
static LedMode_t s_mode = LED_MODE_ALL_OFF;

/* key1/2/3 各自 LED 的状态: 0=灭, 1=亮 */
static uint8_t s_led1_state = 0;
static uint8_t s_led2_state = 0;
static uint8_t s_led3_state = 0;

/* 流水/跑马灯位置索引 (0,1,2)；闪烁时 0/1 表示灭/亮 */
static uint8_t  s_step_index = 0;

/* 上次推进的时间戳 */
static uint32_t s_last_tick = 0;

/* 模式步进间隔（ms） */
#define MODE_INTERVAL_MS 300u

/* 内部辅助：写单个 LED（index: 0=led1, 1=led2, 2=led3） */
static void Led_SetSingle(uint8_t index, GPIO_PinState state)
{
    switch (index)
    {
        case 0: HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin, state); break;
        case 1: HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, state); break;
        case 2: HAL_GPIO_WritePin(led3_GPIO_Port, led3_Pin, state); break;
        default: break;
    }
}

/* 内部辅助：同时设置全部 LED */
static void Led_SetAll(GPIO_PinState state)
{
    HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin, state);
    HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, state);
    HAL_GPIO_WritePin(led3_GPIO_Port, led3_Pin, state);
}

/* 按当前模式刷一次初始状态（用于切换模式时立即生效） */
static void Led_ApplyModeInitial(void)
{
    switch (s_mode)
    {
        case LED_MODE_ALL_ON:
            s_led1_state = s_led2_state = s_led3_state = 1;
            Led_SetAll(GPIO_PIN_RESET);   /* 低电平点亮 */
            break;

        case LED_MODE_WATERFALL:
            s_step_index = 0;
            Led_SetAll(GPIO_PIN_SET);     /* 全灭，再单独点亮 led1 */
            Led_SetSingle(0, GPIO_PIN_RESET);
            break;

        case LED_MODE_BLINK:
            s_step_index = 0;
            Led_SetAll(GPIO_PIN_SET);     /* 初始为灭 */
            break;

        case LED_MODE_MARQUEE:
            s_step_index = 0;
            Led_SetAll(GPIO_PIN_SET);     /* 全灭，再单独点亮 led1 */
            Led_SetSingle(0, GPIO_PIN_RESET);
            break;

        case LED_MODE_ALL_OFF:
        default:
            s_led1_state = s_led2_state = s_led3_state = 0;
            Led_SetAll(GPIO_PIN_SET);     /* 全灭 */
            break;
    }
    s_last_tick = HAL_GetTick();
}

void LedSwich_Init(void)
{
    s_mode       = LED_MODE_ALL_OFF;
    s_led1_state = 0;
    s_led2_state = 0;
    s_led3_state = 0;
    s_step_index = 0;
    s_last_tick  = HAL_GetTick();
    Led_SetAll(GPIO_PIN_SET);             /* 默认全部熄灭 */
}

void LedSwich_OnKey1Pressed(void)
{
    s_led1_state ^= 1u;
    HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin,
                      s_led1_state ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void LedSwich_OnKey2Pressed(void)
{
    s_led2_state ^= 1u;
    HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin,
                      s_led2_state ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void LedSwich_OnKey3Pressed(void)
{
    s_led3_state ^= 1u;
    HAL_GPIO_WritePin(led3_GPIO_Port, led3_Pin,
                      s_led3_state ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void LedSwich_OnKey4Pressed(void)
{
    /* 切到下一模式（循环） */
    s_mode = (LedMode_t)((s_mode + 1) % LED_MODE_MAX);
    Led_ApplyModeInitial();
}

void LedSwich_Process(void)
{
    uint32_t now = HAL_GetTick();

    switch (s_mode)
    {
        case LED_MODE_WATERFALL:
        case LED_MODE_MARQUEE:
            if ((now - s_last_tick) >= MODE_INTERVAL_MS)
            {
                s_last_tick = now;
                s_step_index = (s_step_index + 1u) % 3u;
                Led_SetAll(GPIO_PIN_SET);
                Led_SetSingle(s_step_index, GPIO_PIN_RESET);
            }
            break;

        case LED_MODE_BLINK:
            if ((now - s_last_tick) >= MODE_INTERVAL_MS)
            {
                s_last_tick = now;
                s_step_index ^= 1u;
                Led_SetAll(s_step_index ? GPIO_PIN_RESET : GPIO_PIN_SET);
            }
            break;

        case LED_MODE_ALL_ON:
        case LED_MODE_ALL_OFF:
        default:
            /* 静态状态，无需推进 */
            break;
    }
}