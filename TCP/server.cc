//
// Created by 86178 on 2023/12/22.
//

#include "server.h"
#include "admin.h"

namespace asio = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;
namespace tcs{
    server::server() {
        tcs::admin::ptr admin(new tcs::admin());
        m_user_list =admin->getAllusers();
    }

    void server::run () {
        // 创建一个 I/O 上下文对象
        asio::io_context io_context;

        // 创建一个 TCP acceptor 监听端口
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

        while (true) {
            // 等待并接受连接
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            // 读取 HTTP 请求
            beast::flat_buffer buffer;
            beast::http::request<beast::http::dynamic_body> request;
            beast::http::read(socket, buffer, request);
            //TCS_LOG_INFO(db_log)<<"请求内容 ："<<request;
            //获取请求内容
            std::string method = request.method_string().to_string();
            std::string route = request.target().to_string();

            std::string path;
            std::map<std::string, std::string> param_map;

            // 构建 HTTP 响应
            beast::http::response<beast::http::string_body> response;
            response.version(request.version());
            response.result(beast::http::status::ok);
            response.set(beast::http::field::server, "Boost server");

            //解析请求内容
            // 匹配路由
            std::regex routeRegex(R"([^?]+)");
            std::smatch routeMatch;
            if (std::regex_search(route, routeMatch, routeRegex) && routeMatch.size() > 0) {
                std::string path = routeMatch[0];
                //std::cout << "Route: " << path << std::endl;
                // 匹配查询参数
                std::string queryParams = route.substr(route.find("?") + 1);
                std::regex paramsRegex(R"(([^=&\s]+)=([^&\s]+))");
                std::smatch paramsMatch;
                while (std::regex_search(queryParams, paramsMatch, paramsRegex)) {
                    if (paramsMatch.size() > 2) {
                        std::string key = paramsMatch[1];
                        std::string value = paramsMatch[2];
                        param_map.insert(std::pair(key, value));
                    }
                    queryParams = paramsMatch.suffix().str();
                }
            }
            switch (route[1]) {
                case 'l': {

                    std::string user_name = param_map.find("user_name")->second;
                    std::string user_card = param_map.find("user_card")->second;
                    std::string user_password = param_map.find("user_password")->second;
                    auto tuple = m_user_list.find(user_card)->second;
                    int id = std::get<0>(tuple);
                    std::string name= std::get<1>(tuple);
                    std::string password = std::get<2>(tuple);
                    std::string content;
                    if((user_name==name)&&(user_password==password)){
                        content = "验证成功,用户id为"+std::to_string(id);
                        TCS_LOG_INFO(db_log)<<"匹配用户成功";
                    }else {
                        content = "验证失败";
                        TCS_LOG_ERROR(db_log)<<"匹配用户失败";
                    }
                    rapidjson::Document document;
                    document.SetObject();
                    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

                    rapidjson::Value obj(rapidjson::kObjectType);
                    obj.AddMember("user_id", id, allocator);
                    document.AddMember("user", obj, allocator);
                    rapidjson::StringBuffer buffer;
                    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                    document.Accept(writer);
                    //发送
                    response.set(beast::http::field::content_type, "application/json");
                    response.body() = buffer.GetString();
                    response.prepare_payload();
                    break;
                }
                case 's': {
                    std::string departure = param_map.find("departure")->second;
                    std::string arrival = param_map.find("arrival")->second;
                    //查询验证
                    tcs::station::ptr station(new tcs::station(departure,arrival));
                    station->query_station();
                    std::vector<std::tuple<int,int,std::string,std::string>> result=\
                        station->getResult();
                    //创建json对象
                    rapidjson::Document document;
                    document.SetArray(); // 设置为数组类型
                    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
                    // 添加键值对到 JSON 对象
                    for(auto it:result) {
                        int value0, value2;
                        std::string str1, str2;
                        std::tie(value0, value2, str1, str2) = it;
                        rapidjson::Value obj(rapidjson::kObjectType);
                        obj.AddMember("Train_Number", value0, allocator);
                        obj.AddMember("Departure_Time", rapidjson::Value(str1.c_str(), allocator), allocator);
                        obj.AddMember("Arrival_Time", rapidjson::Value(str2.c_str(), allocator), allocator);
                        obj.AddMember("price", value2, allocator);
                        document.PushBack(obj, allocator);
                    }
                    // 将 JSON 对象序列化为字符串
                    rapidjson::StringBuffer buffer;
                    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                    document.Accept(writer);
                    //发送
                    response.set(beast::http::field::content_type, "application/json");
                    response.body() = buffer.GetString();
                    response.prepare_payload();
                    break;
                }
                case 'b':{
                    int Train_number = std::stoi(param_map.find("Train_Number")->second);
                    int user_id = std::stoi(param_map.find("user_id")->second);
                    std::string seat_type = param_map.find("seat_type")->second;
                    char seatType = seat_type[0];
                    /*TCS_LOG_DEBUG(db_log)<<Train_number;
                    TCS_LOG_DEBUG(db_log)<<user_id;
                    TCS_LOG_DEBUG(db_log)<<seatType;
                    TCS_LOG_DEBUG(db_log)<<seat_type;*/
                    //修改数据库座位数量
                    tcs::seat::ptr seat(new tcs::seat(Train_number));
                    TCS_LOG_DEBUG(db_log)<<"进行到这里";
                    seat->update_seat(seatType);
                    //增加订单
                    seat->insert_orders(user_id,seat_type);
                    //发送
                    std::string content = "购票成功,购买了"+std::to_string(Train_number)+"号列车";
                    response.body() = content;
                    response.prepare_payload();
                    break;
                }

                default:
                    break;
            }
            // 发送 HTTP 响应
            beast::http::write(socket, response);
            // 断开连接
            socket.shutdown(tcp::socket::shutdown_send);
        }
    }
}
