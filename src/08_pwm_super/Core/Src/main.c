/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  /* 清除 TIM1 更新中断标志，避免上电后立刻误触发一次中断 */
  __HAL_TIM_CLEAR_FLAG(&htim1, TIM_IT_UPDATE);
  /* 使能 TIM1 更新中断（计数器溢出时进入 TIM1_UP_IRQHandler） */
  __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
  /* 启动 TIM1 通道 1 的 PWM 输出，使能 PA8 复用功能 */
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

  /* ⚠️ 注意：若 HAL 库版本较旧（如 STM32CubeF1 v1.7 及更早），
     HAL_TIM_PWM_Start 不会自动使能 TIM1 主输出（MOE 位），
     需要在此处追加一行：__HAL_TIM_MOE_ENABLE(&htim1);
     否则 PA8 引脚将无 PWM 信号输出，表现为 LED 不亮 / 不闪烁。
     新版 HAL 库（v1.8+）内部已自动处理 MOE，可忽略此提示。 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* 主循环：当前为空，所有 PWM 输出由 TIM1 硬件自动完成（PA8 输出固定占空比） */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* 在此可添加用户业务逻辑，例如：
       - 动态修改 __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse) 调节亮度
       - 按键扫描
       - 串口数据处理
       - 配合 DMA 实现呼吸灯效果 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
    | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/* 此处可添加用户自定义函数，例如：
   - 定时器中断回调函数（如 HAL_TIM_PeriodElapsedCallback）
   - USART 接收/发送完成回调
   - 业务逻辑处理函数（呼吸灯算法、按键扫描等） */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* 错误处理入口：当 HAL 外设初始化失败或运行错误时被调用
     用户可在此添加自定义错误上报，例如通过 USART 输出错误码或点亮错误指示灯 */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();   /* 关闭全局中断，避免错误状态下产生意外中断 */
  while (1)
  {
    /* 死循环停留，便于在线调试时观察现场（断点/变量窗口） */
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* 参数断言失败回调：当 assert_param 宏检测到非法参数时被调用
     file：触发断言的源文件名指针；line：触发断言的源文件行号
     建议添加上报，例如：printf("Wrong parameters value: file %s on line %d\r\n", file, line);
     编译时需要开启 USE_FULL_ASSERT 宏（默认在 stm32f1xx_hal_conf.h 中开启） */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line); */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
