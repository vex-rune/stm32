/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    m24c02.c
  * @brief   M24C02-WMN6TP I2C EEPROM 驱动实现。
  *          基于 STM32 HAL I2C 层（hi2c2）编写。
  *          使用 HAL_I2C_Mem_Write / HAL_I2C_Mem_Read / HAL_I2C_IsDeviceReady 接口。
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
#include "m24c02.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* HAL I2C 接口要求的设备地址需要左移 1 位（预留 R/W 位）。 */
#define M24C02_HAL_ADDR            ((uint16_t)(M24C02_I2C_ADDR << 1))

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef M24C02_WaitReady(uint32_t timeout_ms);

/* Private user code ---------------------------------------------------------*/

/**
  * @brief  探测 I2C 总线上的 M24C02 设备。
  * @note   多次发送地址应答轮询以确认设备处于工作状态。
  * @retval 设备应答返回 HAL_OK，否则返回 HAL_ERROR / HAL_TIMEOUT / HAL_BUSY。
  */
HAL_StatusTypeDef M24C02_Init(void)
{
    return HAL_I2C_IsDeviceReady(&hi2c2,
                                 M24C02_HAL_ADDR,
                                 3U,
                                 M24C02_IO_TIMEOUT_MS);
}

/**
  * @brief  轮询 M24C02 直到其产生应答（即上一次写操作已完成）。
  * @retval 设备就绪返回 HAL_OK，超时返回 HAL_TIMEOUT。
  */
HAL_StatusTypeDef M24C02_IsReady(void)
{
    return M24C02_WaitReady(M24C02_WRITE_TIMEOUT_MS);
}

/**
  * @brief  向 EEPROM 指定地址写入单个字节。
  * @note   函数会阻塞，直到芯片内部写周期完成。
  * @param  addr : 存储地址（0x00 ~ 0xFF）。
  * @param  data : 待写入的字节。
  * @retval 成功返回 HAL_OK。
  */
HAL_StatusTypeDef M24C02_WriteByte(uint16_t addr, uint8_t data)
{
    HAL_StatusTypeDef status;

    if (addr >= M24C02_CAPACITY)
    {
        return HAL_ERROR;
    }

    status = HAL_I2C_Mem_Write(&hi2c2,
                               M24C02_HAL_ADDR,
                               addr,
                               I2C_MEMADD_SIZE_8BIT,
                               &data,
                               1U,
                               M24C02_IO_TIMEOUT_MS);
    if (status != HAL_OK)
    {
        return status;
    }

    return M24C02_WaitReady(M24C02_WRITE_TIMEOUT_MS);
}

/**
  * @brief  随机读：从指定地址读取一个字节。
  * @param  addr : 存储地址（0x00 ~ 0xFF）。
  * @param  data : 用于接收数据的指针。
  * @retval 成功返回 HAL_OK。
  */
HAL_StatusTypeDef M24C02_ReadByte(uint16_t addr, uint8_t *data)
{
    if ((data == NULL) || (addr >= M24C02_CAPACITY))
    {
        return HAL_ERROR;
    }

    return HAL_I2C_Mem_Read(&hi2c2,
                            M24C02_HAL_ADDR,
                            addr,
                            I2C_MEMADD_SIZE_8BIT,
                            data,
                            1U,
                            M24C02_IO_TIMEOUT_MS);
}

/**
  * @brief  读取内部地址计数器当前指向的字节。
  * @param  data : 用于接收数据的指针。
  * @retval 成功返回 HAL_OK。
  */
HAL_StatusTypeDef M24C02_ReadCurrent(uint8_t *data)
{
    if (data == NULL)
    {
        return HAL_ERROR;
    }

    return HAL_I2C_Master_Receive(&hi2c2,
                                  M24C02_HAL_ADDR,
                                  data,
                                  1U,
                                  M24C02_IO_TIMEOUT_MS);
}

/**
  * @brief  页写入：从指定地址起最多写入一页（16 字节）。
  * @note   写入操作严禁跨页，否则会回卷覆盖并静默破坏存储内容（数据手册要求）。
  * @param  addr : 起始存储地址。
  * @param  buf  : 待写入数据的指针。
  * @param  len  : 写入字节数（1 ~ M24C02_PAGE_SIZE）。
  * @retval 成功返回 HAL_OK。
  */
HAL_StatusTypeDef M24C02_WritePage(uint16_t addr, const uint8_t *buf, uint16_t len)
{
    HAL_StatusTypeDef status;

    if ((buf == NULL) || (len == 0U) || (len > M24C02_PAGE_SIZE))
    {
        return HAL_ERROR;
    }
    if (((uint32_t)addr + len) > M24C02_CAPACITY)
    {
        return HAL_ERROR;
    }
    /* 拒绝跨 16 字节页边界的写入 */
    if (((addr % M24C02_PAGE_SIZE) + len) > M24C02_PAGE_SIZE)
    {
        return HAL_ERROR;
    }

    status = HAL_I2C_Mem_Write(&hi2c2,
                               M24C02_HAL_ADDR,
                               addr,
                               I2C_MEMADD_SIZE_8BIT,
                               (uint8_t *)buf,
                               len,
                               M24C02_IO_TIMEOUT_MS);
    if (status != HAL_OK)
    {
        return status;
    }

    return M24C02_WaitReady(M24C02_WRITE_TIMEOUT_MS);
}

/**
  * @brief  顺序读：从指定地址起连续读取 @p len 个字节。
  * @param  addr : 起始存储地址。
  * @param  buf  : 接收缓冲区指针。
  * @param  len  : 读取字节数（1 ~ M24C02_CAPACITY）。
  * @retval 成功返回 HAL_OK。
  */
HAL_StatusTypeDef M24C02_ReadBuffer(uint16_t addr, uint8_t *buf, uint16_t len)
{
    if ((buf == NULL) || (len == 0U))
    {
        return HAL_ERROR;
    }
    if (((uint32_t)addr + len) > M24C02_CAPACITY)
    {
        return HAL_ERROR;
    }

    return HAL_I2C_Mem_Read(&hi2c2,
                            M24C02_HAL_ADDR,
                            addr,
                            I2C_MEMADD_SIZE_8BIT,
                            buf,
                            len,
                            M24C02_IO_TIMEOUT_MS);
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  反复轮询设备，直到其产生 ACK 或超时。
  * @param  timeout_ms : 总超时时间，单位毫秒。
  * @retval 设备就绪返回 HAL_OK，超时返回 HAL_TIMEOUT。
  */
static HAL_StatusTypeDef M24C02_WaitReady(uint32_t timeout_ms)
{
    uint32_t start_tick = HAL_GetTick();
    HAL_StatusTypeDef status;

    do
    {
        status = HAL_I2C_IsDeviceReady(&hi2c2,
                                       M24C02_HAL_ADDR,
                                       1U,
                                       M24C02_WRITE_CYCLE_MS);
        if (status == HAL_OK)
        {
            return HAL_OK;
        }
    } while ((HAL_GetTick() - start_tick) < timeout_ms);

    return HAL_TIMEOUT;
}
