#ifndef _LOCK_H
#define _LOCK_H
#include<pthread.h>
class Lock{
public:
	Lock();
	~Lock();
	void lock();
	void unlock();

private:
	pthread_mutex_t mutex;
};
#endif