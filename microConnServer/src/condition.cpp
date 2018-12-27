#include"include/condition.h"
#include<iostream>
using namespace std;

Condition::Condition(){
	if(pthread_mutex_init(&mutex,NULL)!=0)
		cout<<"mutex init error"<<endl;
	if(pthread_cond_init(&m_cond,NULL)!=0)
		cout<<"cond init error"<<endl;
}

Condition::~Condition()
{
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&m_cond);
}

bool Condition::wait(){
	int ret=0;
	//pthread_mutex_lock(&mutex);
	ret=pthread_cond_wait(&m_cond,&mutex);
	//pthread_mutex_unlock(&mutex);
	return ret==0;
}


bool Condition::signal(){
	return pthread_cond_signal(&m_cond)==0;
}
