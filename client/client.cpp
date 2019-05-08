#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;

#define SERVER_PORT 17899
#define BUFFER_SIZE 4096
#define FILE_NAME_MAX_SIZE 512



int main(int argc, char **argv)
{
    //声明并初始化一个客户端的socket地址结构
    struct sockaddr_in client_addr;
    bzero(&client_addr,sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);
    client_addr.sin_port = htons(0);

    //创建socket
    int client_socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if(client_socket_fd < 0){
        cout<<"client创建socket失败"<<endl;
        exit(1);
    }

    //绑定客户端socket和客户端的socket地址结构，非必须
    if(-1 == bind(client_socket_fd,(struct sockaddr*)&client_addr,sizeof(client_addr))){
        cout<<"client绑定失败"<<endl;
        exit(1);
    }
    
    //声明一个服务端socket地址结构，并用服务器那边的IP地址及端口进行初始化，用于后面的连接
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    //将十进制IP转换为网络二进制
    //返回值，1表示成功，0输入的不是有效格式，-1失败
    if(inet_pton(AF_INET,"127.0.0.1",&server_addr.sin_addr) != 1){
        cout<<"Server IP 错误"<<endl;
        exit(1);
    }
    server_addr.sin_port = htons(SERVER_PORT);

    //向服务器发起连接，连接成功后client_socket_fd代表了客户端和服务器的一个socket连接
    if(connect(client_socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0){
        cout<<"client 请求连接失败"<<endl;
        exit(1);
    }

    //输入文件名，并放到缓冲区buffer中等待发送
    string send_file_name;
    
    cout<<"要发送的文件名：";
    cin>>send_file_name;

    char buffer[BUFFER_SIZE];
    bzero(buffer,BUFFER_SIZE);
    strncpy(buffer,send_file_name.c_str(),send_file_name.length()>BUFFER_SIZE?BUFFER_SIZE:send_file_name.length());//将file_name的前n个字符串拷贝到buffer中

    //向服务端发送buffer中的数据
    if(send(client_socket_fd,buffer,send_file_name.length(),0) < 0){
        cout<<"发送文件名失败"<<endl;
        exit(1);
    }

    //打开文件，准备写入
    FILE *fp = fopen(send_file_name.c_str(),"r");
    if(NULL==fp){
        cout<<"文件 "<<send_file_name<<" 读取失败！"<<endl;
        exit(1);
    }
    cout<<"send_file_name = "<<send_file_name<<endl;
    //从服务端接收数据到buffer中
    //每接收一段数据，将其写入内存中，循环直到文件接收写玩为止
    bzero(buffer,BUFFER_SIZE);
    int length = 0;
    int allCount = 0;
    while((length = (int)fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0){
        if(send(client_socket_fd, buffer, length, 0) < 0){
            cout<<"文件"<<send_file_name<<"发送失败！"<<endl;
            break;
        }
        allCount += length;
        bzero(buffer,BUFFER_SIZE);
    }
    
    //接收成功后，关闭文件，关闭socket
    cout<<"发送"<<send_file_name<<"成功!  "<<"  共"<<allCount<<" 字节"<<endl;
    fclose(fp);
    close(client_socket_fd);
    return 0;
}


