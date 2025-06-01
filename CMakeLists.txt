cmake_minimum_required(VERSION 3.16)
project(ChatApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 检查操作系统
if(WIN32)
    set(SOCKET_LIBS ws2_32)
else()
    set(SOCKET_LIBS)
endif()

# 服务端可执行文件
add_executable(server
    src/server.cpp
    src/common.cpp
)

# 客户端可执行文件
add_executable(client
    src/client.cpp
    src/common.cpp
)

# 链接必要的库
target_link_libraries(server ${SOCKET_LIBS})
target_link_libraries(client ${SOCKET_LIBS})

# 包含头文件目录
target_include_directories(server PRIVATE include)
target_include_directories(client PRIVATE include) 