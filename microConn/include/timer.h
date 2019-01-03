#ifndef _TIME_H
#define _TIME_H
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