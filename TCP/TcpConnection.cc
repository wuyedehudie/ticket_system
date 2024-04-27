#include "TCP/TcpConnection.h"

namespace tcs {
    TcpConnection::ptr TcpConnection::CreateTCPSocket(std::string host) {
        return  TcpConnection::ptr(new TcpConnection(host,tcs::TcpConnection::IPV4,tcs::TcpConnection::TCP));
    }
    TcpConnection::ptr TcpConnection::CreateUDPSocket(std::string host) {
        return  TcpConnection::ptr(new TcpConnection(host,tcs::TcpConnection::IPV4,tcs::TcpConnection::UDP));
    }
    TcpConnection::ptr TcpConnection::CreateTCPSocket6(std::string host) {
        return  TcpConnection::ptr(new TcpConnection(host,tcs::TcpConnection::IPV6,tcs::TcpConnection::TCP));
    }
    TcpConnection::ptr TcpConnection::CreateUDPSocket6(std::string host) {
        return  TcpConnection::ptr(new TcpConnection(host,tcs::TcpConnection::IPV6,tcs::TcpConnection::UDP));
    }
    TcpConnection::ptr TcpConnection::CreateUnixTCPSocket(std::string host) {
        return  TcpConnection::ptr(new TcpConnection(host,tcs::TcpConnection::UNIX,tcs::TcpConnection::TCP));
    }
    TcpConnection::ptr TcpConnection::CreateUnixUDPSocket(std::string host) {
        return  TcpConnection::ptr(new TcpConnection(host,tcs::TcpConnection::UNIX,tcs::TcpConnection::UDP));
    }
    TcpConnection::TcpConnection(std::string host,Family family,Type type)
            :m_peerAddr(Address::LookupAny(host, family,type,0))
            ,m_family(family)
            ,m_type(type)
            ,m_isConnected(0){
        m_sockfd = ::socket(m_family, m_type, 0);
        if (m_sockfd) {
            TCS_LOG_INFO(rootlog) << "successfully create m_sockfd";
        }
        else TCS_LOG_ERROR(rootlog) << "cannot create m_sockfd";
    }
    bool TcpConnection::connect() {

        m_isConnected =  ::connect(m_sockfd, m_peerAddr->getAddr(), m_peerAddr->getAddrLen());
        if (m_isConnected==-1) {
            TCS_LOG_ERROR(rootlog) << "无法连接服务器";
            return m_isConnected;
        }
        else {
            TCS_LOG_INFO(rootlog) << "成功连接服务器";
            return m_isConnected;
        }
    }
    void TcpConnection::send(std::string quest) {
        if (m_isConnected!=-1) {
            ::send(m_sockfd, quest.c_str(), quest.size(), 0);
        }
        else TCS_LOG_ERROR(rootlog) << "请先连接到服务器";
    }
    void TcpConnection::setHost(std::string host) {
        m_peerAddr = Address::LookupAny(host);
    }
    void TcpConnection::recv(int sockfd = -1) {
        char buffer[1024];
        int bytesReceived;
        if(sockfd == -1){
            sockfd = m_sockfd;
        }
        while ((bytesReceived = ::recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
            m_content.append(buffer, bytesReceived);
            TCS_LOG_INFO(rootlog) << "收到一份数据";
        }

    }
    void TcpConnection::close() {
        ::close(m_sockfd);
    }
    void TcpConnection::binding() {
        struct sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr)); // 清空结构体
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(8080); // 服务器端口号，使用 htons 将端口号转为网络字节序
        // 绑定本地 IP 地址
        const char* localIP = "192.168.23.128"; // 本地 IP 地址
        if (inet_pton(AF_INET, localIP, &serverAddr.sin_addr) <= 0) {
            TCS_LOG_ERROR(rootlog)<<"错误";
        }
        if (::bind(m_sockfd, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
            TCS_LOG_ERROR(rootlog)<<"无法绑定";
        }

    }
    void TcpConnection::listening() {
        if (::listen(m_sockfd, 5) == -1) {
            return;
        }
        // 开始监听成功，等待客户端连接
        while (true) {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int client_sockfd = ::accept(m_sockfd, reinterpret_cast<struct sockaddr *>(&clientAddr), &clientAddrLen);
            int flags = fcntl(client_sockfd, F_GETFL, 0);
            fcntl(client_sockfd, F_SETFL, flags | O_NONBLOCK);
            if (client_sockfd == -1) {
                // 处理接受连接失败的情况
                continue; // 如果接受连接失败，继续等待下一个连接
            }
            // 获取客户端 IP 地址和端口号
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);

            int clientPort = ntohs(clientAddr.sin_port);
            recv(client_sockfd);














            ::send(client_sockfd,"收到",sizeof("收到"),0);
            // 关闭连接
        }

    }
}//namespace tcs
