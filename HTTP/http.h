#ifndef __TCS_HTTP_HTTP_H__
#define __TCS_HTTP_HTTP_H__

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <boost/beast.hpp>


namespace beast = boost::beast;
namespace http = beast::http;

namespace tcs{
    class response{
    public:
        void setBody(std::string);



    private:
        http::response<http::string_body> response;
        std::string m_content;
    };
}

#endif
