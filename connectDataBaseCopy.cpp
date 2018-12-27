#include<iostream>
#include"connectDataBase.h"
#include<mysql.h>
#include<stdio.h>
#include<string>
#include<sys/socket.h>
#include<arpa/inet.h>
#include"data.h"
//#include"util.h"

using namespace std;

void Connection::connect()
{
	if(mysql_real_connect(&conn,"localhost","root","123456","mydatabase",0,NULL,0)){
	//cout<<"connect success!\n";
}
	else{
		cout<<"connect the server failed!"<<endl;}
}


bool  Connection::checkUser(int sockfd,char *uname,char*passwd){
	MYSQL_RES *res_ptr;
	MYSQL_FIELD *field;
	MYSQL_ROW result_row;//按行返回查询信息
  	int row,column;//查询返回的行数和列数
	int res=mysql_query(&conn,"select *from user");
	if(res) cout<<"check error"<<endl;
	else{
		res_ptr=mysql_store_result(&conn);
		if(res_ptr){
		column=mysql_num_fields(res_ptr);
		row=mysql_num_rows(res_ptr);
		bool ok=false;
		for(int i=1;i<row+1;++i)
		{
		result_row=mysql_fetch_row(res_ptr);
		string user=result_row[0];
		string pwd=result_row[1];
		if(user==uname&&pwd==passwd){
			ok=true;    //confirm successfully!
			struct sockaddr_in peeraddress;
			socklen_t len;
			len=sizeof(peeraddress);
			getpeername(sockfd,(struct sockaddr*)&peeraddress,&len);//获取对端地址和ip
			string address=inet_ntoa(peeraddress.sin_addr);
			char port[20];
			sprintf(port,"%d",ntohs(peeraddress.sin_port));
			address+="\n";
			address+=port;
			//cout<<"user:"<<user<<"address:"<<address<<endl;
			ip_mp[user]=address;
			}
			//int res=mysql_query(&conn,"select *from user");
		}
		if(ok)
		cout<<"login successful!"<<endl;
		else
		cout<<"your username or passwd error!"<<endl;
		return ok;
	    }	
	}
}


void Connection::updatefriendmp(){
	MYSQL_RES *res_ptr;
	MYSQL_FIELD *field;
	MYSQL_ROW result_row;//按行返回查询;
	int row,column;//查询返回的行数和列数
	int res=mysql_query(&conn,"select *from friends");
	if(res) cout<<"query failed!"<<endl;
	else{
		res_ptr=mysql_store_result(&conn);
		if(res_ptr){
			column=mysql_num_fields(res_ptr);
			row=mysql_num_rows(res_ptr);
			for(int i=1;i<row+1;++i)
			{
				result_row=mysql_fetch_row(res_ptr);
				//if(result_row[0]==uname){
				friendmp[result_row[0]]+=result_row[1];
				friendmp[result_row[0]]+="\n";
				//}
			}
		}
		for(map<string,string>::iterator it=friendmp.begin();it!=friendmp.end();++it)
		{
			//cout<<"key:"<<it->first<<" value:"<<it->second<<endl;
		}
	}
	
				       
}
