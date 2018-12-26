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
