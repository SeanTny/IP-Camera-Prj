#include "mywidget.h"
#include "ui_mywidget.h"
#include "QColor"
#include "QColorDialog"
#include "QDebug"
#include "QFileDialog"
#include "QFontDialog"



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



void myWidget::on_pushButton_2_clicked()
{
    QStringList fileNames=QFileDialog::getOpenFileNames(this,
                                                        tr("文件对话框"),
                                                        "D:",
                                                        tr("图片文件(*png *jpg);;文本文件(*.txt)")
                                                        );
    qDebug()<<"fileNames:"<<fileNames;

}


//字体对话框
void myWidget::on_pushButton_3_clicked()
{
    bool ok;
    QFont font=QFontDialog::getFont(&ok,this);
    if(ok)
    {
        ui->pushButton_3->setFont(font);
    }
    else
    {
        qDebug()<<"没有选择字体";
    }
}



