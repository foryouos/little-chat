/***
Server 端

Auther：Liang jie
Objective：服务端将收到的消息转发给其他客户端
*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>
#include <pthread.h>
#define PORT 10005

#define Max 10   //最大连接数,也就是可以参与群聊的最大人数

#define MAXSIZE 1024

//转发函数的声明
int SendToClient(int fd,char* buf,int Size);


/*定义全局变量*/
int fdt[Max]={0};    //用来存套接字(文件描述符)的数组
char mes[MAXSIZE];   //接收缓冲区
/**/


//子线程函数
void *pthread_service(void* sfd)
{
    int fd=*(int *)sfd;
    while(1)
    {
        int numbytes;  //返回的实际字节数
        int i;

        numbytes=recv(fd,mes,MAXSIZE,0);

        if(numbytes<=0)
        {
            for(i=0;i<Max;i++){
                if(fd==fdt[i]){
                    fdt[i]=0;
                }
            }
            printf("客户端 %d 已退出\n",fd);
            break;
        }

        printf("receive message from %d：\n",fd);
        printf("转发的信息=%s\n",mes);

        //开始转发
        SendToClient(fd,mes,numbytes);
        bzero(mes,MAXSIZE);
    }

    close(fd);
    pthread_exit(0);
}


/*转发函数*/
int SendToClient(int fd,char* buf,int Size)
{
    int i;
    int e;

    for(i=0;i<Max;i++)
    {
        //给在线的客户端转发消息
        //if((fdt[i]!=0)&&(fdt[i]!=fd))
            send(fdt[i],buf,Size,0);
    }
    bzero(buf,sizeof(buf));
    return 0;
}



int  main()
{


    int listenfd, connectfd;
    struct sockaddr_in server;
    struct sockaddr_in client;
    int sin_size;
    sin_size=sizeof(struct sockaddr_in);
    int number=0;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Creating socket failed.");
        exit(1);
    }

    bzero(&server,sizeof(server));

    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr = htonl (INADDR_ANY);


    if (bind(listenfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        perror("Bind error.");
        exit(1);
    }


    if(listen(listenfd,1) == -1){
        perror("listen() error\n");
        exit(1);
    }
    printf("Waiting for client....\n");


    while(1)
    {

        if ((connectfd = accept(listenfd,(struct sockaddr *)&client,&sin_size))==-1) {
            perror("accept() error\n");
            exit(1);
        }

        if(number>=Max){
            printf("no more client is allowed\n");
            close(connectfd);
        }

        int i;
        for(i=0;i<Max;i++)
        {
            if(fdt[i]==0)
            {
                fdt[i]=connectfd;
                break;
            }
        }

        pthread_t tid;
        pthread_create(&tid,NULL,(void*)pthread_service,&connectfd);
        pthread_detach(tid);
        number=number+1;
    }

    close(listenfd);
    return 0;
}

