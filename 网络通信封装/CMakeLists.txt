cmake_minimum_required(VERSION 3.25)
project()

set(CMAKE_CXX_STANDARD 11)

add_executable(client.cpp server.cpp TcpServer.cpp TcpServer.h TcpSocket.cpp TcpSocket.h)
