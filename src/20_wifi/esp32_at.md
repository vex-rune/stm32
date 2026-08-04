# ESP32-C3 AT 固件烧写指南

## 接线

```
ESP32-C3          USB-TTL
GPIO6 (RX)   <-   TX
GPIO7 (TX)   ->   RX
GND          <->   GND
3.3V         <->   3.3V
EN           <->   3.3V (使能)
```

**烧录模式**: GPIO9 接地 (按 BOOT 键或短接 GPIO9-GND)

## 烧写步骤

1. **GPIO9 接地** 进入下载模式
2. **打开 flash_download_tool**，选择 ESP32-C3
3. **加载固件**: `factory_MINI-1.bin` -> 地址 `0x0`
4. **配置**: 115200, 8N1
5. **点击 START**，开始烧写
6. **烧写完成后**，断开 GPIO9，重新上电

## 验证

串口助手 115200 发送:
```
AT
```
返回:
```
OK
```

## 常见问题

| 现象 | 原因 | 解决 |
|-----|------|------|
| 连接失败 | GPIO9 未接地 | 按住 BOOT 键再点 START |
| 无响应 | EN 未接 3.3V | 检查 EN 引脚电压 |
| 乱码 | 波特率错误 | 确认 115200 |
| 烧写失败 | 供电不足 | 换 USB 口或外接电源 |

## STA模式 TCP服务器（接入现有WiFi）

连接现有WiFi，同网段设备可直接访问。

### 命令序列

```
AT+CWMODE=1                              # STA模式
AT+CWJAP="你的WiFi名","你的WiFi密码"      # 连接WiFi（等3-5秒）
AT+CIPMUX=1                              # 多连接模式
AT+CIPSERVER=1,8080                      # 启动TCP服务器，端口8080
AT+CIFSR                                 # 查询IP地址（记下来）
```

### 使用流程

1. **手机/电脑连接同一路由器WiFi**
2. **获取ESP32的IP**（CIFSR返回的STA IP）
3. **TCP客户端连接该IP:8080**
4. **双向通信**

### 数据收发

**收到客户端数据**（ESP32主动上报）:
```
+IPD,0,5:HELLO       # 连接0发来5字节数据: HELLO
```

**发送数据给客户端**:
```
AT+CIPSEND=0,5       # 向连接0发送5字节
> HELLO              # 输入数据（无需\r\n）
```

## 常用指令

```
AT                      # 测试
AT+RST                  # 复位
ATE0                    # 关回显
AT+GMR                  # 版本信息
AT+CWMODE=1             # STA模式
AT+CWJAP="ssid","pass"  # 连WiFi
AT+CIFSR                # 查IP
AT+CWLAP                # 扫描WiFi
AT+CWQAP                # 断开WiFi
AT+CIPMUX=1             # 多连接
AT+CIPSERVER=1,8080     # 开TCP服务器
AT+CIPCLOSE=0           # 关闭连接0
```