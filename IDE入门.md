
# STM32F103ZET6 点灯入门完整文档（CubeMX + STM32CubeIDE）
## 文档说明
适用人群：STM32零基础入门
目标功能：控制PA0引脚LED周期性500ms闪烁
工具：STM32CubeMX 图形配置工具 + STM32CubeIDE 编译下载调试软件
芯片：STM32F103ZET6

# 一、前期准备工作
## 1. 软件安装与固件包
1. 安装STM32CubeMX、STM32CubeIDE两个软件
2. 打开CubeMX → 顶部菜单栏 `Help` → `Manage embedded software packages`
3. 找到 `STM32F1 Series MCU`，下载对应固件包（STM32Cube_FW_F1）

## 2. 新建CubeMX工程
1. 打开CubeMX，点击`New Project`
2. 芯片检索框输入：`STM32F103ZET6`，选中LQFP144封装芯片，点击`Start Project`
3. 工程保存要求：**文件夹全英文，无中文、空格、特殊符号**

# 二、CubeMX硬件配置（核心步骤）
## 2.1 系统核心SYS配置
左侧菜单栏 `System Core` → `SYS`
1. Debug：选择 `Serial Wire`（ST-Link下载调试必备，不选无法烧录程序）
2. Timebase Source：选择 `TIM6`（保证HAL_Delay延时函数正常工作）

## 2.2 时钟RCC配置
左侧菜单栏 `System Core` → `RCC`
1. High Speed Clock (HSE)：选择 `Crystal/Ceramic Resonator`（开发板8MHz外部晶振）
2. Low Speed Clock (LSE)：`Crystal/Ceramic Resonator`（32.768K晶振，RTC时钟，可选）
3. 切换顶部标签 `Clock Configuration`
    - HSE输入频率填写8MHz
    - PLL倍频设置×9，系统总主频72MHz
    - AHB预分频：1；APB1预分频：2；APB2预分频：1
    - 界面无红色报错即代表时钟配置正确

## 2.3 LED引脚GPIO配置
1. 在芯片引脚预览图，点击引脚`PA0`，下拉选择 `GPIO_Output`
2. 左侧 `System Core` → `GPIO`，选中PA0进行参数设置：
    - GPIO output level：Low（上电默认低电平）
    - GPIO mode：Output Push Pull（推挽输出）
    - Pull-up/Pull-down：No pull-up and no pull-down
    - Maximum output speed：Low
    - User Label：填写`LED1`（代码内自动生成别名，方便识别）

## 2.4 工程输出参数设置（Project Manager）
切换顶部 `Project Manager` 标签
### 1. Project 子页面
1. Project Name：填写项目名（例：01_led）
2. Project Location：选择英文路径文件夹
3. Toolchain/IDE：选择 `STM32CubeIDE`

### 2. Code Generator 子页面（按下图勾选）
1. 库文件选项：`Copy only the necessary library files`
2. Generated files 全部勾选：
    - Generate peripheral initialization as a pair of .c/.h files per peripheral
    - Backup previously generated files when re-generating
    - Keep User Code when re-generating（**最重要，自定义代码不会被覆盖**）
    - Delete previously generated files when not re-generated
3. HAL Settings 两个复选框全部不勾选

## 2.5 生成工程代码
点击右上角蓝色按钮 `GENERATE CODE`，等待代码生成完成，弹窗选择打开工程文件夹。

# 三、STM32CubeIDE导入工程（解决路径报错）
## 3.1 报错原因回顾
报错提示：`Inconsistency detected between the project and workspace`
产生原因：工作区文件夹 和 项目文件夹路径完全一致，IDE不允许嵌套重合。

## 3.2 正确导入步骤
1. 关闭报错弹窗，IDE弹出选择工作空间窗口
2. 工作区选择**项目上级文件夹**，示例：
    项目路径：`C:\workspace\stm32\src\01_led`
    工作区路径：`C:\workspace\stm32`
3. 进入IDE主页，点击左侧 `Import STM32 project`
4. 选中生成好的`01_led`工程文件夹，完成导入

# 四、编写点灯代码（入门唯一需要手写代码区域）
## 4.1 打开源码文件
左侧工程目录展开 `Core` → `Src` → 双击打开 `main.c`

## 4.2 固定代码写入位置
所有自定义代码必须写在 `/* USER CODE BEGIN 数字 */` 和 `/* USER CODE END 数字 */` 之间，重新生成CubeMX配置不会删除你的代码。

## 4.3 完整点灯代码
找到主函数while(1)死循环，在USER CODE BEGIN 3区块写入：
```c
while (1)
{
  /* USER CODE BEGIN 3 */
  // 翻转PA0引脚电平，LED亮灭切换
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
  // 延时500毫秒
  HAL_Delay(500);
  /* USER CODE END 3 */
}
```

## 4.4 代码说明
1. `HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0)`：翻转PA0引脚高低电平
2. `HAL_Delay(500)`：系统延时500ms，单位ms

# 五、编译、硬件接线、下载调试
## 5.1 硬件接线
ST-Link调试器与开发板接线：
1. ST-Link SWDIO ↔ 开发板PA13
2. ST-Link SWCLK ↔ 开发板PA14
3. ST-Link 3.3V ↔ 开发板3.3V
4. ST-Link GND ↔ 开发板GND

## 5.2 编译工程
1. 点击顶部工具栏锤子图标 `Build` 编译项目
2. 控制台输出 `0 Errors, 0 Warnings` 代表编译成功

## 5.3 下载程序到开发板
1. 点击工具栏虫子图标 `Debug` 进入调试模式
2. 程序自动下载到芯片Flash
3. 点击运行按钮，开发板PA0对应LED开始500ms闪烁

# 六、工程目录结构入门讲解
```
01_led/
├── 01_led.ioc               # CubeMX配置源文件（修改硬件配置只用这个）
├── Core/                    # 核心用户代码目录（我们主要修改这里）
│   ├── Inc/                 # 头文件 .h
│   │   ├── main.h
│   │   ├── gpio.h
│   │   └── rcc.h
│   └── Src/                 # 源文件 .c
│       ├── main.c           # 主函数，点灯代码写在这里
│       ├── gpio.c           # 自动生成GPIO初始化代码，禁止修改非USER区域
│       └── rcc.c            # 时钟初始化代码
├── Drivers/                 # ST官方HAL底层库，全程禁止手动修改
│   ├── CMSIS/               # 芯片内核、寄存器定义
│   └── STM32F1xx_HAL_Driver # GPIO、延时等底层驱动函数
└── STM32F103ZET6_FLASH.ld   # 内存分配链接脚本，入门无需改动
```

# 七、入门常见问题排查
## 1. ST-Link无法识别/下载失败
1. CubeMX内SYS未开启Serial Wire调试模式
2. SWDIO、SWCLK接线接反、虚接
3. 开发板未上电

## 2. LED不闪烁
1. 引脚配置错误，核对原理图LED实际引脚
2. 电平逻辑相反：硬件LED高电平点亮，代码需要更换`HAL_GPIO_WritePin`
3. 时钟配置错误，时钟树出现红色警告

## 3. 重新生成代码后自己写的代码消失
未勾选Code Generator页面的 `Keep User Code when re-generating`，自定义代码必须放在USER CODE注释区块内。

## 4. CubeIDE工作空间路径报错
工作区文件夹不能和项目文件夹完全一致，工作区选项目的上级目录。

# 八、入门开发硬性规则（必看）
1. 硬件引脚、时钟、外设配置，**只在CubeMX内修改.ioc文件**，不要手动改gpio.c/rcc.c
2. 所有手写业务代码，仅允许写在`USER CODE BEGIN/END`注释中间
3. Drivers文件夹下所有官方库文件，禁止手动修改
4. 工程路径全程英文，禁止中文、空格、中文文件夹名
5. 每次修改CubeMX配置后，必须重新点击GENERATE CODE刷新初始化代码