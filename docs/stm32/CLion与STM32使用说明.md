# CLion 与 STM32 使用说明

> 来源：BV1pnjizYEAk《爽！手把手教你用CLion开发STM32【大人，时代变啦！！！】》（UP：keysking）
> 整理日期：2026-07-15

## 1. 为什么选择 CLion 开发 STM32

视频作者给出的核心理由：

- AI 时代下，Keil、STM32CubeIDE 等传统 IDE 的体验已显陈旧。
- JetBrains 已开放 CLion **非商业免费**，个人学习不再有授权门槛。
- CLion 在以下方面明显优于传统 IDE：
  - 智能补全、跳转、重构（Refactor）能力远超 Keil/CubeIDE。
  - 内置 CMake/CMake Presets 支持，与 STM32CubeMX 生成的 CMake 工程天然契合。
  - 调试器前端（基于 LLDB/GDB）体验更现代，Watch、内存视图、反汇编一应俱全。
  - 插件生态丰富（串口插件、OpenOCD 集成、串口绘图 Serial Plotter 等）。

> 结论：**CLion 编辑代码 + STM32CubeMX 生成工程 + arm-none-eabi-gcc 编译 + OpenOCD 烧录调试** 是目前最舒适的 STM32 开发组合。

## 2. 工具链总览

| 角色 | 工具 | 备注 |
| --- | --- | --- |
| 编辑器 / IDE | CLion | 非商业免费版即可 |
| MCU 配置 | STM32CubeMX | 生成初始化代码与 CMake 工程 |
| 编译器 | arm-none-eabi-gcc | 工具链前缀需在 CLion 中配置 |
| 构建系统 | CMake + Ninja | CLion 内置 CMake 支持 |
| 烧录 / 调试 | OpenOCD | 同时支持 ST-Link、J-Link、DAP-Link |
| 串口工具 | CLion Serial Port Monitor 插件 / PuTTY / MobaXterm | 任选 |

## 3. 环境准备（Windows）

### 3.1 安装 CLion

1. 前往 JetBrains 官网下载 CLion。
2. 使用非商业（Non-Commercial）许可证激活（注册 JetBrains 账号即可）。
3. 首次启动进入 `Settings → Plugins`，推荐安装：
   - **Serial Port Monitor**：内置串口监听
   - **OpenOCD Support**（可选，提供图形化烧录）
   - **Markdown** / **Chinese Language Pack**（按需）

### 3.2 安装 STM32CubeMX

1. 从 ST 官网下载 STM32CubeMX。
2. 安装后首次启动会要求下载对应芯片的 **MCU Package**（例如 STM32F1 / F4 / H7）。
3. 在 `Help → Updater Settings` 中可管理已安装的固件包。

### 3.3 安装 arm-none-eabi-gcc

推荐使用 xpack-arm-toolchain 或 GNU Arm Embedded Toolchain：

- 下载后解压到无中文、无空格的目录，例如 `C:\toolchains\arm-none-eabi-gcc\`。
- 将其 `bin` 目录加入 `PATH`。
- 验证：
  ```bash
  arm-none-eabi-gcc --version
  ```

### 3.4 安装 OpenOCD

推荐使用 xpack-openocd：

- 下载解压到 `C:\toolchains\openocd\`。
- 将 `bin` 加入 `PATH`。
- 验证：
  ```bash
  openocd --version
  ```

### 3.5 安装 Ninja（可选）

```bash
choco install ninja
```

或使用 CLion 自带。

## 4. STM32CubeMX 生成 CMake 工程

1. 打开 CubeMX → `File → New Project`，选择芯片（例如 STM32F103ZET6）。
2. 配置外设：
   - RCC：HSE / 时钟树按需配置
   - SYS：Debug 选 `Trace Swd` 或 `Serial Wire`
   - GPIO / USART / TIM 等按需
3. `Project Manager`：
   - **Project Name**：例如 `01_led`
   - **Toolchain/IDE** 选 **`CMake`**
   - **Generate Under Root** 取消勾选（保持目录结构清晰）
4. `Code Generator`：勾选 `Generate peripheral initialization as a pair of .c/.h files per peripheral`。
5. 点击 `GENERATE CODE`，得到如下结构：
   ```
   01_led/
   ├── Core/
   ├── Drivers/
   ├── cmake/
   ├── .ioc
   ├── CMakeLists.txt
   ├── CMakePresets.json
   └── startup_stm32f103xe.s
   ```

## 5. 在 CLion 中打开工程

1. `File → Open`，选中 CubeMX 生成的 `CMakeLists.txt` 所在目录。
2. CLion 识别 `CMakePresets.json`，弹出工具链选择：
   - **Toolchain**：`arm-none-eabi-gcc`（CLion 通常能自动检测到）
   - **Generator**：Ninja
   - **Build/Debug/Release** Preset：按需
3. 若 CLion 未自动识别编译器：
   - `Settings → Build, Execution, Deployment → Toolchains`
   - 添加 `System` toolchain，**C Compiler** / **C++ Compiler** 指向
     ```
     C:\toolchains\arm-none-eabi-gcc\bin\arm-none-eabi-gcc.exe
     ```
     `arm-none-eabi-g++.exe`
4. 右下角状态栏切换到 `Debug` 或 `Release`，点击 **Build**，确认能产出 `.elf` / `.bin` / `.hex`。

## 6. 烧录与调试配置（OpenOCD）

### 6.1 添加 Run/Debug Configuration

1. 右上角 `Add Configuration → OpenOCD Download & Run`。
2. 关键参数：
   - **Executable**：`<project>/cmake-build-debug/01_led.elf`
   - **Board config file**：选择 OpenOCD 自带的板级脚本，例如
     - `interface/stlink.cfg` + `target/stm32f1x.cfg`
     - 或合并写法 `-f board/stm32f103c8_bluepill.cfg`
   - **Reset**：勾选 `reset`
   - **Pre-download commands**：可留空
3. 保存后点击 **Debug**（虫子图标），CLion 会自动调用 OpenOCD 烧录并停在 `main()` 第一行。

### 6.2 常用调试操作

| 操作 | 快捷键 |
| --- | --- |
| 单步跳过 | `F8` |
| 单步进入 | `F7` |
| 跳出函数 | `Shift + F8` |
| 继续运行 | `F9` |
| 复位 MCU | `Ctrl + Shift + F9`（需配合 OpenOCD） |
| 添加断点 | 行号旁单击 |
| 查看外设寄存器 | `Run → View → Specials → Registers` |

## 7. 一个最小示例：点亮 LED

```c
/* main.c 中 while 循环 */
while (1)
{
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
    HAL_Delay(500);
}
```

CubeMX 中需要：

1. 将 `PB5` 配置为 `GPIO_Output`。
2. 在 `Project Manager → Code Generator` 中勾选
   `Initialize all peripherals with their default Mode`（或单独初始化）。

烧录后即可看到 LED 以 1Hz 闪烁。

## 8. CLion 在 STM32 开发中的高阶技巧

1. **Live Templates**：在 `Settings → Editor → Live Templates` 中添加
   `HAL_GPIO_WritePin(..., GPIO_PIN_SET);` 等代码片段，减少手敲。
2. **CMake Presets 多配置**：通过 `CMakePresets.json` 同时管理 Debug / Release / 自定义下载器。
3. **串口打印**：
   - 在 CubeMX 中开启 USART1（115200 8N1）。
   - 重定向 `printf` 至 `HAL_UART_Transmit`。
   - 配合 CLion `Serial Port Monitor` 插件实时查看日志。
4. **Static Analysis**：
   - `Code → Inspect Code` 可发现未初始化变量、内存泄漏等隐患。
5. **Git 集成**：CLion 内置 Git 工具窗口，提交粒度可视化，适合配合本仓库的提交规范使用。
6. **AI Assistant**（JetBrains AI）：开启后可对 CubeMX 生成的 HAL 代码进行解释与重构。

## 9. 与传统开发方式的对比

| 维度 | Keil / CubeIDE | CLion + CubeMX + OpenOCD |
| --- | --- | --- |
| 智能补全 | 一般 | 极强 |
| 重构 | 弱 | 完善 |
| CMake 支持 | 不支持 | 原生 |
| 调试前端 | 较老 | 现代化 |
| 跨平台 | 仅 Windows | Windows / macOS / Linux |
| 学习成本 | 低 | 中（需熟悉 CMake） |
| 授权 | Keil 收费 | 非商业免费 |

## 10. 常见问题

| 问题 | 排查方向 |
| --- | --- |
| `arm-none-eabi-gcc` 未识别 | 检查 PATH 与 `Settings → Toolchains` 中的 C Compiler 路径 |
| OpenOCD 报 `Can't find board config` | 路径写错或 `-f` 后参数遗漏 |
| 烧录失败 `Error: open failed` | 检查 ST-Link 驱动；Win10/11 需使用 WinUSB 版驱动（Zadig 安装） |
| 调试断点不生效 | 优化等级过高（`-O0`）或 Release 编译被优化掉 |
| 中文路径报错 | 工程路径中禁止出现中文、空格、括号 |
| CubeMX 重新生成代码后自定义代码丢失 | 必须写在 `USER CODE BEGIN / END` 区域内 |

## 11. 推荐实践

1. **版本管理**：将 `Drivers/`、`Core/`、`cmake/`、`CMakeLists.txt`、`CMakePresets.json`、启动文件、链接脚本纳入 Git；`cmake-build-*` 加入 `.gitignore`。
2. **CubeMX 修改后再生成**：每次改动 `.ioc` 后，先在 CubeMX 中重新生成，再到 CLion 中 Reload CMake Project。
3. **统一工具链版本**：团队成员使用同一版本 `arm-none-eabi-gcc` 与 OpenOCD，避免链接脚本差异。
4. **自动化构建**：可在 CI 中直接调用 `cmake -B build -G Ninja && cmake --build build`，产出 `.bin` 后用 `openocd` 远程烧录。
5. **善用 Serial Plotter**：调试 PID、ADC、波形时非常直观。

---

> 本文档为对原视频内容的结构化总结，建议结合视频实际操作完成首次环境搭建。如需进一步深入 CMake 自定义、链接脚本、FreeRTOS 集成等内容，可在本仓库 `进阶篇/` 目录下查阅对应章节。
