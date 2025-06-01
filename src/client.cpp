#include "common.h"
#include <atomic>

class ChatClient {
private:
    SOCKET clientSocket;
    std::string username;
    std::atomic<bool> running;
    std::thread receiveThread;

public:
    ChatClient() : clientSocket(INVALID_SOCKET), running(false) {}
    
    ~ChatClient() {
        disconnect();
    }
    
    bool connect(const std::string& serverIP = "127.0.0.1") {
        if (!initializeNetwork()) {
            return false;
        }
        
        // 创建socket
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "创建socket失败" << std::endl;
            cleanupNetwork();
            return false;
        }
        
        // 连接到服务器
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);
        
        if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
            std::cerr << "无效的服务器地址" << std::endl;
            closesocket(clientSocket);
            cleanupNetwork();
            return false;
        }
        
        if (::connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "连接服务器失败" << std::endl;
            closesocket(clientSocket);
            cleanupNetwork();
            return false;
        }
        
        running = true;
        std::cout << "成功连接到服务器!" << std::endl;
        return true;
    }
    
    bool login(const std::string& name) {
        if (!running) {
            return false;
        }
        
        username = name;
        
        // 发送加入消息
        Message joinMsg;
        joinMsg.type = MessageType::JOIN;
        joinMsg.username = username;
        joinMsg.content = "";
        
        std::string data = joinMsg.serialize();
        if (send(clientSocket, data.c_str(), data.length(), 0) == SOCKET_ERROR) {
            std::cerr << "发送加入消息失败" << std::endl;
            return false;
        }
        
        // 启动接收消息线程
        receiveThread = std::thread(&ChatClient::receiveMessages, this);
        
        return true;
    }
    
    void run() {
        if (!running) {
            return;
        }
        
        std::cout << "\n=== 欢迎来到聊天室 ===" << std::endl;
        std::cout << "输入消息并按回车发送" << std::endl;
        std::cout << "输入 /quit 退出聊天室" << std::endl;
        std::cout << "========================\n" << std::endl;
        
        std::string input;
        while (running && std::getline(std::cin, input)) {
            if (input.empty()) {
                continue;
            }
            
            if (input == "/quit") {
                break;
            }
            
            // 发送消息
            Message msg;
            msg.type = MessageType::MESSAGE;
            msg.username = username;
            msg.content = input;
            
            std::string data = msg.serialize();
            if (send(clientSocket, data.c_str(), data.length(), 0) == SOCKET_ERROR) {
                std::cerr << "发送消息失败" << std::endl;
                break;
            }
        }
        
        // 发送离开消息
        Message leaveMsg;
        leaveMsg.type = MessageType::LEAVE;
        leaveMsg.username = username;
        leaveMsg.content = "";
        
        std::string data = leaveMsg.serialize();
        send(clientSocket, data.c_str(), data.length(), 0);
    }
    
    void disconnect() {
        running = false;
        
        if (clientSocket != INVALID_SOCKET) {
            closesocket(clientSocket);
            clientSocket = INVALID_SOCKET;
        }
        
        if (receiveThread.joinable()) {
            receiveThread.join();
        }
        
        cleanupNetwork();
    }

private:
    void receiveMessages() {
        char buffer[BUFFER_SIZE];
        
        while (running) {
            int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
            if (bytesReceived <= 0) {
                if (running) {
                    std::cout << "\n[系统] 与服务器连接断开" << std::endl;
                }
                running = false;
                break;
            }
            
            buffer[bytesReceived] = '\0';
            Message msg = Message::deserialize(std::string(buffer));
            
            // 显示消息
            if (msg.type == MessageType::MESSAGE) {
                std::cout << "[" << getCurrentTime() << "] " 
                          << msg.username << ": " << msg.content << std::endl;
            } else if (msg.type == MessageType::ERROR_MSG) {
                std::cout << "[错误] " << msg.content << std::endl;
            }
        }
    }
};

int main() {
    ChatClient client;
    
    std::cout << "=== C++ 聊天客户端 ===" << std::endl;
    
    // 获取服务器地址
    std::string serverIP;
    std::cout << "请输入服务器地址 (按回车使用默认 127.0.0.1): ";
    std::getline(std::cin, serverIP);
    if (serverIP.empty()) {
        serverIP = "127.0.0.1";
    }
    
    // 连接到服务器
    if (!client.connect(serverIP)) {
        std::cerr << "连接服务器失败" << std::endl;
        return 1;
    }
    
    // 获取用户名
    std::string username;
    std::cout << "请输入您的用户名: ";
    std::getline(std::cin, username);
    
    if (username.empty()) {
        std::cout << "用户名不能为空" << std::endl;
        return 1;
    }
    
    // 登录
    if (!client.login(username)) {
        std::cerr << "登录失败" << std::endl;
        return 1;
    }
    
    // 运行聊天
    try {
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "客户端运行错误: " << e.what() << std::endl;
    }
    
    std::cout << "再见!" << std::endl;
    return 0;
} 