#ifndef _THREADPOOL_H
#define _THREADPOOL_H
#include <queue>
#include "sem.h"
#include "lock.h"
#include "condition.h"
using namespace std;

class ThreadPool
{
  public:
	ThreadPool(int thread_number = 8, int max_requests = 10000);
	~ThreadPool();
	void set_epollfd(int fd);
	int get_epollfd();
	bool append(int connfd);

  private:
	static void *worker(void *arg);
	void run();

  private:
	int epollfd;
	int m_thread_number;
	int m_max_requests;
	pthread_t *m_threads;
	//请求队列
	queue<int> m_workqueue;
	queue<string> m_worktype;
	Lock m_queuelocker;
	Condition sem;
	Sem m_sem;
	bool m_stop;
};
#endif