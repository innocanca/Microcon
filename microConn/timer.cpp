#include"timer.h"
Timer::Timer(){

}

Timer::~Timer(){

}

char * Timer::getCurrentTime()
{
	time_t t;
	struct tm* tmp;
	time(&t);
	tmp=localtime(&t);
	strftime(timebuf,32,"%T",tmp);
	return timebuf;
}
