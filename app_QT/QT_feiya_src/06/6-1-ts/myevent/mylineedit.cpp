#include "mylineedit.h"
#include <QKeyEvent>
#include <QDebug>
#include "QLineEdit"

MyLineEdit::MyLineEdit(QWidget *parent) :
    QLineEdit(parent)
{

}

void MyLineEdit::keyPressEvent(QKeyEvent *event) // 键盘按下事件
{
    qDebug() << tr("MyLineEdit键盘按下事件");
    QLineEdit::keyPressEvent(event);  //执行QlineEdit类默认事件
    event->ignore();
}

