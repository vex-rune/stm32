/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{
  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 36;
  hcan.Init.Mode = CAN_MODE_SILENT_LOOPBACK;
  hcan.Init.SyncJumpWidth = CAN_SJW_2TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_3TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_6TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = ENABLE;
  hcan.Init.AutoWakeUp = ENABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = ENABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (canHandle->Instance == CAN1)
  {
    /* USER CODE BEGIN CAN1_MspInit 0 */

    /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN GPIO Configuration
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_CAN1_2();

    /* USER CODE BEGIN CAN1_MspInit 1 */

    /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{
  if (canHandle->Instance == CAN1)
  {
    /* USER CODE BEGIN CAN1_MspDeInit 0 */

    /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8 | GPIO_PIN_9);

    /* USER CODE BEGIN CAN1_MspDeInit 1 */

    /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

// 配置过滤器
void CAN_Filter_Config(void)
{
  CAN_FilterTypeDef def = {0};
  // 绑定FIFO
  def.FilterFIFOAssignment = CAN_RX_FIFO0;
  // 过滤器组
  def.FilterBank = 0;

  // 模式选择: 标识符掩码模式
  def.FilterMode = CAN_FILTERMODE_IDMASK;
  // 一个 32 位过滤器
  def.FilterScale = CAN_FILTERSCALE_32BIT;

  // 指定过滤器标识号 FR1
  def.FilterIdHigh = 0x0000;
  // 指定过滤器标识号
  def.FilterIdLow = 0x0000;

  // 根据模式指定过滤器掩码号或标识号
  def.FilterMaskIdHigh = 0x0000;
  // 根据模式指定过滤器掩码号或标识号
  def.FilterMaskIdLow = 0x0000;

  // 激活
  def.FilterActivation = ENABLE;

  HAL_CAN_ConfigFilter(&hcan, &def);
}

// 发送报文
void CAN_Send_Msg(uint16_t id, uint8_t* data, uint8_t len)
{
  // 1. 检测有空闲邮箱, 直到有空闲为止
  while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0)
  {
  }

  // 2. 配置数据
  CAN_TxHeaderTypeDef TxHeader;
  // 标准标识符
  TxHeader.StdId = id;
  // 扩展标识符 不用
  // TxHeader.ExtId = 0;
  // 数据帧: 数据帧
  TxHeader.RTR = CAN_RTR_DATA;
  // 标准帧: 标准 ID
  TxHeader.IDE = CAN_ID_STD;
  // 数据长度
  TxHeader.DLC = len;
  // 不使用定时器
  TxHeader.TransmitGlobalTime = DISABLE;

  uint32_t TxMailbox;

  // 3. 发送报文
  HAL_CAN_AddTxMessage(&hcan, &TxHeader, data, &TxMailbox);
}

// 接受报文
void CAN_Receive_Msg(uint8_t* RxData)
{
  // 1. 检测是否有数据(检测FIFO0中的报文个数)
  while (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) == 0)
  {
  }

  // 2. 接收

  CAN_RxHeaderTypeDef RxHeader;


  HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxHeader, RxData);
}

/* USER CODE END 1 */

