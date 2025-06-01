@echo off
echo ==========================================
echo     C++ 聊天软件手动编译脚本
echo ==========================================

:: 检查是否有MSVC编译器
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 未找到MSVC编译器
    echo 请运行 "Developer Command Prompt for VS" 或者安装 Visual Studio
    pause
    exit /b 1
)

:: 创建输出目录
if not exist bin (
    echo 创建输出目录...
    mkdir bin
)

echo 编译服务端...
cl /EHsc /std:c++17 /I include src\server.cpp src\common.cpp ws2_32.lib /Fe:bin\server.exe
if %errorlevel% neq 0 (
    echo 错误: 服务端编译失败
    pause
    exit /b 1
)

echo 编译客户端...
cl /EHsc /std:c++17 /I include src\client.cpp src\common.cpp ws2_32.lib /Fe:bin\client.exe
if %errorlevel% neq 0 (
    echo 错误: 客户端编译失败
    pause
    exit /b 1
)

:: 清理临时文件
del *.obj >nul 2>&1

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