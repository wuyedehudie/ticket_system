//
// Created by 86178 on 2024/4/29.
//

#ifndef TICKETSYSTEM_TCPSERVER_H
#define TICKETSYSTEM_TCPSERVER_H

#include "TCP/socket.h"
#include "TicketDB/sql_connection_pool.h"
#include "thread/thread_pool.h"
#include <sys/epoll.h>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include "utils/timer.h"

#define MAX_EVENT_NUM 50000

namespace tcs {
    class tcpServer {
    public:
        tcpServer(tcs::socket sock, std::string user,std::string password, std::string DatabaseName);
        //处理新的请求
        void init();
        void creatTimer(int connfd);
        void dealwithTimer(util_timer *timer, int sockfd);
        void dealwithThread(int fd);
        bool dealwithSignal(bool &timeout, bool &stop_server);
        void handle_client(tcs::socket::ptr&& sock);
        void run();
    private:
        //server
        tcs::socket::ptr m_socket;
        int port;
        bool m_isStop;
        int m_pipefd[2];                        //用于定时器相关的系统信号
        client_data *users_timer;
        Utils utils;

        //epoll
        int m_epollfd;
        epoll_event events[MAX_EVENT_NUM];

        //thread
        threadpool::ptr m_threadpool;
        //sql
        string m_user;
        string m_password;
        string m_DatabaseName;
        sql_connection_pool* m_sql_pool;

    };
}


#endif //TICKETSYSTEM_TCPSERVER_H
