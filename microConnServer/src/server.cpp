#include<iostream>
#include<pthread.h>
#include"connectDataBase.h"
#include"include/util.h"
//#include"udp.h"
#include<string>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
using namespace std;
char response[2][100]={"success\n","fail"};
void process(long connfd);
void process2(long connfd);
void *thr_fn(void *);
void *thr_fn2(void *);

int main(int argc,char *argv[])
{ 
	Connection con;    //connect the database
        con.connect();          //连接数据库
        con.updatefriendmp();//查询friends表,保存
	int err1;
	pthread_t tid1;
	err1=pthread_create(&tid1,NULL,thr_fn,NULL);//udp 进行数据发送必须放置在线程中执行，否则会阻塞整个进程的执行。
	int listenfd,connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr,servaddr;
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(8080);
	bind(listenfd,(struct sockaddr*) &servaddr,sizeof(servaddr));
	listen(listenfd,20);

	//Connection con;    //connect the database
        //con.connect();          //连接数据库
        //con.updatefriendmp();//查询friends表,保存
	pthread_t tid2;
	for(;;){
		clilen=sizeof(cliaddr);
		connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);
		pthread_create(&tid2,NULL,&thr_fn2,(void *) connfd);//创建线程执行登录操作，传入connfd参数
	}
}

void *thr_fn2(void *arg){
	pthread_detach(pthread_self());
	//int *connfd=(int *)arg;
	process((long) arg);//因为在linux x64 机器上指针占8个字节 int占用四个字节，所以不能用process((int) arg);
	close((long) arg);
	
	return NULL;
}

/*
void process2(long connfd)
{
	int n;
	char recvline[4096];
	while((n=read(connfd,recvline,sizeof(recvline)))>0){
		cout<<"recv:"<<recvline<<endl;
		write(connfd,response[0],strlen(response[0]));
	}
	//当process返回时 即客户端发送一个fin 服务器会关闭connfd;
}
*/

void process(long connfd)
{
	int rn;
	char recvline[4096];
	while((rn=read(connfd,recvline,sizeof(recvline)))>0){
	char uname[1024];
	char passwd[1024];
	//split the string to get  username and passwd
	for(int i=0;i<strlen(recvline);++i){
		if(recvline[i]!='\n')
		       	uname[i]=recvline[i];
		else { 
			uname[strlen(uname)]='\0';	
			break; 
			}
	}
	int ok=0;
	for(int i=0,j=0;i<strlen(recvline);++i){
		if(ok) passwd[j++]=recvline[i];
		else if(recvline[i]!='\n'&&!ok) continue;
		else if(recvline[i]=='\n') ok=1;
	}
	passwd[strlen(passwd)]='\0';
	//这里需要改进，不能每次登陆都进行一次查询数据库的操作。
	string user=uname;
	Connection con;    //connect the database
	con.connect();		//连接数据库
	//con.updatefriendmp(user);//查询friends表，准备发送数据
	//string user=uname;
	char f[20000];
	strcpy(f,friendmp[user].c_str());//不能对未初始化的char[】使用strcat函数
	//strcat(sendline,f);      //为什么运行这里就bug?
	int n;
	if(con.checkUser(connfd,uname,passwd)) {
		char user_ip[100];
		strcpy(user_ip,ip_mp[user].c_str());

		if((n=writen(connfd,response[0],strlen(response[0])))>0)
			cout<<"transport "<<n<<" bytes"<<endl;
		else
			cout<<"write error!"<<endl;
		writen(connfd,f,strlen(f));//服务器返回客户的好友信息。
		//cout<<"-------------user_ip="<<user_ip<<"------"<<endl;
		writen(connfd,user_ip,strlen(user_ip));
	}
	else     //login failed!
	{
		if((n=writen(connfd,response[1],strlen(response[1])))>0)
		{	
			cout<<"transport "<<n<<" bytes"<<endl;
			cout<<endl;
		}
		else
			cout<<"write error!"<<endl;
	}
	}
	
}

void *thr_fn(void *arg)
{
	//UDP serv;
	serv.start();
	serv.dataTransmit();
	return ((void*) 0);
}
