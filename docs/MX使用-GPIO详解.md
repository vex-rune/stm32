# STM32CubeMX GPIO 配置详解

> 适配 STM32F103ZET6 / F1 系列。

---

## 一、配置流程（4 步）

```
1. 芯片视图点击引脚      → 选 GPIO_Output / GPIO_Input / 复用功能
2. 左侧 System Core → GPIO → 选中具体引脚
3. 右侧面板设置 模式 + 上下拉 + 速度 + User Label
4. 已配置引脚显示绿色（已设）/ 黄色（复用）
```

---

## 二、配置项逐项解释

### 1. GPIO mode（工作模式）— 重点

| 选项 | 含义 | 典型用途 |
| --- | --- | --- |
| **Input floating** 浮空输入 | 引脚电平完全由外部决定，内部无上下拉 | 外部已带上下拉的信号（如 I2C） |
| **Input pull-up** 输入上拉 | 内部接上拉电阻，默认高电平 | 按键检测（按下低电平） |
| **Input pull-down** 输入下拉 | 内部接下拉电阻，默认低电平 | 按键检测（按下高电平） |
| **Analog** 模拟输入 | 引脚直连 ADC，无数字施密特触发 | ADC 通道采集、模拟比较 |
| **Output push-pull** 推挽输出 | 高低电平均可主动驱动 | LED、数码管、通用输出 |
| **Output open-drain** 开漏输出 | 只能主动拉低；输出高需外部上拉 | I2C 总线、电平转换、线与 |
| **AF push-pull** 复用推挽 | 引脚交给片内外设控制，推挽方式 | USART TX、SPI MOSI、FSMC |
| **AF open-drain** 复用开漏 | 交给片内外设控制，开漏方式 | I2C 外设引脚 |

### 2. GPIO Pull-up / Pull-down（上下拉电阻）

仅输入模式生效，输出模式一般无意义（开漏输出例外）：

- **No pull-up and no pull-down**：浮空，内部上下拉全关
- **Pull-up**：接 ~30~50kΩ 上拉到 VDD
- **Pull-down**：接 ~30~50kΩ 下拉到 VSS

> STM32 内部上下拉电阻阻值较大（数十 kΩ），不能用于驱动，仅作电平偏置。

### 3. GPIO output speed（输出速度）— 重点

STM32F1 系列可选三档，对应 MODEy 位：

| 选项 | 含义 | 应用 |
| --- | --- | --- |
| Low | 2 MHz | 低功耗、低速信号 |
| Medium | 10 MHz | 默认；多数情况够用 |
| High | 50 MHz | 高速：SPI、FSMC、复用外设 |

> 速度越快 → 翻转斜率越陡 → EMI 越严重。不需要时不要无脑选 High。

### 4. User Label（用户标签）— 强烈推荐

给引脚起一个易读的别名（如 `LED1`、`KEY1`、`LCD_BL`），生成代码时会：

- 自动定义宏 `#define LED1_Pin GPIO_PIN_0` 等
- 代码里写 `HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET)` 而非裸引脚号

可读性、防错都靠它，强烈建议每个用到的引脚都填。

### 5. 已分配外设查看

引脚状态颜色提示：

- 🟢 绿色 = 已配 GPIO 功能
- 🟡 黄色 = 被片内外设复用（如 USART1_TX）
- ⬜ 灰色 = 未配置

---

## 三、配置重点 & 易踩坑

| 重点 / 坑 | 正确做法 |
| --- | --- |
| **LED 灯不亮** | ① 模式选 `Output push-pull` ② 看原理图确认高/低电平点亮 ③ 检查引脚是否被其它外设占用 |
| **按键一直触发** | 浮空输入 → 加上下拉；输入浮空+无外部电阻=电平随机 |
| **I2C 不通** | 必须选 `AF open-drain` + 外接上拉（或确认内部是否使能） |
| **高速通信波形失真** | SPI / FSMC / 高速 USART → 输出速度必须选 High |
| **功耗偏高** | 不用的引脚留为 Analog 或 Input pull-down，禁浮空 |
| **BOOT0 引脚** | 保持 `Reset_State`，禁止软件改动（由硬件决定启动模式） |
| **JTAG / SWD 引脚** | PA13/PA14/PA15/PB3/PB4 默认被 JTAG 占用，开启 SWD 后 PB3/PB4 才会释放 |
| **同名引脚** | 不要把同一引脚既配 GPIO_Output 又配外设，会冲突 |
| **复用冲突** | 开启 USART1 后 TX/RX 引脚自动锁定，不能再随意改 |

---

## 四、与寄存器位的对应关系（了解即可）

MX 设置最终写入 GPIO 的 `CRL` / `CRH` 寄存器，每 4 位控制一个引脚：

```
bit[1:0] MODEy  → 00=输入  01=输出10MHz  10=输出2MHz  11=输出50MHz
bit[3:2] CNFy   → 输入: 00=浮空 01=上拉 10=下拉 11=保留
                  输出: 00=推挽 01=开漏 10=AF推挽 11=AF开漏
```

例如 LED 输出：MODEy=11 (50MHz) + CNFy=00 (推挽) → 0x3 → `GPIO_SetBits` 即可点亮。