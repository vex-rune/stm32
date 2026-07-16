/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 主程序入口
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * 保留所有权利。
  *
  * 本软件遵循可在软件根目录的 LICENSE 文件中找到的条款进行授权。
  * 若软件未随附 LICENSE 文件，则按“原样”提供。
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "m24c02.h"
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
  * @brief  应用程序入口。
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU 配置--------------------------------------------------------*/

  /* 复位所有外设，初始化 Flash 接口与 Systick。 */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* 配置系统时钟 */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* 初始化所有已配置的外设 */
  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  {
      /* ---- M24C02 示例：写入一页并读回 ---- */
      uint8_t tx_buf[M24C02_PAGE_SIZE];
      uint8_t rx_buf[M24C02_PAGE_SIZE];
      uint16_t test_addr = 0x10U;
      HAL_StatusTypeDef st;
      uint32_t i;

      /* 准备测试数据 */
      for (i = 0; i < M24C02_PAGE_SIZE; i++)
      {
          tx_buf[i] = (uint8_t)(0xA0U + i);
      }

      /* 首先探测设备 */
      st = M24C02_Init();
      printf("M24C02_Init   -> %s\r\n",
             (st == HAL_OK) ? "OK"   :
             (st == HAL_TIMEOUT) ? "TIMEOUT" : "ERROR");

      /* 单字节写入示例 */
      st = M24C02_WriteByte(0x00U, 0x5AU);
      printf("WriteByte 0x00=0x5A -> %s\r\n",
             (st == HAL_OK) ? "OK" : "FAIL");

      /* 单字节读取示例 */
      {
          uint8_t v = 0;
          st = M24C02_ReadByte(0x00U, &v);
          printf("ReadByte  0x00   = 0x%02X (%s)\r\n", v,
                 (st == HAL_OK) ? "OK" : "FAIL");
      }

      /* 页写入示例 */
      st = M24C02_WritePage(test_addr, tx_buf, M24C02_PAGE_SIZE);
      printf("WritePage 0x%02X, %u bytes -> %s\r\n",
             test_addr, M24C02_PAGE_SIZE,
             (st == HAL_OK) ? "OK" : "FAIL");

      /* 顺序读取示例 */
      st = M24C02_ReadBuffer(test_addr, rx_buf, M24C02_PAGE_SIZE);
      printf("ReadBuffer 0x%02X, %u bytes -> %s\r\n",
             test_addr, M24C02_PAGE_SIZE,
             (st == HAL_OK) ? "OK" : "FAIL");

      /* 打印读回的字节 */
      printf("RX: ");
      for (i = 0; i < M24C02_PAGE_SIZE; i++)
      {
          printf("%02X ", rx_buf[i]);
      }
      printf("\r\n");
  }
  /* USER CODE END 2 */

  /* 无限循环 */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief 系统时钟配置
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** 根据 RCC_OscInitTypeDef 结构体中的指定参数初始化 RCC 振荡器。
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** 初始化 CPU、AHB 和 APB 总线时钟。
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
  * @brief  将 printf() / puts() 重定向到 UART1（由 syscalls.c 调用）。
  * @param  ch : 待发送的字符。
  * @retval 成功时返回相同字符。
  */
int __io_putchar(int ch)
{
    uint8_t c = (uint8_t)ch;
    HAL_UART_Transmit(&huart1, &c, 1U, M24C02_IO_TIMEOUT_MS);
    return ch;
}

/* USER CODE END 4 */

/**
  * @brief  当发生错误时执行该函数。
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* 用户可在此添加自己的实现，用于上报 HAL 错误返回状态 */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  上报 assert_param 错误所在的源文件名称与行号。
  * @param  file: 源文件名称指针
  * @param  line: assert_param 错误所在行号
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* 用户可在此添加自己的实现，用于上报文件名与行号，
     例如：printf("参数错误：文件 %s 第 %d 行\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
