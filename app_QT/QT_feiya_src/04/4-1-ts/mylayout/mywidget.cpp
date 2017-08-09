#include "mywidget.h"
#include "ui_mywidget.h"
#include <QHBoxLayout>



MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget)
{
    ui->setupUi(this);

    QHBoxLayout *layout =new QHBoxLayout; //新建水平布局管理器
    layout->addWidget(ui->fontComboBox);
    layout->addWidget(ui->textEdit);
    layout->setSpacing(50);
    layout->setContentsMargins(0,0,50,100); // 这是布局管理器到桌面widget的距离  左 上 右 下

    setLayout(layout);

}

MyWidget::~MyWidget()
{
    delete ui;
}
