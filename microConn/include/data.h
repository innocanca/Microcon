#ifndef _DATA_H
#define _DATA_H
#include<vector>
#include<string>
#include<map>
using namespace std;
#define MAXSIZE 3145728  //max picture size(3M)
#define PRIVATE_CHAT "6666\n"
#define GROUP_CHAT   "8888\n"
#define RECORD_EVENT "0001"
#define IMG_EVENT    "0010"
#define FILE_EVENT   "0011"
#define UPDATE_FRIENDS "0100"
//好友列表
extern vector<string> friends;
//群聊列表
extern vector<string> groups;
extern map<string,string> message;
//消息接受缓冲区
extern map<string,vector<string> >recvbuffer;
//消息类型
extern map<string,vector<string> > mesgtype;
#endif