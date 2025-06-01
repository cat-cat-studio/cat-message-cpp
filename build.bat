@echo off
echo ==========================================
echo        C++ 聊天软件编译脚本
echo ==========================================

:: 检查CMake是否安装
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 未找到CMake，请先安装CMake
    pause
    exit /b 1
)

:: 创建构建目录
if not exist build (
    echo 创建构建目录...
    mkdir build
)

:: 进入构建目录
cd build

echo 配置项目...
cmake .. -G "Visual Studio 16 2019" -A x64
if %errorlevel% neq 0 (
    echo 错误: CMake配置失败
    pause
    exit /b 1
)

echo 开始编译...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo 错误: 编译失败
    pause
    exit /b 1
)

echo.
echo ==========================================
echo 编译成功！可执行文件位置：
echo   服务端: build\Release\server.exe
echo   客户端: build\Release\client.exe
echo ==========================================

pause 