#ifndef _LINK_SERVER_H
#define _LINK_SERVER_H
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>

class linkServer{
public:
	linkServer()=default;
	int getsockfd(){ return sockfd;}
	int getUsockfd(){ return usockfd;}
	struct sockaddr_in  getUservaddr() { return servaddr;}
	void TcpConnect();
	void UdpConnect();
	~linkServer()=default;
private:
	int sockfd;
	int usockfd;
	struct sockaddr_in servaddr;
};
#endif