#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

constexpr int PORT = 8888;
constexpr int BUFFER_SIZE = 1024;
constexpr int MAX_CLIENTS = 50;

// 消息类型枚举
enum class MessageType {
    JOIN = 1,
    LEAVE = 2,
    MESSAGE = 3,
    ERROR_MSG = 4
};

// 消息结构体
struct Message {
    MessageType type;
    std::string username;
    std::string content;
    
    std::string serialize() const;
    static Message deserialize(const std::string& data);
};

// 网络初始化和清理函数
bool initializeNetwork();
void cleanupNetwork();

// 实用函数
std::string getCurrentTime(); 