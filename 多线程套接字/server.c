/* 多进程开发:
 * 主线程:负责监听，处理客户端的连接请求，accept函数，创建子进程，回收子进程
 * 子线程负责通信，基于父进程建立连接之后得到的文件描述符，和客户端对应的接收数据和发送数据
 * 在多线程的服务器端程序中，多个线程共用一个地址空间，有些数据是共享的，有些数据是独占的
 * 同一个地址空间中多个栈空间是独占的
 * 多个线程共享数据区，堆区，以及内核区的文件描述符等资源， 需要注意数据覆盖问题，并且在多个线程访问共享资源的时候，还需要进行线程同步
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include <pthread.h>
//
struct SockInfo
{
    int fd;  //通信
    pthread_t tid; //线程ID
    struct sockaddr_in addr; //地址信息
};
struct SockInfo infos[128];

void* working(void* arg) //接受主线程传输的结构体信息
{
    while(1)
    {
        struct SockInfo* info = (struct SockInfo*)arg;
        //
        //接受数据
        char buf[1024];
        //接受数据
        //读数据通过文件描述符将内存中的数据读出，称之为读缓冲区
        //如果通信的套接字对应的读缓冲区没有数据，阻塞当前进程，线程，检测到数据解除阻塞，接收数据
        int len = read(info->fd,buf,sizeof (buf));
        if(len == 0)
        {
            printf("客户端断开了连接...\n");
            info->fd = -1;
            break;

        }
        else if(len == -1)
        {
            printf("接受数据失败\n");
            //发送数据，若写缓存区被写满，阻塞当前线程/进程
            info->fd = -1;
            break;
        }
        else
        {
            write(info->fd,buf, strlen(buf)+1);
            printf("");
        }
    }
    return  NULL;
}


int main() {
    //创建监听的套接字，此套接字是一个文件描述符
    //AF_INET 代表IPV4
    //
    //
    int fd =  socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1)
    {
        perror("socket");
        exit(0); //结束进程
    }
    // 将socket的返回值和本地的IP端口绑定在一起

    //设置监听,绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET; //设置类型IPV4
    addr.sin_port = htons(8989); //大端端口，同时设置端口哦
    addr.sin_addr.s_addr = INADDR_ANY; //这个hong值为0 = 0.0.0.0，监听本机的IP，此宏可以表示任意一个IP地址
    //将得到的监听点点滴滴的文件描述符和本地的IP端口进行绑定
    int ret = bind(fd,(struct sockaddr*)&addr,sizeof(addr));
    if(ret == -1)
    {
        perror("bind");
        exit(0);
    }
    //设置监听（成功之后开始监听，监听的是客户端的连接)
    ret = listen(fd,100);
    if(ret == -1)
    {
        perror("listen");
        exit(0);
    }
    // 等待，接受连接请求
    int len = sizeof(struct sockaddr);
    //数据初始化
    int max = sizeof(infos) / sizeof(infos[0]);
    for(int i = 0;i<max;++i)
    {
        bzero(&infos[i],sizeof(infos[i]));
        infos[i].fd = -1;  //通信
        infos[i].tid = -1;   //线程ID
    }
    // 父进程监听，子进程通信
    while(1) {
        //创建子线程
        struct SockInfo *pinfo;
        //存储相关线程信息
        for (int i = 0; i < max; i++) {
            if (infos[i].fd == -1) {
                pinfo = &infos[i];
                break;
            }
            if (i == max - 1) {
                sleep(1);
                i--;
            }
        }
        int connfd = accept(fd, (struct sockaddr *) &infos->addr, &len);
        printf("parent thread ,connfd: %d\n", connfd);
        if (connfd == -1) {
            perror("accept");
            exit(0);
        }
        pinfo->fd = connfd;
        pthread_create(&pinfo->tid, NULL, working, pinfo);
        pthread_detach(pinfo->tid);

    }
    //释放资源
    close(fd);//监听
    return 0;

}
