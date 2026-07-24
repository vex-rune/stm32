# 11_adc_RV — STM32 ADC 单通道 + 轮询读取电位器

> 基于 STM32F103ZE，ADC1 通道 10（PC0）采集电位器分压，按分压反算未知电阻。

---

## 截图说明

| 文件 | 内容 |
|------|------|
| `cubemx_adc1_in10配置.png` | ADC1 → Parameter Settings：IN10 勾选、Right alignment、Continuous=Enable、7.5 Cycles、Rank=1、SW trigger、No DMA |
| `cubemx_clock_tree_adc_12mhz.png` | Clock Configuration：HSE→PLL×9 = 72 MHz，APB2=/1 → ADC Prescaler **/6 → 12 MHz to ADC1,2,3** |

## 关键配置（来自 `.ioc`）

- ADC1 IN10（PC0），SamplingTime = 7.5 Cycles
- ContinuousConvMode = ENABLE，DataAlignment = Right
- 无 DMA、不开扫描、单通道轮询
- f_ADC = 12 MHz，ADC 时钟不可超过 14 MHz（这里有富余）

## 硬件电路（示意）

```
        3.3V
         |
         ⌒ 10 kΩ 已知分压电阻
         |
        PC0 --- 接电位器滑动端 & 与未知电阻串联到 GND
         |
        ???   未知电阻
         |
        GND
```

## main.c 关键逻辑

```c
HAL_ADC_Start(&hadc1);                       // 单通道轮询
while (1) {
    uint32_t value     = HAL_ADC_GetValue(&hadc1);
    uint32_t voltage   = value * 3300 / 4096;                // mV
    uint32_t resistance = 10000 * voltage / (3300 - voltage); // Ω
    printf("value=%lu  voltage=%lu mV  resistance=%lu Ω\r\n",
           value, voltage, resistance);
    HAL_Delay(1000);
}
```

PC0 上电压：

```
V_PC0 = 3.3 V × R_unknown / (R_known + R_unknown)
```

已知 `R_known = 10 kΩ`，反算即可。

## 参考

- STM32F103 参考手册 RM0008 — 第 11 章 ADC
- STM32CubeF1 HAL 库说明 — `Drivers/STM32F1xx_HAL_Driver/`
