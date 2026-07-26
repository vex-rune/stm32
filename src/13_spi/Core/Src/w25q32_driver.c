#include "spi.h"

// 空的数据
const uint8_t null_data = 0xFF;

// 命令 jedec 地址
const uint8_t W25Q32_CMD_JEDEC_ID = 0x9F;

void w25q32_init(void)
{
    spi_start();
}

// 交换读取
uint8_t w25q32_swap_read(uint8_t byte)
{
    uint8_t rByte;
    HAL_SPI_TransmitReceive(&hspi1, &byte, &rByte, 1, 2000);
    return rByte;
}

// 读取ID
void w25q32_read_id(uint8_t* mid, uint16_t* did)
{
    // 1. 发送 jedec 命令
    w25q32_swap_read(W25Q32_CMD_JEDEC_ID);

    // 2. 获取厂商ID, 发送任意数据
    *mid = w25q32_swap_read(null_data);

    // 3. 获取设备ID, 获取两个字节
    *did = 0;
    *did |= w25q32_swap_read(null_data) << 8; // 先获取高8位
    *did |= w25q32_swap_read(null_data) & null_data; // 再获取低8位
}
