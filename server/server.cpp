#include<iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define SERVER_PORT 17899
#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512
using namespace std;
int main(int argc, const char * argv[]) {
    
    // 声明并初始化一个服务器端的socket地址结构
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    
    // 创建socket，若成功，返回socket描述符
    int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(server_socket_fd < 0){
        perror("Create Socket Failed:");
        exit(1);
    }
    int opt = 1;
    setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // 绑定socket和socket地址结构
    if(-1 == (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)))){
        perror("Server Bind Failed:");
        exit(1);
    }
    // socket监听
    if(-1 == (listen(server_socket_fd, LENGTH_OF_LISTEN_QUEUE))){
        perror("Server Listen Failed:");
        exit(1);
    }
    
    while(1){
        // 定义客户端的socket地址结构
        struct sockaddr_in client_addr;
        socklen_t client_addr_length = sizeof(client_addr);
        // 接受连接请求，返回一个新的socket(描述符)，这个新socket用于同连接的客户端通信
        // accept函数会把连接到的客户端信息写到client_addr中
        int new_server_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_length);
        if(new_server_socket_fd < 0){
            perror("Server Accept Failed:");
            break;
        }
        // recv函数接收数据到缓冲区buffer中
        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        if(recv(new_server_socket_fd, buffer, BUFFER_SIZE, 0) < 0){
            perror("Server Recieve Data Failed:");
            break;
        }
        // 然后从buffer(缓冲区)拷贝到file_name中
        char file_name[FILE_NAME_MAX_SIZE+1];
        bzero(file_name, FILE_NAME_MAX_SIZE+1);
        strncpy(file_name, buffer, strlen(buffer)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(buffer));
        printf("%s\n", file_name);
        
        // 打开文件并读取文件数据
        FILE *fp = fopen(file_name, "w");
        if(NULL == fp){
            cout<<"文件"<<file_name<<"创建失败"<<endl;
        }else{
            bzero(buffer, BUFFER_SIZE);
            int length = 0;
            int allCount = 0;
            // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止
            //
            while((length = (int)recv(new_server_socket_fd,buffer,BUFFER_SIZE,0)) > 0){
                if(fwrite(buffer,sizeof(char),length,fp) < length){
                    cout<<"文件 "<<file_name<<" 写入失败"<<endl;
                    break;
                }
                allCount++;
                bzero(buffer, BUFFER_SIZE);
            }
            // 关闭文件
            fclose(fp);
            printf("File:%s Transfer Successful! 共%dK\n", file_name,allCount);
        }
        // 关闭与客户端的连接
        close(new_server_socket_fd);
    }
    // 关闭监听用的socket
    close(server_socket_fd);

    return 0;
}