//
// Created by 86178 on 2023/12/22.
//

#ifndef TICKETSYSTEM_TCPSERVER_H
#define TICKETSYSTEM_SERVER_H
#include "TCP/socket.h"
#include "TicketDB/station.h"
#include "admin.h"
#include "TicketDB/seat.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <regex>
#include <boost/beast.hpp>
#include <boost/asio.hpp>

namespace tcs {
    class server {
    public:
        typedef std::shared_ptr<server> ptr;
        server();
        void run();
        void axio();
        void start();
    private:
        std::vector<socket> m_tcpList;
        std::map<std::string,std::tuple<int,std::string,std::string>> m_user_list;
    };
}

#endif //TICKETSYSTEM_TCPSERVER_H
