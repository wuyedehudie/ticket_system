//
// Created by 86178 on 2024/4/29.
//

#include "tcpServer.h"

namespace tcs{
    tcpServer::tcpServer(tcs::socket sock,std::string user,std::string password, std::string DatabaseName)
    :m_socket(&sock)
    ,m_epollfd(epoll_create(5))
    ,m_sql_pool(sql_connection_pool::GetInstance())
    ,m_user(user)
    ,m_password(password)
    ,m_DatabaseName(DatabaseName){

    }
    void tcpServer::init() {
        //线程池

    }
    void tcpServer::creatTimer(int connfd) {
        //初始化client_data数据
        //创建定时器，设置回调函数和超时时间，绑定用户数据，将定时器添加到链表中
        users_timer[connfd].sockfd = connfd;
        util_timer *timer = new util_timer;
        timer->user_data = &users_timer[connfd];
        timer->cb_func = cb_func;
        time_t cur = time(NULL);
        timer->expire = cur + 3 * TIMESLOT;
        users_timer[connfd].timer = timer;
        utils.m_timer_lst.add_timer(timer);
    }
    void tcpServer::dealwithTimer(util_timer *timer, int sockfd){
        timer->cb_func(&users_timer[sockfd]);
        if (timer){
            utils.m_timer_lst.del_timer(timer);
        }
        TCS_LOG_ERROR(rootlog)<<"close fd %d", users_timer[sockfd].sockfd;
    }
    void tcpServer::dealwithThread(int fd) {
        namespace beast = boost::beast;
        namespace http = beast::http;
        string&& request=tcs::socket::recvAll(fd);
        size_t length=request.size();

        boost::asio::mutable_buffer buffer = boost::asio::buffer(request);

        boost::asio::buffer_copy(buffer, boost::asio::buffer(request.data(), length));

        // 创建响应对象并解析报文内容
        http::request<http::string_body> request_boost;
        beast::error_code ec;
        // 提取请求方法、请求路径和查询参数
        std::string method = request_boost.method_string().to_string();
        std::string target = request_boost.target().to_string();
        std::string query_param;

        // 解析查询参数
        size_t pos = target.find('?');
        if (pos != std::string::npos) {
            query_param = target.substr(pos + 1);
            target = target.substr(0, pos);
        }
        requestMsg();
    }

    bool tcpServer::dealwithSignal(bool &timeout, bool &stop_server)
    {
        int ret = 0;
        int sig;
        char signals[1024];
        ret = recv(m_pipefd[0], signals, sizeof(signals), 0);
        if (ret == -1){
            return false;
        }
        else if (ret == 0){
            return false;
        }
        else{

            for (int i = 0; i < ret; ++i){
                switch (signals[i]){
                    case SIGALRM:{
                        timeout = true;
                        break;
                    }
                    case SIGTERM:{
                        stop_server = true;
                        break;
                    }
                }
            }
        }
        return true;
    }
    void tcpServer::handle_client(tcs::socket::ptr &&sock) {
        util_timer *timer=users_timer[sock->getSock()].timer;
        creatTimer(sock->getSock());
    }

    void tcpServer::run() {
        bool is_timeout = false;            //是否超时，由系统信号处理
        bool is_stopedServer = false;       //是否停止运行，用户输入后经过系统信号通知
        while (!is_stopedServer)
        {
            int number = epoll_wait(m_epollfd, events, MAX_EVENT_NUM, -1);
            if (number < 0 && errno != EINTR){
                TCS_LOG_ERROR(rootlog)<<"%s", "epoll failure";
                break;
            }
            for (int i = 0; i < number; i++)
            {
                int sockfd = events[i].data.fd;
                //处理新到的客户连接
                if (sockfd == m_socket->getSock())
                {
                    //m_socket是服务器的socket，accept返回新创建的客户端的socket
                    handle_client(std::move(m_socket->accept()));
                }
                else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
                {
                    //服务器端关闭连接，移除对应的定时器
                    util_timer *timer = users_timer[sockfd].timer;
                    dealwithTimer(timer, sockfd);
                }
                //处理信号，定时器和是否停止服务器
                else if ((sockfd == m_pipefd[0]) && (events[i].events & EPOLLIN))
                {
                    bool flag = dealwithSignal(is_timeout, is_stopedServer);
                    if (false == flag)
                        TCS_LOG_INFO(rootlog)<<"%s", "dealclientdata failure";
                }
                //处理客户连接上接收到的数据, 处理业务
                else if (events[i].events & EPOLLIN)
                {
                    //由线程读取客户端传来的数据
                    //dealwitThread(sockfd);
                }
                //写入，待完善
                else if (events[i].events & EPOLLOUT)
                {
                    //dealwithwrite(sockfd);
                }
            }
            //定时器超时了，需要将超出时间未活跃的tcp连接释放，即释放对于的fd
            if (is_timeout)
            {
                utils.timer_handler();
                TCS_LOG_ERROR(rootlog)<<"%s", "timer tick";
                is_timeout = false;
            }
        }
    }
}