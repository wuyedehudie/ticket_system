cmake_minimum_required(VERSION 3.22)
project(ticketsystem)
set(CMAKE_CXX_STANDARD 17)

add_executable (test test/test.cc)
add_dependencies(test tcs)
target_link_libraries(test tcs)


add_executable(application application.cc)
add_dependencies(application tcs)
target_link_libraries(application tcs)
add_executable(try_use try_use.cc)


add_executable(test_http test/test_http.cc)
add_dependencies(test_http tcs)
target_link_libraries(test_http tcs)

target_link_libraries(test mysqlclient zstd ssl)
target_link_libraries(application mysqlclient zstd ssl)
target_link_libraries(test_http mysqlclient zstd ssl)

include_directories(.)

set(LIB_SRC
        admin.cc
        Log/log.cc
        TCP/socket.cc
        TCP/InetAddress.cc
        TicketDB/sql.cc
        TicketDB/station.cc
        TicketDB/seat.cc
        TCP/server.cc
        application.h
        thread/thread_pool.h
        locker.h
        TicketDB/sql_connection_pool.cc TicketDB/sql_connection_pool.h

        tcpServer.cc tcpServer.h
        TicketDB/sql_query.cc
        TicketDB/sql_query.h
        thread/Message_queue.cc
        thread/Message_queue.h
        thread/thread.cc thread/thread.h
        HTTP/http.cc
        utils/timer.cc
        utils/timer.h
        )

set(LIBS
       tcs
)
add_library(tcs SHARED ${LIB_SRC} )
target_include_directories(tcs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tcs)


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

