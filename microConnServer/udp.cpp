#include<sys/socket.h>
#include<stdlib.h>
#include<iostream>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<string>
#include<arpa/inet.h>
#include<stdlib.h>
#include<time.h>
#include"udp.h"
#include"data.h"
#include"timer.h"
using namespace std;

UDP::UDP(){

}


UDP::~UDP(){

}

void UDP::start(){
	//int sockfd;
	//struct sockaddr_in servaddr,cliaddr;
	sockfd=socket(AF_INET,SOCK_DGRAM,0);//采用UDP协议
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(8090);
	bind(sockfd,(struct sockaddr*) &servaddr,sizeof(servaddr));
	
	int n;
	socklen_t len;
	char mesg[8192];
	for(;;)
	{
		len=sizeof(cliaddr);
		n=recvfrom(sockfd,mesg,sizeof(mesg),0,(struct sockaddr*) &cliaddr,&len);//调用recvfrom 客户端的地址就会保存到cliaddr中
		//cout<<"mesg:"<<mesg<<"."<<endl;
		string message=mesg;
		//获取对端的ip 和端口
		string ipstr=inet_ntoa(cliaddr.sin_addr);
		char portstr[10];
		sprintf(portstr,"%d",ntohs(cliaddr.sin_port)-4000);
		cout<<"--------peerip----"<<ipstr<<endl;
		cout<<"--------peerport--------"<<portstr<<endl;
		//组成发送消息对端地址
		ipstr+="\n";
		ipstr+=portstr;
		//查询对应的用户名
		string user1;
		for(map<string,string>::iterator it=ip_mp.begin();it!=ip_mp.end();++it)
		{
			if(it->second==ipstr){
				user1=it->first;
				break;
			}
		}
		
		//cout<<"---------------user1-------------"<<user1<<endl;
		int cnt=0;
		for(;mesg[cnt]!='\n';++cnt)
			;
		//UDP数据包长度
		string length(message.begin(),message.begin()+cnt);
		cout<<"length:"<<length<<endl;
		++cnt;
		/*
		 * type
		 * user+mesg
		 */
		string str1(message.begin()+cnt,message.end());
		int cnt2=0;
		for(;str1[cnt2]!='\n';++cnt2)
		       	;
		string type(str1.begin(),str1.begin()+cnt2);
		++cnt2;
		cout<<"record_type="<<type<<endl;
		//若接受的消息为私聊
		if(type==PRIVATE_CHAT)
		{	
		//user+mesg
		string str2(str1.begin()+cnt2,str1.end());
		//user
		string user2(str2.begin(),str2.begin()+9);
		//mesg
		string context(str2.begin()+9,str2.end());
		string contextBuf=user1;
		contextBuf+=" ";
		//获取本地时间
		Timer m_timer;
		char *timebuf=m_timer.getCurrentTime();
		contextBuf+=timebuf;
		contextBuf+="\n";
		string realcontext(context.begin(),context.begin()+atoi(length.c_str())-9);
		contextBuf+=realcontext;	
		//MesgBuffer 第一个字段按照用户字典序存储
		if(user1>user2){
			MesgBuffer[make_pair(user2,user1)].push_back(contextBuf);
			MesgType[make_pair(user2,user1)].push_back("0");
			//"0"表示文本数据类型
		}
		else{
			MesgBuffer[make_pair(user1,user2)].push_back(contextBuf);
			MesgType[make_pair(user1,user2)].push_back("0");
		}
		//服务器将数据转发给用户
		//解析为接收消息端的ip 地址,user2 表示即将要发送的用户的名字,user1 表示发送消息的用户名
		string address=ip_mp[user2];
		//cout<<"address:"<<address<<endl;
		string ip;
		string port;
		//string::iterator it=find(address.begin(),address.end(),'\n');
		int i=0;
		for(;i<address.size();++i)
		{
			if(address[i]!='\n')
			       	ip+=address[i];
			else
			       	break;
		}
		++i;
		for(;i<address.size();++i)
			port+=address[i];
		char sendline[4096];
		strcpy(sendline,length.c_str());
		strcat(sendline,"\n");
		strcat(sendline,user1.c_str());
		strcat(sendline,"\n");
		strcat(sendline,context.c_str());
		struct sockaddr_in cliaddr2;
		bzero(&cliaddr2,sizeof(cliaddr2));
		cliaddr.sin_family=AF_INET;
		inet_pton(AF_INET,ip.c_str(),&cliaddr2.sin_addr);
		char cliport[10];
		strcpy(cliport,port.c_str());
		cliaddr2.sin_port=htons(atoi(cliport));
		sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr*)&cliaddr2,sizeof(cliaddr2));//服务器转发数据给指定客户端。
		}
		else if(type==GROUP_CHAT)
		{
		
			string str2(str1.begin()+cnt2,str1.end());
			int i=0;
			for(;str2[i]!='\n';++i)   ;
			string groupname_len(str2.begin(),str2.begin()+i);
			string name_mesg(str2.begin()+i+1,str2.end());
			int len=atoi(groupname_len.c_str());
			string group_name(name_mesg.begin(),name_mesg.begin()+len);
			string group_mesg(name_mesg.begin()+len,name_mesg.end());
			//cout<<"group name:"<<group_name<<endl;
			group_name+="\n";
			char sendline[4096];
			strcpy(sendline,length.c_str());
			strcat(sendline,"\n");
			strcat(sendline,group_name.c_str());
			//strcat(sendline,"\n");这里不加换行符因为group_name包含换行符
			strcat(sendline,user1.c_str());
			strcat(sendline,"\n");
			strcat(sendline,group_mesg.c_str());
			//cout<<"group mesg:"<<group_mesg<<endl;
			vector<string>user_list;
			for(map<string,string>::iterator it=groupmp.begin();it!=groupmp.end();++it)
			{
				if(it->second==group_name&&it->first!=user1)
					user_list.push_back(it->first);
			}
			vector<string>ip_list;
			for(vector<string>::iterator it=user_list.begin();it!=user_list.end();++it){
			
				if(ip_mp.find(*it)!=ip_mp.end())
					ip_list.push_back(ip_mp[*it]);
				
			}

			for(vector<string>::iterator it=ip_list.begin();it!=ip_list.end();++it){
			//转发给ip_list 所有用户
			//1.解析对应的IP 和 PORT
			string ip_str=*it;
			int index=0;
			for(;ip_str[index]!='\n';++index) ;
			string ip(ip_str.begin(),ip_str.begin()+index);
			string port(ip_str.begin()+index+1,ip_str.end());
			//发送给对端地址
			struct sockaddr_in cliaddr;
			bzero(&cliaddr,sizeof(cliaddr));
			cliaddr.sin_family=AF_INET;
			inet_pton(AF_INET,ip.c_str(),&cliaddr.sin_addr);
			cliaddr.sin_port=htons(atoi(port.c_str()));
			sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr*)&cliaddr,sizeof(cliaddr));
			}
				
			
		}		
	}
}
