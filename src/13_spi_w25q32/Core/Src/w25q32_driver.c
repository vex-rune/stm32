#include <stdio.h>
#include <sys/types.h>

#include "spi.h"
#include "w25q32_driver.h"


// 空的数据
const uint8_t null_data = 0xFF;

const uint8_t W25Q32_CMD_JEDEC_ID = 0x9F;
const uint8_t W25Q32_CMD_WRITE_ENABLE = 0x06;
const uint8_t W25Q32_CMD_WRITE_DISABLE = 0x04;
const uint8_t W25Q32_CMD_STATUS_REG = 0x05;
const uint8_t W25Q32_CMD_SECTOR_ERASE = 0x20;
const uint8_t W25Q32_CMD_PAGE_PROGRAM = 0x02;
const uint8_t W25Q32_CMD_READ_DATA = 0x03;

void w25q32_start(void)
{
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
}

void w25q32_stop(void)
{
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

// 交换读取
uint8_t w25q32_swap(uint8_t byte)
{
    uint8_t rByte;
    HAL_SPI_TransmitReceive(&hspi1, &byte, &rByte, 1, 2000);
    return rByte;
}

// 读取ID
void w25q32_read_id(uint8_t* mid, uint16_t* did)
{
    w25q32_start();
    // 1. 发送 jedec 命令
    w25q32_swap(W25Q32_CMD_JEDEC_ID);

    // 2. 获取厂商ID, 发送任意数据
    *mid = w25q32_swap(null_data);

    // 3. 获取设备ID, 获取两个字节
    *did = 0;
    *did = (uint16_t)w25q32_swap(null_data) << 8; // 高8位
    *did |= w25q32_swap(null_data); // 低8位

    w25q32_stop();
}

// 写使能
// 注意: 本函数不操作 CS! 调用前必须保证 CS 已经是低电平
// 因为写使能命令需要和后续的擦除/写入命令保持在同一个 CS 拉低周期内
void w25q32_write_enable(void)
{
    w25q32_start();
    w25q32_swap(W25Q32_CMD_WRITE_ENABLE);
    w25q32_stop();
}


// 关闭写使能
// 注意: 本函数不操作 CS! 调用前后由调用者控制 CS
void w25q32_write_disable(void)
{
    w25q32_start();
    w25q32_swap(W25Q32_CMD_WRITE_DISABLE);
    w25q32_stop();
}


// 等待状态不为忙
void w25q32_wait_not_busy(void)
{
    w25q32_start();
    // 1. 获取状态寄存器
    w25q32_swap(W25Q32_CMD_STATUS_REG);
    // 2. 获取状态寄存器
    while ((w25q32_swap(null_data) & 0x01) == 0x01);
    // 3. 停止
    w25q32_stop();
}


// 扇区擦除（4KB）
void w25q32_erase_sector(const uint8_t block, const uint8_t sector)
{
    // 1. 等待 不忙
    w25q32_wait_not_busy();

    // 2. 重新拉低 CS, 进行写使能
    // (wait_not_busy 内部最后 stop, 现在 CS 是高的, 需要重新拉低)
    w25q32_write_enable(); // CS 保持低, 写使能命令成功

    // 3. 计算地址
    // ------------------------------------------------------------------
    // W25Q32 24 位地址位分配 (bit 23 ~ bit 0):
    // ┌────────────┬────────────┬────────────┬────────────┐
    // │  bit23-18  │  bit17-12  │  bit11-8   │   bit7-0   │
    // ├────────────┼────────────┼────────────┼────────────┤
    // │   Block    │   Sector   │    Page    │    Byte    │
    // │    6 位    │    4 位    │    4 位    │    8 位    │
    // │   64 个    │   16 个    │   16 个    │   256 个   │
    // └────────────┴────────────┴────────────┴────────────┘
    //
    // (block << 16)  : 将块号 (0~63)  左移 16 位，放到地址 bit16-21
    // (sector << 12) : 将扇区号 (0~15) 左移 12 位，放到地址 bit12-15
    // | (按位或)     : 把两个字段拼接起来，构成 24 位地址的高 12 位
    // 低 12 位 (page + byte) 由后续 page 变量和 byte 直接补齐
    //
    // 示例: block=2, sector=5
    //   block << 16  = 0x02 << 16 = 0x020000
    //                  0000 0010 0000 0000 0000 0000
    //   sector << 12 = 0x05 << 12 = 0x000050
    //                  0000 0000 0000 0000 0101 0000
    //   按位或结果   = 0x020500
    //                  0000 0010 0000 0101 0000 0000
    //                   └──┬──┘ └──┬──┘ └────┬────┘
    //                     Block  Sector  Page+Byte (后续补充)
    // ------------------------------------------------------------------
    uint32_t addr = (block << 16) | (sector << 12);

    // 4. 擦除
    w25q32_swap(W25Q32_CMD_SECTOR_ERASE);

    // 5. 发送地址
    // ------------------------------------------------------------------
    // W25Q32 地址协议要求从机按 big-endian 顺序接收 3 字节地址:
    //   第 1 字节 (高) : addr[23:16]  →  Block 字段
    //   第 2 字节 (中) : addr[15:8]   →  Sector + Page 高位
    //   第 3 字节 (低) : addr[7:0]    →  Page 低位 + Byte
    //
    // (addr >> 16)          : 把高 8 位移到低 8 位 (得到地址高字节)
    // (addr >> 8)           : 把中 8 位移到低 8 位 (得到地址中字节)
    // (addr >> 0) = addr    : 直接使用低 8 位      (得到地址低字节)
    // & null_data (0xFF)    : 按位与 0xFF，截断到单字节
    //                         (uint32_t -> uint8_t 必须屏蔽高 24 位)
    //                         注意: 必须用 & (按位与), 不能用 && (逻辑与)!
    //                         否则结果只能是 0 或 1 (布尔值)
    // ------------------------------------------------------------------
    w25q32_swap((addr >> 16) & null_data); // 发送地址高字节 (Block)
    w25q32_swap((addr >> 8) & null_data); // 发送地址中字节 (Sector+Page)
    w25q32_swap((addr >> 0) & null_data); // 发送地址低字节 (Page+Byte)

    // 释放 CS, 擦除命令已发送, Flash 开始内部擦除

    w25q32_start();
    w25q32_swap(W25Q32_CMD_SECTOR_ERASE);
    w25q32_swap((addr >> 16) & 0xFF);
    w25q32_swap((addr >> 8) & 0xFF);
    w25q32_swap(addr & 0xFF);
    w25q32_stop();

    w25q32_wait_not_busy();
}

// 页写入

// 写入
// data: 要写入的字节数组
// len:  要写入的字节数 (≤ 256)
// W25Q32 页编程要求发送完整 256 字节, 不足部分以 0xFF 填充
void w25q32_write(
    const uint8_t block,
    const uint8_t sector,
    const uint8_t page,
    const uint8_t* data,
    const uint16_t len)
{
    // 等待 不忙
    w25q32_wait_not_busy();

    // 拉低 CS, 进行写使能 (CS 保持低, 后续命令才能继续发送)
    w25q32_write_enable();

    // 计算地址
    uint32_t addr = (block << 16) | (sector << 12) | (page << 8);

    // 发送页写入命令

    w25q32_start();
    w25q32_swap(W25Q32_CMD_PAGE_PROGRAM);
    w25q32_swap((addr >> 16) & 0xFF);
    w25q32_swap((addr >> 8) & 0xFF);
    w25q32_swap(addr & 0xFF);

    // 发送数据 - 完整 256 字节, 不足部分填 0xFF
    // 【修正：循环变量i改为uint16_t，与len类型匹配，避免类型不一致警告】
    for (uint16_t i = 0; i < len; i++)
    {
        w25q32_swap(data[i]);
    }

    // 释放 CS, 页写入命令已发送, Flash 开始内部编程
    w25q32_stop();

    // 等待内部编程完成 (BUSY=0)
    w25q32_wait_not_busy();
}

// 读取
uint8_t w25q32_read(
    const uint8_t block,
    const uint8_t sector,
    const uint8_t page,
    const uint8_t innerAddr,
    uint8_t* data,
    const int len
)
{
    // 先等待上一次操作完成
    w25q32_wait_not_busy();


    // 计算地址 + 页内地址
    uint32_t addr = ((uint32_t)block << 16)
        | ((uint32_t)sector << 12)
        | ((uint32_t)page << 8)
        | innerAddr;


    w25q32_start();
    w25q32_swap(W25Q32_CMD_READ_DATA);
    w25q32_swap((addr >> 16) & 0xFF);
    w25q32_swap((addr >> 8) & 0xFF);
    w25q32_swap(addr & 0xFF);


    // 接受数据
    for (uint16_t i = 0; i < len; i++)
    {
        data[i] = w25q32_swap(null_data);
    }
    w25q32_stop();

    return 0;
}
