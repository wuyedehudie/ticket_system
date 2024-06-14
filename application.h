//
// Created by 86178 on 2024/4/29.
//

#ifndef TICKETSYSTEM_APPLICATION_H
#define TICKETSYSTEM_APPLICATION_H

#include <TCP/socket.h>
#include <memory>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

class application{
public:
    typedef std::shared_ptr<application> ptr;
    void init();
};

#endif //TICKETSYSTEM_APPLICATION_H
