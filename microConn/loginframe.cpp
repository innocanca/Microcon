#include "loginframe.h"
#include "register.h"
#include "util.h"
#include "data.h"
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <QtGui>
#include "mainwindow.h"
#include "home.h"
#include <QApplication>
#include <iostream>
#include <QMainWindow>
#include "linkServer.h"
using namespace std;

LoginFrame::LoginFrame()
{
	window=new QWidget();
}

LoginFrame::~LoginFrame()
{
}

//initialize the login frame
void LoginFrame::init()
{
    //QWidget* window=new QWidget();
    //window->setWindowTitle("Login");
    QVBoxLayout *layout=new QVBoxLayout;
    QHBoxLayout *layoutTop=new QHBoxLayout;
    QVBoxLayout *layoutBelow=new QVBoxLayout;
    QHBoxLayout *layoutBelow1=new QHBoxLayout;
    QHBoxLayout *layoutBelow2=new QHBoxLayout;
    uname=new QLabel("username:");
    passwd=new QLabel("password:");
    text1=new QLineEdit;
    text2=new QLineEdit;
    //设置为密码输入模式
    text2->setEchoMode(QLineEdit::Password);
    layoutBelow1->addWidget(uname);
    layoutBelow1->addWidget(text1);
    btn=new QPushButton("登陆");
    regis=new QPushButton("注册");
    layoutBelow2->addWidget(passwd);
    layoutBelow2->addWidget(text2);
    layoutBelow->addLayout(layoutBelow1);
    layoutBelow->addLayout(layoutBelow2);
    layoutBelow->addWidget(btn);
    layoutBelow->addWidget(regis);
    //click the button,return the value
    QObject::connect(btn,SIGNAL(clicked()),this,SLOT(func()));
    QObject::connect(regis,SIGNAL(clicked()),this,SLOT(regis_clicked()));
    layout->addLayout(layoutTop);
    layout->addLayout(layoutBelow);
    window->setLayout(layout);
    window->setFixedSize(400,300);
    window->show();
}

//button 注册时间
void LoginFrame::func()
{
	string uname=text1->text().toStdString();
	//获取登陆的用户名和密码
	string passwd=text2->text().toStdString();
	//连接服务其验证用户名和密码
	linkServer link; 
	link.TcpConnect();

	//封装发送的数据包
	string message="0001\n";
	message+=uname;
	message+="\n";
	message+=passwd;
	int mlen=message.size();
	char sendMessage[100];
	char packLen[10];
	sprintf(packLen,"%d",mlen);
	strcpy(sendMessage,packLen);
	for(size_t i=0;i<10-strlen(packLen);++i)
		strcat(sendMessage,"\n");
	//cout<<sendMessage<<endl;
	
	strcat(sendMessage,message.c_str());
        writen(link.getsockfd(),sendMessage,strlen(sendMessage));
	//发送用户名和密码到服务器端验证
	
	char recv[30000];
	int n;
	//添加封包和解包操作
	if((n=read(link.getsockfd(),recv,sizeof(recv)))>0)//can't use strlen(recv) here!
	{
		//解析接受的字符串
		int ok=0;
		 for(unsigned int i=0;i<strlen(recv);++i)
		{
			string str="";	
			while(i<strlen(recv)&&recv[i]!='\n'){
				str+=recv[i];
				i++;
			}
			if(str=="end") ok=1;
			if(!ok) 	 friends.push_back(str); 	
			else if(str!="end")  groups.push_back(str);
			//friends 表示一个好友数组,服务器将好友的数据发送给客户端
		}
		for(vector<string>::iterator it=groups.begin();it!=groups.end();++it)
			cout<<*it<<endl;
		 
		if(strcmp(friends[0].c_str(),"success")==0){
			window->close();
			Home* myHome=new Home();
			myHome->setname(uname);
			myHome->setip(friends[friends.size()-2]);
			myHome->setport(friends[friends.size()-1]);
			myHome->setconnfd(link.getsockfd());
			//绑定用户的UDP发送端口
			myHome->bindSockfd();
			myHome->init();	
			friends.erase(friends.begin());
			friends.erase(friends.end());
			friends.erase(friends.end()-1);

		}
		else if(strcmp(friends[0].c_str(),"fail")==0){
			//cout<<"login error!"<<endl;
			::close(link.getsockfd());
			window->close();
			
		}
	} 
	else if(n==0){
		::close(link.getsockfd());
		window->close();

	}
}


void LoginFrame::regis_clicked()
{
	Register regis;
	regis.InitFrame();
}

