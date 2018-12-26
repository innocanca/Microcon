#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include"include/linkServer.h"
void linkServer::TcpConnect(){
	struct sockaddr_in servaddr;
        sockfd=socket(AF_INET,SOCK_STREAM,0);
        bzero(&servaddr,sizeof(servaddr));
        servaddr.sin_family=AF_INET;
        servaddr.sin_port=htons(8080);
        inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
	connect(sockfd,(struct sockaddr*) &servaddr,sizeof(servaddr));
}

void linkServer::UdpConnect(){
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(8090);
	inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
	usockfd=socket(AF_INET,SOCK_DGRAM,0);	
}
