#ifndef REGISTER_H
#define REGISTER_H

#include<QtGui>
#include<QLabel>
#include<QPushButton>
#include<QLineEdit>
#include<QMainWindow>
class Register:public QMainWindow{
	Q_OBJECT

public:
	Register();
	~Register();
	void InitFrame();

private:
	QPushButton *ack;
	QPushButton *rst;
	QWidget* window;
	QLabel* nickname;
	QLabel* pswd;
	QLabel* r_pswd;
	QLineEdit* in_nickname;
	QLineEdit* in_pswd;
	QLineEdit* in_r_pswd;

};

#endif
