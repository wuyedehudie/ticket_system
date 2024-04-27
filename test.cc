
#include "Log/log.h"
#include "admin.h"
#include "TicketDB/station.h"
#include "TicketDB/seat.h"
#include "TCP/TcpConnection.h"

int main() {


    //测试tcp连接
tcs::TcpConnection::ptr test_conn(tcs::TcpConnection::CreateTCPSocket("baidu.com"));
    const char* request = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\n\r\n";
    test_conn->connect();
    tcs::printSocketAddress(test_conn->getPeerAddr());
    test_conn->send(request);
    test_conn->recv();
    TCS_LOG_INFO(rootlog) << test_conn->getContent();
    test_conn->close();



    //tcs::admin::ptr test_admin(new tcs::admin("李华","440981200001036666","123456"));
    //if(test_admin->check()){
    //    TCS_LOG_DEBUG(rootlog)<< "用户匹配成功";
    //}else TCS_LOG_DEBUG(rootlog)<< "用户匹配失败";
    //test_admin->close();

    //测试查询座位
    tcs::seat::ptr test_seat(new tcs::seat(0));
std::vector<int> a;
    a = test_seat->getResult();
    for(int i = 0;i<5;++i){
        TCS_LOG_ERROR(rootlog)<<"seat"<<std::to_string(i)<<" "<<a[i];
    }

    test_seat->update_seat('d');
    test_seat->insert_orders(1,"a");

    //测试查询车票
    //1.创建station,输入两地
    //2.执行查询
    //3.获取result
    //4.循环打印每一条结果
     tcs::station::ptr test_station(new tcs::station("北京","上海"));
     test_station->query_station();
    std::vector<std::tuple<int,int,std::string,std::string>> result=\
    test_station->getResult();
    for (const auto& tuple : result) {
        int value0, value2;
        std::string str1, str2;
        std::tie(value0, value2, str1, str2) = tuple;
        TCS_LOG_INFO(rootlog)<<value0<<" "<<value2<<" "<<str1<<" "<<str2;
    }

    //test_insert_station->close();
    //测试插入车次
    // tcs::station::ptr test_insert_station(new tcs::station("汕头","深圳"));
    //test_insert_station->insert_station(2,"12:30:00","14:00:00",140);
    //test_insert_station->query_station();
    //test_insert_station->passer_result();
    //std::vector<std::tuple<int,int,std::string,std::string>> result=\

}
