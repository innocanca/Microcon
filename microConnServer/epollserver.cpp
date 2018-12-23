#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<sys/types.h>
//#include<pthread.h>
#include<iostream>
#include<algorithm>
#include<stdio.h>
#include"connectDataBase.h"
#include"util.h"
#include"udp.h"
#include"threadpool.h"
#include<map>
#include<string>
#include"data.h"
using namespace std;

//默认启动4个线程作线程池,最大连接量10000
ThreadPool pool(4);
//绑定服务器TCP端口
int socket_bind(const char *ip ,int port);
void do_epoll(int listenfd);
void handle_events(int epollfd,struct epoll_event *events,int num,int listenfd,char *buf);
void add_event(int epollfd , int fd, int state);
void handle_accept(int epollfd,int listenfd);
void do_read(int epollfd,int fd ,char *buf);
void do_write(int epollfd,int fd ,char *buf);
//void delete_event(int epollfd,int fd,int state);
static void *thr_fn(void *);
static void *timer_fun(void *);

int main(int argc,char*argv[])
{
	/*
	 * 开启服务器UDP服务
	 * */
	Connection con;
	con.connect();
	//更新好友列表
	con.updatefriendmp();
	con.updategroupmp();
	con.updateUser();
	/*
	// 输出所有的用户名账号和密码
	for(map<string,string>::iterator it=groupmp.begin();it!=groupmp.end();++it)
		cout<<"username="<<it->first<<" groupmesg="<<it->second<<endl;
	*/


	//开启UDP服务
	pthread_t tid;
	pthread_create(&tid,NULL,thr_fn,NULL);
	//执行定时事件
	pthread_t tid1;
	pthread_create(&tid1,NULL,timer_fun,NULL);

	int listenfd=0;
	listenfd=socket_bind(IPADDRESS,PORT);
	listen(listenfd,LISTENQ);
	do_epoll(listenfd);
	return 0;
}

void do_epoll(int listenfd){
	int epollfd;
	struct epoll_event events[EPOLLEVENTS];
	int ret;
	char buf[MAXSIZE];
	bzero(buf,sizeof(buf));
	/*
	 * FDSIZE 告诉内核需要监听的数目 当创建好句柄后它会占用一个fd值
	在linux下通过/proc进程的id/fd 查看,使用epoll后必须close掉，否则
	fd会被消耗完。
	*/
	epollfd=epoll_create(FDSIZE);
	pool.set_epollfd(epollfd);
	//对监听套接字注册一个读事件
	add_event(epollfd,listenfd,EPOLLIN);
	while(1)
	{
		//-1表示将永久阻塞
		ret=epoll_wait(epollfd,events,EPOLLEVENTS,-1);
		//ret 表示返回的要处理的事件总数.参数events 用来从内核得到事件的集合。
		
		handle_events(epollfd,events,ret,listenfd,buf);
	}
}


void handle_events(int epollfd , struct epoll_event *events,int num,int listenfd,char *buf){
	int i;
	int fd;
	for(i=0;i<num;++i){
		//cout<<"Events num:"<<num<<endl;
		fd=events[i].data.fd;
		if((fd==listenfd)&&(events[i].events & EPOLLIN))
			handle_accept(epollfd,listenfd);
		else if(events[i].events & EPOLLIN)
			do_read(epollfd,fd,buf);
		else if(events[i].events & EPOLLOUT)
			do_write(epollfd,fd,buf);
	}

}

void add_event(int epollfd,int fd,int state){
	struct epoll_event ev;
	ev.events=state|EPOLLET;
	//将注册时间设置为边缘触发模式
	ev.data.fd=fd;
	//表示对epollfd添加一个state事件（包括EPOLLIN EPOLLOUT.等)
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
}


int socket_bind(const char *ip,int port)
{
	int listenfd;
	struct sockaddr_in servaddr;
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	if(listenfd==-1){
		cout<<"socket error"<<endl;
		exit(1);
	}
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	inet_pton(AF_INET,ip,&servaddr.sin_addr);
	servaddr.sin_port=htons(port);
	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))== -1){
		cout<<"bind error!"<<endl;
		exit(1);
	}
	return listenfd;
}


void handle_accept(int epollfd,int listenfd)
{
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t cliaddrlen;
	connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
	if(connfd==-1)
		cout<<"accept error"<<endl;
	else{
		cout<<"accept a new client"<<endl;
	}
	add_event(epollfd,connfd,EPOLLIN);

}
/*
void delete_event(int epollfd,int fd,int state)
{
	struct epoll_event ev;
	ev.events=state;
	ev.data.fd=fd;
	//第三个参数表示要监听什么事，第四个参数表示要监听的事件。
	epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
}
*/
void do_read(int epollfd,int fd,char *buf)
{	
	//将工作任务加入工作队列 线程从队列中取出任务并执行
	pool.append(fd);

		
}


void do_write(int epollfd,int fd,char *buf)
{

}


static void *thr_fn(void *){
	UDP udp_serv;
	udp_serv.start();
	return ((void *) 0);
}

static void *timer_fun(void*)
{	
	while(1)
	{
		map<string,vector<string> >mp;

		for(int i=0;i<user_online.size();++i)
		{
		for(int j=0;j<user_online.size();++j)
		{
			vector<string> temp=friendmp[user_online[i]];
			if(find(temp.begin(),temp.end(),user_online[j])!=temp.end())
			mp[user_online[i]].push_back(user_online[j]);
		}
		}

		for(int k=0;k<user_online.size();++k)
		{
		cout<<"----------------"<<user_online[k]<<"----------"<<endl;
		string list;
		for(int n=0;n<mp[user_online[k]].size();++n)
		{
			list+=mp[user_online[k]][n];
			list+="\n";
		}
		char sendline[MAXFILESIZE];
		int len=strlen(list.c_str())+5;
		char clen[10];
		sprintf(clen,"%d",len);
		strcpy(sendline,clen);
		for(int i=0;i<(10-strlen(clen));++i)
			strcat(sendline,"\n");
		strcat(sendline,"0100\n");
		strcat(sendline,list.c_str());
		writen(fd_mp[user_online[k]],sendline,strlen(sendline));
		}
		//每60s检查更新一次
		sleep(60);
	}	
}
