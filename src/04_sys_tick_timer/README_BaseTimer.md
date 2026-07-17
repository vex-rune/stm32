# 04 - 基本定时器(TIM6 / TIM7)

TIM6 和 TIM7 是 STM32 的 **基本定时器**(Basic Timer),挂载在 APB1 总线上,功能最简单——只做纯定时计数,没有捕获、PWM、外部输入。

> 本目录的示例基于 **SysTick** 实现,见 [README_SysTick.md](README_SysTick.md)。本文档仅整理 TIM6/TIM7 的原理与配置流程,作为知识储备。

---

## 1. 项目概览

| 项目    | 说明                                          |
|-------|---------------------------------------------|
| MCU   | STM32F103xE(Cortex-M3)                      |
| 定时器   | TIM6 / TIM7(基本定时器)                          |
| 时钟源   | APB1 Timer Clock(本工程 SYSCLK = 72 MHz)      |
| 功能    | 定时中断、DAC 触发(仅 TIM6)、DMA 请求                  |
| 中断类型  | 仅 **更新中断 UEV**(计数溢出)                         |

---

## 2. 核心特性

### 2.1 共性特点(TIM6、TIM7 完全一致)

1. **内部时钟源**:只能挂载 APB1 总线,不能外部触发计数。
2. **仅向上计数**:CNT 从 0 递增到 ARR,溢出产生更新事件 UEV,自动清零重新计数。
3. **无通道、无 IO**:没有 CH1/CH2 输入捕获、输出比较、PWM,没有 ETR 引脚,不支持编码器模式。
4. **仅 1 种中断**:更新中断 UEV。
5. **预分频器 + 自动重载**:结构为 `APB1 Timer Clock → PSC(1~65536) → CNT(0~ARR)`。
6. **16 位寄存器**:PSC、ARR、CNT 最大值 65535。
7. **支持 DMA 更新请求**:溢出时可触发 DMA 搬运数据。

### 2.2 TIM6 与 TIM7 的区别

二者硬件逻辑一致,差别仅在于 **独立外设、独立中断向量、独立寄存器**,可分开使用互不干扰。

| 项目   | TIM6                | TIM7        |
|------|---------------------|-------------|
| 中断向量 | TIM6_DAC(与 DAC 共用通道) | TIM7 独立中断   |
| 典型用途 | 配合 DAC 做波形定时输出        | 系统节拍、软件延时  |

---

## 3. 内部结构

```
APB1 Timer Clock
        ↓
Prescaler PSC(分频,1~65536)
        ↓
Counter CNT(0 → ARR,16 位)
        ↓
溢出 → Update Event(UEV)
        ├─ 更新中断 UEV
        └─ DMA 请求
```

### 3.1 预加载寄存器与影子寄存器

写入自动重载寄存器 ARR 时,数据先进入 **预加载寄存器**,再在下一个更新事件时转移到 **影子寄存器** 生效。这种机制保证 ARR 的修改与计数周期同步,避免出现长短不一的计数周期。

---

## 4. 典型用途

| 场景                   | 说明                                            |
|----------------------|-----------------------------------------------|
| 毫秒/微秒定时中断            | 1 ms 进中断扫描按键、刷新屏幕                             |
| 软件延时                 | 用 CNT 计数做精准延时,不占用 SysTick                    |
| DAC 定时触发(TIM6 专属)    | DAC 触发源优先选 TIM6,用于周期性输出电压波形                   |
| 定期 DMA 搬运            | 每固定时间触发 DMA,搬运 ADC 采样、串口缓存等                   |
| 系统后台节拍(TIM7 常用)      | FreeRTOS 等系统的辅助心跳、巡检定时                       |

---

## 5. 与通用定时器对比

| 特性       | TIM6/TIM7(基本) | TIM2/TIM3/TIM4(通用) |
|----------|---------------|--------------------|
| PWM 输出   | ❌             | ✅ 4 路              |
| 输入捕获     | ❌             | ✅                  |
| 外部时钟计数   | ❌ 仅内部         | ✅ ETR              |
| 编码器模式    | ❌             | ✅                  |
| DAC 触发   | ✅ TIM6 推荐     | ❌                  |
| 功能定位     | 纯定时、DAC 触发   | 电机、脉冲测量、波形输出        |

---

## 6. 配置流程(标准库示例)

```c
// 1. 开启 APB1 时钟
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

// 2. 设置预分频 PSC、自动重载 ARR
TIM_TimeBaseInitTypeDef cfg = {0};
cfg.TIM_Prescaler         = 71;          // 72MHz / (71+1) = 1MHz
cfg.TIM_Period            = 999;         // 1ms 周期
cfg.TIM_CounterMode       = TIM_CounterMode_Up;
cfg.TIM_ClockDivision     = TIM_CKD_DIV1;
TIM_TimeBaseInit(TIM6, &cfg);

// 3. 使能更新中断、配置 NVIC
TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
HAL_NVIC_SetPriority(TIM6_IRQn, 0, 0);
HAL_NVIC_EnableIRQ(TIM6_IRQn);

// 4. 使能定时器
TIM_Cmd(TIM6, ENABLE);
```

中断服务函数:

```c
void TIM6_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
        // 业务逻辑
    }
}
```

> HAL 库下可使用 `HAL_TIM_Base_Start_IT()` + `HAL_TIM_PeriodElapsedCallback()` 的写法,详见 HAL 文档。

---

## 7. STM32CubeMX 配置

以 **TIM6** 为例,TIM7 配置完全相同,仅外设与中断向量名不同。

### 7.1 Timers → TIM6

- `Activated`:勾选
- `Clock Source`:`Internal Clock`
- `Channel1~4`:`Disabled`(基本定时器无通道)

### 7.2 Parameter Settings

| 参数                      | 推荐值       | 说明                                  |
|-------------------------|-----------|-------------------------------------|
| Prescaler(PSC)         | `71`      | 72 MHz → 1 MHz 计数时钟                  |
| Counter Mode            | `Up`      | 仅支持向上计数                             |
| Counter Period(ARR)     | `999`     | 1 ms 周期                              |
| Internal Clock Division | `Division 1` | 不分频                              |
| auto-reload preload     | `Enable`  | ARR 修改后在下一次 UEV 生效                 |
| Trigger Event Selection | `Update Event`(触发 DAC 时)| 启用主模式输出,普通定时不用选              |

> **周期公式**:`T = (PSC + 1) × (ARR + 1) / Tclk`

### 7.3 NVIC

`System Core → NVIC`,勾选 `TIM6 global interrupt`,按需配置抢占优先级。

### 7.4 触发 DAC(可选,仅 TIM6)

- `Timers → TIM6 → Trigger Event Selection` → `Update Event`
- `Analog → DAC → Trigger` → `Timer 6 Trigger Out event`

---

## 8. 关键 API

| API                                | 作用              |
|------------------------------------|-----------------|
| `RCC_APB1PeriphClockCmd`           | 开启 TIM6/TIM7 时钟 |
| `TIM_TimeBaseInit`                 | 配置 PSC、ARR、计数模式 |
| `TIM_ITConfig`                     | 使能更新中断          |
| `TIM_Cmd`                          | 启动/停止定时器        |
| `TIM_GetITStatus` / `TIM_ClearITPendingBit` | 查询/清除更新中断标志  |

---

## 9. 注意事项

1. TIM6/TIM7 **默认关闭**,必须手动开启 APB1 时钟。
2. ARR 修改后 **不会立即生效**,需等待一次更新事件(预加载机制)。
3. 不支持重复计数寄存器 RCR。
4. PSC 与 ARR 均为 16 位,组合最大计数周期受 SYSCLK 与分频共同限制。
5. F1/F4/L4/H7 全系均带有 TIM6、TIM7。
6. 若与 DAC 共用,注意 TIM6 的中断向量名为 `TIM6_DAC_IRQn`,中断函数名为 `TIM6_DAC_IRQHandler`。


## 10. HAL 示例代码

### 10.1 main.c — 启动定时器

在 `MX_TIM6_Init()` 之后调用 `HAL_TIM_Base_Start_IT()` 启动更新中断:

```c
/* USER CODE BEGIN 2 */

// 启动 TIM6 更新中断
HAL_TIM_Base_Start_IT(&htim6);

/* USER CODE END 2 */
```

### 10.2 stm32f1xx_it.c — 周期回调

HAL 通过 `htim->Instance` 区分不同的定时器。`TIM2 ~ TIM7` 在 `stm32f103xe.h` 中的宏定义:

```c
#define TIM2 ((TIM_TypeDef *)TIM2_BASE)
#define TIM3 ((TIM_TypeDef *)TIM3_BASE)
#define TIM4 ((TIM_TypeDef *)TIM4_BASE)
#define TIM5 ((TIM_TypeDef *)TIM5_BASE)
#define TIM6 ((TIM_TypeDef *)TIM6_BASE)
#define TIM7 ((TIM_TypeDef *)TIM7_BASE)
```

回调实现:

```c
/* USER CODE BEGIN 1 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    // 翻转 LED2
    HAL_GPIO_TogglePin(led2_GPIO_Port, led2_Pin);
  }
}
/* USER CODE END 1 */
```