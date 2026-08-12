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


#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>

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

// 把 "Aug 11 2026" / "14:35:22" 拆成数字
static uint8_t compile_month(char* m)
{
  const char* names[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  for (int i = 0; i < 12; i++)
    if (strncmp(m, names[i], 3) == 0) return i + 1;
  return 1;
}

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
  MX_RTC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  // 1. 这些标志是"残留状态"
  // 当 MCU 从 STOP / Standby 模式被 RTC 闹钟唤醒后:
  // SB 被置1 → 告诉系统"我是从 Standby 醒来的"
  // WU 被置 1 → 告诉系统"发生过一次唤醒"
  // 这些标志一旦置位,就不会自动清零。下次上电/复位前会一直保留。
  // 2. 不清除会带来两个问题
  // ❶ 无法判断"本次唤醒"的来源
  // 如果不先清除,上次唤醒留下的 WU=1 会污染本次判断。
  // ❷ 干扰下一次 STOP / Standby 进入
  // 进入低功耗模式前,某些 HAL 流程会读取这些标志位作为状态机依据。残留标志可能导致:
  // HAL_PWR_EnterSTOPMode() 内部状态判断异常
  // 下次唤醒后 __HAL_PWR_GET_FLAG() 返回错误结果

  printf("清除PWR标志位\r\n");

  // 清除PWR标志位, 如果存在的话
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB))
  {
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  }

  if (__HAL_PWR_GET_FLAG(PWR_FLAG_WU))
  {
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  }

  // led 闪烁 10秒
  for (int i = 0; i < 50; i++)
  {
    // 点亮蓝色LED
    HAL_GPIO_TogglePin(led_blue_GPIO_Port, led_blue_Pin);

    // 等待1秒
    HAL_Delay(200);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // 点亮蓝色LED
    HAL_GPIO_WritePin(led_blue_GPIO_Port, led_blue_Pin, GPIO_PIN_RESET);

    // 等待3秒
    HAL_Delay(3000);


    // 获取日期
    RTC_DateTypeDef sDate = {0};
    if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) Error_Handler();

    printf("当前日期: %04d-%02d-%02d\r\n", sDate.Year, sDate.Month, sDate.Date);


    // 得到当前时间
    RTC_TimeTypeDef sTime = {0};
    if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) Error_Handler();

    printf("当前时间: %02d:%02d:%02d\r\n", sTime.Hours, sTime.Minutes, sTime.Seconds);

    RTC_AlarmTypeDef sAlarm = {0};
    sAlarm.AlarmTime.Hours = sTime.Hours;
    sAlarm.AlarmTime.Minutes = sTime.Minutes;
    sAlarm.AlarmTime.Seconds = sTime.Seconds + 10;
    sAlarm.Alarm = RTC_ALARM_A;
    // 设置RTC闹钟
    if (HAL_RTC_SetAlarm(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK) Error_Handler();

    // 设置完成
    printf("闹钟设置: %02d:%02d:%02d\r\n", sAlarm.AlarmTime.Hours, sAlarm.AlarmTime.Minutes, sAlarm.AlarmTime.Seconds);

    // 进入待机模式
    HAL_PWR_EnterSTANDBYMode();

    ////////////////////////////////////

    // 唤醒以后, led 亮
    HAL_GPIO_WritePin(led_blue_GPIO_Port, led_blue_Pin, GPIO_PIN_RESET);
    printf("待机模式结束\r\n");

    // 持续5秒(时间短了, 待机模式烧写有问题, 时间长点, 确保观察到效果, 切保证可以烧写)
    HAL_Delay(5000);

    printf("完成工作进入下一轮\r\n");


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
    | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  printf("Error_Handler\r\n");
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
