#include<pthread.h>
class Condition{
public:
	Condition();
	~Condition();
	bool wait();
	bool signal();

private:
	pthread_mutex_t mutex;
	pthread_cond_t m_cond;
};
