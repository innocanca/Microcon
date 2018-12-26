#ifndef CLICKLABEL_H
#define CLICKLABEL_H
#include <QLabel>
#include <QString>
#include <QMouseEvent>

class ClickLabel:public QLabel
{
	Q_OBJECT
public:
	explicit ClickLabel(QWidget *parent=0, Qt::WindowFlags f=0);
	explicit ClickLabel(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0);
	//ClickLabel(const QString &text);
	~ClickLabel();
signals:
	//自定义clicked()信号，在mousePressEvent 事件发生时触发
	void clicked();
protected:
	void mousePressEvent(QMouseEvent *event);

};
#endif //CLICKLABEL_H
