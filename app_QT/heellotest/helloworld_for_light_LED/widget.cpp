#include "widget.h"
#include "ui_widget.h"
#include "led.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    pMyLed=new led(1);

}



Widget::~Widget()
{
    delete ui;
}





void Widget::on_pushButton_pressed()
{
    pMyLed->led_on();
}



void Widget::on_pushButton_released()
{
    pMyLed->led_off();
}
