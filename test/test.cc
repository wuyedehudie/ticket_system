
#include "Log/log.h"
#include "admin.h"
#include "TicketDB/station.h"
#include "TicketDB/seat.h"
#include "TCP/socket.h"

tcs::Logger::ptr g_logger =TCS_LOG_ROOT();

//检查一个sockfd关联的地址的值
void check_bind(int sock){
    struct sockaddr_in local_addr;
    socklen_t addrlen = sizeof(local_addr);
    if (getsockname(sock, reinterpret_cast<struct sockaddr*>(&local_addr), &addrlen) == -1) {
        std::cerr << "Failed to get socket name" << std::endl;
    }
    uint16_t port = ntohs(local_addr.sin_port);
    TCS_LOG_INFO(g_logger)<<"检查port:"<<port<<endl;
    char ip_str[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &local_addr.sin_addr, ip_str, INET_ADDRSTRLEN) == nullptr) {
        std::cerr << "Failed to convert IP to string" << std::endl;
    }
    TCS_LOG_INFO(g_logger) << "ip:" << ip_str;
    TCS_LOG_INFO(g_logger) << "len:"<< sizeof(local_addr);
}

//测试Address能否将域名转为ip
void test1(){
    //tcs::socket::ptr server(tcs::socket::CreateTCP("localhost:8080"));
    std::vector<tcs::Address::ptr> addrs;
    TCS_LOG_INFO(g_logger) << "begin";
    bool v = tcs::Address::Lookup(addrs, "www.baidu.com", AF_INET);
    TCS_LOG_INFO(g_logger) << "end";
    if(!v) {
        TCS_LOG_ERROR(g_logger) << "lookup fail";
    }
    for(size_t i = 0; i < addrs.size(); ++i) {
        TCS_LOG_INFO(g_logger) << i << " - " << addrs[i]->toString();
    }
}
//检测系统调用有没有问题bind()
bool test2(){
    // 创建 IPv4 地址结构体
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr)); // 清零结构体
    //(sockaddr*)&m_addr;
    // 设置地址族和端口号
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080); // 设置端口号，注意要使用网络字节序

    // 设置 IP 地址
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        std::cerr << "Invalid address" << std::endl;
        return 1;
    }
    // 输出地址信息
    int fd=2;
    int rt=::bind(fd,(sockaddr*)&addr,sizeof(addr));
    std::cout <<"rt" <<rt<<std::endl;
    std::cout << "Address: " << inet_ntoa(addr.sin_addr) << std::endl;
    std::cout << "Port: " << ntohs(addr.sin_port) << std::endl;
    std::cout << "addrlen: " << sizeof(addr)<<std::endl;
    return 0;
};
//测试自己封装的socket绑定服务器、监听服务器的sockfd、接收请求功能是否正常
void test3(){
    tcs::Address::ptr addr = tcs::Address::LookupAny("0.0.0.0:8080");
    tcs::socket::ptr server_sock(std::move(tcs::socket::CreateTCP(addr)));

    server_sock->bind();
    if(addr) {
        TCS_LOG_INFO(g_logger) << *addr;
        const sockaddr_in* addr_in = reinterpret_cast<const sockaddr_in*>(addr->getAddr());
        TCS_LOG_INFO(g_logger) <<"Port:" << ntohs(addr_in->sin_port);
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr_in->sin_addr), ip, INET_ADDRSTRLEN);
        TCS_LOG_INFO(g_logger) <<"ip"<<ip;
        check_bind(server_sock->getSock());
    } else {
        TCS_LOG_ERROR(g_logger) << "error";
    }
    server_sock->listening();
}

//服务器全流程实例，检测基本环境有没有问题
void test4(){
    // 创建套接字
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket\n";
    }
    // 绑定地址
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080); // 监听端口8080
    server_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Failed to bind address\n";
    }
    // 开始监听连接
    if (listen(server_socket, 5) == -1) {
        std::cerr << "Failed to listen for connections\n";
    }
    std::cout << "Server listening on port 8080...\n";
    // 接受连接
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_size);
    if (client_socket == -1) {
        std::cerr << "Failed to accept connection\n";
    }

    check_bind(server_socket);
    // 打印连接信息
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
    ::send(client_socket,"收到",sizeof("收到"),0);
    std::cout << "Connection accepted from " << client_ip << ":" << ntohs(client_address.sin_port) << "\n";
    // 关闭套接字
    close(client_socket);
    close(server_socket);
}

int main() {
    //test();
    //test2();
    //test3();
    //test4();




   /* //测试tcp连接
    tcs::socket::ptr test_conn(tcs::socket::CreateTCPSocket("baidu.com"));
    const char* request = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\n\r\n";
    test_conn->connect();
    tcs::printSocketAddress(test_conn->getAddr());
    test_conn->send(request);
    test_conn->recv();
    TCS_LOG_INFO(rootlog) << test_conn->getContent();
    test_conn->close();
*/
    //tcs::admin::ptr test_admin(new tcs::admin("李华","440981200001036666","123456"));
    //if(test_admin->check()){
    //    TCS_LOG_DEBUG(rootlog)<< "用户匹配成功";
    //}else TCS_LOG_DEBUG(rootlog)<< "用户匹配失败";
    //test_admin->close();

   /* //测试查询座位
    tcs::seat::ptr test_seat(new tcs::seat(0));
    std::vector<int> a;
    a = test_seat->getResult();
    for(int i = 0;i<5;++i){
        TCS_LOG_ERROR(rootlog)<<"seat"<<std::to_string(i)<<" "<<a[i];
    }

    test_seat->update_seat('d');
    test_seat->insert_orders(1,"a");
*/
    //测试查询车票
    //1.创建station,输入两地
    //2.执行查询
    //3.获取result
    //4.循环打印每一条结果
   /*  tcs::station::ptr test_station(new tcs::station("北京","上海"));
     test_station->query_station();
    std::vector<std::tuple<int,int,std::string,std::string>> result=\
    test_station->getResult();
    for (const auto& tuple : result) {
        int value0, value2;
        std::string str1, str2;
        std::tie(value0, value2, str1, str2) = tuple;
        TCS_LOG_INFO(rootlog)<<value0<<" "<<value2<<" "<<str1<<" "<<str2;
    }*/

    //test_insert_station->close();
    //测试插入车次
    // tcs::station::ptr test_insert_station(new tcs::station("汕头","深圳"));
    //test_insert_station->insert_station(2,"12:30:00","14:00:00",140);
    //test_insert_station->query_station();
    //test_insert_station->passer_result();
    //std::vector<std::tuple<int,int,std::string,std::string>> result=\

}
