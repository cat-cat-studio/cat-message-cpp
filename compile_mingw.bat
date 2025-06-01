@echo off
chcp 65001 >nul
echo ==========================================
echo     C++ 聊天软件编译脚本 (MinGW)
echo ==========================================

:: 检查是否有g++编译器
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 未找到g++编译器
    echo 请安装MinGW-w64: https://www.mingw-w64.org/downloads/
    echo 或者使用MSYS2: https://www.msys2.org/
    echo 或者安装Code::Blocks with MinGW
    pause
    exit /b 1
)

:: 创建输出目录
if not exist bin (
    echo 创建输出目录...
    mkdir bin
)

echo 编译服务端...
g++ -std=c++17 -I include src\server.cpp src\common.cpp -lws2_32 -o bin\server.exe
if %errorlevel% neq 0 (
    echo 错误: 服务端编译失败
    pause
    exit /b 1
)

echo 编译客户端...
g++ -std=c++17 -I include src\client.cpp src\common.cpp -lws2_32 -o bin\client.exe
if %errorlevel% neq 0 (
    echo 错误: 客户端编译失败
    pause
    exit /b 1
)

echo.
echo ==========================================
echo 编译成功！可执行文件位置：
echo   服务端: bin\server.exe
echo   客户端: bin\client.exe
echo ==========================================
echo.
echo 使用方法：
echo 1. 先运行 bin\server.exe 启动服务器
echo 2. 再运行 bin\client.exe 启动客户端
echo ==========================================

pause 