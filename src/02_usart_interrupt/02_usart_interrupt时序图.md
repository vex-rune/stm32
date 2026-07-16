# `02_usart_interrupt` 时序图

> 对应优化后的代码：`main.c` + `stm32f1xx_it.c`
>
> 设计原则：**主循环永远不阻塞**，所有耗时操作走中断 + 标志位。

***

## 一、整体流程总览

```mermaid
sequenceDiagram
    autonumber
    participant App as 上位机串口助手
    participant MCU as STM32 USART1
    participant IT  as USART1_IRQHandler
    participant Main as main 主循环
    participant RxCb as RxCpltCallback
    participant TxCb as TxCpltCallback

    Note over Main: 系统上电或复位
    Main->>MCU: HAL_Init
    Main->>MCU: SystemClock_Config
    Main->>MCU: MX_USART1_UART_Init
    Main->>MCU: HAL_UART_Receive_IT 接收只开一次
    Main-->>Main: 进入 while(1) 不再调用任何阻塞API

    rect rgb(235, 245, 255)
    Note over App,MCU: 正常收发一次
    App->>MCU: TX 字节流
    loop 每个字节
        MCU->>IT: RXNE 中断
        IT->>MCU: 读DR 写入rx_buf
    end
    Note over MCU: RX 接收计数清零
    MCU->>RxCb: HAL_UART_RxCpltCallback
    RxCb->>RxCb: rx_done 等于 1
    RxCb->>MCU: HAL_UART_Receive_IT 立刻重启下一轮
    RxCb-->>IT: 返回

    Note over Main: 主循环轮询到 rx_done 等于 1
    Main->>Main: rx_done 清零
    Main->>MCU: HAL_UART_Transmit_IT 非阻塞发送
    loop 每个字节
        MCU->>IT: TXE 中断
        IT->>MCU: 写DR
    end
    Note over MCU: TX 发送计数清零
    MCU->>TxCb: HAL_UART_TxCpltCallback
    TxCb->>TxCb: tx_done 等于 1
    TxCb-->>IT: 返回
    end

    MCU-->>App: 回显字节流
```

***

## 二、初始化阶段（只发生一次）

```mermaid
sequenceDiagram
    autonumber
    participant Boot as 启动代码
    participant Main as main()
    participant HAL as HAL 库
    participant UART as USART1 外设

    Boot->>Main: 跳转到 main()
    Main->>HAL: HAL_Init()
    HAL-->>Main: OK
    Main->>HAL: SystemClock_Config()
    HAL-->>Main: OK (HSI × PLL × 9 = 64 MHz)
    Main->>HAL: MX_GPIO_Init()
    HAL-->>Main: OK (PA9 AF_PP, PA10 INPUT)
    Main->>HAL: MX_USART1_UART_Init()
    HAL->>UART: 配置 115200 8N1
    HAL->>UART: NVIC Enable(USART1_IRQn, prio=0)
    HAL-->>Main: OK
    Main->>HAL: HAL_UART_Receive_IT(rx_buf, 128)
    HAL->>UART: RXNEIE = 1 (打开接收中断)
    HAL-->>Main: OK (RxState = BUSY_RX)
    Main->>Main: 进入 while(1)
```

***

## 三、正常收发一轮（核心循环）

```mermaid
sequenceDiagram
    autonumber
    participant Host as 上位机
    participant USART as USART1
    participant IRQ as USART1_IRQHandler
    participant HAL as HAL_UART_IRQHandler
    participant RxCb as RxCpltCallback
    participant Main as main 主循环
    participant TxCb as TxCpltCallback

    Host->>USART: 发送字节流
    loop 每个字节
        USART->>IRQ: RXNE 中断
        IRQ->>HAL: HAL_UART_IRQHandler 入口
        HAL->>USART: 读DR
        HAL->>USART: 写入 rx_buf 下一个位置
    end

    Note over HAL: 接收计数清零 触发完成回调
    HAL->>RxCb: HAL_UART_RxCpltCallback
    RxCb->>RxCb: rx_done 置 1
    RxCb->>HAL: HAL_UART_Receive_IT 立刻重启
    HAL->>USART: RXNEIE 置 1 接收状态BUSY

    Note over Main: 任意时刻主循环轮询
    Main->>Main: 读到 rx_done 等于 1
    Main->>Main: rx_done 清零
    Main->>Main: tx_done 清零
    Main->>HAL: HAL_UART_Transmit_IT 非阻塞发送
    HAL->>USART: TXEIE 置 1 发送状态BUSY

    loop 每个字节
        USART->>IRQ: TXE 中断
        IRQ->>HAL: HAL_UART_IRQHandler 入口
        HAL->>USART: 写DR 取下一字节
    end

    Note over HAL: 发送计数清零 触发完成回调
    HAL->>TxCb: HAL_UART_TxCpltCallback
    TxCb->>TxCb: tx_done 置 1
    HAL->>USART: TXEIE 清零 发送状态READY

    USART-->>Host: 字节流 回显完成
```

***

## 四、出错处理（ORE / FE / NE）

```mermaid
sequenceDiagram
    autonumber
    participant Host as 上位机
    participant USART as USART1
    participant IRQ as USART1_IRQHandler
    participant HAL as HAL_UART_IRQHandler
    participant ErrCb as ErrorCallback
    participant RxCb as RxCpltCallback

    Note over USART: 异常场景 RXNE 未及时读 ORE 置位
    USART->>IRQ: ORE 中断
    IRQ->>HAL: HAL_UART_IRQHandler 入口
    HAL->>HAL: 读 SR 和 DR 清错误码
    HAL->>ErrCb: HAL_UART_ErrorCallback
    ErrCb->>ErrCb: 清 OREFEF 和 NEF 标志
    ErrCb->>HAL: HAL_UART_Receive_IT 强制重启接收
    HAL->>USART: RXNEIE 置 1

    Note over ErrCb,RxCb: 关键 不进入 Error_Handler 不关中断 不死循环

    Host->>USART: 继续发新数据
    USART->>IRQ: RXNE 中断
    IRQ->>HAL: HAL_UART_IRQHandler 入口
    HAL->>RxCb: HAL_UART_RxCpltCallback
    RxCb->>RxCb: rx_done 置 1
    RxCb->>HAL: HAL_UART_Receive_IT 正常重启
```

***

## 五、与旧代码对比

| 维度      | 旧代码 ❌                                                  | 新代码 ✅                                                                   |
| ------- | ------------------------------------------------------ | ----------------------------------------------------------------------- |
| 主循环发送   | `HAL_UART_Transmit(..., 1000)` **阻塞**                  | `HAL_UART_Transmit_IT(...)` **非阻塞**                                     |
| 接收开启    | 每轮循环重新 `HAL_UART_Receive_IT`                           | 初始化时只开 **一次**，回调里重启                                                     |
| 接收回调    | `RxHalfCpltCallback` + `RxEventCallback`（与 IT 模式不匹配）   | `HAL_UART_RxCpltCallback`（IT 模式正确回调）                                    |
| 错误处理    | 默认进 `Error_Handler()` → `__disable_irq()` + `while(1)` | 自定义 `ErrorCallback`：清错误码 + 重启接收；`Error_Handler` 改为 `NVIC_SystemReset()` |
| NVIC 状态 | 关全局中断 → SWD 失联 → ST-LINK 报 "Target no device found"    | 中断始终可用 → 调试器随时能 halt → **永不卡死**                                         |

***

## 六、关键时序保证

1. **从** **`while(1)`** **看，CPU 永远只在做三件事**：
   - 读 `rx_done` 标志
   - 进入 `if` 分支，启动一次非阻塞发送
   - 回到循环顶
2. **从中断看，所有"长任务"都被打散成短 ISR**：
   - 每收到 1 字节进一次 RXNE ISR
   - 每发完 1 字节进一次 TXE ISR
   - 收满 / 发完才进完成回调（置标志 + 重启）
3. **状态机永不脏**：
   - `RxState` 始终是 `BUSY_RX`（被回调保证连续）
   - `TxState` 每次进 `if(rx_done)` 时是 `READY`（发送完才回 `READY`），调用 `Transmit_IT` 必然成功
4. **出错不致命**：
   - ORE/FE/NE → `ErrorCallback` 清错误 + 重启接收
   - `Error_Handler()` 仅在 `HAL_RCC_*` 等真正致命错误时触发，且只做软复位，不再关中断

