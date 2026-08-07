# LLCC68 驱动

本软件包提供了 **LLCC68** 射频组件的 C 语言驱动实现。
更多信息请参阅 [更新日志](CHANGELOG.md)。

## 结构

驱动程序定义如下:

- **llcc68.c**:驱动函数的实现
- **llcc68.h**:驱动函数的声明
- **llcc68_regs.h**:所有有用寄存器的定义(地址和字段)
- **llcc68_hal.h**:HAL 函数的声明(由用户实现 —— 见下文)

## HAL

HAL(硬件抽象层,Hardware Abstraction Layer)是一组用户需要实现的函数,用于编写与平台相关的主机调用。函数列表如下:

- `llcc68_hal_reset`
- `llcc68_hal_wakeup`
- `llcc68_hal_write`
- `llcc68_hal_read`

## CMake 使用

本驱动提供了一个 CMake 配置,可将其集成到支持 CMake 的应用程序中。

### 集成方式

如果驱动代码位于使用它的应用程序的目录中,可以通过将子目录添加到配置中来集成,如下所示:

```cmake
add_subdirectory(llcc68_driver) # 其中 llcc68_driver 是包含驱动代码的文件夹名称
```

或者,如果驱动代码以代码归档的形式提供,也可以通过以下方式直接引入:

```cmake
include(FetchContent)
FetchContent_Declare(
    llcc68_driver
    URL "path_to_archive" # 其中 path_to_archive 需替换为驱动归档的路径
)
FetchContent_MakeAvailable(llcc68_driver)
```