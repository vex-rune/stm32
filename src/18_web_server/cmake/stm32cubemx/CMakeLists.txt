cmake_minimum_required(VERSION 3.22)
# 启用 CMake 对 ASM 和 C 语言的支持
enable_language(C ASM)
# STM32CubeMX 生成的宏定义
set(MX_Defines_Syms
	USE_HAL_DRIVER
	STM32F103xE
    $<$<CONFIG:Debug>:DEBUG>
)

# STM32CubeMX 生成的头文件包含路径
set(MX_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/Ethernet
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/httpServer
    ${CMAKE_CURRENT_SOURCE_DIR}/../../App
    ${CMAKE_CURRENT_SOURCE_DIR}/../../App/Eth
    ${CMAKE_CURRENT_SOURCE_DIR}/../../App/Tcp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../App/Udp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../App/Web
)

# STM32CubeMX 生成的应用源码
set(MX_Application_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/usart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f1xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f1xx_hal_msp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/sysmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../startup_stm32f103xe.s
)

# STM32 HAL/LL 驱动
set(STM32_Drivers_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/system_stm32f1xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
)

# 驱动中间件：W5500 以太网驱动
set(Ethernet_Drivers_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/Ethernet/socket.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/Ethernet/w5500.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/Ethernet/wizchip_conf.c
)

# 驱动中间件：HTTP 服务器
set(HttpServer_Drivers_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/httpServer/httpParser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/httpServer/httpServer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/httpServer/httpUtil.c
)

# 应用层源码：业务模块
set(App_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../App/Eth/eth.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../App/Tcp/tcp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../App/Udp/udp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../App/Web/web_server.c
)



# 链接目录设置
set(MX_LINK_DIRS

)
# 项目静态库
set(MX_LINK_LIBS
    STM32_Drivers
    ${TOOLCHAIN_LINK_LIBRARIES}

)
# 用于包含路径与宏定义的接口库
add_library(stm32cubemx INTERFACE)
target_include_directories(stm32cubemx INTERFACE ${MX_Include_Dirs})
target_compile_definitions(stm32cubemx INTERFACE ${MX_Defines_Syms})

# 创建 STM32_Drivers 静态库
add_library(STM32_Drivers OBJECT)
target_sources(STM32_Drivers PRIVATE
    ${STM32_Drivers_Src}
    ${Ethernet_Drivers_Src}
    ${HttpServer_Drivers_Src}
    ${App_Src}
)
target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)


# 将 STM32CubeMX 生成的应用源码添加到项目中
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${MX_Application_Src})

# 链接目录设置
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MX_LINK_DIRS})

# 向项目添加库
target_link_libraries(${CMAKE_PROJECT_NAME} ${MX_LINK_LIBS})

# 将 map 文件添加到 'clean' 目标需删除的文件列表中
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# 校验 STM32CubeMX 生成的代码是否兼容 C 语言标准
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()
