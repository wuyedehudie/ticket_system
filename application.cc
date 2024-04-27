//
// Created by 86178 on 2023/12/22.
//
#include "TicketDB/sql.h"
#include "TCP/TcpConnection.h"
#include "TCP/server.h"


using namespace tcs;
int main(){

    server::ptr server(new tcs::server());
    server->run();
}