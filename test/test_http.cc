#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <iostream>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>

int main()
{
    try
    {
        // Construct an HTTP request with custom fields
        http::request<http::string_body> req{http::verb::get, "/", 11};
        req.set(http::field::host, "www.example.com");
        req.set(http::field::user_agent, "MyCustomUserAgent/1.0");

        // Print the HTTP request
        std::cout << "HTTP Request:" << std::endl;
        std::cout << req << std::endl;

        // Construct an HTTP response with custom fields
        std::string response_body = "Hello, Boost.Beast!";
        http::response<http::string_body> res{http::status::ok, 11};
        res.set(http::field::server, "MyCustomServer/1.0");
        res.set(http::field::content_type, "text/plain");
        res.content_length(response_body.size());

        // Print the HTTP response
        std::cout << "HTTP Response:" << std::endl;
        std::cout << res << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}













/*
#include "Log/log.h"
#include <boost/beast.hpp>
namespace beast = boost::beast;
namespace http = beast::http;


void test_response() {
    // 创建一个 HTTP 响应对象
    http::response<http::string_body> response;

    // 设置响应状态码和原因短语
    response.result(http::status::ok);
    response.reason("OK");

    // 设置响应头部
    response.set(http::field::server, "MyServer/1.0");
    response.set(http::field::content_type, "text/plain");

    // 设置响应体
    response.body() = "Hello, world!";
    response.prepare_payload();

    // 创建一个字符串缓冲区，用于存储 HTTP 报文
    std::string response_string;

    // 将 HTTP 响应对象写入字符串缓冲区中
    beast::flat_buffer buffer;
    response_string = beast::buffers_to_string(buffer.data());

    // 打印 HTTP 响应报文内容
    TCS_LOG_DEBUG(rootlog) << "HTTP Response:\n" << response_string << std::endl;
}

int main(int argc, char** argv) {
    test_response();
    return 0;
}
*/
