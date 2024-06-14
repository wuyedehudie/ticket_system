#include "TCP/socket.h"

namespace tcs {
    static tcs::Logger::ptr logger = TCS_LOG_NAME("system");
    //用host直接, 使用的是时候不需要再使用lookupAny()
    socket::ptr socket::CreateTCP(std::string host) {
        auto address = tcs::Address::LookupAny(host);
        socket::ptr sock(new socket(address->getFamily(),tcs::socket::TCP));
        sock->set_addr(address);
        return sock;
    }
    socket::ptr socket::CreateUDP(std::string host) {
        auto address = tcs::Address::LookupAny(host);
        socket::ptr sock(new socket(address->getFamily(),tcs::socket::UDP));
        sock->set_addr(address);
        return sock;
    }
    //用Address创建，应对需要重新绑定的状况
    socket::ptr socket::CreateTCP(Address::ptr address) {
       socket::ptr sock(new socket(address->getFamily(),tcs::socket::TCP));
       sock->set_addr(address);
        return sock;
    }
    socket::ptr socket::CreateUDP(Address::ptr address) {
        socket::ptr sock(new socket(address->getFamily(),tcs::socket::UDP));
        sock->set_addr(address);
        return sock;
    }

    socket::ptr socket::CreateTCPSocket() {
        return  socket::ptr(new socket(tcs::socket::IPV4, tcs::socket::TCP));
    }
    socket::ptr socket::CreateUDPSocket() {
        return  socket::ptr(new socket(tcs::socket::IPV4, tcs::socket::UDP));
    }
    socket::ptr socket::CreateTCPSocket6() {
        return  socket::ptr(new socket(tcs::socket::IPV6, tcs::socket::TCP));
    }
    socket::ptr socket::CreateUDPSocket6() {
        return  socket::ptr(new socket(tcs::socket::IPV6, tcs::socket::UDP));
    }
    socket::ptr socket::CreateUnixTCPSocket() {
        return  socket::ptr(new socket(tcs::socket::UNIX, tcs::socket::TCP));
    }
    socket::ptr socket::CreateUnixUDPSocket() {
        return  socket::ptr(new socket(tcs::socket::UNIX, tcs::socket::UDP));
    }
    socket::socket(int family, int type, int protocol)
            :m_sockfd(-1)
            ,m_family(family)
            ,m_type(type)
            ,m_protocol(protocol)
            ,m_isConnected(0){

        if(m_family==IPV4){m_Addr.reset(new IPv4Address());}
        else if(m_family==IPV6){m_Addr.reset(new IPv6Address());}
    }

    socket::~socket() {
        ::close(m_sockfd);
    }
    void socket::bind(Address::ptr server_addr) {
        if(m_sockfd!=-1){
            close();
        }else m_sockfd = ::socket(getFamily(),getType(),0);
        set_addr(server_addr);
        if (::bind(m_sockfd, m_Addr->getAddr(), m_Addr->getAddrLen())){
            TCS_LOG_ERROR(rootlog)  << "bind error errrno=" << errno
                                    << " errstr=" << strerror(errno);
        }
    }
    void socket::bind() {
        if(m_sockfd==-1){
            m_sockfd = ::socket(getFamily(),getType(),0);
            //TCS_LOG_DEBUG(rootlog)<<getFamily();
            //TCS_LOG_DEBUG(rootlog)<<getType();
            if(m_sockfd==-1){
                TCS_LOG_ERROR(rootlog)<<"::socket创建失败";
                TCS_LOG_ERROR(rootlog)<<errno <<" errstr= "<<strerror(errno);
            }
            TCS_LOG_DEBUG(rootlog)<<"创建sockefd: "<<m_sockfd;
        }

        if (::bind(m_sockfd, m_Addr->getAddr(), m_Addr->getAddrLen())){
            TCS_LOG_ERROR(rootlog)<<"::bind()失败";
            TCS_LOG_ERROR(rootlog)  << "bind error errrno=" << errno
                                    << " errstr=" << strerror(errno);
        }
        TCS_LOG_INFO(rootlog)<<"len"<<m_Addr->getAddrLen();
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &m_Addr->getAddr()->sa_data, ip_str, INET_ADDRSTRLEN);
        TCS_LOG_INFO(rootlog)<<"addr:"<<ip_str;
    }

    bool socket::connect() {

        m_isConnected =  ::connect(m_sockfd, m_Addr->getAddr(), m_Addr->getAddrLen());
        if (m_isConnected==-1) {
            TCS_LOG_ERROR(rootlog) << "无法连接服务器";
            return m_isConnected;
        }
        else {
            TCS_LOG_INFO(rootlog) << "成功连接服务器";
            return m_isConnected;
        }
    }
    void socket::send(std::string quest) {
        if (m_isConnected!=-1) {
            ::send(m_sockfd, quest.c_str(), quest.size(), 0);
        }
        else TCS_LOG_ERROR(rootlog) << "请先连接到服务器";
    }
    void socket::setHost(std::string host) {
        m_Addr = Address::LookupAny(host);
    }
    void socket::recv() {

        char buffer[1024];
        int bytesReceived;

        while ((bytesReceived = ::recv(m_sockfd, buffer, sizeof(buffer), 0)) > 0) {
            m_content.append(buffer, bytesReceived);
            memset(buffer, 0, sizeof(buffer));
            TCS_LOG_INFO(rootlog) << "收到一份数据";
            TCS_LOG_INFO(rootlog) << getContent();
        }

    }
    int socket::recv(void* buffer,size_t bufferSize) {
       return ::recv(m_sockfd,buffer,bufferSize,0);

    }
    std::string&& socket::recvAll(int fd) {
        std::string result;
        char buffer[1024]; // 适当调整缓冲区大小

        while (true) {
            int bytesReceived = ::recv(fd,buffer, sizeof(buffer), 0);
            if (bytesReceived > 0) {
                result.append(buffer, bytesReceived);
            } else if (bytesReceived == 0) {
                // 连接已关闭
                break;
            } else {
                TCS_LOG_ERROR(rootlog)<<"::recv发生错误";
                break;
            }
        }
        return std::move(result);
    }

    void socket::close() {
        m_isConnected=false;
        ::close(m_sockfd);
    }

    tcs::socket::ptr socket::accept() {
         tcs::socket::ptr sock = CreateTCPSocket();
         ::close(sock->getSock());
         sock->setfd(::accept(m_sockfd,nullptr,nullptr));
         if(sock->getSock()){
             m_isConnected=true;
         }
         return sock;
    }

    void socket::listening() {
        if (::listen(m_sockfd, 5) == -1) {
            return;
        }
        // 开始监听成功，等待客户端连接
        //while (true) {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            socket::ptr client = accept();
            TCS_LOG_INFO(rootlog)<<"收到连接请求";
            int clientSock = client->getSock();
            //int flags = fcntl(clientSock, F_GETFL, 0);
            //fcntl(clientSock, F_SETFL, flags | O_NONBLOCK);

            if (clientSock == -1) {
                //continue; // 如果接受连接失败，继续等待下一个连接
            }
            char buffer[1024];
            TCS_LOG_INFO(rootlog)<<client->recv(buffer,sizeof(buffer))<<buffer;
            const char* response =
                    "HTTP/1.1 204 No Content\r\n"
                    "Access-Control-Allow-Origin: *\r\n"
                    "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                    "Access-Control-Allow-Headers: content-type\r\n"
                    "Access-Control-Max-Age: 86400\r\n"  // 24小时缓存
                    "\r\n";
            ::send(clientSock,response,strlen(response),0);
            std::string response2 =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Content-Length: 52\r\n"
                "\r\n"
                "{\"status\":\"success\",\"message\":\"Login successful\"}";
            if(::send(clientSock,response2.c_str(),strlen(response2.c_str()),0)){
                TCS_LOG_INFO(rootlog)<<"发送成功:字节数"<<strlen(response);
            }else {
                TCS_LOG_ERROR(rootlog) <<"发送失败";
                client->close();
                return ;
            }
            char buff[10024];
            client->recv(buff,sizeof(buff));


            TCS_LOG_DEBUG(rootlog)<<buff;
            client->close();
            // 关闭连接
        //}

    }
}//namespace tcs
