# 04 - SysTick 定时器

SysTick 只能是递减的计数器,用于定时触发中断。

本示例基于 STM32F103,使用 Cortex-M3 内核的 **SysTick** 定时器每 1 秒翻转一次 LED,演示 HAL 基于 tick 的时间机制。

---

## 1. 项目概览

| 项目    | 说明                                      |
|-------|-----------------------------------------|
| MCU   | STM32F103xE(Cortex-M3)                  |
| LED   | `led1` 接在 **PA0**(开漏输出)                 |
| 时钟源   | HSI 8 MHz → PLL × 9 = **72 MHz** SYSCLK |
| 功能    | 每 **1000 ms** 翻转一次 LED                  |
| 使用的外设 | SysTick(系统滴答定时器)                        |

---

## 2. STM32CubeMX 配置

本项目所需配置极少:

1. **引脚分配**
    - 将 **PA0** 配置为 `GPIO_Output`(即 `led1`)。其他 LED 引脚(PE / PC / PB)保留为兼容配置,实际只有 PA0 会被程序翻转。
2. **时钟配置**
    - HSI → PLL × 9 → SYSCLK = 72 MHz(本项目默认配置)。
3. **无需额外外设** — SysTick 属于 Cortex-M3 内核,由 `HAL_Init()` 自动初始化。

---

## 3. 初始化流程

在 [`Core/Src/main.c`](Core/Src/main.c) 中:

```c
HAL_Init();                          // 初始化 Flash 接口和 SysTick
SystemClock_Config();                // 配置 72 MHz 系统时钟
MX_GPIO_Init();                      // 初始化 PA0 为开漏输出
```

`HAL_Init()` 内部会调用 `HAL_InitTick(TICK_INT_PRIORITY)`,把 SysTick 配置为默认 **1 ms** 产生一次中断。

---

## 4. SysTick 中断实现

SysTick 中断在 [`Core/Src/stm32f1xx_it.c`](Core/Src/stm32f1xx_it.c) 中处理:

```c
/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();                       // 必须保留:用于递增 uwTick
  /* USER CODE BEGIN SysTick_IRQn 1 */

  if (uwTick % 1000 == 0)              // 每 1000 ms
  {
    HAL_GPIO_TogglePin(led1_GPIO_Port, led1_Pin);
  }

  /* USER CODE END SysTick_IRQn 1 */
}
```

### 为什么在 `SysTick_Handler` 中加逻辑,而不是重写 `HAL_IncTick`?

HAL 中的 `HAL_IncTick()` 用 `__weak` 声明:

```c
__weak void HAL_IncTick(void)
{
  uwTick += uwTickFreq;
}
```

常见的两种写法对比如下:

| 方案    | 位置                           | 取舍                                   |
|-------|------------------------------|--------------------------------------|
| **A** | 在用户代码中重写 `HAL_IncTick()`     | 更简洁,但若不显式调用,会破坏 `HAL_Delay()` 的行为。   |
| **B** | 在 `SysTick_Handler` 中加业务逻辑 ✅ | **推荐**。不会影响 `HAL_Delay()`,业务逻辑集中在一起。 |

本项目采用的是 **方案 B**。

---

## 5. 工作原理

1. `HAL_Init()` 调用 `HAL_InitTick()`,配置 SysTick 重装载寄存器,使计数器每 **1 ms** 溢出一次。
2. 每次溢出,Cortex-M3 内核触发 `SysTick` 异常。
3. `SysTick_Handler()` 执行,调用 `HAL_IncTick()`,把全局变量 `uwTick` 加 `uwTickFreq`(默认 = 1)。
4. `uwTick % 1000 == 0` 每 1000 次中断(即 **每秒**)成立一次,翻转 `led1`。

```
SysTick 滴答(1 ms) ──► HAL_IncTick() ──► uwTick++
                                            │
                              uwTick % 1000 == 0?
                                            │
                                      是 ─┼─► HAL_GPIO_TogglePin(led1)
```

---

## 6. 关键 API

| API                             | 所在源文件                       | 作用                        |
|---------------------------------|-----------------------------|---------------------------|
| `HAL_Init()`                    | `stm32f1xx_hal.c`           | 复位外设、初始化 Flash、启动 SysTick |
| `HAL_InitTick()`                | `stm32f1xx_hal.c`           | 配置 SysTick 中断周期(1 ms)     |
| `HAL_IncTick()`                 | `stm32f1xx_hal.c`(`__weak`) | 递增 `uwTick` 计数器           |
| `HAL_GetTick()`                 | `stm32f1xx_hal.c`           | 读取当前 `uwTick` 值           |
| `HAL_Delay(uint32_t ms)`        | `stm32f1xx_hal.c`           | 阻塞 `ms` 毫秒(基于 `uwTick`)   |
| `HAL_GPIO_TogglePin(port, pin)` | `stm32f1xx_hal_gpio.c`      | 翻转指定 GPIO 引脚的输出电平         |

---

## 7. 注意事项

- **不要删掉 `HAL_IncTick()`**。一旦删除,整个 HAL 的 `HAL_Delay()` 和 `HAL_GetTick()` 都会失效。
- SysTick 计数器是 **24 位** 的,在 72 MHz CPU 下最大重装载周期约 233 ms,默认的 1 ms 滴答完全在范围内。
- 中断服务程序里要尽量少做事。`uwTick % 1000` 这个判断很轻量,但凡耗时操作都应该在中断里置标志,放到主循环处理。
- 如果修改了系统时钟,要复查 `HAL_InitTick()` — 它会根据 `SystemCoreClock` 重新计算 1 ms 周期。

---

## 8. 构建与烧录

本项目使用 **CMake**,工具链定义在 [`cmake/gcc-arm-none-eabi.cmake`](cmake/gcc-arm-none-eabi.cmake),预设配置在 [
`CMakePresets.json`](CMakePresets.json)。

PowerShell 下常用的构建命令:

```powershell
cmake --preset debug
cmake --build build
```

构建完成后,用你熟悉的下载器(ST-Link、OpenOCD 等)把生成的 `.elf` / `.bin` 烧录到板子即可。

---

## 9. 文件结构

```
04_sys_tick_timer/
├── Core/
│   ├── Inc/                # main.h、gpio.h、stm32f1xx_it.h 等
│   └── Src/
│       ├── main.c          # 程序入口、系统时钟、主循环
│       ├── gpio.c          # MX_GPIO_Init —— 把 PA0 配置为开漏输出
│       └── stm32f1xx_it.c  # SysTick_Handler —— 每 1 秒翻转 LED
├── Drivers/                # CMSIS + STM32F1xx HAL
├── cmake/                  # 工具链文件
├── CMakeLists.txt
├── CMakePresets.json
├── STM32F103xx_FLASH.ld    # 链接脚本
└── startup_stm32f103xe.s   # 启动文件
```