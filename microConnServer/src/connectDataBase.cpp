#include<iostream>
#include<mysql.h>
#include"connectDataBase.h"
#include<stdio.h>
#include<string.h>
#include<string>
#include<sys/socket.h>
#include<arpa/inet.h>
#include"data.h"

using namespace std;

void Connection::connect()
{
	if(mysql_real_connect(&conn,"localhost","root","123456","mydatabase",0,NULL,0)){
	//cout<<"connect success!\n";
}
	else{
		cout<<"connect the server failed!"<<endl;}
}


bool  Connection::updateUser(){
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
		user_pswd[user]=pwd;	
		}
		return 0;
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
				friendmp[result_row[0]].push_back(result_row[1]);
			}
		}
	}
	
				       
}

void Connection::updategroupmp()
{
	MYSQL_RES *res_ptr;
	MYSQL_FIELD *field;
	MYSQL_ROW result_row;
	int row,column;
	int res=mysql_query(&conn,"select *from group_chat");
	if(res) cout<<"query failed!"<<endl;
	else
	{
		res_ptr=mysql_store_result(&conn);
		if(res_ptr)
		{
			column=mysql_num_fields(res_ptr);
			row=mysql_num_rows(res_ptr);
			for(int i=1;i<row+1;++i)
			{
			result_row=mysql_fetch_row(res_ptr);
			vector<string> name_list;
				//cout<<result_row[2]<<endl;
			for(int i=0;i<strlen(result_row[2]);++i)
			{
			string name="";
			while(i<strlen(result_row[2])&&result_row[2][i]!=' '){
				name+=result_row[2][i];
				++i;
			}
				name_list.push_back(name);
			}
			for(int j=0;j<name_list.size();++j)
			{
				string group_msg=result_row[1];
				group_msg+="(";
				group_msg+=result_row[0];
				group_msg+=")";
				groupmp[name_list[j]]+=group_msg;
				groupmp[name_list[j]]+="\n";
			}
			}
		}
	}


}

