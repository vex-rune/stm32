# 10_dma_ram_com — STM32 RAM→USART1 DMA 发送示例

> 基于 STM32F103ZE，使用 `HAL_UART_Transmit_DMA()` 把 RAM 中的字符串搬运到 USART1_TX（PA9）。

---

## 截图说明

| 文件 | 内容 |
|------|------|
| `cubemx_dma_usart1_tx配置.png` | DMA → DMA1 子页，红框：USART1_TX → DMA1 Channel 4 / Memory To Peripheral / Low / Normal / Byte×Byte |

## 关键配置（来自 `.ioc`）

- DMA Request：**USART1_TX**
- Channel：**DMA1 Channel 4**
- Direction：Memory → Peripheral
- Mode：Normal
- Data Width：Byte / Byte
- Priority：Low
- NVIC：USART1 global interrupt（Preemption = 0）

## main.c 逻辑（节选）

```c
const uint8_t src[] = { 'H','e','l','l','o',' ','W','o','r','l','d','!',
                        ' ','D','M','A',' ','t','o',' ','U','A','R','T' };
HAL_UART_Transmit_DMA(&huart1, src, sizeof(src));
while (1) { /* 空循环，DMA 后台搬运 */ }
```

烧录后串口助手即可看到 `Hello World! DMA to UART`。

## 参考

- STM32F103 参考手册 RM0008 — 第 10、14 章
- STM32CubeF1 HAL 库说明 — `Drivers/STM32F1xx_HAL_Driver/`
