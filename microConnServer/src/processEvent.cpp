#include"processEvent.h"
#include"timer.h"
#include"util.h"
#include<stdio.h>
#include<unistd.h>
#include"data.h"
#include<iostream>
#include<arpa/inet.h>
#include<fcntl.h>
#include<string.h>
using namespace std;
char response[2][10]={"success\n","fail\n"};
//char content[MAXSIZE];
//构造函数
processEvent::processEvent(){

}
//析构函数
processEvent::~processEvent(){

}

void processEvent::Handle_Login_Event(int connfd,const char *recvline){
	//cout<<"-----------------"<<recvline<<endl;
	string usermsg=recvline;
	int i=0;
	for(;usermsg[i]!='\n';++i)
		;
	string user(usermsg.begin(),usermsg.begin()+i);
	++i;
	string pwd(usermsg.begin()+i,usermsg.end());
	
	char friendList[MAXSIZE];
	string list;
	for(int i=0;i<friend_online[user].size();++i)
	{
		list+=friendmp[user][i];
		list+="\n";
	}
	strcpy(friendList,list.c_str());
	int n;
	if(user_pswd.find(user)!=user_pswd.end()&&user_pswd[user]==pwd){
		user_online.push_back(user);
		//建立用户名到connfd的映射
		fd_mp[user]=connfd;
		struct sockaddr_in peeraddress;
		socklen_t len;
		len=sizeof(peeraddress);
		getpeername(connfd,(struct sockaddr*)&peeraddress,&len);
		string address=inet_ntoa(peeraddress.sin_addr);
		char port[20];
		sprintf(port,"%d",ntohs(peeraddress.sin_port));
		address+="\n";
		address+=port;
		ip_mp[user]=address;
		char user_ip[100];
		strcpy(user_ip,ip_mp[user].c_str());
		if((n=writen(connfd,response[0],strlen(response[0])))>0)
			cout<<"Login successfully! "<<endl<<endl;
		else   
			cout<<"write error!"<<endl;
		writen(connfd,friendList,strlen(friendList));
		writen(connfd,user_ip,strlen(user_ip));
		writen(connfd,"\nend\n",5);
		writen(connfd,groupmp[user].c_str(),strlen(groupmp[user].c_str()));
		}
		else
		{
			writen(connfd,response[1],strlen(response[1]));
		}
	
}

void processEvent::Handle_Img_Event(int connfd,const char*mesg)
{
	string s_user="";
	for(map<string,int>::iterator it=fd_mp.begin();it!=fd_mp.end();++it)
	{
		if(it->second==connfd)
		{
			s_user=it->first;
			break;
		}
	}
	char dstUname[100];
	char length[10];
	char path[100];
	int i=0;
	//解析字符串
	for(;mesg[i]!='\n';++i) {
		dstUname[i]=mesg[i];
	}
	dstUname[i++]='\0';
	string r_user=dstUname;
	const char *str1=&mesg[i];
	cout<<"------"<<"dstUname ="<<dstUname<<"--------"<<endl;
	cout<<"------"<<"s_user="<<s_user<<"-------"<<endl;
	int j=0;
	for(;str1[j]!='\n';++j){
	       length[j]=str1[j];
	}
	length[j++]='\0';
	//cout<<"--------------file length="<<length<<"---------"<<endl;
	const char *str2=&str1[j];
	int k=0;
	for(;str2[k]!='\n';++k)
		path[k]=str2[k];
	path[k++]='\0';

	string pathStr=path;
	int cnt=pathStr.size()-1;
	for(;cnt>=0;--cnt)
	{
		if(pathStr[cnt]=='/') 
			break;
		else 
			continue;
	}
	string relativePath(pathStr.begin()+cnt+1,pathStr.end());
	/*
	time_t t;
	struct tm*tmp;
	char timebuf[16];
	time(&t);
	tmp=localtime(&t);
	strftime(timebuf,16,"%T",tmp);
	*/
	Timer m_time;
	char*timebuf=m_time.getCurrentTime();
	string contextbuf=s_user;
	contextbuf+=" ";
	contextbuf+=timebuf;
	contextbuf+="\n";
	contextbuf+=relativePath;
	//cout<<"文件名为："<<relativePath<<endl;
	if(s_user>r_user)
	{
		MesgBuffer[make_pair(r_user,s_user)].push_back(contextbuf);
		MesgType[make_pair(r_user,s_user)].push_back("1");
	}
	else
	{
		MesgBuffer[make_pair(s_user,r_user)].push_back(contextbuf);
		MesgType[make_pair(s_user,r_user)].push_back("1");
	}

	const char *file=&str2[k];
	//存储到服务器本地
	//当MAXSIZE过大时，应声明为全局数组，太大的局部数组会破坏堆栈的结构。
	string absolutePath="ImgRecv/"+relativePath;
	FILE *fp=fopen(absolutePath.c_str(),"w");
	fwrite(file,sizeof(char),atoi(length),fp);
	::fclose(fp);
	//发送给dstUser
	
	char sendline[MAXSIZE];
	//文件长度＋文件名＋换行符＋事件标识码＋换行符+发送用户名
	int packsize=atoi(length)+relativePath.size()+1+5+10;
	char packsize_c[10];
	sprintf(packsize_c,"%d",packsize);
	strcpy(sendline,packsize_c);
	for(int i=0;i<(10-strlen(packsize_c));++i)
		strcat(sendline,"\n");
	strcat(sendline,"0010");
	strcat(sendline,"\n");
	string destname_str=dstUname;
	//cout<<"--------------sendline-----------"<<sendline<<endl;
	if(destname_str.size()==9)
	{
		strcat(sendline,s_user.c_str());
		strcat(sendline,"\n");
		strcat(sendline,relativePath.c_str());
		strcat(sendline,"\n");
		if(fd_mp.find(destname_str)!=fd_mp.end()){
		write(fd_mp[destname_str],sendline,strlen(sendline));
		write(fd_mp[destname_str],file,atoi(length));
	}
	}
	//群发
	else
	{
		strcat(sendline,destname_str.c_str());
		strcat(sendline,"\n");
		strcat(sendline,s_user.c_str());
		strcat(sendline,"\n");
		strcat(sendline,relativePath.c_str());
		strcat(sendline,"\n");
		destname_str+="\n";
		//cout<<"-------------------------------------"<<endl;
		vector<string>user_list;
		for(map<string,string>::iterator it=groupmp.begin();it!=groupmp.end();++it)
		{
			if(it->second==destname_str&&it->first!=s_user)
				user_list.push_back(it->first);
		}
		
		for(vector<string>::iterator it=user_list.begin();it!=user_list.end();++it)
		{
		if(fd_mp.find(*it)!=fd_mp.end()){
		write(fd_mp[*it],sendline,strlen(sendline));
		write(fd_mp[*it],file,atoi(length));
		}
		}
	}
	

	
}

void processEvent::Handle_File_Event(int connfd,const char*mesg){
	char dstUname[10];
	//文件长度
	char length[10];
	//文件路径
	char path[100];
	int i=0;
	for(;mesg[i]!='\n';++i){
		dstUname[i]=mesg[i];
	}
	dstUname[i++]='\0';
	cout<<"dstUname="<<dstUname<<endl;
	const char *str1=&mesg[i];
	int j=0;
	for(;str1[j]!='\n';++j){
		length[j]=str1[j];
	}
	length[j++]='\0';
	cout<<"length="<<length<<endl;
	const char *str2=&str1[j];
	int k=0;
	for(;str2[k]!='\n';++k)
		path[k]=str2[k];
	path[k++]='\0';
	cout<<"path="<<path<<endl;
	string pathStr=path;
	int cnt=pathStr.size()-1;
	for(;cnt>=0;--cnt)
		if(pathStr[cnt]=='/')
			break;
		else continue;
	string relativePath(pathStr.begin()+cnt+1,pathStr.end());
	const char* file=&str2[k];

	string absolutePath="FileRecv/"+relativePath;
	FILE *fp=fopen(absolutePath.c_str(),"w");
	fwrite(file,sizeof(char),atoi(length),fp);
	::fclose(fp);
	//发送给dstUser
	
	char sendline[MAXSIZE];
	int packsize=atoi(length)+relativePath.size()+1;
	char packsize_c[10];
	sprintf(packsize_c,"%d",packsize);
	strcpy(sendline,packsize_c);
	for(int i=0;i<(10-strlen(packsize_c));++i)
		strcat(sendline,"\n");
	strcat(sendline,"0011");
	strcat(sendline,"\n");
	strcat(sendline,relativePath.c_str());
	strcat(sendline,"\n");
	string destname_str=dstUname;
	if(fd_mp.find(destname_str)!=fd_mp.end()){
	write(fd_mp[destname_str],sendline,strlen(sendline));
	write(fd_mp[destname_str],file,atoi(length));
	}

}

void processEvent::Handle_Record_Event(int connfd,const char *mesg)
{
	char user1[20];
	char user2[20];
	int i=0;
	for(;mesg[i]!='\n';++i)
		user1[i]=mesg[i];
	user1[i]='\0';
	++i;
	const char *mesg2=&mesg[i];
	int j=0;
	for(;mesg2[j]!='\n';++j)
		user2[j]=mesg2[j];
	user2[j]='\0';
	cout<<"---------user1-----------"<<user1<<endl;
	cout<<"---------user2-----------"<<user2<<endl;
	string struser1=user1;
	string struser2=user2;
	vector<string> msgRecords;
	vector<string> type;
	string sendline="0001\n";
	if(struser1<struser2){
		msgRecords=MesgBuffer[make_pair(struser1,struser2)];
		type=MesgType[make_pair(struser1,struser2)];
	}
	else{
		msgRecords=MesgBuffer[make_pair(struser2,struser1)];
		type=MesgType[make_pair(struser2,struser1)];
	}
	vector<string>::iterator it1;
	vector<string>::iterator it2;
	for(it1=type.begin(),it2=msgRecords.begin();it2!=msgRecords.end();++it2,++it1)
	{
		sendline+=*it1;
		sendline+=*it2;
		sendline+="\n";
	}
	cout<<"sendline------------"<<sendline<<endl;
	
	char package[MAXFILESIZE];
	int len=sendline.size();
	char packlen[10];
	sprintf(packlen,"%d",len);
	strcpy(package,packlen);
	size_t plen=strlen(package);
	for(int i=0;i<(10-plen);++i)
		strcat(package,"\n");
	strcat(package,sendline.c_str());
	writen(connfd,package,strlen(package));

}
