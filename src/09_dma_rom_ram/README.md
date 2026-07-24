# 09_dma_rom_ram — STM32 内存到内存 DMA 演示工程

> 基于 STM32F103ZE，使用 DMA1 Channel 1 以 **Memory-to-Memory（MemToMem）** 模式实现 RAM ↔ RAM 数据搬运，配合 USART1 输出验证。

---

## 一、CubeMX 截图说明（DMA MemToMem 配置）

工程根目录下两张截图记录了 `09_dma_rom_ram.ioc` 在 STM32CubeMX 里的关键配置页，配合文字便于快速复用与排错。

| 文件 | 对应 Tab | 关键参数 / 重点 | 状态 |
|------|---------|----------------|------|
| `cubemx_dma_memtomem配置_正常态.png` | DMA → **MemToMem** 子页 | Request=`MEMTOMEM` / Channel=`DMA1 Channel 1` / Direction=Memory To Memory / Priority=Medium / Mode=**Normal** / Increment Address = ✅✅(Src & Dst) / Data Width = **Byte / Byte** | ✅ 正常样例：完整 M2M 传输任务，搬运一次后停止 |
| `cubemx_dma_NVIC中断开启.png` | NVIC Mode and Configuration | 红框标注 **DMA1 channel1 global interrupt** 选中（Preemption=0） | ✅ 用于 `HAL_DMA_Start_IT()` 后回调 `DMA1_Channel1_IRQHandler` → `HAL_DMA_IRQHandler` 链式调用 `HAL_DMA_MemCpltCallback()` |

> 推荐命名说明：
> - 「正常态」= CubeMX 配置本身正确，可直接生成代码
> - 「中断开启」= 名称直接对应 Tab 名，便于检索
> - 注意：本工程的 `.ioc` 未启用 `Force DMA channels interrupts` 的全局开关也行，因为 NVIC 表里已勾选 DMA1_Channel1

### 1.1 配置解析（图 1）

| 项 | 设定值 | 含义 |
|---|---|---|
| Request | MEMTOMEM | 固定由 `DMA_Request0 = MEMTOMEM` 触发（详见 `.ioc` 第 5-12 行） |
| Direction | Memory To Memory | 源/目的均为 RAM，不依赖任何外设 |
| Mode | Normal | 一次传输（搬运完整数据量）后自动停止 |
| Src / Dst Increment | Enable / Enable | 每个字节后地址 +1 |
| Data Width | Byte / Byte | `uint8_t` 数组最合适；若搬运 `uint32_t`，应改 Word/Word |
| Priority | Medium | 唯一一条 DMA 通道，Medium 即可 |

### 1.2 中断设置（图 2）

红框中的 **DMA1 channel1 global interrupt** 用于驱动基于中断的 M2M（`HAL_DMA_Start_IT()`）。在生成代码后，对应：

- `void DMA1_Channel1_IRQHandler(void)` → `HAL_DMA_IRQHandler(&hdma_memtomem);`
- 应用层回调 `HAL_DMA_MemCpltCallback(DMA_HandleTypeDef *hdma)` 中处理完成事件

---

## 二、目录结构

```
09_dma_rom_ram/
├── Core/
│   ├── Inc/        # 头文件 (dma.h / gpio.h / main.h / usart.h …)
│   └── Src/        # 源文件 (dma.c / main.c / usart.c …)
├── Drivers/
│   ├── CMSIS/      # Cortex-M3 内核支持
│   └── STM32F1xx_HAL_Driver/  # ST 官方 HAL 库
├── cmake/          # CMake 工具链配置
├── startup_stm32f103xe.s
├── STM32F103xx_FLASH.ld
├── 09_dma_rom_ram.ioc   # STM32CubeMX 工程配置
└── README.md       # 本文档
```

---

## 三、构建与烧录

| IDE | 工程文件 |
|-----|----------|
| STM32CubeIDE | 直接 `Import` 根目录 |
| CMake + arm-none-eabi-gcc | 根目录 `CMakeLists.txt` + `cmake/gcc-arm-none-eabi.cmake` |
| CMake + clang | `cmake/starm-clang.cmake` |

---

## 四、参考

- STM32F103 参考手册 RM0008 — 第 10 章 DMA 控制器
- STM32CubeF1 HAL 库说明 — `Drivers/STM32F1xx_HAL_Driver/`
