/* 多进程开发:
 * 父进程:负责监听，处理客户端的连接请求，accept函数，创建子进程，回收子进程
 * 子进程负责通信，基于父进程建立连接之后得到的文件描述符，和客户端对应的接收数据和发送数据
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
//信号处理函数
void callback(int num)
{
    while(1)
    {
        pid_t pid = waitpid(-1,NULL,WNOHANG);
        if(pid<= 0)
        {
            printf("子进程正在运行，或者子进程被回收完毕");
            break;
        }
        printf("child die,pid = %d\n",pid);
    }
}
int childWork(int cfd);
int main() {
    //创建监听的套接字，此套接字是一个文件描述符
    //AF_INET 代表IPV4
    //
    //
    int lfd =  socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1)
    {
        perror("socket");
        exit(0); //结束进程
    }
    // 将socket的返回值和本地的IP端口绑定在一起

    //设置监听
    struct sockaddr_in addr;
    addr.sin_family = AF_INET; //设置类型IPV4
    addr.sin_port = htons(10000); //大端端口，同时设置端口哦
    addr.sin_addr.s_addr = INADDR_ANY; //这个hong值为0 = 0.0.0.0，监听本机的IP，此宏可以表示任意一个IP地址
    //将得到的监听点点滴滴的文件描述符和本地的IP端口进行绑定
    int ret = bind(lfd,(struct sockaddr*)&addr,sizeof(addr));
    if(ret == -1)
    {
        perror("bind");
        exit(0);
    }
    //设置监听（成功之后开始监听，监听的是客户端的连接)
    ret = listen(lfd,128);
    if(ret == -1)
    {
        perror("listen");
        exit(0);
    }

    //注册信号的捕捉
    //信号的优先级较高
    struct sigaction act;
    act.sa_flags =0;
    act.sa_handler = callback;
    sigemptyset(&act.sa_mask);
    sigaction(SIGCHLD,&act,NULL);
    // 接受多个客户端连接，对需要循环调用accept
    while(1)
    {
        //阻塞等待并接受客户端连接
        struct sockaddr_in cliaddr;
        int clilen = sizeof(cliaddr);
        //等待并接受客户端的连接请求，建立新的连接，会得到一个新的文件描述符,没有新连接请求就阻塞
        int cfd = accept(lfd,(struct sockaddr*)&cliaddr,&clilen);
        if(cfd == -1)
        {
            if(errno == EINTR)
            {
                //accept调用被信号中断了，解除阻塞，返回了-1
                //重新调用一次accept
                continue;
            }
            perror("accept");
            exit(0);
        }
        //打印客户端的地址信息
        char ip[24] = {0};
        printf("客户端的IP地址: %s,端口:%d\n", inet_ntop(AF_INET,&cliaddr.sin_addr.s_addr,ip,sizeof(ip)),ntohs(cliaddr.sin_port));
        //新的连接已经建立，创建子进程，让子进程和这个客户端通信
        pid_t pid = fork();
        if(pid == 0)
        {
            //子进程 - > 和客户端通信
            //通信的文件描述符cfd被拷贝到子进程中
            // 子进程不负责监听
            close(lfd); //  关闭子进程的socket套接字，接下来进行通信
            while(1)
            {
                int ret = childWork(cfd); //和客户端通信函数
                if(ret <= 0)
                {
                    break;
                }
            }
            //退出子进程
            close(cfd);
            exit(0);
        }
        else if(pid > 0)
        {
            //父进程不和客户端通信
            close(cfd); //关闭通信函数
        }

    }
    return 0;
}
//和客户端通信
int childWork(int cfd)
{
    //接受数据
    char buf[1024];
    memset(buf,0,sizeof (buf));
    //接受数据
    //读数据通过文件描述符将内存中的数据读出，称之为读缓冲区
    //如果通信的套接字对应的读缓冲区没有数据，阻塞当前进程，线程，检测到数据解除阻塞，接收数据
    int len = read(cfd,buf,sizeof (buf));
    if(len > 0)
    {
        printf("客户端say: %s\n",buf);
        //发送数据，若写缓存区被写满，阻塞当前线程/进程
        write(cfd,buf,len);
    }
    else if(len == 0)
    {
        printf("客户端断开了连接...\n");
    }
    else
    {
        perror("read");
    }
    return len;
}
