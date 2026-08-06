#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "esp32.h"
#include "bluetooth.h"
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

void BLE_Init()
{
    printf("======================== BLE INITIALIZING ====================\r\n");

    // 1. 初始化esp32，最多重试3次
    esp32_init();

    // 2. 设置角色为服务端 AT+BLEINIT=2
    BLE_SendCmdAndWait("AT+BLEINIT=2\r\n", "BLEINIT", 3000);

    if (strstr((char*)resp, "ERROR") != NULL)
    {
        printf("BLE INIT FAILED\r\n");
    }

    // 3. 服务端创建服务
    BLE_SendCmdAndWait("AT+BLEGATTSSRVCRE\r\n", "GATTSSRVCRE", 3000);

    if (strstr((char*)resp, "ERROR") != NULL || rlen == 0)
    {
        printf("BLE INIT FAILED\r\n");
    }

    // 4. 服务端开启服务
    BLE_SendCmdAndWait("AT+BLEGATTSSRVSTART\r\n", "GATTSSRVSTART", 3000);

    if (strstr((char*)resp, "ERROR") != NULL || rlen == 0)
    {
        printf("BLE INIT FAILED\r\n");
    }


    // 5. 服务端获取其 MAC 地址
    BLE_SendCmdAndWait("AT+BLEADDR?\r\n", "BLEADDR", 3000);


    if (strstr((char*)resp, "ERROR") != NULL || rlen == 0)
    {
        printf("BLE INIT FAILED\r\n");
    }


    // 6. 服务端设置广播参数 AT+BLEADVPARAM=50,50,0,0,7,0,,
    BLE_SendCmdAndWait("AT+BLEADVPARAM=50,50,0,0,7,0\r\n", "BLEADVPARAM", 3000);


    if (strstr((char*)resp, "ERROR") != NULL || rlen == 0)
    {
        printf("BLE INIT FAILED\r\n");
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


    if (strstr((char*)resp, "ERROR") != NULL || rlen == 0)
    {
        printf("BLE INIT FAILED\r\n");
    }


    // BLE_SendCmdAndWait("AT+BLEADVDATAEX=\"ESP-AT\",\"A002\",\"0102030405\",1", "BLEADVDATAEX", 3000);

    // 8. 服务端开始广播
    BLE_SendCmdAndWait("AT+BLEADVSTART\r\n", "BLEADVSTART", 3000);


    if (!(strstr((char*)resp, "OK") != NULL))
    {
        printf("BLE INIT FAILED\r\n");
    }


    // 9. 配置 SPP 参数 AT+BLESPPCFG=1,1,7,1,5
    BLE_SendCmdAndWait("AT+BLESPPCFG=1,1,7,1,5\r\n", "BLESPPCFG", 3000);

    // 注意: BLE 模式下不要使用 AT+SYSMSG=4
    // 它是为经典蓝牙 SPP 透传设计的，会干扰 BLE 状态上报
    BLE_SendCmdAndWait("AT+SYSMSG=4\r\n", "SYSMSG", 3000);


    if (!(strstr((char*)resp, "OK") != NULL))
    {
        printf("BLE INIT FAILED\r\n");
    }

    printf("======================== BLE INITIALIZED ====================\r\n");
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
