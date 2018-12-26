#include<arpa/inet.h>
#include<iostream>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
using namespace std;

int main(int argc,char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(8080);
	inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
	connect(sockfd,(struct sockaddr*) &servaddr,sizeof(servaddr));

	char sendline[4096]="i'm your friend";
	write(sockfd,sendline,strlen(sendline));
		
	exit(0);
}


