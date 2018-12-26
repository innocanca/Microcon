#include<map>
#include<string>
#include<list>
#include<vector>
#define IPADDRESS "127.0.0.1"
#define PRIVATE_CHAT 	"6666"
#define GROUP_CHAT 	"8888"
#define PORT 		8080
#define MAXSIZE 	1024	//缓冲区默认大小
#define LISTENQ 	100	//套接字排队的最大连接个数
#define FDSIZE 		1000	//内核要监听的fd数目
#define EPOLLEVENTS 	100	//返回从内核得到的事件集合(不超过FDSIZE)
#define MAXFILESIZE 	3145728 //表示用于传送文件的缓冲区大小
#define LOGIN    	"0001"	
#define IMGSEND  	"0010"
#define FILESEND 	"0011"
#define RECORD  	"0100"
using std::string;
using std::pair;
using std::vector;
using std::map;
//聊天记录类型
extern map<pair<string,string>,vector<string> > MesgType;
//聊天记录缓冲区
extern map<pair<string,string>,vector<string> > MesgBuffer;
//用户名和密码表
extern std::map<std::string , std::string> user_pswd;
//用户名和ip 表
extern std::map<std::string , std::string>  ip_mp;
//用户的群列表
extern std::map<std::string , std::string> groupmp;
//用户的好友列表
extern std::map<std::string , std::vector<std::string> > friendmp;
//用户在线好友列表
extern std::map<std::string , std::vector<std::string> >friend_online;
//map<username ,connfd>
extern std::map<string,int> fd_mp;
//记录在线用户
extern std::vector<string> user_online;
