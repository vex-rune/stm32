cmake_minimum_required(VERSION 3.22)

#
# 本文件只生成一次，
# 多次调用转换器时不会重新生成。
#
# 用户可根据需要自由修改此文件
#

# 编译器设置
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# 定义构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# 设置项目名称
set(CMAKE_PROJECT_NAME 18_web_server)

# 启用编译命令导出，便于 clangd 等工具进行索引
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# 核心项目设置
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# 启用 CMake 对 ASM 和 C 语言的支持
enable_language(C ASM)

# 创建可执行文件目标
add_executable(${CMAKE_PROJECT_NAME})

# 添加 STM32CubeMX 生成的源代码
add_subdirectory(cmake/stm32cubemx)

# 链接目录设置
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # 在此处添加用户自定义库搜索路径
)

# 向可执行文件添加源文件
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # 在此处添加用户源文件
)

# 添加头文件包含路径
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # 在此处添加用户自定义包含路径
)

# 添加项目宏定义
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # 在此处添加用户自定义宏
)

# 使用 cpp 文件时移除错误的 libob.a 库依赖
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# 添加链接库
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # 在此处添加用户自定义库
)