#include "widget.h"
#include "ui_widget.h"
#include "QPainter"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}




Widget::~Widget()
{
    delete ui;
}




void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawLine(QPoint(0,0),QPoint(100,100));

    QPen pen(Qt::green,5,Qt::DotLine,Qt::RoundCap,Qt::RoundJoin);
    painter.setPen(pen);
    QRectF rectangle(70.0,40.0,80.0,60.0);
    int startAngle=30*16;
    int SpanAngle=120*16;
    painter.drawArc(rectangle,startAngle,SpanAngle);




}






