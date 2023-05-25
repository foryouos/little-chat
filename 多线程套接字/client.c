// 客户端代码
// Created by root on 2023/5/22.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
int main(void)
{
    //创建套接字通信
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1)
    {
        perror("socket");
        exit(0);
    }
    //连接服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8989); //大端端口
    inet_pton(AF_INET,"192.168.131.137",&addr.sin_addr.s_addr);

    int ret = connect(fd,(struct sockaddr*)&addr,sizeof (addr));
    if(ret == -1)
    {
        perror("connect");
        exit(0);
    }
    //和服务器进行通信
    int number = 0;
    while(1)
    {
        //发送数据
        char buf[1024];
        sprintf(buf,"你好，服务器....%d/n",number++);
        write(fd,buf, strlen(buf)+1);
        //接收数据
        memset(buf,0,sizeof(buf)+1);
        int len = read(fd,buf, sizeof(buf));
        if(len>0)
        {
            printf("服务器say:%s\n",buf);

        }
        else if(len == 0)
        {
            printf("服务器断开了连接");
            break;
        }
        else
        {
            perror("read");
            break;
        }
        sleep(1);

    }
    close(fd);
    return 0;
}
