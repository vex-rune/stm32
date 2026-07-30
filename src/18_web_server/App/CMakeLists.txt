# App业务模块：静态库
add_library(app_lib STATIC
        # TCP模块
        ${CMAKE_CURRENT_SOURCE_DIR}/Tcp/tcp.c
        # UDP模块
        ${CMAKE_CURRENT_SOURCE_DIR}/Udp/udp.c
        # ETH模块，后续新增eth.c在这里添加
        ${CMAKE_CURRENT_SOURCE_DIR}/Eth/eth.c
)

# 设置头文件搜索路径
target_include_directories(app_lib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Eth
        ${CMAKE_CURRENT_SOURCE_DIR}/Tcp
        ${CMAKE_CURRENT_SOURCE_DIR}/Udp
)