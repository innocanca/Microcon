#include<time.h>
class Timer{
public:
	Timer();
	~Timer();
	char * getCurrentTime();

private:
	char timebuf[32];

};
