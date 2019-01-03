#ifndef _TIMER_H
#define _TIMER_H
#include<time.h>
class Timer{
public:
	Timer();
	~Timer();
	char * getCurrentTime();

private:
	char timebuf[32];

};
#endif
