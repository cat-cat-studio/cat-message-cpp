#include "common.h"
#include <ctime>
#include <sstream>
#include <iomanip>

bool initializeNetwork() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup 失败: " << result << std::endl;
        return false;
    }
#endif
    return true;
}

void cleanupNetwork() {
#ifdef _WIN32
    WSACleanup();
#endif
}

std::string getCurrentTime() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    return oss.str();
}

std::string Message::serialize() const {
    std::ostringstream oss;
    oss << static_cast<int>(type) << "|" << username << "|" << content;
    return oss.str();
}

Message Message::deserialize(const std::string& data) {
    Message msg;
    std::istringstream iss(data);
    std::string token;
    
    // 解析消息类型
    if (std::getline(iss, token, '|')) {
        msg.type = static_cast<MessageType>(std::stoi(token));
    }
    
    // 解析用户名
    if (std::getline(iss, token, '|')) {
        msg.username = token;
    }
    
    // 解析消息内容
    if (std::getline(iss, token)) {
        msg.content = token;
    }
    
    return msg;
} 