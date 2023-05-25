//
// Created by root on 2023/5/23.
//

#ifndef _TCPSOCKET_H
#define _TCPSOCKET_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>
using namespace std;
class TcpSocket{
public:
    TcpSocket();  //主要在客户端使用，通过此与服务器连 接
    TcpSocket(int socket);  //主要在服务器使用
    ~TcpSocket();
    int connectToHost(string ip,unsigned short port);
    //发送数据
    int sendMsg(string msg);

    string recvMsg(); //接受数据    int acceptConn(struct sockaddr_in *addr); //接受客户端连接结构体
    //获取当前的fd
    int get_fd();
    //获取当前发送文件的
    int m_size; //存储每次发送数据的长度

private: //解决TCP粘包：包头+数据包内容
    int readn(char* buf,int size); //接受数据,buf存储接受数据的内存起始地址，size接受的字节数
    int writen(const char* msg,int size);  //传输数据，msg需要发送的原始数据，size发送的总字节数
private:
    int m_fd; //通信的套接字


};

#endif //网络通信封装_TCPSOCKET_H
