# ESP32-C3 MINI-1 ESP-AT 启动故障修复笔记
## 故障现象
上电串口持续输出：`Invalid image block, can't boot. invalid header: 0xffffffff`
成因：仅单独烧录 esp-at.bin，缺少 bootloader、分区表，启动链路断裂；Flash残留旧数据也会诱发启动失败。

## 修复方案（factory完整镜像一键烧录，推荐）
固件文件：`factory/factory_MINI-1.bin`
烧录地址：`0x00000`
SPI参数：DIO、40MHz

### 操作流程
1. 按住BOOT按键上电，进入下载模式；
2. Flash Download Tool清空原有条目，仅添加factory_MINI-1.bin，地址设0x00000；
3. 先点击【ERASE】整片擦除Flash；
4. 擦除完毕执行【START】烧录；
5. 烧录完成断电静置10s；
6. 松开BOOT，拔掉全部外接杜邦线（保护Flash总线GPIO6/GPIO7），仅保留USB上电测试。

## 原理说明
factory镜像内置 bootloader + 分区表 + AT主程序，单一文件包含全部启动所需程序，规避多文件分烧漏烧、地址填写错误问题；整片擦除清除Flash旧碎片数据。

## 成功判定
不再循环报错，串口正常打印ESP-AT启动日志，最终输出 `ready`，可接收AT指令。

## 备选方案
如需单独升级应用，使用四段分烧：
