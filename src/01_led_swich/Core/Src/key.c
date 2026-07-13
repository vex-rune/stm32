#include "key.h"

void Key_Init()
{
    // 1. 配置时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    // 2. GPIO 工作模式
    GPIOF->CRH &= ~GPIO_CRH_MODE10;
    GPIOF->CRH |= GPIO_CRH_MODE10_1;
    GPIOF->CRH &= ~GPIO_CRH_CNF10_0;

    GPIOF->ODR &= ~GPIO_ODR_ODR10;

    // 3. AFIO配置引脚复用
    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI10_PF;

    // 4. 配置EXTI
    EXTI->RTSR |= EXTI_RTSR_TR10;
    EXTI->IMR |= EXTI_IMR_MR10;

    // 5. NCIC 配置
    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(EXTI15_10_IRQn, 3);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}


// 中断处理程序
void EXTi15_10_IRQHandler(void)
{
    // 清除中断, 挂起标志位
    EXTI->PR |= EXTI_PR_PR10;

    // 防抖延迟
    HAL_Delay(10);

    // 依然保持高电平, 就反转
    if ((GPIOF->IDR & GPIO_IDR_IDR10) != 0)
    {
        // todo 翻转函数
    }
}
