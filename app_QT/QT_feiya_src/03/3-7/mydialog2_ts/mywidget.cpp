#include "mywidget.h"
#include "ui_mywidget.h"
#include "QColor"
#include "QColorDialog"
#include "QDebug"

myWidget::myWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::myWidget)
{
    ui->setupUi(this);
}

myWidget::~myWidget()
{
    delete ui;
}

void myWidget::on_pushButton_clicked()
{
    QColor color=QColorDialog::getColor(Qt::red,this,tr("颜色对话框"));
    qDebug()<<"color:"<<color;
}
