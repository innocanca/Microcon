#include "mainwindow.h"
#include "loginframe.h"
#include "home.h"
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "linkServer.h"
#include <vector>
using namespace std;

void func();
int main(int argc, char *argv[])
{
    //linkServer link;
	
    QApplication a(argc, argv);
    LoginFrame *myframe=new LoginFrame();
    myframe->init();
    	
    return a.exec();
    //MainWindow w;
    //w.show();
}
