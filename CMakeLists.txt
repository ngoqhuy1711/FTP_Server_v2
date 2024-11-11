cmake_minimum_required(VERSION 3.29)
project(FTP_Server_v2)

set(CMAKE_CXX_STANDARD 14)

add_library(Lib Lib/Header/cli.h
        Lib/Header/serverconfig.h
        Lib/Header/session.h
        Lib/Header/socket.h
        Lib/Header/tcpserver.h
        Lib/Header/tcpsocket.h
        Lib/Header/tcpserversocket.h
        Lib/Implementation/cli.cpp
        Lib/Implementation/serverconfig.cpp
        Lib/Implementation/session.cpp
        Lib/Implementation/socket.cpp
        Lib/Implementation/tcpserver.cpp
        Lib/Implementation/tcpsocket.cpp
        Lib/Implementation/tcpserversocket.cpp)

add_executable(FTP_Server_v2 Source/main.cpp
        Lib/Header/cli.h
        Lib/Header/serverconfig.h
        Lib/Header/session.h
        Lib/Header/socket.h
        Lib/Header/tcpserver.h
        Lib/Header/tcpsocket.h
        Lib/Header/tcpserversocket.h
        Lib/Implementation/cli.cpp
        Lib/Implementation/serverconfig.cpp
        Lib/Implementation/session.cpp
        Lib/Implementation/socket.cpp
        Lib/Implementation/tcpserver.cpp
        Lib/Implementation/tcpsocket.cpp
        Lib/Implementation/tcpserversocket.cpp
        Source/Header/ftpserverconfig.h
        Source/Implementation/ftpserverconfig.cpp
        Source/Header/ftpsession.h
        Source/Implementation/ftpsession.cpp
        Source/Header/ftpserver.h
        Source/Implementation/ftpserver.cpp
        Source/Header/ftpservercli.h
        Source/Implementation/ftpservercli.cpp)

include_directories(Lib/Header Lib/Implementation Source/Header Source/Implementation)

target_link_libraries(FTP_Server_v2 Lib ws2_32)