# 12_adc_mul — STM32 ADC 多通道 + DMA 循环采集

> 基于 STM32F103ZE，ADC1 通道 10（PC0）+ 通道 11（PC1），DMA1 Channel 1 自动搬运，USART1 打印两通道原始值。

---

## 截图说明

| 文件 | 内容 |
|------|------|
| `cubemx_adc1_in10_in11双通道配置.png` | ADC1 → Parameter Settings：Scan=Enable / Continuous=Enable / Number of Conversion=**2** / Rank1=Channel 10 / Rank2=Channel 11 / 7.5 Cycles / SW trigger（红框标注 `2`、`Channel 10`、`Channel 11`） |
| `cubemx_dma_adc1通道_normal配置.png` | ADC1 → DMA Settings：**DMA1 Channel 1 / Peripheral→Memory / Low / Normal / Half Word × Half Word** |

## 关键配置（来自 `.ioc`）

- ADC1：ScanConvMode = ENABLE，ContinuousConvMode = ENABLE，NbrOfConversion = 2
- Rank1 = IN10（PC0），Rank2 = IN11（PC1），SamplingTime = 7.5 Cycles
- DMA1 Channel 1：Peripheral → Memory，HALFWORD，Priority = Low
- NVIC：DMA1_Channel1_IRQn 已使能

## main.c 关键逻辑

```c
uint16_t adc_buffer[2] = {0, 0};              // ⚠️ 必须用 uint16_t
HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 2);

while (1) {
    printf("ADC1: %u, %u\r\n", adc_buffer[0], adc_buffer[1]);
    HAL_Delay(1000);
}
```

- `adc_buffer[0]` ↔ Rank1（PC0 / Channel 10）
- `adc_buffer[1]` ↔ Rank2（PC1 / Channel 11）

## ⚠️ 常见踩坑

1. **buffer 类型必须 `uint16_t`，不能用 `uint32_t`**——DMA 是 Half Word，若用 `uint32_t[2]`，两个通道的 12-bit 值会被并到一个 32-bit，打印就会出现 `262734350` 这种"巨大异常值"，其实是两个半字拼在一起。
2. `DMA_NORMAL` 模式只会搬运一次，**配合 ContinuousConvMode 会卡死**：DMA 完成后不再响应请求，ADC 仍在转换但 buffer 不更新。推荐改成 `DMA_CIRCULAR`。
3. PC1 没接外部信号时会因浮空输出近 Vref 的满量程噪声——这是电路侧，不是代码 bug。

## 参考

- STM32F103 参考手册 RM0008 — 第 10 章 DMA、第 11 章 ADC
- STM32CubeF1 HAL 库说明 — `Drivers/STM32F1xx_HAL_Driver/`
