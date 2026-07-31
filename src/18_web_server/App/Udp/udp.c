#include "udp.h"

#include <stdio.h>

#include "../../Drivers/Ethernet/socket.h"
#include "../../Drivers/Ethernet/w5500.h"

/// 启动 服务器
void UDP_Start(void)
{
    // printf("UDP_Server_Start\n");
    // 获取状态
    uint8_t state = getSn_SR(SN);

    // printf("\tstate: %d\n", state);
    // 判断状态
    if (state == SOCK_CLOSED)
    {
        // 创建 Socket, sn = port 则表示成功后
        int8_t sn = socket(SN, Sn_MR_UDP, 8081, 0);

        if (((int)sn) == SN)
        {
            printf("\tUDP_OPEN\n");
        }
        else
        {
            printf("\tUDP_ERROR sn = %d\n", sn);
        }
    }
}


/// 接受客户端连接并接收数据
/// @param buff 接收数据缓冲区,用于存放从客户端读取到的字节
/// @param len  输入:buff 缓冲区容量(字节);输出:实际接收到的数据长度(字节)
int UDP_Server_Accept(uint8_t* srcIp, uint16_t* srcPort, uint8_t buff[], uint16_t* len)
{
    // 获取状态
    uint8_t state = getSn_SR(SN);

    // printf("\tstate: %d\n", state);

    // 如果是建立连接状态
    if (state == SOCK_UDP)
    {
        // 根据标志位判断是否接收到数据
        // 获取SN寄存器值 对比 Sn_IR_RECV 接收中断 当从对等方接收到数据时发出。
        if (getSn_IR(SN) & Sn_IR_RECV)
        {
            printf("UDP_Server_Accept\n");

            // 清零标志位 (写1清0)

            printf("\t清零标志位\n");
            setSn_IR(SN, Sn_IR_RECV);

            printf("\t接收数据\n");

            // 获取长度, 当前长度包含了8字节的首部
            uint16_t tmp_len = getSn_RX_RSR(SN);
            // 判断如果 > 8, 表示接收到数据
            if (tmp_len <= 8)
            {
                return 0;
            }

            *len = tmp_len - 8;

            printf("\t接收长度: %d\n", *len);

            // 接收数据
            recvfrom_W5x00(SN, buff, *len, srcIp, srcPort);

            // 手动加结束符,避免打印越界
            if (*len > 0 && *len < 0xFFFF)
            {
                buff[*len] = '\0';
            }

            printf("\t接收数据(%u 字节): %s\r\n", (unsigned)*len, (char*)buff);

            return 0;
        }
    }

    return 1;
}

/// 向客户端发送数据
/// @param buff 要发送的数据缓冲区
/// @param len  要发送的数据长度(字节)
int UDP_Send_Message(uint8_t* dstIp, uint16_t* dstPort, uint8_t buff[], uint16_t len)
{
    printf("UDP_Send_Message\n");
    // 获取状态
    uint8_t state = getSn_SR(SN);

    printf("\tstate: %d\n", state);

    // 如果是建立连接状态
    if (state == SOCK_UDP)
    {
        printf("\t已连接\n");
        if (len > 0 && buff[len - 1] != '\n')
        {
            buff[len] = '\n';
            len++;
        }
        sendto_W5x00(SN, buff, len, dstIp, *dstPort);
        printf("\t发送数据 %u 字节: %s\r\n", (unsigned)len, (char*)buff);
        return 0;
    }

    return 1;
}

