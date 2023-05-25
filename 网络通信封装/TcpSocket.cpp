//
// Created by root on 2023/5/23.
//

#include "TcpSocket.h"
TcpSocket::TcpSocket()
{
    cout<<"建立socket连接......"<<endl;
    m_fd = socket(AF_INET,SOCK_STREAM,0);
}
TcpSocket::TcpSocket(int socket)
{
    m_fd = socket;
}
TcpSocket::~TcpSocket()
{
    if(m_fd > 0)
    {
        close(m_fd);
    }
}
int TcpSocket::get_fd()
{
    return m_fd;
}
int TcpSocket::connectToHost(std::string ip, unsigned short port)
{
    //连接服务器IP port
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    inet_pton(AF_INET,ip.data(),&saddr.sin_addr.s_addr);
    int ret = connect(m_fd,(struct sockaddr*)&saddr,sizeof(saddr));
    if(ret == -1)
    {
        perror("connect");
        return -1;
    }
    cout << "成功和服务器建立连接..."<<endl;
    return ret; //返回建立的连接id

}
int TcpSocket::sendMsg(std::string msg)
{
    //申请内存空间:数据长度+包头4字节(存储数据长度)
    char * data = new char[msg.size() +4];
    int bigLen = htonl(msg.size());
    memcpy(data,&bigLen,4); //复制内存块，将要复制的const void*指针副知道第一个目的的，第三个是要复制的字节数
    memcpy(data+4,msg.data(),msg.size());
    int ret = writen(data,msg.size()+4);
    delete[] data; //释放申请的空间
    return ret;
}
string TcpSocket::recvMsg()
{
    //🉑数据
    //1,读数据头
    //cout<<"进入recv内部"<<endl;
    int len =0;
    readn((char*)&len,4); //读取长度
    len= ntohl(len);
    m_size = len;
    // cout<<"数据块大小:"<<len<<endl;
    //根据读出的长度分配内存
    char* buf = new char[len+1];
    int ret = readn(buf,len); //再次读取数据
    if(ret!=len)
    {
        return string();
    }
    if(ret == 0)
    {
        cout<<"服务器断开了连接......"<<endl;
        return string();
    }
    buf[len] = '\0';
    string retStr(buf);
    delete[]buf;
    return retStr; //返回整理后的数据
}
int TcpSocket::readn(char *buf, int size)
{
    int nread = 0;
    int left = size;
    char*p = buf;
    while(left > 0)
    {
        nread = read(m_fd,p,left);
        if(nread > 0) //客户端发送了数据
        {
            p+=nread;
            left -= nread;
        }
        else if(nread == 0) //客户端断开了连接
        {
            return 0;
        }
        else if(nread == -1) //
        {
            cout<<"读取数据失败...."<<endl;
            return -1;
        }
    }
    return size;
}
int TcpSocket::writen(const char *msg, int size)
{
    int left = size;
    int nwrite = 0;
    const char* p = msg;
    while(left > 0)
    {
        nwrite = write(m_fd,msg,left);
        if(nwrite> 0)
        {
            p+=nwrite;
            left-=nwrite;
        }
        else if(nwrite == -1)
        {
            cout<<"发送数据失败..."<<endl;
            return -1;
        }
    }
    return size;
}