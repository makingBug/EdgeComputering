// 发送端
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <map>

using namespace std;


int main()
{
	map<string,int>map;
	struct sockaddr_in user_addr,my_addr;
  	char my_ip[13];
  	int socket_fd;
  	int so_broadcast=1;
  	char buf[1024];
   	socklen_t size;
  
    my_addr.sin_family=AF_INET;
    my_addr.sin_port=htons(7778);
    my_addr.sin_addr.s_addr=inet_addr("255.255.255.255");
    
 
    user_addr.sin_family=AF_INET;
    user_addr.sin_port=htons(7778);
    user_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    
   	if((socket_fd=(socket(AF_INET,SOCK_DGRAM,0)))==-1) {
		perror("socket");
        exit(1);
    }
   	setsockopt(socket_fd,SOL_SOCKET,SO_BROADCAST,&so_broadcast,sizeof(so_broadcast));//??socket发送的数据具有广播特性
 
   	if((bind(socket_fd,(struct sockaddr *)&user_addr, sizeof(struct sockaddr)))==-1) {
		perror("bind");
		exit(1);
    }
 
    strcpy(buf,"broadcast test");
    sendto(socket_fd,buf,strlen(buf),0,(struct sockaddr *)&my_addr,sizeof(my_addr));
    size=sizeof(user_addr);
	while(true){
		
		recvfrom(socket_fd,buf,1024,0,(struct sockaddr *)&user_addr,&size);
		strcpy(my_ip,inet_ntoa(user_addr.sin_addr));
		string s(my_ip);
		cout<<"IP = "<<s<<endl;
	}
    
}
