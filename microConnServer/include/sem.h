#ifndef _SEM_H
#define _SEM_H
#include<semaphore.h>
#include<iostream>
using namespace std;

class Sem{
public:
	Sem(){
	
	if(sem_init(&m_sem,0,0)!=0)
		cout<<"sem init error"<<endl;
	}

	~Sem()
	{
		sem_destroy(&m_sem);
	}

	bool wait()
	{
		return sem_wait(&m_sem)==0;
	
	}

	bool signal()
	{
		return sem_post(&m_sem)==0;
	}
private:
	sem_t m_sem;

};
#endif
