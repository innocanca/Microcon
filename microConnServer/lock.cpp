#include"lock.h"
#include<iostream>
using namespace std;

Lock::Lock(){
	if(pthread_mutex_init(&mutex,NULL)!=0)
		cout<<"mutex init error"<<endl;
}

Lock::~Lock()
{
	pthread_mutex_destroy(&mutex);
}

void Lock::lock(){
	pthread_mutex_lock(&mutex);
}


void Lock::unlock(){
	pthread_mutex_unlock(&mutex);
}
