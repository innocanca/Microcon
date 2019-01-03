#ifndef _PROCESSEVENT_H
#define _PROCESSEVENT_H
class processEvent{
public:
	processEvent();
	~processEvent();
	void Handle_Login_Event(int fd,const char *mesg);
	void Handle_Img_Event(int fd,const char *mesg);
	void Handle_File_Event(int fd,const char *mesg);
	void Handle_Record_Event(int fd,const char *mesg);
private:
	int fd;

	
};
#endif
