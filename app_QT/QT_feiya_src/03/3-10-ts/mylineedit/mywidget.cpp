#include "mywidget.h"
#include "ui_mywidget.h"
#include <QDebug>
#include <QValidator>
#include <QCompleter>


MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget)
{
    ui->setupUi(this);
    //新建验证器，指定范围100-999
    QValidator *validator=new QIntValidator(100,999,this);
    //在行编辑中使用验证器
    ui->lineEdit3->setValidator(validator);


    QStringList wordlist;
    wordlist <<"Qt"<<"Qt Creater"<<tr("你好");
    QCompleter *completer=new QCompleter (wordlist,this); //新建自动补全器
    completer->setCaseSensitivity(Qt::CaseInsensitive);   //设置大小写不敏感
    ui->lineEdit4->setCompleter(completer);s
}

MyWidget::~MyWidget()
{
    delete ui;
}

void MyWidget::on_lineEdit2_returnPressed()
{
    ui->lineEdit3->setFocus();          //让line3获得焦点
    qDebug()<<ui->lineEdit2->text();    //输出line2的内容
    qDebug()<<ui->lineEdit2->displayText(); //输出line2到的显示内容
}

void MyWidget::on_lineEdit3_returnPressed()
{
    ui->label_4->setFocus();
}
