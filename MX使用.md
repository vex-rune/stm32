# STM32CubeMX完整使用教程总结（适配你的STM32F103ZET6项目）
## 前期准备
1. 安装匹配芯片的固件包：打开CubeMX进入`Help → Manage embedded software packages`，下载`STM32F1 Cube MCU Package`对应版本固件。
2. 新建项目：主页点击`New Project`，在芯片检索栏搜索`STM32F103ZET6`，选中LQFP144封装，完成芯片选定。
3. 存放规范：后续工程保存路径全程使用英文，不可出现中文、空格、特殊符号，规避编译出错。

## 第一步：Pinout & Configuration引脚与外设配置
### 1. 系统核心System Core配置
（1）RCC时钟
- HSE选择`Crystal/Ceramic Resonator`，适配板载8MHz外部晶振；需要RTC功能就将LSE同样设置晶振模式适配32.768kHz的低速晶振。
- 跳转`Clock Configuration`标签，设置HSE输入频率8MHz，PLL倍频×9得到72MHz主频；总线分频设置：AHB不分频、APB1二分频、APB2不分频。
（2）SYS设置
Debug选择`Serial Wire`，适配ST‑Link SW下载调试；Timebase Source选择TIM6，保障`HAL_Delay`延时稳定。
（3）NVIC：入门阶段沿用默认优先级；后续启用串口、触控、定时器中断时，再在此处调配抢占优先级与子优先级。

### 2. GPIO配置
1. 在芯片预览图点击目标引脚，下拉选择`GPIO_Output`（输出，适配LED）或者`GPIO_Input`（输入，适配触控检测按键）；
2. 进入System Core→GPIO精细化配置：推挽/开漏模式、上下拉选项、输出速率，还可以填写User Label自定义引脚别名，优化代码可读性；
3. BOOT0引脚固定处于Reset_State，硬件完成启动电平控制，禁止软件修改。

### 3. 通信外设（Connectivity分组）按需开启
- U(S)ART：选择Asynchronous异步模式，配置波特率、校验位等参数，引脚会自动复用；
- SPI / CAN：点开对应外设选项即可自动锁定引脚；
- FSMC：适配LCD屏幕，选定NOR/SRAM模式、匹配原理图的片选通道，微调时序参数。

### 4. 其他外设
定时器、ADC、DMA、RTC等模块依照项目需求逐个启用，留意引脚复用冲突。

## 第二步：Project Manager工程参数配置
### 1. Project子标签
填写项目名称、存储路径，选择适配IDE：Keil MDK‑ARM V5 或者 STM32CubeIDE，设置堆栈大小（新手保持默认即可）。
### 2. Code Generator子标签（沿用你当下合适的配置）
✅ 库模式：选中`Copy only the necessary library files`
✅ Generated files全部勾选：
1. 外设拆分生成成对的.c/.h文件
2. 重新生成时备份旧文件
3. 保留USER CODE区间的自定义代码（重中之重）
4. 自动清理废弃的旧文件
❌ 常规开发不勾选两个HAL Settings复选框，低功耗场景再酌情开启
✅ 脚本模板（User Actions、Template Settings）入门阶段保持默认空白

## 第三步：生成代码
点击右上角`GENERATE CODE`等待生成完成，弹窗可直接跳转IDE打开工程。

## 第四步：IDE端后续开发规范
1. 代码编写限制：自己写的业务代码必须放进`/* USER CODE BEGIN ... */`和`/* USER CODE END ... */`之间，重新生成CubeMX代码时才不会被覆盖；不要手动修改外设自动生成的初始化源码。
2. 最小验证：优先写LED循环翻转代码，测试时钟、GPIO、下载链路是否正常。
3. 驱动开发：屏幕、SPI外设、CAN、触摸芯片等外设单独新建驱动文件夹存放代码。
4. 编译下载：完成代码编写后进行编译，排查警告报错；借助ST‑Link下载，可开启断点调试排查问题。

## 高频踩坑清单💡
1. 烧录失败：检查硬件BOOT0接地、SWD调试模式已经开启、下载接线牢靠；
2. 时钟异常：确认HSE选型正确，时钟树没有出现红色超限提示；
3. 引脚冲突：复用外设前核对原理图，避免同一个引脚被多个外设占用；
4. 版本留存：妥善备份.ioc工程文件，后续修改硬件配置要回到CubeMX调整ioc，不要直接改动初始化源文件；
5. 移植项目：完整拷贝整个工程文件夹，不要单独零散复制源码。