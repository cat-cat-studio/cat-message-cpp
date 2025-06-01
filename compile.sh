#!/bin/bash

echo "=========================================="
echo "      C++ 聊天软件编译脚本 (Linux)"
echo "=========================================="

# 检查g++编译器
if ! command -v g++ &> /dev/null; then
    echo "错误: 未找到g++编译器"
    echo "请安装g++: sudo apt-get install g++ (Ubuntu/Debian)"
    exit 1
fi

# 创建输出目录
if [ ! -d "bin" ]; then
    echo "创建输出目录..."
    mkdir bin
fi

echo "编译服务端..."
g++ -std=c++17 -pthread -I include src/server.cpp src/common.cpp -o bin/server
if [ $? -ne 0 ]; then
    echo "错误: 服务端编译失败"
    exit 1
fi

echo "编译客户端..."
g++ -std=c++17 -pthread -I include src/client.cpp src/common.cpp -o bin/client
if [ $? -ne 0 ]; then
    echo "错误: 客户端编译失败"
    exit 1
fi

echo ""
echo "=========================================="
echo "编译成功！可执行文件位置："
echo "  服务端: bin/server"
echo "  客户端: bin/client"
echo "=========================================="
echo ""
echo "使用方法："
echo "1. 先运行 ./bin/server 启动服务器"
echo "2. 再运行 ./bin/client 启动客户端"
echo "==========================================" 