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
#include <fcntl.h>
#include<string>
using namespace std;


//根据ip地址算出端口
int split(string &s){
	int i;
	for(i=s.size()-1;i>0;i--){
		if(s[i]=='.')
			break;
	}
	return std::stoi(s.substr(i+1,s.size()-i));
}

int main()
{
	map<string,int>m;
	struct sockaddr_in user_addr,my_addr;
  	char my_ip[13];
  	int socket_fd;
  	int so_broadcast=1;
  	char send_buf[1024];
	char recv_buf[1024];
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

	//设置成非堵塞
	int flag = fcntl(socket_fd, F_GETFL, 0);
    if (flag < 0) {
        perror("fcntl F_GETFL fail");
        exit(1);
    }
    if (fcntl(socket_fd, F_SETFL, flag | O_NONBLOCK) < 0) {
        perror("fcntl F_SETFL fail");
    }
 


   	if((bind(socket_fd,(struct sockaddr *)&user_addr, sizeof(struct sockaddr)))==-1) {
		perror("bind");
		exit(1);
    }
 
    strcpy(send_buf,"broadcast test");
    
    size=sizeof(user_addr);
	while(true){
		m.clear();
		sendto(socket_fd,send_buf,strlen(send_buf),0,(struct sockaddr *)&my_addr,sizeof(my_addr));
		for(int i=0;i<10;i++){
			sleep(0.1);
			recvfrom(socket_fd,recv_buf,1024,0,(struct sockaddr *)&user_addr,&size);
			strcpy(my_ip,inet_ntoa(user_addr.sin_addr));
			string s(my_ip);

			m[s]=split(s)+10000;
		}
		
		//输出当前IP
		for(auto it = m.begin();it!=m.end();it++){
			cout<<it->first<<"   "<<it->second<<endl;
		}
		sleep(3);
	}
    
}
