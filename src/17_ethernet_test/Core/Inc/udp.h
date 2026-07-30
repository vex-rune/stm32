#ifndef UDP_H_
#define UDP_H_

// 默认使用 Socket 0
#define SN 0
#define SERVER_PORT 8080

#define SOCK_FLAG 0

/// 客户端模式
#define CLIENT_ROLE 0
/// 服务器模式
#define SERVER_ROLE 1

/// 默认为服务器模式
#define ROLE SERVER_ROLE


#include <stdint.h>

/// 启动 UDP 服务器
void UDP_Start(void);

/// 接受客户端连接并接收数据
/// @param buff 接收数据缓冲区,用于存放从客户端读取到的字节
/// @param len  输入:buff 缓冲区容量(字节);输出:实际接收到的数据长度(字节)
int UDP_Server_Accept(uint8_t* srcIp, uint16_t* srcPort, uint8_t buff[], uint16_t* len);

/// 向客户端发送数据
/// @param buff 要发送的数据缓冲区
/// @param len  要发送的数据长度(字节)
int UDP_Send_Message(uint8_t* dstIp, uint16_t* dstPort, uint8_t buff[], uint16_t len);

#endif

