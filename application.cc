//
// Created by 86178 on 2023/12/22.
//
#include "TicketDB/sql.h"
#include "TCP/socket.h"
#include "TCP/server.h"
#include "application.h"
#include <thread>

using namespace tcs;

namespace beast = boost::beast;
namespace http = beast::http;

void sql_thread(){

}
std::string handle_request(std::string request){
    // Create a buffer for the raw request data
    beast::flat_buffer buffer;
    buffer.commit(boost::asio::buffer_copy(
            buffer.prepare(request.size()),
            boost::asio::buffer(request)));
    // Create a request parser
    http::request<http::string_body> req;
    http::request_parser<http::string_body> parser;

    // Parse the request
    beast::error_code ec;
    parser.put(buffer.data(), ec);
    if (ec) {
        throw beast::system_error{ec};
    }
    req = parser.get();
    return std::string(req.target());
}
void deal_OPTION(std::string& response){
    response = "HTTP/1.1 204 No Content\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
    "Access-Control-Allow-Headers: content-type\r\n"
    "Access-Control-Max-Age: 86400\r\n"  // 24小时缓存
    "\r\n";
}
void deal_LOGIN(std::string& response){
    response =
}

void task(std::string request){
    std::string response;
    std::string mathod=handle_request(request);
    //路由解析
    if(mathod=="OPTION"){
        deal_OPTION(response);
    }else if(mathod=="Login"){
        deal_LOGIN(response);
    }else if(mathod=="station")
}

void application::init() {
    tcs::socket::ptr server(std::move(tcs::socket::CreateTCP("0.0.0.0:8080")));
    server->bind();
    server->listening();
}


int main(){
    application::ptr app(new application);
    app->init();
    //tcpServer::ptr tcpServer(new tcs::tcpServer());
    //tcpServer->run();

}