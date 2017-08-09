#include "widget.h"
#include "ui_widget.h"
#include <QDialog>


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



void Widget::on_showChildButton_clicked()
{
    QDialog *dialog=new QDialog (this);

    dialog->show();

}

void Widget::on_showChildButton_pressed()
{

}
