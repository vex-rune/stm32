#include "eth.h"

#include <stdint.h>
#include <stdio.h>

#include "main.h"
#include "stm32f1xx_hal_spi.h"
#include "../../Drivers/Ethernet/w5500.h"

// IP 地址
uint8_t ip[4] = {192, 168, 31, 11};

// MAC 地址
uint8_t mac[6] = {0x94, 0xB6, 0x09, 0x4C, 0xC4, 0x11};

// 子网掩码
uint8_t sub_mask[4] = {255, 255, 255, 0};

// 网关
uint8_t gateway[4] = {192, 168, 31, 1};


void ETH_Reset(void)
{
    // 复位 W5500: CS 拉低 300ms 后拉高
    printf("[ETH] reset W5500 ...\r\n");
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_Delay(300);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

    // 读取版本寄存器: 通信正常时应为 0x04
    uint8_t ver = getVERSIONR();
    printf("[ETH] W5500 reset done (300ms), VERSIONR=0x%02X\r\n", ver);
}

void ETH_Set_Ip(void)
{
    setSIPR(ip);
    printf("[ETH] IP      : %d.%d.%d.%d  OK\r\n", ip[0], ip[1], ip[2], ip[3]);

    setSUBR(sub_mask);
    printf("[ETH] Mask    : %d.%d.%d.%d  OK\r\n", sub_mask[0], sub_mask[1], sub_mask[2], sub_mask[3]);

    setGAR(gateway);
    printf("[ETH] Gateway : %d.%d.%d.%d  OK\r\n", gateway[0], gateway[1], gateway[2], gateway[3]);
}

void ETH_Set_Mac(void)
{
    setSHAR(mac);
    printf("[ETH] MAC     : %02X:%02X:%02X:%02X:%02X:%02X  OK\r\n",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

// 打印芯片版本与 PHY 链路状态
static void ETH_PrintChipInfo(void)
{
    uint8_t ver  = getVERSIONR();
    uint8_t phy  = getPHYCFGR();
    uint8_t link = (phy & PHYCFGR_LNK_ON)   ? 1 : 0;
    uint8_t spd  = (phy & PHYCFGR_SPD_100)  ? 100 : 10;
    uint8_t dpx  = (phy & PHYCFGR_DPX_FULL) ? 1 : 0;

    printf("[ETH] Chip ver    : 0x%02X (expect 0x04)\r\n", ver);
    printf("[ETH] PHY link    : %s\r\n",        link ? "UP"   : "DOWN");
    printf("[ETH] PHY speed   : %u Mbps\r\n", (unsigned)spd);
    printf("[ETH] PHY duplex  : %s\r\n",        dpx  ? "FULL" : "HALF");
}

// 打印 8 个 Socket 的 TX/RX 缓冲区配置 (单位: KB)
static void ETH_PrintSocketBuf(void)
{
    printf("[ETH] Socket buffer (KB):\r\n");
    for (uint8_t i = 0; i < 8; i++)
    {
        // 寄存器值含义: 0=1KB 1=2KB 2=4KB 3=8KB 4=16KB
        uint8_t tx = getSn_TXBUF_SIZE(i);
        uint8_t rx = getSn_RXBUF_SIZE(i);
        printf("[ETH]   Sn%u: TX=%uKB RX=%uKB\r\n",
               (unsigned)i,
               (unsigned)(1u << tx),
               (unsigned)(1u << rx));
    }
}


void ETH_Init(void)
{
    // 复位 W5500
    ETH_Reset();

    // 设置 MAC
    ETH_Set_Mac();

    // 设置 IP / 子网掩码 / 网关
    ETH_Set_Ip();

    // 打印芯片与 PHY 状态
    ETH_PrintChipInfo();

    // 打印 Socket 缓冲区配置
    ETH_PrintSocketBuf();

    printf("[ETH] init done\r\n");
}