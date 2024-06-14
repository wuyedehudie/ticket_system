#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <memory>
#include <netinet/in.h>
#include <fcntl.h>
#include "Log/log.h"
#include "TCP/InetAddress.h"

namespace tcs{
    class socket {
    public:
        typedef std::shared_ptr<socket> ptr;

        enum Type {
            TCP = SOCK_STREAM,
            UDP = SOCK_DGRAM,
        };
        enum Family {
            IPV4 = AF_INET,
            IPV6 = AF_INET6,
            UNIX = AF_UNIX
        };
        //根据地址创建socket
        static socket::ptr CreateTCP(std::string host);
        static socket::ptr CreateUDP(std::string host);
        //根据addresss创建socket
        static socket::ptr CreateTCP(Address::ptr address);
        static socket::ptr CreateUDP(Address::ptr address);

        //直接创建对应socket
        static socket::ptr CreateTCPSocket();
        static socket::ptr CreateUDPSocket();
        static socket::ptr CreateTCPSocket6();
        static socket::ptr CreateUDPSocket6();
        static socket::ptr CreateUnixTCPSocket();
        static socket::ptr CreateUnixUDPSocket();


        socket(int family,int type,int protocol=0);
        ~socket();

        void bind(Address::ptr server_addr);
        void bind();

        void send(std::string quest);
        bool connect();
        //调用recv将收到的数据直接存入m_content,每次
        void recv();
        int recv(void* buffer,size_t bufferSize) ;

        static std::string&&  recvAll(int fd);
        void close();

        tcs::socket::ptr accept();
        void listening();

        void setfd(int value){m_sockfd=value;}


        const int getSock() { return m_sockfd; }
        const int getType() { return m_type; }
        const int getFamily() { return m_family; }
        const int getProtocol() {return m_protocol;}
        const std::string getContent() { return m_content; }
        void set_addr(Address::ptr address) { m_Addr=address;}
        Address::ptr get_addr() { return m_Addr; }

        void setHost(std::string host);

    private:
        int m_sockfd;
        int m_clientfd;
        int m_type;
        int m_family;
        int m_protocol;

        int m_isConnected;
        Address::ptr m_Addr;
        std::string m_content;
    };

}//namespace tcs


#endif
