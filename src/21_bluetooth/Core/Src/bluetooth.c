#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "esp32.h"
#include "bluetooth.h"

#include <stdbool.h>

#include "usart.h"

uint8_t resp[1024] = {0};
uint16_t rlen = 0;

uint16_t len = 0;

/**
 * @brief 发送AT命令并等待响应的辅助函数
 * @param cmd  AT命令字符串
 * @param tag  响应标签（用于打印）
 * @param timeout_ms 超时时间
 */
static void BLE_SendCmdAndWait(const char* cmd, const char* tag, uint32_t timeout_ms)
{
    USART2_ClearBuf();
    esp32_send_cmd(cmd);
    memset(resp, 0, sizeof(resp));
    rlen = 0;
    esp32_read_resp(resp, &rlen, 1024, timeout_ms);
    printf("\r\n--------------------\r\n req:\r\n %s \r\n resp: \n %s \r\n--------------------\r\n",
           cmd, (char*)resp);
}

/**
 * @brief 剔除首尾 \r \n 空格
 * @param buf 输入输出缓冲区，原地修改
 * @return 处理之后有效字符串长度
 */
static int trim_at_response(char* buf)
{
    if (buf == NULL) return 0;

    // 跳过开头空格 \r \n
    char* p = buf;
    while (*p == ' ' || *p == '\r' || *p == '\n')
    {
        p++;
    }

    // 尾部截掉
    int len = (int)strlen(p);
    while (len > 0)
    {
        char ch = p[len - 1];
        if (ch == ' ' || ch == '\r' || ch == '\n')
        {
            len--;
        }
        else
        {
            break;
        }
    }
    p[len] = '\0';
    memmove(buf, p, len + 1);
    return len;
}


/**
 * @brief 判断AT应答是否成功
 * @param resp 串口读到的AT返回原始字符串
 * @return true:成功(有OK，无ERROR，非空) false:失败
 */
bool at_response_ok(char* resp)
{
    if (resp == NULL)
    {
        return false;
    }
    int valid_len = trim_at_response(resp);

    // 除去空字符后长度为0
    if (valid_len == 0)
    {
        return false;
    }

    // 包含ERROR →失败
    if (strstr(resp, "ERROR") != NULL)
    {
        return false;
    }

    // 没有OK →失败
    if (strstr(resp, "OK") == NULL)
    {
        return false;
    }

    return true;
}

bool BLE_Init()
{
    printf("======================== BLE INITIALIZING ====================\r\n");

    // 1. 初始化esp32，最多重试3次
    esp32_init();

    // 2. 设置角色为服务端 AT+BLEINIT=2
    BLE_SendCmdAndWait("AT+BLEINIT=2\r\n", "BLEINIT", 3000);

    if (!at_response_ok((char*)resp))
    {
        printf("设置角色为服务端 FAILED\r\n");
        return false;
    }

    // 3. 服务端创建服务
    BLE_SendCmdAndWait("AT+BLEGATTSSRVCRE\r\n", "GATTSSRVCRE", 3000);

    // 特殊处理 'AT+BLEGATTSSRVCRE' 命令 返回的是 'AT+BLEGATTSSRVCRE'
    if ( strstr((char*)resp, "BLEGATTSSRVCRE") == NULL)
    {
        printf("创建服务失败\r\n");
        return false;
    }

    // 4. 服务端开启服务
    BLE_SendCmdAndWait("AT+BLEGATTSSRVSTART\r\n", "GATTSSRVSTART", 3000);

    if (!at_response_ok((char*)resp))
    {
        printf("开启服务失败\r\n");
        return false;
    }

    // 5. 服务端获取其 MAC 地址
    BLE_SendCmdAndWait("AT+BLEADDR?\r\n", "BLEADDR", 3000);


    if (!at_response_ok((char*)resp))
    {
        printf("获取MAC地址失败\r\n");
        return false;
    }


    // 6. 服务端设置广播参数 AT+BLEADVPARAM=50,50,0,0,7,0,,
    BLE_SendCmdAndWait("AT+BLEADVPARAM=50,50,0,0,7,0\r\n", "BLEADVPARAM", 3000);


    if (!at_response_ok((char*)resp))
    {
        printf("设置广播参数失败\r\n");
        return false;
    }

    // 7. 设置广播数据 AT+BLEADVDATAEX=<dev_name>,<uuid>,<manufacturer_data>,<include_power>
    // 设备名缩短为 VEX_BLE，规避BLE广播31字节payload上限
    char cmd[128];
    snprintf(cmd, sizeof(cmd),
             "AT+BLEADVDATAEX=\"%s\",\"%s\",\"%s\",1\r\n",
             "VEX_BLE", // 设备名缩短，防止广播包溢出
             "A002", // 16bit Service UUID
             "766578" // vex 的16进制 HEX字符串，厂商自定义数据
    );
    BLE_SendCmdAndWait(cmd, "BLEADVDATAEX", 3000);

    if (!at_response_ok((char*)resp))
    {
        printf("设置广播数据失败\r\n");
        return false;
    }


    // BLE_SendCmdAndWait("AT+BLEADVDATAEX=\"ESP-AT\",\"A002\",\"0102030405\",1", "BLEADVDATAEX", 3000);

    // 8. 服务端开始广播
    BLE_SendCmdAndWait("AT+BLEADVSTART\r\n", "BLEADVSTART", 3000);

    if (!at_response_ok((char*)resp))
    {
        printf("广播失败\r\n");
        return false;
    }

    // 9. 配置 SPP 参数 AT+BLESPPCFG=1,1,7,1,5
    BLE_SendCmdAndWait("AT+BLESPPCFG=1,1,7,1,5\r\n", "BLESPPCFG", 3000);

    // 注意: BLE 模式下不要使用 AT+SYSMSG=4
    // 它是为经典蓝牙 SPP 透传设计的，会干扰 BLE 状态上报
    BLE_SendCmdAndWait("AT+SYSMSG=4\r\n", "SYSMSG", 3000);

    if (!at_response_ok((char*)resp))
    {
        printf("配置 SPP 参数失败\r\n");
        return false;
    }

    printf("======================== BLE INITIALIZED ====================\r\n");

    return true;
}

void BLE_SendData(uint8_t data[], uint16_t len)
{
    // 发送数据
    HAL_UART_Transmit(ESP_HUART, data, len, 100);
}

// 判断数据是否为连接状态改变数据
uint8_t BLE_isConnChanged(uint8_t* data)
{
    // 1. 判断包含字符串, 如果是建立链接就进入透传模式;
    if ((strstr((char*)data, "+BLECONN:") != NULL))
    {
        // 有客户端链接, 马上进入 SPP 透传模式
        BLE_SendCmdAndWait("AT+BLESPP\r\n", "BLESPP", 3000);
        return 1;
    }

    // 2. 断开链接, 退出透传模式
    else if (strstr((char*)data, "+BLEDISCONN:") != NULL)
    {
        esp32_send_cmd("+++"); // 退出透传+++不能带回车换行，发+++\r\n是无效的。
        // 等待, 确保 esp32 退出透传模式
        HAL_Delay(2000);
        return 1;
    }

    // 3, 如果是其他类型的数据, 忽略
    else if (strstr((char*)data, "WIFI") != NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void BLE_ReadData(uint8_t data[], uint16_t* rxlen)
{
    esp32_read_resp(data, rxlen, 1024, 1000);

    if (BLE_isConnChanged(data))
    {
        *rxlen = 0; //修复：修改指针指向的值，清零上层长度
    }
}
