/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    m24c02.h
  * @brief   M24C02-WMN6TP I2C EEPROM driver header file.
  *          Device : ST M24C02 (2Kbit / 256 Bytes, page = 16 Bytes).
  *          Bus    : I2C2 (PB10 = SCL, PB11 = SDA).
  *          A2/A1/A0 pins are tied to GND on the board,
  *          therefore the 7-bit slave address is 0x50.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __M24C02_H__
#define __M24C02_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"

/* Exported constants --------------------------------------------------------*/
/* M24C02 7-bit slave address (A2/A1/A0 all tied to GND) */
#define M24C02_I2C_ADDR            (0x50U)

/* Total capacity: 2 Kbit = 256 Bytes */
#define M24C02_CAPACITY            (256U)

/* Page size in bytes (M24C02 supports 16-byte page writes) */
#define M24C02_PAGE_SIZE           (16U)

/* Maximum internal write cycle time (datasheet: 5 ms typical) */
#define M24C02_WRITE_CYCLE_MS      (10U)

/* Overall timeout used when polling for write completion */
#define M24C02_WRITE_TIMEOUT_MS    (100U)

/* Generic I2C transaction timeout */
#define M24C02_IO_TIMEOUT_MS       (100U)

/* Exported types ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
HAL_StatusTypeDef M24C02_Init(void);
HAL_StatusTypeDef M24C02_IsReady(void);

HAL_StatusTypeDef M24C02_WriteByte(uint16_t addr, uint8_t data);
HAL_StatusTypeDef M24C02_ReadByte(uint16_t addr, uint8_t *data);
HAL_StatusTypeDef M24C02_ReadCurrent(uint8_t *data);

HAL_StatusTypeDef M24C02_WritePage(uint16_t addr, const uint8_t *buf, uint16_t len);
HAL_StatusTypeDef M24C02_ReadBuffer(uint16_t addr, uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __M24C02_H__ */
