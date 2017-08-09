#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QMenu>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->pushBtn1->setText(tr("&nihao"));
    ui->pushBtn2->setText("帮助(&H)");
    ui->pushBtn2->setIcon(QIcon("../../mybutton/images/help.png"));
    ui->pushBtn3->setText("z&oom");

    QMenu *menu=new QMenu(this);
    menu->addAction(QIcon("../../mybutton/images/zoom-in.png"),
                    "放大"
                    );
    ui->pushBtn3->setMenu(menu);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushBtn1_toggled(bool checked)
{
    qDebug()<<"按钮是否按下"<<checked;
}
