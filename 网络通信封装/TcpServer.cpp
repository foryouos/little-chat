//
// Created by root on 2023/5/23.
//
#include "TcpServer.h"
TcpServer::TcpServer()
{
    cout<<"已创建监听的套接字....."<<endl;
    J_fd = socket(AF_INET,SOCK_STREAM,0);
}
TcpServer::~TcpServer()
{
    cout<<"已关闭监听的套接字....."<<endl;
    close(J_fd);
}
int TcpServer::setListen(unsigned short port)
{
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(J_fd,(struct sockaddr*)&saddr, sizeof(saddr));
    if(ret == -1)
    {
        perror("bind");
        return -1;
    }
    cout<<"套接字绑定成功,ip:"<<inet_ntoa(saddr.sin_addr) <<", port :" <<port<<endl;
    ret = listen(J_fd,128);
    if(ret == -1)
    {
        perror("listen");
        return -1;
    }
    cout<< "设置监听成功...."<<endl;
    return ret;
}
TcpSocket* TcpServer::acceptconn(struct sockaddr_in *addr)
{
    if(addr == NULL)
    {
        return nullptr;
    }
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int cfd = accept(J_fd,(struct sockaddr*)addr,&addrlen);
    if(cfd == -1)
    {
        perror("accept");
        return nullptr;
    }
    printf("成功和客户端建立连接...\n");
    return new TcpSocket(cfd);
}