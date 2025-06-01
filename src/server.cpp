#include "common.h"
#include <algorithm>

class ChatServer {
private:
    SOCKET serverSocket;
    std::unordered_map<SOCKET, std::string> clients; // socket -> username
    std::mutex clientsMutex;
    bool running;

public:
    ChatServer() : serverSocket(INVALID_SOCKET), running(false) {}
    
    ~ChatServer() {
        stop();
    }
    
    bool start() {
        if (!initializeNetwork()) {
            return false;
        }
        
        // 创建socket
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "创建socket失败" << std::endl;
            cleanupNetwork();
            return false;
        }
        
        // 设置socket选项，允许地址重用
        int opt = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, 
                      reinterpret_cast<const char*>(&opt), sizeof(opt)) < 0) {
            std::cerr << "设置socket选项失败" << std::endl;
        }
        
        // 绑定地址
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);
        
        if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "绑定地址失败" << std::endl;
            closesocket(serverSocket);
            cleanupNetwork();
            return false;
        }
        
        // 开始监听
        if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR) {
            std::cerr << "监听失败" << std::endl;
            closesocket(serverSocket);
            cleanupNetwork();
            return false;
        }
        
        running = true;
        std::cout << "聊天服务器启动成功，监听端口: " << PORT << std::endl;
        std::cout << "等待客户端连接..." << std::endl;
        
        return true;
    }
    
    void run() {
        while (running) {
            sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            
            SOCKET clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
            if (clientSocket == INVALID_SOCKET) {
                if (running) {
                    std::cerr << "接受客户端连接失败" << std::endl;
                }
                continue;
            }
            
            std::cout << "新客户端连接: " << inet_ntoa(clientAddr.sin_addr) 
                      << ":" << ntohs(clientAddr.sin_port) << std::endl;
            
            // 为每个客户端创建处理线程
            std::thread clientThread(&ChatServer::handleClient, this, clientSocket);
            clientThread.detach();
        }
    }
    
    void stop() {
        running = false;
        if (serverSocket != INVALID_SOCKET) {
            closesocket(serverSocket);
        }
        
        // 关闭所有客户端连接
        std::lock_guard<std::mutex> lock(clientsMutex);
        for (auto& client : clients) {
            closesocket(client.first);
        }
        clients.clear();
        
        cleanupNetwork();
    }
    
private:
    void handleClient(SOCKET clientSocket) {
        char buffer[BUFFER_SIZE];
        std::string username;
        bool authenticated = false;
        
        while (running) {
            int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
            if (bytesReceived <= 0) {
                break;
            }
            
            buffer[bytesReceived] = '\0';
            Message msg = Message::deserialize(std::string(buffer));
            
            switch (msg.type) {
                case MessageType::JOIN:
                    if (!authenticated) {
                        username = msg.username;
                        {
                            std::lock_guard<std::mutex> lock(clientsMutex);
                            clients[clientSocket] = username;
                        }
                        authenticated = true;
                        
                        std::cout << "用户 " << username << " 加入聊天室 [" 
                                  << getCurrentTime() << "]" << std::endl;
                        
                        // 通知其他用户
                        Message joinMsg;
                        joinMsg.type = MessageType::MESSAGE;
                        joinMsg.username = "系统";
                        joinMsg.content = username + " 加入了聊天室";
                        broadcastMessage(joinMsg, clientSocket);
                    }
                    break;
                    
                case MessageType::MESSAGE:
                    if (authenticated) {
                        std::cout << "[" << getCurrentTime() << "] " 
                                  << username << ": " << msg.content << std::endl;
                        broadcastMessage(msg, INVALID_SOCKET);
                    }
                    break;
                    
                case MessageType::LEAVE:
                    if (authenticated) {
                        std::cout << "用户 " << username << " 离开聊天室 [" 
                                  << getCurrentTime() << "]" << std::endl;
                        
                        Message leaveMsg;
                        leaveMsg.type = MessageType::MESSAGE;
                        leaveMsg.username = "系统";
                        leaveMsg.content = username + " 离开了聊天室";
                        broadcastMessage(leaveMsg, clientSocket);
                    }
                    goto cleanup;
                    
                default:
                    break;
            }
        }
        
    cleanup:
        // 清理客户端
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.erase(clientSocket);
        }
        closesocket(clientSocket);
        
        if (authenticated && !username.empty()) {
            std::cout << "客户端 " << username << " 断开连接" << std::endl;
        }
    }
    
    void broadcastMessage(const Message& msg, SOCKET excludeSocket) {
        std::string data = msg.serialize();
        
        std::lock_guard<std::mutex> lock(clientsMutex);
        for (auto it = clients.begin(); it != clients.end();) {
            if (it->first != excludeSocket) {
                int result = send(it->first, data.c_str(), data.length(), 0);
                if (result == SOCKET_ERROR) {
                    // 发送失败，移除客户端
                    std::cout << "向客户端 " << it->second << " 发送消息失败，移除连接" << std::endl;
                    closesocket(it->first);
                    it = clients.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }
};

int main() {
    ChatServer server;
    
    if (!server.start()) {
        std::cerr << "服务器启动失败" << std::endl;
        return 1;
    }
    
    // 注册信号处理，支持优雅关闭
    std::cout << "按 Ctrl+C 停止服务器" << std::endl;
    
    try {
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "服务器运行错误: " << e.what() << std::endl;
    }
    
    return 0;
} 