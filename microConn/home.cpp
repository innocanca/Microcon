#include "home.h"
#include <QMessageBox>
#include "linkServer.h"
#include "data.h"
#include "timer.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "mainwindow.h"
#include <QLabel>
#include <algorithm>
#include <QtGui>
#include <QString>
#include <QSpinBox>
#include <iostream>
#include <QMetaType>
#include <fcntl.h>
#include <pthread.h>
#include <QSplitter>
#include <QFileDialog>
#include <stdio.h>
#include <sys/stat.h>
#include <stdio.h>
using namespace std;

//char buf[MAXSIZE];
//static void * ThreadStart(void *arg);
Home::Home()
{
	//record btn开关初始化为1
	ok=1;
	qRegisterMetaType< QVector<int> >("QVector<int>");
	record=new QPushButton("消息记录");
	group_chat=new QPushButton("创建群聊");
	MesgRecord=new QListWidget();
	contentWidgets=new QListWidget();
	textarea=new QTextEdit();
	scrollArea=new QTextEdit();
	bzero(&pservaddr,sizeof(pservaddr));
	pservaddr.sin_family=AF_INET;
	pservaddr.sin_port=htons(8090);
	inet_pton(AF_INET,"127.0.0.1",&pservaddr.sin_addr);
	//socklen_t len=sizeof(pservaddr);
}

void Home::init(){
	
	pthread_t tid1,tid2;
	//接受UDP数据包
	tid1=pthread_create(&tid1,NULL,&ThreadStart,this);
	//接收TCP数据包
	tid2=pthread_create(&tid2,NULL,&TcpReceiver,this);
	//QSplitter * window=new QSplitter();
	//窗口切分
	QSplitter *window=new QSplitter(Qt::Horizontal);
	QSplitter *left=new QSplitter(Qt::Vertical);
	QSplitter *right=new QSplitter(Qt::Vertical);
	//window->setWindowTitle("home");
	//contentWidgets=new QListWidget();
	contentWidgets->setStyleSheet(
		"QListWidget{width:140px;background:#F7F7F7;color:#000000;border:0px solid gray;padding:0px;}"
		"QListWidget::item{width:100px;height:50px;background:#E6E6E6;border:0px solid gray;padding-left:20px;}"
		"QListWidget::item:selected{background:#BBBBBB;}"
	);
	MesgRecord->setStyleSheet(
			"QListWidget{width:280px;background:#F7F7F7;border:0px solid gray;padding:0px;}"
			"QListWidget::item{width:260px;background:#F7F7F7;}"
			"QListWidget::item:selected{background:#FFD39B;}"
			);

	contentWidgets->setIconSize(QSize(30,40));
	//设置只可读 不可编辑
	scrollArea->setReadOnly(true);
	//初始化好友列表
	InitWidgetList();
	
	QObject::connect(contentWidgets,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(ClickedFunc(QListWidgetItem *)));
	scrollArea->setFixedSize(550,300);
	
	scrollArea->setStyleSheet(
			"QListWidget::item{width:500px;}"
			);
	textarea->setFixedSize(550,100);

	send=new QPushButton("发送");
	send->setEnabled(false);
	file=new QPushButton("上传文件");
	file->setEnabled(false);
	pic=new QPushButton("发送图片");
	pic->setEnabled(false);
	record->setEnabled(false);
	group_chat->setFixedSize(80,30);
	record->setFixedSize(80,30);
	send->setFixedSize(40,30);
	file->setFixedSize(80,30);
	pic->setFixedSize(80,30);

	QObject::connect(send,SIGNAL(clicked()),this,SLOT(BtnClicked()));
	QObject::connect(pic,SIGNAL(clicked()),this,SLOT(PicSent()));
	QObject::connect(file,SIGNAL(clicked()),this,SLOT(UploadFile()));
	QObject::connect(record,SIGNAL(clicked()),this,SLOT(RecordClicked()));
	QObject::connect(group_chat,SIGNAL(clicked()),this,SLOT(Group_chatClicked()));

	contentWidgets->setFixedSize(180,460);
	MesgRecord->setFixedSize(300,500);
	QWidget *topleft=new QWidget();
	topleft->setStyleSheet(
			"QWidget{ background:#E6E6E6; }"
			"QWidget::QLabel:hover{background:#F7F7F7;}"
			);
	QHBoxLayout *grid=new QHBoxLayout();
	//基类指向派生类的指针 实现对MouthClick的动态绑定
	l_friends=new ClickLabel("好友");
	l_friends->setAlignment(Qt::AlignHCenter); 
	l_groups=new ClickLabel("群聊");
	QObject::connect(l_groups,SIGNAL(clicked()),this,SLOT(Label_groupClicked()));
	QObject::connect(l_friends,SIGNAL(clicked()),this,SLOT(Label_friendClicked()));
	l_groups->setAlignment(Qt::AlignHCenter);
	grid->setStretchFactor(l_friends,1);
	grid->setStretchFactor(l_groups,1);
	grid->addWidget(l_friends);
	grid->addWidget(l_groups);
	topleft->setLayout(grid);
	topleft->setFixedSize(180,40);
	left->addWidget(topleft);
	left->addWidget(contentWidgets);
	window->addWidget(left);
	window->addWidget(right);
	window->addWidget(MesgRecord);
	MesgRecord->hide();
	right->addWidget(scrollArea);
	QWidget *tool=new QWidget();
	QHBoxLayout *toollayout=new QHBoxLayout();
	toollayout->addWidget(group_chat);
	toollayout->addWidget(file);
	toollayout->addWidget(pic);
	toollayout->addWidget(record);
	tool->setLayout(toollayout);
	tool->setFixedSize(550,40);
	right->addWidget(tool);
	right->addWidget(textarea);

	QWidget *below=new QWidget();
	QHBoxLayout *btnlayout=new QHBoxLayout();
		
	btnlayout->addWidget(send);
	//btnlayout->addWidget(file);
	below->setLayout(btnlayout);
	below->setFixedSize(550,40);
	right->addWidget(below);
	window->setFixedSize(1030,480);
	window->setStyleSheet("background-color:white;");
	window->show();
	window->activateWindow();
}

void* Home::TcpReceiver(void *arg)
{
	Home *mainwindow=(Home*) arg;
	int n;
	//char file[MAXSIZE];
	//char head[10];
	while(1)
	{
		//char file[MAXSIZE];
		char head[10];
		if((n=read(mainwindow->connfd,head,sizeof(head))>0)){
		
		//cout<<"head--------------"<<head<<endl;
		int i=0;
		for(;head[i]!='\n';++i)
				;
		head[i]='\0';
		int recvsize=atoi(head);
		cout<<"接受的数据量为:"<<recvsize<<endl;
		int rn;
		char file[MAXSIZE];
		bzero(file,sizeof(file));
		if((rn=read(mainwindow->connfd,file,recvsize))>0)
		{
		char eventType[10];
		char path[256];
		int i=0;
		for(;file[i]!='\n';++i)
			eventType[i]=file[i];
		eventType[i++]='\0';
		cout<<"eventType="<<eventType<<endl;
		if(!strcmp(eventType,UPDATE_FRIENDS))
		{
			friends.clear();	
			mainwindow->contentWidgets->clear();
			const char *f_list=&file[i];
			cout<<"----------------------"<<endl;
			cout<<f_list<<endl;
			for(int i=0;i<strlen(f_list);i++)
			{
				string temp;
				while(i<strlen(f_list)&&f_list[i]!='\n')
					temp+=f_list[i++];
				friends.push_back(temp);
			}
			int len=friends.size();
			for(int i=0;i<len;++i)
			{
				QListWidgetItem *item=new QListWidgetItem();
				item->setIcon(QIcon("ss.png"));
				item->setText(tr(friends[i].c_str()));
				item->setSizeHint(QSize(150,50));
				mainwindow->contentWidgets->addItem(item);
			}
		}
		if(!strcmp(eventType,IMG_EVENT))
		{
		char s_user[128];	
		const char *m_str=&file[i];
		int k=0;
		for(;m_str[k]!='\n';++k)
			s_user[k]=m_str[k];
		s_user[k++]='\0';

		char r_user[128];
		const char* pathstr;
		if(strlen(s_user)!=9)
		{
		const char*s_str=&m_str[k];
		int n=0;
		for(;s_str[n]!='\n';++n)
			r_user[n]=s_str[n];
		r_user[n++]='\0';
		pathstr=&s_str[n];
		}
		//写入本地文件
		else {
			pathstr=&m_str[k];
			strcpy(r_user,s_user);
		}
		int j=0;
		for(;pathstr[j]!='\n';++j)
			path[j]=pathstr[j];
		path[j++]='\0';
		//cout<<"path="<<path<<endl;
		const char* context=&pathstr[j];
		mkdir(mainwindow->username.c_str(),0755);
		char absolutePath[255];
		strcpy(absolutePath,mainwindow->username.c_str());
		strcat(absolutePath,"/");
		strcat(absolutePath,path);
		FILE *fp=fopen(absolutePath,"w");
		int FileLength=recvsize-strlen(path)-1;
		fwrite(context,sizeof(char),FileLength,fp);
		::fclose(fp);
		
		Timer m_timer;
		char *c_time=m_timer.getCurrentTime();
		string u_context=r_user;
		u_context+=" ";
		u_context+=c_time;
		u_context+="\n";
		//若发送的用户为被选择的item,则显示
		if(!strcmp(r_user,mainwindow->peername.c_str()))
		{
		mainwindow->scrollArea->append(QString::fromStdString(u_context));
		string a_path=absolutePath;
		QImage image=QImageReader(QString::fromStdString(a_path)).read();
		QTextCursor cursor = mainwindow->scrollArea->textCursor();
		cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
		QTextImageFormat imageFormat;
		imageFormat.setName(QString::fromStdString(a_path));
		imageFormat.setWidth(image.width()/10);
		imageFormat.setHeight(image.height()/10);
		cursor.insertImage(imageFormat);
		cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

		}
		//将消息放入缓冲区
		u_context+=path;
		//mainwindow->peername
		string user_buf=r_user;
		recvbuffer[user_buf].push_back(u_context);
		mesgtype[user_buf].push_back(string("picture"));
		
		}
		//处理消息记录事件
		else if(!strcmp(eventType,RECORD_EVENT))
		{
		const char *recv=&file[i];
		cout<<"recv--------------"<<recv<<endl;
		if(strlen(recv)!=0){
		//存取消息记录的数组
		vector<string> mesgList;
		for(size_t i=0;i<(strlen(recv)-1);++i)
		{
			string temp;
			while(recv[i]!='\n')
			{
				temp+=recv[i];
				++i;
			}
			mesgList.push_back(temp);
		}
		int cnt=1;
		string str,record_type;
		for(vector<string>::iterator it=mesgList.begin();it!=mesgList.end();++it,++cnt)
		{
		if(cnt%2==1)
		{
			//单数列表示user ＋time
		string tmp=*it;
		string type(tmp.begin(),tmp.begin()+1);
		record_type=type;
		string time(tmp.begin()+1,tmp.end());		
		str+=time;
		str+="\n";
		}		
		else	
		{
			//表示文字数据
			if(record_type=="0")
			{
			str+=*it;
			QListWidgetItem *item=new QListWidgetItem();
			item->setText(tr(str.c_str()));
			item->setSizeHint(QSize(300,50));
			mainwindow->MesgRecord->addItem(item);
			str="";
			//重置record_type
			record_type="-1";
			}
			//表示图片类型数据
			else if(record_type=="1")
			{
			QListWidgetItem *item=new QListWidgetItem();
			//item->setText(tr(str.c_str()));
			item->setIcon(QIcon((*it).c_str()));
			item->setSizeHint(QSize(300,160));
			mainwindow->MesgRecord->setIconSize(QSize(180,150));
			mainwindow->MesgRecord->addItem(item);
			str="";
			record_type="-1";
			}
		}		
	  	}
		}	
			mainwindow->MesgRecord->show();		
		}
		else if(!strcmp(eventType,FILE_EVENT))
		{
			cout<<"i am dealing with the file event!"<<endl;
			/*
			QTextDocument *doc = ui->textEdit->document();
			QTextFrame *rootFrame = doc->rootFrame();
			QTextFrameFormat frameFormat;
			frameFormat.setBorderBrush(Qt::red);
			frameFormat.setBorder(5);
			rootFrame->setFrameFormat(frameFormat);
			
			QTextFrameFormat frameFormat2;
			frameFormat2.setBackground(Qt::lightGray);
			frameFormat2.setMargin(10);
			frameFormat2.setPadding(5);
			frameFormat2.setBorder(2);
			frameFormat2.setBorderStyle(QTextFrameFormat::BorderStyle_Dotted);
			QTextCursor cursor = mainwindow->scrollArea->textCursor();
			cursor.insertFrame(frameFormat2);
			*/

		}
		}
}
}
	return ((void*) 0);

}

void* Home::ThreadStart(void *arg)
{
	Home *myhome=(Home*) arg;
	int sockfd;
	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in myaddr;
	myaddr.sin_family=AF_INET;
	inet_pton(AF_INET,myhome->ip.c_str(),&myaddr.sin_addr);
	myaddr.sin_port=htons(atoi(myhome->port.c_str()));
	bind(sockfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
	int n;
	char recvline[4096];
	for(;;)
	{
		//表示接收从任何ip和端口发送的数据包	
		n=recvfrom(sockfd,recvline,4096,0,NULL,NULL);
		if(n>0){
			
			string str=recvline;
			int i=0;
			for(;recvline[i]!='\n';++i)
				;
			//消息长度
			string len(str.begin(),str.begin()+i);
			++i;
			int j=i;
			for(;recvline[i]!='\n';++i)
				;
			//发送的用户名
			string s_name="";
			string context="";
			string uname(str.begin()+j,str.begin()+i);
			++i;
			//cout<<"uname---"<<uname<<endl;
			//表示接受的是一个群聊包
			if(uname.size()!=9)
			{
			int k=i;
			for(;recvline[i]!='\n';++i)
				;
			string temp(str.begin()+k,str.begin()+i);
			cout<<"temp---"<<temp<<endl;
			s_name=temp;
			++i;
			string text(str.begin()+i,str.begin()+i+atoi(len.c_str())-uname.size());
			context=text;
			}
			//消息内容
			else{
			string text(str.begin()+i,str.begin()+i+atoi(len.c_str())-uname.size());			
			s_name=uname;
			context=text;
			}
			char *c_time;
			Timer m_timer;
			c_time=m_timer.getCurrentTime();
			string u_context=s_name;
			u_context+=" ";
			u_context+=c_time;
			u_context+="\n";
			u_context+=context;
			recvbuffer[uname].push_back(u_context);
			mesgtype[uname].push_back(string("mesg"));
			//如果正接受的用户被选择 则直接将消息显示在界面上
			if(uname==myhome->peername){
			myhome->scrollArea->append(QString::fromStdString(u_context));
			}
	
			for(int i=0;i<(myhome->contentWidgets->count());++i)
			{
				QListWidgetItem *tmp=myhome->contentWidgets->item(i);
				string text=tmp->text().toStdString();
				if(text==uname)
				{
				
				tmp->setBackground(QBrush(QColor(Qt::blue)));
				tmp->setTextColor(Qt::red);
				//tmp->setSelected(true);
				//tmp->setBackground(QColor(Qt::red));
				break;
				}
			}
		}
	}
	return ((void *)0);

}

void Home::Label_friendClicked()
{
	contentWidgets->clear();
	MesgRecord->clear();
	scrollArea->clear();
	int size=friends.size();
	for(int i=0;i<size;++i){
	//cout<<"=================="<<endl;
	QListWidgetItem *item=new QListWidgetItem();
	item->setIcon(QIcon("ss.png"));
	item->setText(tr(friends[i].c_str()));
	item->setSizeHint(QSize(150,50));
	contentWidgets->addItem(item);
	
	}
}

void Home::Label_groupClicked(){
	//QMessageBox::information(NULL, "Title", "Content", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	contentWidgets->clear();
	scrollArea->clear();
	MesgRecord->clear();
	for(size_t i=0;i<groups.size();++i)
	{
		QListWidgetItem *item=new QListWidgetItem();
		item->setIcon(QIcon("group.png"));
		item->setText(tr(groups[i].c_str()));
		item->setSizeHint(QSize(150,50));
		contentWidgets->addItem(item);
	}
}


void Home::Group_chatClicked()
{
	
}

void Home::RecordClicked()
{
	if(ok)
	{
	char package[100];
	//表示获取消息记录
	strcpy(package,"0100\n");
	strcat(package,username.c_str());
	strcat(package,"\n");
	strcat(package,peername.c_str());
	strcat(package,"\n");
	int len=strlen(package);
	char clen[10];
	sprintf(clen,"%d",len);

	char sendline[100];
	strcpy(sendline,clen);
	//封包
	for(size_t i=0;i<(10-strlen(clen));++i)
		strcat(sendline,"\n");
	strcat(sendline,package);
	//cout<<sendline<<endl;
	write(connfd,sendline,strlen(sendline));
	ok=0;
	}
	else if(!ok)
	{
	MesgRecord->clear();
	MesgRecord->close();
	//重置record开关
	ok=1;
	}
	
}
//点击好友
void Home::ClickedFunc(QListWidgetItem *item){
	//初始化聊天界面
	scrollArea->clear();
	MesgRecord->clear();
	textarea->setFocus();
	QString pname=item->text();
	peername=pname.toStdString();
	//设置按钮可见
	pic->setEnabled(true);
	send->setEnabled(true);
	file->setEnabled(true);
	record->setEnabled(true);
	//cout<<"-----------------recvbuffer size----------------"<<recvbuffer[peername].size()<<endl;


	for(size_t i=0;i<recvbuffer[peername].size();++i)
	{
		if(mesgtype[peername][i]=="mesg")
		{
		scrollArea->append(QString::fromStdString(recvbuffer[peername][i]));	
		//移动光标
		QTextCursor cursor = scrollArea->textCursor();
		cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
		scrollArea->setTextCursor(cursor);
		}
		else if(mesgtype[peername][i]=="picture")
		{	

		string recv=recvbuffer[peername][i];
		string::iterator it=::find(recv.begin(),recv.end(),'\n');
		string usr_time(recv.begin(),it);
		string path(it+1,recv.end());	
		//接收的图片默认位于/root/microConn/username/目录下
		char absolutePath[128];
		strcpy(absolutePath,"/root/microConn/");
		strcat(absolutePath,username.c_str());
		strcat(absolutePath,"/");
		strcat(absolutePath,path.c_str());
		string absolute_path=absolutePath;
		QImage image=QImageReader(QString::fromStdString(absolute_path)).read();
		scrollArea->append(QString::fromStdString(usr_time));
		scrollArea->append(QString("\n"));
		QTextCursor cursor = scrollArea->textCursor();
		cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
		QTextImageFormat imageFormat;
		imageFormat.setName(QString::fromStdString(absolute_path));
		imageFormat.setWidth(image.width()/10);
		imageFormat.setHeight(image.height()/10);
		cursor.insertImage(imageFormat);
		cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
		scrollArea->setTextCursor(cursor);

		}
		else if(mesgtype[peername][i]=="send_picture")
		{
		string recv=recvbuffer[peername][i];
		string::iterator it=::find(recv.begin(),recv.end(),'\n');
		string usr_time(recv.begin(),it);
		string path(it+1,recv.end());
		usr_time+="\n";
		scrollArea->append(QString::fromStdString(usr_time));
		QImage image=QImageReader(QString::fromStdString(path)).read();
		QTextCursor cursor = scrollArea->textCursor();
		cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
		QTextImageFormat imageFormat;
		imageFormat.setName(QString::fromStdString(path));
		imageFormat.setWidth(image.width()/10);
		imageFormat.setHeight(image.height()/10);
		cursor.insertImage(imageFormat);
		cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
		scrollArea->setTextCursor(cursor);
		
		}
	}
	scrollArea->show();
}

void Home::UploadFile(){

	QFileDialog *fileDialog = new QFileDialog();
	fileDialog->setFileMode(QFileDialog::AnyFile);
	fileDialog->setViewMode(QFileDialog::Detail);
	fileDialog->setGeometry(10,30,300,200);
	fileDialog->show();
	QString file_name = QFileDialog::getOpenFileName(NULL,"请选择一个文件",".","*");
	fileDialog->close();
	string path=file_name.toStdString();
	//cout<<"path:"<<path<<endl;
	if(path.size()!=0){
		cout<<"path:"<<path<<endl;
		QListWidgetItem *fileItem=new QListWidgetItem();
		fileItem->setText(tr(path.c_str()));
		//scrollArea->addItem(fileItem);
		scrollArea->show();
		struct stat st;
		stat(path.c_str(),&st);
		//构造文件标识头部
		char message[MAXSIZE]="0011\n";
		char sendline[MAXSIZE];
		char FileLength[10];
		sprintf(FileLength,"%ld",st.st_size);
		//发送的客户端id
		strcat(message,peername.c_str());
		strcat(message,"\n");
		//报文长度
		strcat(message,FileLength);
		strcat(message,"\n");
		//文件名
		strcat(message,path.c_str());
		strcat(message,"\n");
		long len=strlen(message)+st.st_size;
		char packLen[10];
		sprintf(packLen,"%ld",len);
		strcpy(sendline,packLen);
		for(size_t i=0;i<10-strlen(packLen);++i)
			strcat(sendline,"\n");
		strcat(sendline,message);
		int slen=strlen(sendline);
		FILE * fp=fopen(path.c_str(),"rb");
		char *buf=new char[MAXSIZE];
		int length=fread(buf,sizeof(char),MAXSIZE,fp);
		::fclose(fp);
		write(connfd,sendline,slen);
		write(connfd,buf,length);
		delete [] buf;
	}

}

void Home::PicSent(){
	//cout<<"----------------"<<endl;
	QFileDialog *fileDialog = new QFileDialog();
	fileDialog->setFileMode(QFileDialog::AnyFile);
	fileDialog->setViewMode(QFileDialog::Detail);
	fileDialog->setGeometry(10,30,300,200);
	//过滤打开文件打开的类型
	fileDialog->show();
	
	QString file_name = QFileDialog::getOpenFileName(NULL,"请选择一个图片",".","*.jpg *.png *.gif *.jpeg *.bmp");
	fileDialog->close();
	//文件路径
	string path=file_name.toStdString();
	//cout<<"path= "<<path<<endl;
	//如果选择了图片
	if(path.size()!=0){
	/* 
	 * 显示图片
	 *
	*/
	Timer m_timer;
	char *c_time=m_timer.getCurrentTime();
	string usr_time=username;
	usr_time+=" ";
	usr_time+=c_time;
	usr_time+="\n";
	scrollArea->append(QString::fromStdString(usr_time));
	usr_time+=path;
	recvbuffer[peername].push_back(usr_time);
	mesgtype[peername].push_back("send_picture");
	QImage image=QImageReader(file_name).read();
	//获取鼠标的位置
	QTextCursor cursor = scrollArea->textCursor();
	QTextImageFormat imageFormat;
	imageFormat.setName(file_name);
	//将图片的大小缩小10倍显示
	imageFormat.setWidth(image.width()/10);
	imageFormat.setHeight(image.height()/10);
	cursor.insertImage(imageFormat);
	cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
	scrollArea->setTextCursor(cursor);

	//发送图片
	struct stat st;
	stat(path.c_str(),&st);
	//发送图片头部
	char message[MAXSIZE]="0010\n";
	//发送的对象
	strcat(message,peername.c_str());
	strcat(message,"\n");
	char sendline[MAXSIZE];
	char FileLength[10];
	sprintf(FileLength,"%ld",st.st_size);
	//文件长度
	strcat(message,FileLength);
	strcat(message,"\n");
	//文件名
	strcat(message,path.c_str());
	strcat(message,"\n");

	long len=strlen(message)+st.st_size;
	char packLen[10];
	sprintf(packLen,"%ld",len);
	//封包，表示总的数据长度
	strcpy(sendline,packLen);
	for(size_t i=0;i<10-strlen(packLen);++i)
		strcat(sendline,"\n");
	strcat(sendline,message);
	//cout<<"sendline: "<<sendline<<endl;
	int slen=strlen(sendline);
	
	FILE* fp=fopen(path.c_str(),"rb");
	char *buf=new char[MAXSIZE]; 
	int length=fread(buf,sizeof(char),MAXSIZE,fp);
	//cout<<"length="<<length<<endl;
	::fclose(fp);
	//cout<<"total size="<<slen+length<<endl;
	
	//图片文件不能和其他字符数组连接 1.否则会破坏数据结构 2.可能因为二进制文件读到字符数组中也不是标准的字符串 不能用于strcat
	write(connfd,sendline,slen);
	write(connfd,buf,length);
	delete [] buf;
	}
}

void Home::BtnClicked(){		//消息发送按钮
	//QMessageBox::information(this,"消息：",textarea->toPlainText());
	QString text=textarea->toPlainText();
	string msg=text.toStdString();
	//表示私聊
	
	char sendline1[8192];
	strcpy(sendline1,peername.c_str());
	strcat(sendline1,msg.c_str());

	//发送客户端我要发送数据的长度
	char sendline[20000];
	sprintf(sendline,"%d",int(strlen(sendline1)));
	strcat(sendline,"\n");
	//若为私聊
	if(peername.size()==9)
		strcat(sendline,PRIVATE_CHAT);
	//否则为群聊
	else{
		strcat(sendline,GROUP_CHAT);
		//添加一个长度项
		int glen=peername.size();
		char clen[10];
		sprintf(clen,"%d",glen);
		strcat(sendline,clen);
		strcat(sendline,"\n");
	}
	strcat(sendline,sendline1);

	// 绑定端口发送数据,如果用户离线 服务器不负责转发数据
	sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr*) &pservaddr,sizeof(pservaddr));
	
	
	/*
	 *  添加消息到聊天界面上 
	 * 
	 */
	char *c_time;
	Timer m_timer;
	c_time=m_timer.getCurrentTime();
	/*
	 * username + time
	 * message
	 * */
	string usr_time=username;
	usr_time+=" ";
	usr_time+=c_time;
	QString qtime=QString::fromStdString(usr_time);
	//QString format_time=QString("<p align=\"right\"><font color=blue>%1</font></p>").arg(qtime);
	//QString qmesg=QString("<p align=\"right\">%1</p>").arg(QString::fromStdString(msg));
	QString qmesg=QString::fromStdString(msg);
	scrollArea->append(qtime);
	QTextCursor cursor = scrollArea->textCursor();
	cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
	scrollArea->setTextCursor(cursor);
	scrollArea->append(qmesg);
	string m_context=usr_time;
	m_context+="\n";
	m_context+=msg;
	recvbuffer[peername].push_back(m_context);
	mesgtype[peername].push_back(string("mesg"));
	//清空输入框
	textarea->setText(tr(""));
}

//初始化好友列表
void Home::InitWidgetList(){  
 	for(size_t i=1;i<friends.size()-2;++i){
	QListWidgetItem *item=new QListWidgetItem();
	item->setIcon(QIcon("ss.png"));
	item->setText(tr(friends[i].c_str()));
	item->setSizeHint(QSize(150,50));
	contentWidgets->addItem(item);

	}

}

//绑定数据发送UDP端口
void Home::bindSockfd(){
		 sockfd=socket(AF_INET,SOCK_DGRAM,0);
	         struct sockaddr_in myaddr;
	         myaddr.sin_family=AF_INET;
	         inet_pton(AF_INET,ip.c_str(),&myaddr.sin_addr);
	         char mport[10];
	         strcpy(mport,port.c_str());
	         cout<<"myport:"<<mport<<endl;
	         myaddr.sin_port=htons(atoi(mport)+4000);
		 //绑定TCP连接端口＋4000为发送数据UDP端口
	         //cout<<"发送数据ip:"<<ip<<"发送数据端口："<<port<<endl;
	         //绑定数据发送的ip 端口
                 bind(sockfd,(struct sockaddr *)&myaddr,sizeof(myaddr));
}

