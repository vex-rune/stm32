# 14_fsmc_is62wv51216 项目说明

本工程演示了 STM32F103ZE 通过 FSMC 总线访问外部 SRAM（IS62WV51216，1MB）的方法。访问层代码全部集中在 [`Core/Inc/is62wv51216.h`](Core/Inc/is62wv51216.h) 与 [`Core/Src/is62wv51216.c`](Core/Src/is62wv51216.c)；引脚与时序初始化则交给 STM32CubeMX 生成的 [`Core/Src/fsmc.c`](Core/Src/fsmc.c)。

本文档回答三个问题：

1. 为什么不能用 `__attribute__((at()))` 直接把变量映射到 0x68000000？
2. STM32 CPU 向 FSMC 写入一个 16bit 数据，硬件层面到底做了什么？
3. 本工程 `is62wv51216.h/.c` 当前实现的工作机制是什么？

---

## 一、为什么不能用 `__attribute__((at()))`

`__attribute__((at(addr)))` 是 GCC 的一个非标准扩展，作用是把**已声明的全局/静态变量**放到指定的绝对地址上。它经常被用来：

```c
// 把变量映射到外设寄存器地址
#define REG_WATCHDOG   (*(volatile uint32_t *)0x40003000)

// 或用 at() 写变量
uint32_t watchdog __attribute__((at(0x40003000)));
```

看上去好像"可以用来访问 0x68000000 的 SRAM"，但在实际工程里至少有 **6 个致命缺陷**：

### 1. `at()` 是"声明"语法，不是"访问"语法

`__attribute__((at(addr)))` 必须紧跟一个**有名字、有类型、有确定大小的变量声明**。它不能让指针表达式动态寻址：

```c
// 合法：声明一个固定变量到绝对地址
uint8_t buf[1024] __attribute__((at(0x68000000)));

// 不合法：at() 不能修饰解引用表达式
*(volatile uint16_t *)0x68000000 = 0xAABB;  // 这里用不了 at()
```

我们这套 API 的核心诉求是 **"任意字地址、任意时刻读写"**，调用者传 `wordAddr` 进来，没有任何"声明"可言。`at()` 在语义上就够不着这种用法。

### 2. `at()` 会"占满"一段地址空间

即使把 `at()` 强行套上去：

```c
uint8_t fake_sram[1024*1024] __attribute__((at(0x68000000)));  // 1MB
```

编译器会认为 0x68000000 ~ 0x680FFFFF 这 1MB 都被 `fake_sram` 占用了，会在 `.bss` 里给它预留 1MB 启动初始化量。但实际硬件 RAM 只有 64KB，链接器要么报错、要么把 1MB 的 BSS 清零逻辑加进去，启动时间拉长到秒级，**纯粹是浪费**。

### 3. `at()` 不能跨编译器、跨 IDE

| 编译器 | 等价写法 |
|---|---|
| GCC | `__attribute__((at(addr)))` |
| ARMCC (Keil MDK v5) | `__at(addr)` 关键字 |
| IAR | `#pragma location = "addr"` |
| ARM Compiler 6 / armclang | **不支持** `at()`，需用 scatter file |

本工程用 CMake 组织，能在 gcc-arm-none-eabi 与 clang 之间切换。一旦把 `__attribute__((at()))` 写进代码，切换到 armclang 时就会编译失败。

### 4. armclang 已经**移除** `at()` 支持

ARM 官方从 AC6 起就明确不推荐 `__attribute__((at()))`，原因是它和 scatter file / linker script 的语义有冲突。新建工程再用 `at()` 等于给自己埋坑。

### 5. `at()` 本身**不会**阻止编译器优化

`__attribute__((at()))` 只负责告诉链接器把变量放哪里，**它不等同于 `volatile`**。如果漏写 `volatile`：

```c
uint32_t x __attribute__((at(0x68000000)));   // 漏掉 volatile，编译器仍可能把 x 缓存在寄存器里
```

下次再读 `x` 拿到的是寄存器里的旧值，而不是 SRAM 里的最新内容。**正确写法永远是 `volatile + 解引用`，而不是 `at()` 替代 `volatile`**。

### 6. 链接脚本冲突

[`STM32F103xx_FLASH.ld`](STM32F103xx_FLASH.ld) 已经定义：

```
MEMORY
{
  RAM    (xrw) : ORIGIN = 0x20000000, LENGTH = 64K
  FLASH  (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
}
```

链接器只知道 `RAM` 和 `FLASH` 两个区域。强行 `at(0x68000000)` 等于在链接器视野外"凭空捏造"一块内存，调试时 IDE 看不到、.map 文件错乱、断点无法设置。

### 结论

> **FSMC 外部存储的正确访问方式是"volatile 指针 + 运行时地址计算"，而不是 `__attribute__((at()))`。**
> 
> `at()` 只适合静态映射一个**固定大小、固定地址**的变量（如单片机寄存器数组），不适合"动态寻址 1MB SRAM"的场景。

---

## 二、STM32 的写入原理

当 CPU 执行下面这行 C 代码时：

```c
*(volatile uint16_t *)0x68000000 = 0xAABB;
```

编译器把它翻译成一条 ARM 指令（以小端 STM32F103 为例）：

```asm
STRH  r0, [r1]    ; r1 = 0x68000000, [r1] 写入 r0（0xAABB）
```

后续完全是**硬件自动完成的**，软件不再做任何动作。下面按时间顺序拆解：

### 步骤 1：总线矩阵路由

STM32 的 AHB 总线矩阵检查地址 0x68000000，它落在"外部 SRAM 区"（0x60000000 ~ 0x6FFFFFFF）。这个区域不属于内部 Flash 也不属于内部 SRAM，所以总线矩阵把这次访问**转发给 FSMC 控制器**，而不是片内 RAM。

### 步骤 2：FSMC 地址译码

FSMC 控制器读出地址的高位，决定使用哪个 Bank：

| 地址范围 | Bank | 片选引脚 |
|---|---|---|
| 0x60000000 ~ 0x63FFFFFF | Bank1 NE1 | FSMC_NE1 |
| 0x64000000 ~ 0x67FFFFFF | Bank1 NE2 | FSMC_NE2 |
| **0x68000000 ~ 0x6BFFFFFF** | **Bank1 NE3** | **FSMC_NE3** |
| 0x6C000000 ~ 0x6FFFFFFF | Bank1 NE4 | FSMC_NE4 |

0x68000000 命中 NE3，所以 FSMC 拉低 **FSMC_NE3**（PG10）选通 IS62WV51216。

### 步骤 3：FSMC 驱动地址线

地址的低 19 位（A0 ~ A18）由 FSMC 驱动到对应 GPIO 引脚。本工程用到 19 根地址线才能寻址 512K × 16bit = 1MB。

| 地址线 | GPIO 引脚 | 作用 |
|---|---|---|
| A0 ~ A5 | PF0 ~ PF5 | 字地址低 6 位 |
| A6 ~ A9 | PF12 ~ PF15 | 字地址 6~9 位 |
| A10 ~ A11 | PG0 ~ PG1 | 字地址 10~11 位 |
| A12 ~ A15 | PG2 ~ PG5 | 字地址 12~15 位 |
| A16 ~ A18 | PD11 ~ PD13 | 字地址 16~18 位 |

### 步骤 4：FSMC 根据 CPU 指令决定数据宽度与字节使能

CPU 写的是 16bit 数据，所以 FSMC 把 **NBL0 和 NBL1 都拉低**：

| 字节使能 | 引脚 | 控制字节 | 说明 |
|---|---|---|---|
| NBL0 | PE0 | D[7:0]（低字节） | 拉低 = 允许写低字节 |
| NBL1 | PE1 | D[15:8]（高字节） | 拉低 = 允许写高字节 |

> **IS62WV51216 的关键特性**：当只拉低 NBL0 时，D[15:8] 的内容保持不变；这就是"单字节写不影响另一半"的硬件基础。

### 步骤 5：FSMC 时序控制

[`Core/Src/fsmc.c`](Core/Src/fsmc.c) 中配置的关键时序参数：

```c
Timing.AddressSetupTime = 15;   // 地址建立时间
Timing.AddressHoldTime  = 15;   // 地址保持时间
Timing.DataSetupTime    = 71;   // 数据建立时间（必须 >= SRAM 写周期）
Timing.BusTurnAroundDuration = 15;  // 总线转向时间（读后写/写后读切换）
```

整个写时序的波形大致是：

```
       ┌─────────┐                              ┌─────────┐
A0~A18:│ addr    │──────────────────────────────│  next   │
       └─────────┘                              └─────────┘
       ├──15─┤
        ↑ 地址建立时间到了
                   ┌──────────────────┐
NWE:               │      LOW         │
                   └──────────────────┘
                   ├─────71 ns──────┤
                      数据建立时间（写脉冲宽度）
                                          ┌────15────┐
                                          读/写切换等待（下次访问）

NE3:    ──────┐                              ┌──────────
             │______________________________│
             在整个传输期间保持低电平（选中 IS62WV51216）

NBL0:        ───────────────────────────────  (拉低)
NBL1:        ───────────────────────────────  (拉低)
D0~D15:                0xAABB 在数据建立时间内稳定
```

### 步骤 6：SRAM 锁存数据

NWE 上升沿到来时，IS62WV51216 在内部锁存 D[15:0] 的值，写入由 A[18:0] 指定的存储单元。整个过程对 CPU 而言就是一条 `STRH` 指令的时长（在 72MHz HCLK、上述时序下大约 100ns ≈ 7 个 CPU 周期）。

### 步骤 7：CPU 继续执行下一条指令

CPU 不会等 FSMC 真正完成——实际上 STRH 是同步的，FSMC 会拉长总线响应直到时序结束，CPU 拿到"完成"信号后立即继续。所以从软件视角看，写外部 SRAM 和写内部 RAM 在指令层面**完全一样**。

### 字节写入（8 位）的差异

如果代码是：

```c
*(volatile uint8_t *)0x68000000 = 0xAB;   // 写低字节
```

CPU 编译为 `STRB` 指令，FSMC 检测到 8 位访问：

| 字节使能 | 状态 |
|---|---|
| NBL0（控制 D[7:0]） | 拉低 → 允许写 |
| NBL1（控制 D[15:8]） | 保持高 → 高字节不变 |

结果：**只更新低字节，高字节原样保留**。这就是本工程 `SRAM_WriteByteLow` / `SRAM_WriteByteHigh` 函数能正确工作的硬件基础。

---

## 三、当前实现解读

### 3.1 文件结构

```
Core/
├── Inc/
│   ├── is62wv51216.h     # SRAM 访问 API（内联单字/单字节 + 批量声明）
│   └── fsmc.h            # FSMC 初始化 API
└── Src/
    ├── is62wv51216.c     # 批量读写的循环实现
    ├── fsmc.c            # FSMC 控制器 + GPIO 引脚初始化
    └── main.c            # 初始化顺序 + 演示
```

### 3.2 启动顺序（看 [main.c](Core/Src/main.c)）

```c
HAL_Init();           // 1. 配置 Flash、SYSTick
SystemClock_Config(); // 2. 72MHz HCLK
MX_GPIO_Init();       // 3. 其他普通 GPIO
MX_FSMC_Init();       // 4. FSMC 控制器 + 38 根 FSMC 引脚 + 时序
MX_USART1_UART_Init();
// 5. 此时调用 SRAM_ReadWord/WriteWord 即可访问 0x68000000
```

**第 4 步是关键**：在 `MX_FSMC_Init()` 内部，`HAL_SRAM_Init()` → `HAL_SRAM_MspInit()` → `HAL_FSMC_MspInit()` 把所有 FSMC 引脚配成 `GPIO_MODE_AF_PP`（复用推挽）、打开 FSMC 时钟、设置好读写时序。这步之后 0x68000000 区域才真正"活"起来。

### 3.3 访问层 API 设计

#### 地址换算

```c
#define SRAM_BASE_ADDR  ((uint32_t)0x68000000)   // 字节基址
#define SRAM_MAX_WORD   (512 * 1024)             // 524288 个 16bit 字
```

调用者传"字地址"（0 ~ 524287），内部 `wordAddr << 1` 转字节偏移，再加上 `SRAM_BASE_ADDR` 得到真实字节地址。

#### 16 位整字访问

```c
static inline uint16_t SRAM_ReadWord(uint32_t wordAddr)
{
    return *(volatile uint16_t *)(SRAM_BASE_ADDR + (wordAddr << 1));
}
```

`volatile` 告诉编译器：每次都必须真正读写内存，不能缓存到寄存器。`uint16_t*` 让 CPU 用 16 位指令访问，FSMC 自动拉低 NBL0 + NBL1，写入完整的一个字。

#### 8 位单字节访问

```c
static inline void SRAM_WriteByteLow(uint32_t wordAddr, uint8_t dat)
{
    *(volatile uint8_t *)(SRAM_BASE_ADDR + (wordAddr << 1)) = dat;
}
```

地址取 `wordAddr << 1`（偶地址）就是字内低字节的位置，`uint8_t*` 让 FSMC 只拉低 NBL0，高字节保留。

```c
static inline void SRAM_WriteByteHigh(uint32_t wordAddr, uint8_t dat)
{
    *(volatile uint8_t *)(SRAM_BASE_ADDR + (wordAddr << 1) + 1) = dat;
}
```

加 1 取奇地址 = 字内高字节，只拉低 NBL1，低字节保留。

#### 批量访问

```c
void SRAM_WriteBurst(uint32_t startWord, const uint16_t *buf, uint32_t len);
void SRAM_ReadBurst (uint32_t startWord, uint16_t *buf, uint32_t len);
```

实现在 [`is62wv51216.c`](Core/Src/is62wv51216.c) 里就是一个简单循环：

```c
void SRAM_WriteBurst(uint32_t startWord, const uint16_t *buf, uint32_t len)
{
    uint32_t addr = SRAM_BASE_ADDR + (startWord << 1);
    for (uint32_t i = 0; i < len; i++) {
        *(volatile uint16_t *)(addr + (i << 1)) = buf[i];
    }
}
```

为什么放 `.c` 而不是 `inline`？循环体内有地址递增计算，内联后会让调用点代码膨胀；放成独立函数便于后续替换成 DMA 或中断驱动版本。

### 3.4 软件对硬件的依赖

| 软件写法 | 硬件动作 | 谁负责触发 |
|---|---|---|
| `SRAM_WriteWord(0, 0xAABB)` | FSMC 拉低 NE3 + NWE + NBL0/NBL1 + 驱动地址 + 输出数据 | FSMC 控制器（CPU 只执行 STRH） |
| `SRAM_WriteByteLow(1, 0x11)` | FSMC 拉低 NE3 + NWE + 仅 NBL0 + 地址 = 0x68000002 | FSMC 控制器（CPU 只执行 STRB） |
| `MX_FSMC_Init()` | 把 38 根 GPIO 切到 FSMC 复用功能 + 配置时序寄存器 | HAL 库 + 我们写的 `HAL_FSMC_MspInit` |

**软件全程不直接动一根 GPIO 引脚**，所有引脚切换都是 FSMC 硬件做的。这是 STM32 把外部总线设计成"内存映射型"的最大好处——上层代码可以像访问普通数组一样访问外部 SRAM。

### 3.5 与"传统 51/AVR 单片机"写外部存储的对比

| 项目 | 传统 8 位单片机 | STM32 + FSMC（当前工程） |
|---|---|---|
| 写一个字节 | 软件模拟时序（拉 CS、低 WE、置数据、拉高 WE） | `*(volatile uint8_t *)addr = value;` |
| 地址线驱动 | 软件逐根 GPIO 置位 | FSMC 硬件自动驱动 |
| 时序控制 | 用 `_nop_()` 数指令周期凑延时 | FSMC 时序寄存器精确配置 |
| 速度 | 通常几百 KHz | 最高 HCLK 频率（72MHz） |
| CPU 占用 | 100% 占用整个传输过程 | 仅 1 条指令，剩余时间可执行其他代码 |

这也是为什么 STM32 在嵌入式领域被广泛使用——**FSMC 把"硬件协议"抽象成"内存读写"**，让应用层程序员完全不用关心总线时序。

---

## 四、常见疑问

### Q1：可以同时用 `at()` 和 `volatile` 吗？

可以，但没必要。例如：

```c
volatile uint16_t sram_at_0x68 __attribute__((at(0x68000000)));
```

这相当于声明一个**唯一的** 16bit 变量在 0x68000000。但只能访问一个地址，无法访问 0x68000002、0x68000004 等其他位置，**通用性远不如本工程的指针方案**。

### Q2：为什么 `volatile` 不能被 `at()` 替代？

`at()` 是链接器指令（决定变量放在哪），`volatile` 是 C 语言类型修饰符（决定编译器怎么访问）。两者作用在不同层面：

- 没有 `volatile`：编译器可能优化掉读，或合并多次写
- 没有 `at()`：变量放在默认的 RAM 区（0x20000000），不能访问外部 SRAM

**正确做法是 `volatile` + 解引用**，不用 `at()`。

### Q3：批量访问能不能用 DMA？

能。FSMC 支持 DMA 传输，可以把 DMA 源/目的地址设为 `SRAM_BASE_ADDR + offset`，每次突发传几十到几百字节，比 CPU 循环快 4~8 倍且不占用 CPU。本工程目前用 CPU 循环是为了演示原理，实际产品可改成 DMA 版本。

### Q4：地址能不能改成 `0x60000000`（NE1）？

能，只要把 [`fsmc.c`](Core/Src/fsmc.c) 里的：

```c
hsram1.Init.NSBank = FSMC_NORSRAM_BANK3;   // 改成 FSMC_NORSRAM_BANK1
```

再把 [`is62wv51216.h`](Core/Inc/is62wv51216.h) 里的：

```c
#define SRAM_BASE_ADDR  ((uint32_t)0x60000000)
```

剩下的代码一行都不用动。这就是把"硬件初始化"和"访问层"分离的好处。

---

## 五、参考资源

- STM32F103 参考手册 RM0008 - 第 19 章 FSMC
- IS62WV51216 数据手册 - ISSI
- HAL 库 `stm32f1xx_hal_sram.c` / `stm32f1xx_ll_fsmc.c`
- 本工程 [`14_fsmc_is62wv51216.ioc`](14_fsmc_is62wv51216.ioc) - STM32CubeMX 配置