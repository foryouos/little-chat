// 套接字通信服务器端
// Created by root on 2023/5/23.
//

#ifndef _TCPSERVER_H
#define _TCPSERVER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "TcpSocket.h"

class TcpServer{
public:
    TcpServer(); //构造函数
    ~TcpServer(); //析构函数
    int setListen(unsigned short port); //监听的端口号
    TcpSocket* acceptconn(struct sockaddr_in* addr = nullptr);

private:
    int J_fd; //监听的套接字
};
#endif //网络通信封装_TCPSERVER_H
