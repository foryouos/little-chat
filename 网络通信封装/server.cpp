// 套接字通信客户端头文件
// Created by root on 2023/5/23.
//

#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "TcpSocket.h"
#include "TcpServer.h"
//定义全局变量  //套接字通信数组用于sendclient通信
struct SockInfo
{
    TcpServer* s;
    TcpSocket* tcp;
    struct sockaddr_in addr;
};
int s_max = 0 ;//用于存放当前的总线程个数
int fdt[1024] = {0}; //用于村当套接字(文件描述符)的数组
string buff ;  //接收缓冲区

//搭建一个TCP list存放结构体SockInfo

struct SockInfo* Infos[1024];  //将每个线程的sock信息存入结构体
//list<SockInfo> Infos = new list<SockInfo>();

//定义转发函数声明
int SendToClient(int fd,int size,struct SockInfo* pinfo) //size字节数,fd
{
//    cout<<"给其它客户端发送数据"<<endl;
    //cout<<"在线线程个数"<<s_max<<endl;
    //cout<<"缓冲区数据"<<buff<<endl;

//需要发送给非本tc
    for(int i =0;i < s_max;i++)
    {
        if(Infos[i]->tcp != pinfo->tcp)  //如果列表内的tcp不是当前的tcp则发送数据
        {
            //cout<<"\tInfos[i]: "<<Infos[i]->tcp<<"\tpinfo:"<<pinfo->tcp<<endl;
            cout<<endl;
            //同时传递环行
            Infos[i]->tcp->sendMsg("\n");
            Infos[i]->tcp->sendMsg(buff);
        }

    }
//    cout<<"size的数值为"<<size<<endl;
//    cout<<"发送了数据"<<endl;


//    }
    bzero(&buff, sizeof(buff));
    return 0;
}


void* working(void* arg)
{
    cout<<"创建了多线程:"<< endl;
    struct SockInfo* pinfo = static_cast<struct SockInfo*>(arg);
    //连接建立成功，打印客户端的IP和端口信息
    char ip[32];
    printf("客户端的IP：%s，端口:%d\n",
           inet_ntop(AF_INET,&pinfo->addr.sin_addr.s_addr,ip, sizeof(ip)),
           ntohs(pinfo->addr.sin_port));
    //将当前客户端的socket存入到fdt[]当中，并使当前连接数 n +1
    //fdt[s_max++] = pinfo->tcp->get_fd();

    //将此线程下的pinfo加入到Infos当中
    Infos[s_max++] = pinfo;

    pinfo->tcp->sendMsg("连接测试成功"); //发送成功，接收客户端接收到了

    //5，通信
    while (1)
    {
        //通信



        //printf("接收数据:....\n");
        string msg = pinfo->tcp->recvMsg(); //接收数据
        //cout<<"测试："<<msg<<endl;
        if(!msg.empty())
        {
            cout<< ntohs(pinfo->addr.sin_port) <<"::" <<  msg<<endl; //输出接收信息
            //接收到数据并将数据转达到除了自己的其它客户端
            //当前的socket，当前发送文件的大小，以及所需要的其其它信息
            buff = msg;
            SendToClient(pinfo->tcp->get_fd(),pinfo->tcp->m_size,pinfo);
            continue;
        }
       else if(msg == string()) //如何客户端断开连接
        {
            //使当前的fdt中存储的对应socket进行删除

            //使当前的连接数n - 1;

            cout<<"用户： " << ntohs(pinfo->addr.sin_port) <<" 断开了连接"<<endl;


            //如果客户端断开了连接
            break;
        }
        else
        {
            cout<<"进入else"<<endl;
            continue;
        }

    }

    delete pinfo->tcp;
    //delete pinfo->s; //监听不能关闭，关闭后服务器也关闭，
    delete pinfo;
    pthread_exit(0);  //退出线程
    return nullptr;

}
int main()
{
    //1,创建监听的套接字
    TcpServer s;
    //2,绑定本地的IP port并设置监听
    s.setListen(10000);
    //3,阻塞并等待客户端的连接
    while(1)
    {
        SockInfo* info = new SockInfo;
        TcpSocket* tcp = s.acceptconn(&info->addr);
        if(tcp == nullptr)
        {
            cout<<"重试....."<<endl;
            continue;
        }
        //当服务器输出exit，推出服务器连接
//        char endbuf[10];
//        fgets(endbuf, sizeof(endbuf),stdin);
//        cout<<endbuf<<endl;
//        if(strcmp(endbuf,"exit\n") == 0)
//        {
//            cout<<"服务器退出..."<<endl;
//            //TODO向客户端传递服务器结束
//            break;
//        }


        //创建子线程
        pthread_t tid;
        info->s=&s;
        info->tcp=tcp;
        pthread_create(&tid,NULL,working,info);
        pthread_detach(tid); //与主线程脱离
    }

    return 0;
}


#endif //网络通信封装_TCPCLIENT_H
