#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

class UDP{
public:
	UDP();
	void start();
	int getSockfd() {return sockfd;}
	void dataTransmit();
	~UDP();
private:
	int sockfd;
	struct sockaddr_in servaddr,cliaddr;

};
