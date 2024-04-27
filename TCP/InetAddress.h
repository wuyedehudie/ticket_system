#ifndef __INETADDRESS_H__
#define __INETADDRESS_H__

#include <memory>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <map>
#include "Log/log.h"



namespace tcs {

    //转换为十进制ip，用于检查
    static void printSocketAddress(sockaddr* sa) {
        if (sa->sa_family == AF_INET) {
            sockaddr_in* addr_in = reinterpret_cast<sockaddr_in*>(sa);
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(addr_in->sin_addr), ip, INET_ADDRSTRLEN);
            TCS_LOG_DEBUG(rootlog) << "IPv4 Address: " << ip << std::endl;
            TCS_LOG_DEBUG(rootlog) << "Port: " << ntohs(addr_in->sin_port) << std::endl;
        } else if (sa->sa_family == AF_INET6) {
            sockaddr_in6* addr_in6 = reinterpret_cast<sockaddr_in6*>(sa);
            char ip[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &(addr_in6->sin6_addr), ip, INET6_ADDRSTRLEN);
            std::cout << "IPv6 Address: " << ip << std::endl;
            std::cout << "Port: " << ntohs(addr_in6->sin6_port) << std::endl;
        } else {
            std::cout << "Unknown address family" << std::endl;
        }
    }

    class Address{
    public:
        typedef std::shared_ptr<Address> ptr;

        Address(sockaddr *addr,socklen_t len);

        static bool Lookup(std::vector<Address::ptr>& result, const std::string& host,
                           int family = AF_INET, int type = 0, int protocol = 0);
        static Address::ptr LookupAny(const std::string& host,
                                      int family = AF_INET, int type = 0, int protocol = 0);

        static bool GetInterfaceAddresses(std::multimap<std::string
                , std::pair<Address::ptr, uint32_t> >& result,
                                          int family = AF_INET);
        static bool GetInterfaceAddresses(std::vector<std::pair<Address::ptr, uint32_t> >& result
                , const std::string& iface, int family = AF_INET);

        virtual ~Address() {}

        void saveAddr(sockaddr_storage&& storage);

        int getFamily() const;
        sockaddr* getAddr() {return m_addr;}
        socklen_t getAddrLen() {return m_addrLen;}

        std::string toString() const;

        bool operator<(const Address& rhs) const;

        bool operator==(const Address& rhs) const;

        bool operator!=(const Address& rhs) const;
    private:
        sockaddr* m_addr;
        socklen_t m_addrLen;
        sockaddr_storage m_add_storge;
    };


}

#endif