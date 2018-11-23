#include "clicklabel.h"


ClickLabel::ClickLabel(QWidget *parent, Qt::WindowFlags f):QLabel(parent,f)
{
}

ClickLabel::ClickLabel(const QString &text,QWidget *parent, Qt::WindowFlags f):QLabel(text,parent,f)
{
	setText(text);
}

ClickLabel::~ClickLabel()
{

}

void ClickLabel::mousePressEvent(QMouseEvent *event)
{

	if (event->button() == Qt::LeftButton){
	
	//触发clicked()
	emit clicked();
	}
	QLabel::mousePressEvent(event);
}
