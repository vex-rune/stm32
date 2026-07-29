# HAL 库 CAN 全套操作汇总（STM32 HAL_CAN）

---

## 一、初始化 / 基础配置类

| 函数 | 说明 |
|------|------|
| `MX_CAN_Init()` | 自动生成初始化函数，配置波特率、模式（正常/回环/静默）、位时序、过滤器、中断使能 |
| `HAL_CAN_Init(&hcan)` | 初始化 CAN 外设，加载 hcan 结构体参数 |
| `HAL_CAN_DeInit(&hcan)` | 注销 CAN 外设，恢复寄存器默认状态 |
| `HAL_CAN_MspInit(&hcan)` | 底层初始化：GPIO、时钟、NVIC 中断配置（自动生成，用户可修改） |
| `HAL_CAN_MspDeInit(&hcan)` | 底层反初始化，关闭时钟、释放 GPIO |

---

## 二、过滤器配置（核心，接收报文必须配置）

| 函数 | 说明 |
|------|------|
| `HAL_CAN_ConfigFilter(&hcan, &filter_config)` | 配置 CAN 过滤器：掩码/列表模式、32位/16位、绑定 FIFO0/FIFO1、过滤器启用 |

> **用途**：筛选目标 ID 报文，过滤无关数据

---

## 三、启动 / 停止 CAN 外设

| 函数 | 说明 |
|------|------|
| `HAL_CAN_Start(&hcan)` | 开启 CAN 外设，进入工作状态，可收发报文 |
| `HAL_CAN_Stop(&hcan)` | 关闭 CAN 外设，停止收发 |

---

## 四、发送报文

### 4.1 阻塞发送（轮询）
```c
HAL_CAN_AddTxMessage(&hcan, TxMailbox, &tx_header, tx_data)
```
- 将报文写入发送邮箱，阻塞直到发送成功/超时
- 返回发送邮箱编号

### 4.2 中断发送（异步）
```c
HAL_CAN_AddTxMessage_IT(&hcan, TxMailbox, &tx_header, tx_data)
```
- 开启发送中断，后台发送，不阻塞程序

### 4.3 DMA 发送（高速大批量）
```c
HAL_CAN_AddTxMessage_DMA(&hcan, TxMailbox, &tx_header, tx_data, len)
```
- DMA 搬运数据，适合连续大量报文

### 辅助发送函数

| 函数 | 说明 |
|------|------|
| `HAL_CAN_GetTxMailboxesFreeLevel(&hcan)` | 获取空闲发送邮箱数量 |
| `HAL_CAN_IsTxMessagePending(&hcan, mailbox)` | 查询指定邮箱是否待发送 |
| `HAL_CAN_AbortTxRequest(&hcan, mailbox)` | 中止未发送完成的报文 |

---

## 五、接收报文

### 5.1 轮询读取（阻塞）
```c
HAL_CAN_GetRxMessage(&hcan, FIFONumber, &rx_header, rx_data)
```
- 从 FIFO 读取一帧接收报文（标准/扩展、数据帧/远程帧自动解析）

### 5.2 中断接收（最常用）

#### 使用的具体中断

| 中断源 | 宏定义 | 说明 |
|--------|---------|------|
| FIFO0 有新报文 | `CAN_IT_RX_FIFO0_MSG_PENDING` | FIFO0 收到报文（最常用） |
| FIFO1 有新报文 | `CAN_IT_RX_FIFO1_MSG_PENDING` | FIFO1 收到报文 |
| FIFO0 满 | `CAN_IT_RX_FIFO0_FULL` | FIFO0 已满 |
| FIFO1 满 | `CAN_IT_RX_FIFO1_FULL` | FIFO1 已满 |
| FIFO0 溢出 | `CAN_IT_RX_FIFO0_OVERRUN` | FIFO0 数据被覆盖 |
| FIFO1 溢出 | `CAN_IT_RX_FIFO1_OVERRUN` | FIFO1 数据被覆盖 |

#### 使用步骤

**1. 使能接收中断**
```c
// 开启 FIFO0 接收中断
HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
```

**2. 重写中断回调函数（在 main.c 或 can.c 的 USER CODE 区域）**
```c
/* USER CODE BEGIN 4 */

// FIFO0 收到新报文回调（最常用）
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    uint8_t RxData[8];
    CAN_RxHeaderTypeDef RxHeader;

    // 读取接收数据
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
    {
        // 处理接收到的数据
        // RxHeader.StdId  - 标准 ID
        // RxHeader.DLC    - 数据长度
        // RxData[]        - 数据内容
    }
}

// FIFO0 溢出回调
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)
{
    // FIFO0 满了，调用清除溢出标志
    // HAL_CAN_ClearRxFifoOverrun(hcan, CAN_RX_FIFO0);
}

/* USER CODE END 4 */
```

#### 对比：轮询 vs 中断

| 方式 | 代码 | 优点 | 缺点 |
|------|------|------|------|
| **轮询** | `while(HAL_CAN_GetRxFifoFillLevel() == 0);` | 简单直接 | CPU 浪费等待，无法并行处理 |
| **中断** | 使能中断 + 回调函数 | CPU 高效，实时响应 | 需要中断处理函数 |

### 5.3 DMA 接收
```c
HAL_CAN_GetRxMessage_DMA(&hcan, FIFONumber, rx_buf, len)
```
- DMA 自动搬运接收数据，减少 CPU 占用

### 接收状态查询

| 函数 | 说明 |
|------|------|
| `HAL_CAN_GetRxFifoFillLevel(&hcan, FIFONumber)` | FIFO 缓存报文数量 |
| `HAL_CAN_IsRxFifoFull(&hcan, FIFONumber)` | FIFO 是否满（溢出风险） |
| `HAL_CAN_ClearRxFifoOverrun(&hcan, FIFONumber)` | 清除 FIFO 溢出标志 |

---

## 六、中断相关函数

### 6.1 开启 / 关闭中断

| 函数 | 说明 |
|------|------|
| `HAL_CAN_ActivateNotification(&hcan, interrupt_flags)` | 使能指定中断（发送完成、FIFO 接收、错误、总线离线等） |
| `HAL_CAN_DeactivateNotification(&hcan, interrupt_flags)` | 关闭指定中断 |

### 6.2 中断服务回调（重写实现业务逻辑）

| 回调函数 | 触发条件 |
|----------|----------|
| `HAL_CAN_TxMailbox0CompleteCallback(hcan)` | 邮箱 0 发送完成 |
| `HAL_CAN_TxMailbox1CompleteCallback(hcan)` | 邮箱 1 发送完成 |
| `HAL_CAN_TxMailbox2CompleteCallback(hcan)` | 邮箱 2 发送完成 |
| `HAL_CAN_RxFifo0MsgPendingCallback(hcan)` | FIFO0 收到新报文（最常用） |
| `HAL_CAN_RxFifo1MsgPendingCallback(hcan)` | FIFO1 收到新报文 |
| `HAL_CAN_RxFifo0FullCallback(hcan)` | FIFO0 溢出 |
| `HAL_CAN_RxFifo1FullCallback(hcan)` | FIFO1 溢出 |
| `HAL_CAN_ErrorCallback(hcan)` | CAN 总线错误（位错误、应答错误、总线关闭等） |

---

## 七、总线错误与状态检测

| 函数 | 说明 |
|------|------|
| `HAL_CAN_GetError(&hcan)` | 获取当前所有错误标志 |
| `HAL_CAN_GetTxErrorCounter(&hcan)` | 读取发送错误计数器 TEC |
| `HAL_CAN_GetRxErrorCounter(&hcan)` | 读取接收错误计数器 REC |
| `HAL_CAN_IsBusOff(&hcan)` | 判断 CAN 是否总线关闭（BusOff） |
| `HAL_CAN_AutoRecoveryBusOff(&hcan)` | 自动恢复 BusOff 状态（部分芯片支持） |
| `HAL_CAN_ResetError(&hcan)` | 清除所有错误标志 |

---

## 八、模式切换函数

| 模式 | 配置 | 说明 |
|------|------|------|
| **正常工作模式** | 默认 | 收发总线上所有匹配过滤器报文 |
| **回环模式** | `hcan.Init.Mode = CAN_MODE_LOOPBACK` | 自测，报文内部回环，不输出到总线 |
| **静默模式** | `hcan.Init.Mode = CAN_MODE_SILENT` | 只接收，禁止发送，总线监听 |
| **回环静默模式** | `hcan.Init.Mode = CAN_MODE_SILENT_LOOPBACK` | 纯自测，隔离外部总线 |

---

## 九、远程帧处理

| 操作 | 说明 |
|------|------|
| **发送远程请求帧** | 配置 `tx_header.RTR = CAN_RTR_REMOTE`，发送远程帧请求 |
| **接收远程帧** | `rx_header.RTR` 自动识别远程帧，可在回调中响应发送对应数据帧 |

---

## 十、常用结构体说明

| 结构体 | 用途 |
|--------|------|
| `CAN_HandleTypeDef hcan` | CAN 外设总句柄，存储寄存器、状态、错误 |
| `CAN_InitTypeDef hcan.Init` | 波特率、模式、中断配置 |
| `CAN_FilterTypeDef filter_config` | 过滤器 ID、掩码、FIFO 绑定 |
| `CAN_TxHeaderTypeDef tx_header` | 发送帧参数（ID 类型、帧类型、数据长度 DLC） |
| `CAN_RxHeaderTypeDef rx_header` | 接收帧解析结果（ID、RTR、IDE、DLC） |

---

## 速记表

| 分类 | 函数 |
|------|------|
| **初始化** | Init / DeInit / ConfigFilter / Start / Stop |
| **发送** | AddTxMessage (轮询/IT/DMA)、AbortTxRequest、GetTxMailboxesFreeLevel |
| **接收** | GetRxMessage、GetRxFifoFillLevel、ClearRxFifoOverrun |
| **中断** | ActivateNotification、各类收发错误回调 |
| **故障** | IsBusOff、GetTxErrorCounter/GetRxErrorCounter、GetError |
| **模式** | CAN_MODE_NORMAL / LOOPBACK / SILENT / SILENT_LOOPBACK |
