#include"include/register.h"
#include<QHBoxLayout>
#include<QVBoxLayout>
Register::Register()
{
	window=new QWidget();
	ack=new QPushButton("确认");
	rst=new QPushButton("重置");
	nickname=new QLabel("请输入昵称:");
	pswd=new QLabel("请输入密码:");
	r_pswd=new QLabel("请确认密码:");
	in_nickname=new QLineEdit;
	in_pswd=new QLineEdit;
	in_r_pswd=new QLineEdit;


}

Register::~Register()
{


}


void Register::InitFrame()
{
    QVBoxLayout *layout=new QVBoxLayout;
    QHBoxLayout *layoutTop=new QHBoxLayout;
    QHBoxLayout *layoutCenter=new QHBoxLayout;
    QHBoxLayout *layoutBelow=new QHBoxLayout;
    QHBoxLayout *btn_layout=new QHBoxLayout;
 
    //设置为密码输入模式
    in_pswd->setEchoMode(QLineEdit::Password);
    in_r_pswd->setEchoMode(QLineEdit::Password);

    in_nickname->setFixedSize(200,30);
    in_pswd->setFixedSize(200,30);
    in_r_pswd->setFixedSize(200,30);
    ack->setFixedSize(60,30);
    rst->setFixedSize(60,30);
    layoutTop->addStretch(1);
    layoutTop->addWidget(nickname);
    layoutTop->addSpacing(15);
    layoutTop->addWidget(in_nickname);
    layoutTop->addStretch(1);

    layoutCenter->addStretch(1);
    layoutCenter->addWidget(pswd);
    layoutCenter->addSpacing(15);
    layoutCenter->addWidget(in_pswd);
    layoutCenter->addStretch(1);

    layoutBelow->addStretch(1);
    layoutBelow->addWidget(r_pswd);
    layoutBelow->addSpacing(15);
    layoutBelow->addWidget(in_r_pswd);
    layoutBelow->addStretch(1);

    btn_layout->addStretch(1);
    btn_layout->addSpacing(80);
    btn_layout->addWidget(ack);
    btn_layout->setSpacing(30);
    btn_layout->addWidget(rst);
    btn_layout->addStretch(1);

    //QObject::connect(btn,SIGNAL(clicked()),this,SLOT(func()));
    layout->addStretch(1);
    layout->addSpacing(15);
    layout->addLayout(layoutTop);
    layout->addSpacing(15);
    layout->addLayout(layoutCenter);
    layout->addSpacing(15);
    layout->addLayout(layoutBelow);
    layout->addSpacing(15);
    layout->addLayout(btn_layout);
    //layout->addSpacing(40);
    layout->addStretch(1);
    window->setLayout(layout);
    window->setFixedSize(400,300);
    window->show();
}
