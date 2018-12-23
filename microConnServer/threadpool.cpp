#include"connectDataBase.h"
#include"threadpool.h"
#include"processEvent.h"
#include"util.h"
#include<unistd.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<algorithm>
#include"data.h"
using namespace std;
void processfd(int epollfd,int fd);
void delete_event(int epollfd,int fd,int state);
ThreadPool::ThreadPool(int thread_number,int max_requests):m_thread_number(thread_number),m_max_requests(max_requests),m_stop(false),m_threads(NULL){
	if((thread_number<=0||(max_requests<=0)))
		cout<<"your thread_number or request must be >=0!";
	m_threads=new pthread_t[m_thread_number];
	/*
	 * 创建thread_number 个线程，并将他们设置为脱离线程
	 */
	for(int i=0;i<thread_number;++i)
	{
		cout<<"create the "<<i<<"th"<<"thread!"<<endl;
		if(pthread_create(m_threads+i,NULL,worker,this)!=0)
		{
			//创建失败
			cout<<"thread creating error"<<endl;
			delete [] m_threads;
		}
		if(pthread_detach(m_threads[i]))
		{
			delete [] m_threads;
			cout<<"detach error"<<endl;
		}
	
	}
}


ThreadPool::~ThreadPool()
{
	delete [] m_threads;
	m_stop=true;
}

bool ThreadPool::append(int connfd)
{
	m_queuelocker.lock();
	if(m_workqueue.size()>m_max_requests)
	{
		m_queuelocker.unlock();
		return false;
	}
	m_workqueue.push(connfd);
	//cout<<"worksize="<<m_workqueue.size()<<endl;
	m_queuelocker.unlock();
	//唤醒线程进行处理工作队列
	m_sem.signal();
	return true;
}

void * ThreadPool::worker(void *arg){
	ThreadPool *pool=(ThreadPool *)arg;
	pool->run();
	return pool;
}


void ThreadPool::run(){
		//m_queuelocker.lock();
	while(!m_stop){
		cout<<"pthread number "<<pthread_self()<<" waiting!"<<endl;
		//m_queuelocker.lock();
		m_sem.wait();
		m_queuelocker.lock();
		cout<<"pthread number "<<pthread_self()<<" deal with the fd"<<endl;
		if(m_workqueue.empty())
		{	
			cout<<"m_workqueue is empty()"<<endl;
			m_queuelocker.unlock();
			continue;
		}
		int request=m_workqueue.front();
		m_workqueue.pop();
		m_queuelocker.unlock();
		processfd(get_epollfd(),request);
		//m_workqueue.push(request);
	}
}

void processfd(int epollfd,int connfd)
{
        int rn,n; 
        char recvline[MAXFILESIZE];
	char packageLen[10];
	processEvent p;

	if((n=read(connfd,packageLen,sizeof(packageLen)))>0){
	int i=0;
	for(;packageLen[i]!='\n';)
		++i;
	packageLen[i]='\0';
	int packlen=atoi(packageLen);
	
        if((rn=readn(connfd,recvline,packlen))>0)
	{
		//cout<<"---------recvline-----------"<<recvline<<endl;
		char type[10];
		int i=0;
		for(;recvline[i]!='\n';++i) 
			type[i]=recvline[i];
		type[i]='\0';
		cout<<"type="<<type<<endl;
		//向后移动一个字符
		++i;
		char *msg=&recvline[i];
		if(!strcmp(type,LOGIN))
		{
			cout<<"Login event"<<endl;
			p.Handle_Login_Event(connfd,msg);
		}
		
		else if(!strcmp(type,IMGSEND))
		{
			cout<<"img event"<<endl;
			p.Handle_Img_Event(connfd,msg);
		}
		else if(!strcmp(type,FILESEND))
		{
			cout<<"file event"<<endl;
			p.Handle_File_Event(connfd,msg);
		
		}
		else if(!strcmp(type,RECORD))
		{
			cout<<"record request"<<endl;
			p.Handle_Record_Event(connfd,msg);
		}
		
	}
	else if(rn==0)  close(connfd);
	}
	else if(n==0) 
	{
		//更新在线好友
		string uname="";
		for(map<string,int>::iterator it=fd_mp.begin();it!=fd_mp.end();++it)		
		{
		if(it->second==connfd){
			uname=it->first;
			break;
		}
		}
		//更新ip表
		ip_mp.erase(uname);
		//更新fd_mp
		fd_mp.erase(uname);
		vector<string>::iterator it=find(user_online.begin(),user_online.end(),uname);
		user_online.erase(it);
		//user_online.remove(uname);
		delete_event(epollfd,connfd,EPOLLIN);
		cout<<"connfd="<<connfd<<" closed the connection"<<endl;
		close(connfd);
	}
}

void ThreadPool::set_epollfd(int fd)
{
	epollfd=fd;
}

int ThreadPool::get_epollfd()
{
	return epollfd;
}

void delete_event(int epollfd,int fd,int state)
{
	struct epoll_event ev;
	ev.events=state;
	ev.data.fd=fd;
	epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
}
