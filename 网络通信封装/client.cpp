//
// Created by root on 2023/5/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include "TcpSocket.h"
#define MAXSIZE 4096

string sendbuf; //发送缓冲区
string recvbuf; //接收缓冲区

string name;//存放用户名
int fd;  //套接字

//1,创建通信的套接字
TcpSocket tcp;

//用于接收信息的子线程
void *pthread_recv(void* arg)
{
    //cout<<"进入接收数据线程"<<endl;
    //将传入的之变换关系
    while(1)
    {
        //cout<<"接收数据循环"<<endl;

        recvbuf = tcp.recvMsg(); //TODO这个进入，没有出来
        if(recvbuf == string())
        {
            break;
            exit(1);
            //关闭线程
            pthread_exit(0);
        }
        cout<<recvbuf<<endl;

        //sleep(1);
        recvbuf.clear();//清空缓冲区
    }
    cout<<"接收信息线程结束"<<endl;
}


int main()
{


    //2，连接服务器IP Port
    int ret = tcp.connectToHost("192.168.131.137",10000);
    if(ret == -1)
    {
        return -1;
    }
    cout<<"connet success"<<endl;
    cout<<"请输入你的用户名:";
    cin>>name;
    sendbuf = name + "建立了连接!!!";

    tcp.sendMsg(sendbuf);
    //创建子线程接收数据端的数据
    pthread_t tid;
    pthread_create(&tid,NULL,pthread_recv,NULL);
    pthread_detach(tid);

    //3,通信
    while(1) //循环
    {
        //首先接收服务器发送的数据

        //通过用户输入给服务器发送数据
        //用户输入数据
        cout<<name<<"\t:";
        string send;
        cin>>send;
        cout<<endl;
        sendbuf = name + " : " + send;

        tcp.sendMsg(sendbuf); //把用户名和新信息一起发送



        //sleep(10);
        //sleep(1);

    }
    return 0;
}

