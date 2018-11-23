#ifndef LOGINFRAME_H
#define LOGINFRAME_H
#include "mainwindow.h"
#include <QApplication>
#include <QLabel>
#include <QSpinBox>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMainWindow>
class LoginFrame:public QMainWindow{
    	Q_OBJECT

public:
        LoginFrame();
        void init();
	void connectServer();
    	~LoginFrame();
signals:

private slots:
	void func();
	void regis_clicked();
private:
	QWidget* window;
	QLabel *uname;
	QLabel *passwd;
	QLineEdit *text1;
	QLineEdit *text2;
	QPushButton *btn;
	QPushButton *regis;
};


#endif // LOGINFRAME_H
