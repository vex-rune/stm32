# CMake 入门教程

本教程结合本项目 [stm32cubemx/CMakeLists.txt](file:///c:/workspace/stm32/src/18_web_server/cmake/stm32cubemx/CMakeLists.txt) 的实际代码讲解 CMake 核心概念，适合 STM32 嵌入式开发学习。

---

## 一、CMake 是什么

CMake 是一个**构建系统生成器**：
- 读 `CMakeLists.txt` → 生成 `Makefile`（或 `Ninja`、IDE 工程等） → 由真正的构建工具（make、ninja）去编译

```
CMakeLists.txt  →  CMake  →  Makefile / build.ninja  →  make / ninja  →  可执行文件
```

最小示例：

```cmake
cmake_minimum_required(VERSION 3.22)
project(hello)
add_executable(hello main.c)
```

---

## 二、常用变量

### 内置变量

| 变量 | 含义 |
|---|---|
| `CMAKE_CURRENT_SOURCE_DIR` | 当前处理的 CMakeLists.txt 所在目录 |
| `CMAKE_SOURCE_DIR` | 顶层 CMakeLists.txt 所在目录（项目根） |
| `CMAKE_PROJECT_NAME` | `project()` 设置的项目名 |
| `CMAKE_BUILD_TYPE` | 构建类型（`Debug` / `Release`） |
| `CMAKE_C_COMPILER` / `CMAKE_CXX_COMPILER` | C / C++ 编译器路径 |
| `CMAKE_C_STANDARD` | C 语言标准版本（如 11、17） |

### 自定义变量

用 `set()` 定义，用 `${变量名}` 引用：

```cmake
set(MY_SRCS main.c util.c)
target_sources(my_app PRIVATE ${MY_SRCS})
```

---

## 三、`set()` —— 定义变量

```cmake
set(Ethernet_Drivers_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/Ethernet/socket.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/Ethernet/w5500.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/Ethernet/wizchip_conf.c
)
```

- `set(变量名 值1 值2 ...)`：用空格或换行分隔多个值
- `${变量名}`：取值
- 多个值之间用分号 `;` 分隔（换行或空格都行）

本项目因为 `stm32cubemx/CMakeLists.txt` 在 `cmake/stm32cubemx/` 下，所以用 `/../../` 跳两级回到项目根。

---

## 四、`add_executable()` 与 `add_library()`

### `add_executable(<名> 源文件...)`

创建一个可执行文件目标：

```cmake
add_executable(${CMAKE_PROJECT_NAME})
```

文件可以后面用 `target_sources` 添加，也可以直接列在括号里。

### `add_library(<名> <类型> 源文件...)`

创建一个库目标。常见类型：

| 类型 | 含义 |
|---|---|
| `STATIC` | 静态库（`.a` / `.lib`） |
| `SHARED` | 动态库（`.so` / `.dll`） |
| `OBJECT` | 只编译成目标文件，不打包成库，最后由链接器统一链入 |
| `INTERFACE` | 虚拟库，不编译任何东西，仅用于传递编译选项 |

本项目里：

```cmake
add_library(stm32cubemx INTERFACE)    # 虚拟库，仅用于传递 include/宏
add_library(STM32_Drivers OBJECT)     # 编译为目标文件，最后链入可执行文件
```

为什么用 `OBJECT`？因为 STM32CubeMX 习惯把所有驱动编译成一个整体，再用链接器统一链接，避免重复符号、简化链接脚本。

---

## 五、给目标加东西：`target_*` 系列

CMake 的现代写法是"以目标为中心"，所有配置都挂在目标名上：

| 命令 | 作用 |
|---|---|
| `target_sources(<目标> <可见性> 文件...)` | 加源文件 |
| `target_include_directories(<目标> <可见性> 路径...)` | 加头文件搜索路径 |
| `target_compile_definitions(<目标> <可见性> 宏...)` | 加宏定义 |
| `target_compile_options(<目标> <可见性> 选项...)` | 加编译选项 |
| `target_link_libraries(<目标> ...)` | 链接库 / 传递依赖 |

### 可见性关键字

| 关键字 | 作用 |
|---|---|
| `PRIVATE` | 只在自己编译 / 链接时使用，不向下游传递 |
| `PUBLIC` | 自己用，且下游链接此库的目标也能用 |
| `INTERFACE` | 自己不用，只传递给下游 |

举例：

```cmake
target_sources(STM32_Drivers PRIVATE
    ${STM32_Drivers_Src}
    ${Ethernet_Drivers_Src}
    ${HttpServer_Drivers_Src}
    ${App_Src}
)
```

STM32 驱动源文件只在编译 `STM32_Drivers` 时需要，对链接它的主可执行文件无意义，所以用 `PRIVATE`。

---

## 六、`INTERFACE` 库的妙用

```cmake
add_library(stm32cubemx INTERFACE)
target_include_directories(stm32cubemx INTERFACE ${MX_Include_Dirs})
target_compile_definitions(stm32cubemx INTERFACE ${MX_Defines_Syms})

add_library(STM32_Drivers OBJECT)
target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)
```

`stm32cubemx` 不编译任何代码，但把所有 include 路径和宏定义打包在一起。
`STM32_Drivers` 用 `PUBLIC` 链接它 → `STM32_Drivers` 自己能用，下游链接 `STM32_Drivers` 的目标也能用。

这是 STM32CubeMX 生成的 CMake 工程标准模式。

---

## 七、`target_link_libraries()` —— 链接库

```cmake
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
)
```

把库链入可执行文件，链接器在生成最终 `.elf` 时会找到符号定义。

---

## 八、`message()` —— 打印信息

```cmake
message("Build type: " ${CMAKE_BUILD_TYPE})
message(ERROR "Generated code requires C11 or higher")
```

- 不带级别 / `STATUS` / `WARNING` / `AUTHOR_WARNING` / `SEND_ERROR` / `FATAL_ERROR`
- `FATAL_ERROR` 会停止配置；`ERROR` 等同于 `SEND_ERROR`

本项目里的版本检查：

```cmake
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()
```

---

## 九、条件与控制流

```cmake
if(STARM_TOOLCHAIN_CONFIG STREQUAL "STARM_HYBRID")
    set(TOOLCHAIN_MULTILIBS "...")
elseif(STARM_TOOLCHAIN_CONFIG STREQUAL "STARM_NEWLIB")
    set(TOOLCHAIN_MULTILIBS "...")
elseif(STARM_TOOLCHAIN_CONFIG STREQUAL "STARM_PICOLIBC")
    set(TOOLCHAIN_MULTILIBS "...")
endif()
```

- `if()` / `elseif()` / `endif()`：标准三段式
- 比较运算符：`STREQUAL`（字符串相等）、`EQUAL`（数值相等）、`LESS`、`GREATER` 等

---

## 十、本项目的整体结构

```
18_web_server/
├── CMakeLists.txt                ← 顶层：定义项目名、构建类型，加载子目录
├── cmake/
│   ├── gcc-arm-none-eabi.cmake   ← GCC 工具链配置
│   ├── starm-clang.cmake         ← Clang 工具链配置
│   └── stm32cubemx/
│       └── CMakeLists.txt        ← 子模块：定义 include、宏、所有源文件
├── Core/                         ← 用户应用代码（main、it 等）
├── Drivers/                      ← HAL 驱动、第三方驱动
│   ├── Ethernet/                 ← W5500 以太网
│   ├── httpServer/               ← HTTP 服务器
│   └── STM32F1xx_HAL_Driver/     ← ST 官方 HAL
└── App/                          ← 业务模块（Eth/Tcp/Udp/Web）
```

### 顶层 CMakeLists.txt 流程

```cmake
cmake_minimum_required(VERSION 3.22)        # 最低版本
project(${CMAKE_PROJECT_NAME})              # 定义项目
add_executable(${CMAKE_PROJECT_NAME})       # 创建可执行文件目标
add_subdirectory(cmake/stm32cubemx)         # 加载子 CMakeLists.txt
target_link_libraries(${CMAKE_PROJECT_NAME} stm32cubemx)  # 链接子模块产物
```

### 子 CMakeLists.txt 流程

```cmake
add_library(stm32cubemx INTERFACE)          # 虚拟库：传递 include 与宏
target_include_directories(stm32cubemx INTERFACE ${MX_Include_Dirs})
target_compile_definitions(stm32cubemx INTERFACE ${MX_Defines_Syms})

add_library(STM32_Drivers OBJECT)           # 编译为目标文件
target_sources(STM32_Drivers PRIVATE
    ${STM32_Drivers_Src}                    # HAL 驱动
    ${Ethernet_Drivers_Src}                 # W5500
    ${HttpServer_Drivers_Src}               # HTTP 服务器
    ${App_Src}                              # 业务模块
)
target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${MX_Application_Src})  # main、it 等
target_link_libraries(${CMAKE_PROJECT_NAME} ${MX_LINK_LIBS})         # 链入 STM32_Drivers
```

---

## 十一、常用调试技巧

```cmake
# 打印变量
message(STATUS "Sources: ${Ethernet_Drivers_Src}")

# 打印所有源文件列表
get_target_property(SRC_LIST STM32_Drivers SOURCES)
message(STATUS "STM32_Drivers sources: ${SRC_LIST}")
```

```cmake
# 打开严格模式（推荐放在 CMakeLists.txt 顶部）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()
```

---

## 十二、动手练习建议

1. **加一个新模块**：在 `App/` 下新建一个 `Test/test.c`，仿照 `App_Src` 添加到 `target_sources`
2. **改 include 路径**：把 `App` 改成只用一个根目录，看 CMake 是否还能找到子目录的头文件
3. **加一个宏定义**：在 `target_compile_definitions` 里加 `MY_DEBUG=1`，在某个 `.c` 文件里 `#ifdef MY_DEBUG` 验证
4. **替换工具链**：用 `cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/starm-clang.cmake ..` 看是否会换用 Clang

---

## 十三、参考资料

- 官方文档：https://cmake.org/documentation/
- `cmake --help-command <命令名>`：查看具体命令的详细帮助
- `cmake --help-property <属性名>`：查看目标属性的含义