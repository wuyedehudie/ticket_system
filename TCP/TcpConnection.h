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
    class TcpConnection {
    public:
        typedef std::shared_ptr<TcpConnection> ptr;
        enum Type {
            TCP = SOCK_STREAM,
            UDP = SOCK_DGRAM,
        };
        enum Family {
            IPV4 = AF_INET,
            IPV6 = AF_INET6,
            UNIX = AF_UNIX
        };
        //直接创建对应socket
        static TcpConnection::ptr CreateTCPSocket(std::string host);
        static TcpConnection::ptr CreateUDPSocket(std::string host);
        static TcpConnection::ptr CreateTCPSocket6(std::string host);
        static TcpConnection::ptr CreateUDPSocket6(std::string host);
        static TcpConnection::ptr CreateUnixTCPSocket(std::string host);
        static TcpConnection::ptr CreateUnixUDPSocket(std::string host);


        TcpConnection(std::string host, Family family, Type type);

        void send(std::string quest);
        bool connect();
        //调用recv将收到的数据直接存入m_content,每次
        void recv(int sockfd);
        void close();
        void binding();
        void listening();


        const int getSock() { return m_sockfd; }
        const int getType() { return m_type; }
        const int getProtocol() { return m_protocol; }
        const std::string getContent() { return m_content; }
        sockaddr* getPeerAddr() { return m_peerAddr->getAddr(); }

        void setHost(std::string host);

    private:
        int m_sockfd;
        int m_clientfd;
        int m_type;
        int m_family;
        int m_protocol;

        int m_isConnected;

        sockaddr* m_addr;
        Address::ptr m_localAddr;
        Address::ptr m_peerAddr;

        std::string m_content;
    };

}//namespace tcs


#endif
