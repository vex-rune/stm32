#include "eth.h"

#include <stdint.h>
#include <stdio.h>

#include "main.h"
#include "stm32f1xx_hal_spi.h"
#include "../../Drivers/Ethernet/w5500.h"

// ip
uint8_t ip[4] = {192, 168, 31, 11};

// mac
uint8_t mac[6] = {0x94, 0xB6, 0x09, 0x4C, 0xC4, 0x11};

// 掩码
uint8_t sub_mask[4] = {255, 255, 255, 0};

// 网关
uint8_t gateway[4] = {192, 168, 31, 1};


void ETH_Reset(void)
{
    // 时钟
    // 工作模式: 推完模式
    // main 已经有了 MX_GPIO_Init

    // 复位: 拉低 300 ms, 恢复拉高
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);

    HAL_Delay(300);

    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

    printf("eth 复位完成的\n");
}

void ETH_Set_Ip(void)
{
    // 设置
    printf("eht 开始设置 ip\n");
    setSIPR(ip);
    printf("eht 设置完成 ip\n");

    printf("eht 开始设置 sub_mask\n");
    setSUBR(sub_mask);
    printf("eht 设置完成 sub_mask\n");

    printf("eht 开始设置 gateway\n");
    setGAR(gateway);
    printf("eht 设置完成 gateway\n");
}

void ETH_Set_Mac(void)
{
    // 设置
    printf("eht 开始设置 mac\n");
    setSHAR(mac);
    printf("eht 设置完成 mac\n");
}


void ETH_Init(void)
{
    // 1. 注册自定义回调函数
    // 先省略

    // 2. 复位 w5500
    ETH_Reset();

    // 3. 设置mac
    ETH_Set_Mac();

    // 4. 设置IP 网关 子网掩码
    ETH_Set_Ip();
}
