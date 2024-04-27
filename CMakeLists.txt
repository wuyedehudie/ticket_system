cmake_minimum_required(VERSION 3.22)
project(ticketsystem)

set(CMAKE_CXX_STANDARD 17)



add_executable (test test.cc)
add_dependencies(test tcs)
target_link_libraries(test tcs)

add_executable(application application.cc)
add_dependencies(application tcs)
target_link_libraries(application tcs)

include_directories(/www/server/mysql/include)
link_directories(/www/server/mysql/lib)
target_link_libraries(test mysqlclient zstd ssl)
target_link_libraries(application mysqlclient zstd ssl)

include_directories(.)
include_directories(TCP)

set(LIB_SRC
        TCP/TcpConnection.cc
        Log/log.cc
        TCP/InetAddress.cc
        TicketDB/sql.cc
        admin.cc
        TicketDB/station.cc
        HTTP/HttpResponse.cc
        TicketDB/seat.cc
        TCP/server.cc
)

set(LIBS
       tcs
)
add_library(tcs SHARED ${LIB_SRC} )
target_include_directories(tcs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tcs)


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

