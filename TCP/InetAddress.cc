#include "TCP/InetAddress.h"
#include "Log/log.h"
#include <sstream>
#include <netdb.h>
#include <ifaddrs.h>
#include <stddef.h>
#include "utils/endian.h"

namespace tcs {


    template<class T>
    static T CreateMask(uint32_t bits) {
        return (1 << (sizeof(T) * 8 - bits)) - 1;
    }

    template<class T>
    static uint32_t CountBytes(T value) {
        uint32_t result = 0;
        for (; value; ++result) {
            value &= value - 1;
        }
        return result;
    }
    Address::Address(sockaddr *addr,socklen_t len)
            :m_addr(addr)
            ,m_addrLen(len){
    }

    Address::ptr Address::LookupAny(const std::string& host,
                                    int family, int type, int protocol) {
        std::vector<Address::ptr> result;
        if (Lookup(result, host, family, type, protocol)) {
            return result[0];
        }
        return nullptr;
    }
    void Address::saveAddr(sockaddr_storage &&storage) {
        m_add_storge = std::move(storage);
    }


    bool Address::Lookup(std::vector<Address::ptr>& result, const std::string& host,
                         int family, int type, int protocol) {
        addrinfo hints, *results, *next;
        memset(&hints, 0, sizeof hints);

        hints.ai_flags = 0;
        hints.ai_family = family;
        hints.ai_socktype = type;
        hints.ai_protocol = protocol;
        hints.ai_addrlen = 0;
        hints.ai_canonname = nullptr;
        hints.ai_addr = nullptr;
        hints.ai_next = nullptr;
        std::string service = "http";

        int error = getaddrinfo(host.c_str(), service.c_str(), &hints, &results);
        if (error) {
            std::cerr << "getaddrinfo error: " << gai_strerror(error) << std::endl;
            return false;
        }

        // 循环处理获取的地址信息
        for (addrinfo* next = results; next != nullptr; next = next->ai_next) {
            // 创建 Address 对象并保存地址信息
            Address::ptr address(new Address(next->ai_addr, next->ai_addrlen));
            address->saveAddr(std::move(*reinterpret_cast<sockaddr_storage*>(next->ai_addr)));
            result.push_back(address);
        }

        freeaddrinfo(results);
        return !result.empty();
    }
    bool Address::GetInterfaceAddresses(std::multimap<std::string
            , std::pair<Address::ptr, uint32_t> >& result,
                                        int family) {
        struct ifaddrs *next, *results;
        //待完善
        if (getifaddrs(&results) != 0) {
            TCS_LOG_DEBUG(rootlog) << "Address::GetInterfaceAddresses getifaddrs "
                                      " err=" << errno << " errstr=" << strerror(errno);
            return false;
        }

    }

    bool Address::GetInterfaceAddresses(std::vector<std::pair<Address::ptr, uint32_t> >& result
            , const std::string& iface, int family) {

        std::multimap<std::string
                , std::pair<Address::ptr, uint32_t> > results;

        if (!GetInterfaceAddresses(results, family)) {
            return false;
        }

        auto its = results.equal_range(iface);
        for (; its.first != its.second; ++its.first) {
            result.push_back(its.first->second);
        }
        return !result.empty();
    }


    std::string Address::toString() const {
        std::stringstream ss;
        ss<<m_addr;
        return ss.str();
    }



}
