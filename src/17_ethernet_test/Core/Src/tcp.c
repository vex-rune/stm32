#include "../Inc/tcp.h"

#include <stdio.h>

#include "../../Drivers/Ethernet/socket.h"
#include "../../Drivers/Ethernet/w5500.h"


/// 启动 TCP 服务器
void TCP_Server_Start(void)
{
    // printf("TCP_Server_Start\n");
    // 获取状态
    uint8_t state = getSn_SR(SN);

    // printf("\tstate: %d\n", state);
    // 判断状态
    if (state == SOCK_CLOSED)
    {
        printf("\t打开socket\n");
        // 创建 Socket, sn = port 则表示成功后
        int8_t sn = socket(SN, Sn_MR_TCP, PORT, 0);

        if (sn == PORT)
        {
            printf("\tSOCK_OPEN\n");
        }
        else
        {
            printf("\tSOCK_ERROR sn = %d\n", sn);
        }
    }
    else if (state == SOCK_INIT)
    {
        printf("\t监听socket\n");
        int8_t res = listen(SN);

        if (res == SOCK_OK)
        {
            printf("\tSOCK_LISTEN\n");
        }
        else
        {
            printf("\tSOCK_ERROR res = %d\n", res);
        }
    }
    else if (state == SOCK_LISTEN)
    {
        // printf("\t等待连接\n");
    }
    else if (state == SOCK_ESTABLISHED)
    {
        // printf("\t已连接\n");
    }
    else if (state == SOCK_CLOSE_WAIT)
    {
        printf("\t等待关闭\n");
        // 直接关闭
        close(SN);
    }
}

/// 接受客户端连接并接收数据
/// @param buff 接收数据缓冲区,用于存放从客户端读取到的字节
/// @param len  输入:buff 缓冲区容量(字节);输出:实际接收到的数据长度(字节)
int TCP_Server_Accept(uint8_t buff[], uint16_t* len)
{
    // 获取状态
    uint8_t state = getSn_SR(SN);

    // printf("\tstate: %d\n", state);

    // 如果是建立连接状态
    if (state == SOCK_ESTABLISHED)
    {
        // 根据标志位判断是否接收到数据
        // 获取SN寄存器值 对比 Sn_IR_RECV 接收中断 当从对等方接收到数据时发出。
        if (getSn_IR(SN) & Sn_IR_RECV)
        {
            printf("TCP_Server_Accept\n");

            // 获取对方的ip 端口号

            uint8_t dip[4] = {0};
            getSn_DIPR(SN, dip);
            getSn_DPORT(SN);

            printf("\t已接收数据 %d.%d.%d.%d:%d\n",
                   dip[0], dip[1], dip[2], dip[3],
                   getSn_DPORT(SN)
            );
            // 清零标志位 (写1清0)

            printf("\t清零标志位\n");
            setSn_IR(SN, Sn_IR_RECV);

            printf("\t接收数据\n");
            // 获取长度
            *len = getSn_RX_RSR(SN);
            printf("\t接收长度: %d\n", *len);

            // 接收数据
            recv(SN, buff, *len);

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
int TCP_Server_Send(uint8_t buff[], uint16_t len)
{
    printf("TCP_Server_Send\n");
    // 获取状态
    uint8_t state = getSn_SR(SN);

    printf("\tstate: %d\n", state);

    // 如果是建立连接状态
    if (state == SOCK_ESTABLISHED)
    {
        printf("\t已连接\n");
        if (len > 0 && buff[len - 1] != '\n')
        {
            buff[len] = '\n';
            len++;
        }
        send(SN, buff, len);
        printf("\t发送数据\n");
        return 0;
    }

    return 1;
}

