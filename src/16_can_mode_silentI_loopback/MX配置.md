# CAN 配置参数说明

本文档基于 STM32CubeMX 配置界面截图，详细解释 CAN 模块的各项配置参数。

---

## 1. 模式配置 (Mode)

### CAN_MODE_SILENT_LOOPBACK (静默回环模式)

当前配置：`Silent Loopback`

| 特性 | 说明 |
|------|------|
| **发送行为** | 发送的报文不输出到 CAN 总线引脚（PB9），仅在内部回环 |
| **接收行为** | 能接收自身发送的报文（自发自收） |
| **总线状态** | 不影响总线，不会干扰其他节点 |

> **应用场景**：主要用于单芯片自测试，无需外部 CAN 节点即可验证 CAN 外设功能。

### 其他可选模式

| 模式 | 说明 |
|------|------|
| `Normal` | 正常模式：收发正常，连接 CAN 总线 |
| `Silent` | 静默模式：只能监听总线，不能发送 |
| `Loopback` | 回环模式：发送正常，接收自身报文 |
| `Silent Loopback` | **当前配置**：静默 + 回环组合 |

---

## 2. 时序参数

### 波特率计算公式

```
波特率 = APB1时钟频率 / (Prescaler × (BS1 + BS2 + 1))
```

### 当前配置

| 参数 | 配置值 | 说明 |
|------|--------|------|
| **Prescaler (预分频器)** | 36 | APB1 时钟 36MHz 被 36 分频 |
| **BS1 (时间段1)** | 3 TQ | CAN_BS1_3TQ |
| **BS2 (时间段2)** | 6 TQ | CAN_BS2_6TQ |
| **SJW (同步跳转宽度)** | 2 TQ | CAN_SJW_2TQ |

### 计算结果

- **Time Quantum (时间量子)**: 1000.0 ns
- **Time Bit (位时间)**: 10000 ns
- **Baudrate (波特率)**: 100 Kbps

### 位时序结构图

```
|<------------------- 1 Bit Time ------------------->|
|        |<---- BS1 ---->|<- BS2 ->|
|_______|___________________|_____|...
|  SS   |        BS1        |  SJW |  SJW (同步跳转宽度)
| (同步)|   (相位缓冲段1)    | (相位缓冲段2)
```

---

## 3. 自动管理功能

### ABOM - 自动总线关闭管理 (Automatic Bus-Off Management)

```
当前状态: ENABLE (启用)
```

| 状态 | 行为 |
|------|------|
| **ENABLE** | 总线错误次数过多进入 "Bus-Off" 状态时，CAN 控制器自动退出，无需软件干预 |
| **DISABLE** | 需要软件手动清除 bxCAN_ESR 寄存器中的 INAK 位来恢复 |

### AWUM - 自动唤醒模式 (Automatic Wakeup Mode)

```
当前状态: ENABLE (启用)
```

| 状态 | 行为 |
|------|------|
| **ENABLE** | CAN 总线检测到唤醒信号时，自动退出低功耗模式，无需软件干预 |
| **DISABLE** | 需要软件手动清除 CAN_MCR 寄存器中的 AWUM 位来退出低功耗模式 |

---

## 4. 发送控制

### NART - 自动重传 (No Automatic Retransmission)

```
当前状态: DISABLE (禁用 - 启用自动重传)
```

| 状态 | 行为 |
|------|------|
| **ENABLE** | 发送失败后不自动重传，需软件处理 |
| **DISABLE** | 发送失败后自动重传，直到成功（默认） |

> **当前配置说明**：DISABLE 表示启用自动重传功能，发送失败会持续重试。

---

## 5. 引脚配置

### CAN 通信引脚

| 引脚 | 功能 | 配置模式 | 说明 |
|------|------|----------|------|
| **PB8** | CAN_RX (接收) | Input (输入) | 接收 CAN 总线数据 |
| **PB9** | CAN_TX (发送) | Alternate Push-Pull (复用推挽输出) | 发送 CAN 总线数据 |

### 引脚特性

- **GPIO Speed**: High Speed
- **Pull/None**: No Pull
- **Remap**: CAN1 映射到 PB8/PB9（通过 `__HAL_AFIO_REMAP_CAN1_2()` 实现）

---

## 6. 硬件参数

| 参数 | 值 | 说明 |
|------|-----|------|
| **芯片型号** | STM32F103ZET6 | LQFP144 封装 |
| **CAN 控制器** | CAN1 | bxCAN 外设 |
| **APB1 时钟** | 36 MHz | CAN 使用 APB1 时钟 |

---

## 7. 完整配置对照表

```
CAN 配置参数
├── Mode:           CAN_MODE_SILENT_LOOPBACK (静默回环模式)
├── Prescaler:     36
├── BS1:           CAN_BS1_3TQ (3 个时间量子)
├── BS2:           CAN_BS2_6TQ (6 个时间量子)
├── SJW:           CAN_SJW_2TQ (2 个时间量子)
├── ABOM:          ENABLE (自动总线关闭管理)
├── AWUM:          ENABLE (自动唤醒模式)
├── NART:          DISABLE (启用自动重传)
└── 波特率:        100 Kbps
```

---

## 8. 快速参考

- **需要连接真实 CAN 总线？** → 改用 `CAN_MODE_NORMAL`
- **只需自测？** → 当前 `SILENT_LOOPBACK` 配置即可
- **调整波特率？** → 修改 Prescaler、BS1、BS2 参数
