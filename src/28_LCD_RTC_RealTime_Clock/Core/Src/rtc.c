/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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
#include "rtc.h"

/* USER CODE BEGIN 0 */

// RTC 存储 非后两位的年
uint8_t rtc_year_high = 0;

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{
  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 16;
  sTime.Minutes = 27;
  sTime.Seconds = 0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_TUESDAY;
  DateToUpdate.Month = RTC_MONTH_AUGUST;
  DateToUpdate.Date = 11;
  DateToUpdate.Year = 26;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{
  if (rtcHandle->Instance == RTC)
  {
    /* USER CODE BEGIN RTC_MspInit 0 */

    /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
    /* USER CODE BEGIN RTC_MspInit 1 */

    /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{
  if (rtcHandle->Instance == RTC)
  {
    /* USER CODE BEGIN RTC_MspDeInit 0 */

    /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
    /* USER CODE BEGIN RTC_MspDeInit 1 */

    /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void Time_Init(void)
{
  RTC_DateTypeDef c_date = {0};
  /* 读取当前 RTC 日期 */
  HAL_RTC_GetDate(&hrtc, &c_date, RTC_FORMAT_BIN);

  // 对比备份寄存器中的日期与当前日期, 如果不同则更新
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != rtc_year_high ||
    HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2) != c_date.Month ||
    HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3) != c_date.Date ||
    HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4) != c_date.WeekDay ||
    HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5) != c_date.Year)
  {
    // 不同的话, 从 RTC_BKP_DRx 取出, 然后重新赋值

    rtc_year_high = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);

    RTC_DateTypeDef sDate;
    sDate.Year = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
    sDate.Month = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
    sDate.Date = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
    sDate.WeekDay = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN); // 设置年月日星期

  }
}

void Set_Time(int year, int month, int WeekDay, int day, int hour, int minute, int second)
{
  // year 获取 前两位数字
  rtc_year_high = year / 100;


  RTC_TimeTypeDef sTime;
  sTime.Hours = hour;
  sTime.Minutes = minute;
  sTime.Seconds = second;
  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN); // 设置时分秒

  RTC_DateTypeDef sDate;
  sDate.Year = year % 100; // 只存2位, 所以要取100余数
  sDate.Month = month;
  sDate.Date = day;
  sDate.WeekDay = WeekDay;
  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN); // 设置年月日星期
}

void Get_Time(Rtc_Date* rtc_date)
{
  RTC_DateTypeDef c_date = {0};
  HAL_RTC_GetDate(&hrtc, &c_date, RTC_FORMAT_BIN);
  rtc_date->year = c_date.Year + (rtc_year_high * 100);
  rtc_date->month = c_date.Month;
  rtc_date->day = c_date.Date;

  RTC_TimeTypeDef sTime = {0};
  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  rtc_date->hour = sTime.Hours;
  rtc_date->minute = sTime.Minutes;
  rtc_date->second = sTime.Seconds;
}


// 将日期保存在备份寄存器
void Update_Date(void)
{
  RTC_DateTypeDef c_date = {0};
  /* 读取当前 RTC 日期 */
  HAL_RTC_GetDate(&hrtc, &c_date, RTC_FORMAT_BIN);

  /* 把当前日期 (含 RAM 中的年份高两位) 写入备份寄存器, 实现掉电保持 */
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, (uint32_t)rtc_year_high);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, (uint32_t)c_date.Month);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, (uint32_t)c_date.Date);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, (uint32_t)c_date.WeekDay);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint32_t)c_date.Year);
}


/* USER CODE END 1 */

