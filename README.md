# C++ 聊天软件

一个简单的基于TCP Socket的C++聊天系统，支持多客户端同时在线聊天。

## 功能特性

- **服务端功能**：
  - 支持多客户端同时连接（最多50个）
  - 消息广播到所有在线用户
  - 用户加入/离开通知
  - 实时显示聊天记录和用户状态
  - 异常连接处理和资源清理

- **客户端功能**：
  - 连接服务器并设置用户名
  - 实时收发消息
  - 优雅退出（输入 `/quit`）
  - 跨平台支持（Windows/Linux）

## 项目结构

```
cat-message-cpp/
├── CMakeLists.txt          # CMake构建配置
├── README.md               # 项目说明文档
├── include/
│   └── common.h           # 公共头文件
└── src/
    ├── common.cpp         # 公共功能实现
    ├── server.cpp         # 服务端程序
    └── client.cpp         # 客户端程序
```

## 编译方法

### 使用CMake编译（推荐）

```bash
# 创建构建目录
mkdir build
cd build

# 生成构建文件
cmake ..

# 编译
cmake --build .
```

### Windows（Visual Studio）

```cmd
# 在项目根目录执行
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

### 手动编译（可选）

**Windows (MSVC):**
```cmd
cl /EHsc /std:c++17 /I include src\server.cpp src\common.cpp ws2_32.lib /Fe:server.exe
cl /EHsc /std:c++17 /I include src\client.cpp src\common.cpp ws2_32.lib /Fe:client.exe
```

**Linux (GCC):**
```bash
g++ -std=c++17 -pthread -I include src/server.cpp src/common.cpp -o server
g++ -std=c++17 -pthread -I include src/client.cpp src/common.cpp -o client
```

## 使用方法

### 1. 启动服务器

```bash
# 在build目录中
./server        # Linux/Mac
server.exe      # Windows
```

服务器启动后会显示：
```
聊天服务器启动成功，监听端口: 8888
等待客户端连接...
```

### 2. 启动客户端

```bash
# 在build目录中
./client        # Linux/Mac
client.exe      # Windows
```

客户端启动后会提示：
1. 输入服务器地址（默认127.0.0.1）
2. 输入用户名
3. 开始聊天

### 3. 聊天操作

- 直接输入文字并按回车发送消息
- 输入 `/quit` 退出聊天室
- 按 `Ctrl+C` 停止服务器

## 网络协议

系统使用简单的文本协议进行通信：

```
消息格式：消息类型|用户名|消息内容
```

**消息类型**：
- `1`: JOIN - 用户加入
- `2`: LEAVE - 用户离开  
- `3`: MESSAGE - 聊天消息
- `4`: ERROR_MSG - 错误消息

## 示例运行效果

**服务端输出**：
```
聊天服务器启动成功，监听端口: 8888
等待客户端连接...
新客户端连接: 127.0.0.1:52341
用户 Alice 加入聊天室 [14:30:15]
用户 Bob 加入聊天室 [14:30:32]
[14:30:45] Alice: 大家好！
[14:30:50] Bob: 你好Alice
```

**客户端输出**：
```
=== C++ 聊天客户端 ===
请输入服务器地址 (按回车使用默认 127.0.0.1): 
请输入您的用户名: Alice
成功连接到服务器!

=== 欢迎来到聊天室 ===
输入消息并按回车发送
输入 /quit 退出聊天室
========================

[14:30:32] 系统: Bob 加入了聊天室
大家好！
[14:30:50] Bob: 你好Alice
```

## 技术特点

- **多线程**: 服务端为每个客户端创建独立线程处理
- **线程安全**: 使用mutex保护共享资源
- **跨平台**: 统一的Socket API抽象
- **异常处理**: 完善的错误处理和资源清理
- **现代C++**: 使用C++17标准特性

## 系统要求

- **编译器**: 支持C++17的编译器
  - Windows: Visual Studio 2017+
  - Linux: GCC 7+ 或 Clang 5+
- **CMake**: 3.16+
- **操作系统**: Windows 10+ 或 Linux

## 许可证

MIT License - 详见 LICENSE 文件。