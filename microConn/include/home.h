#ifndef HOME_H
#define HOME_H
#include "mainwindow.h"
#include "clicklabel.h"
#include <vector>
#include <arpa/inet.h>
#include <QApplication>
#include <QLabel>
#include <string>
#include <QSpinBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QTextEdit>
class Home:public QMainWindow{
	Q_OBJECT
public:
	Home();
	void init();
	void setport(std::string userport) { port=userport; }
	void setip(std::string userip) {ip=userip;}
	void setname(std::string uname) {username=uname;}
	void setconnfd(int fd) {connfd=fd;}
	std::string getname() {return username;}
	//每个user 负责产生一个TCP和UDP线程从服务器接收数据
	static void* ThreadStart(void *);
	static void* TcpReceiver(void *);
	void InitWidgetList();
	void bindSockfd();
	~Home()=default;
signals:

private slots:
	void Label_groupClicked();
	void Label_friendClicked();
	void UploadFile();
	void PicSent();
	void RecordClicked();
	void Group_chatClicked();
	//点击好友
	void ClickedFunc(QListWidgetItem *item);
	void BtnClicked();

private:
	std::vector<std::string> recvmesg;
	std::string port;
	std::string ip;
	std::string username;
	std::string peername;	//itemClickName
	QLabel *l_friends;
	QLabel *l_groups;
	QTextEdit *scrollArea;
	QListWidget *MesgRecord;
	QListWidget *contentWidgets; //friends list
	QTextEdit *textarea;		//edit Text
	QPushButton *record;
	QPushButton *send;
	QPushButton *file;	//文件发送按钮
	QPushButton *pic;			
	QPushButton *group_chat;
	int sockfd;		//绑定数据发送端口
	sockaddr_in pservaddr;	//UDP服务器地址
	int connfd;	//保留连接套接字
	//record 开关
	int ok;
};
#endif //HOME_H
