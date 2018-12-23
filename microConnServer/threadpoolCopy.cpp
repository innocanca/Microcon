#include"connectDataBase.h"
#include"threadpool.h"
#include"util.h"
#include<unistd.h>
#include<string.h>
#include<iostream>
#include"data.h"
char response[2][100]={"success\n","fail\n"};
using namespace std;
void processfd(int fd);

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
		//while(m_workqueue.empty())
		m_sem.wait();
		cout<<"pthread number "<<pthread_self()<<" deal with the fd"<<endl;
		m_queuelocker.lock();
		if(m_workqueue.empty())
		{	
			cout<<"m_workqueue is empty()"<<endl;
			m_queuelocker.unlock();
			continue;
		}
		int request=m_workqueue.front();
		m_workqueue.pop();
		//cout<<"work.size()="<<m_workqueue.size()<<endl;
		m_queuelocker.unlock();
		processfd(request);
	}
}

/*
 * for test
 */
/*
void processfd(int request)
{	
	cout<<pthread_self()<<" deal with the request "<<request<<endl;
}

*/

void processfd(int connfd)
{
        int rn; 
        char recvline[4096];
        while((rn=read(connfd,recvline,sizeof(recvline)))>0){
	        char uname[1024];
	        char passwd[1024];
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
		string user=uname;
		Connection con;
		con.connect();
		char friendList[20000];
		strcpy(friendList,friendmp[user].c_str());
		int n;
		if(con.checkUser(connfd,uname,passwd)) {
			 //cout<<"check succesful"<<endl;
			 char user_ip[100];
			 strcpy(user_ip,ip_mp[user].c_str());
			// cout<<"user_ip "<<user_ip<<endl;

			 if((n=writen(connfd,response[0],strlen(response[0])))>0)
				cout<<"transport "<<n<<" bytes"<<endl;
			 else	cout<<"write error!"<<endl;

			 	writen(connfd,friendList,strlen(friendList));
			 	writen(connfd,user_ip,strlen(user_ip));
			
		}
		
		else{
		if((n=writen(connfd,response[1],strlen(response[1])))>0)
			cout<<endl;
		else
			cout<<"write error"<<endl;
		}
	}
}
