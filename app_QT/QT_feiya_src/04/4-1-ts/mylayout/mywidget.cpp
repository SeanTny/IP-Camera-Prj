#include "mywidget.h"
#include "ui_mywidget.h"
#include <QHBoxLayout>
#include <QGridLayout>



MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget)
{
    ui->setupUi(this);

//    QHBoxLayout *layout =new QHBoxLayout; //新建水平布局管理器
//    layout->addWidget(ui->fontComboBox);
//    layout->addWidget(ui->textEdit);
//    layout->setSpacing(50);
//    layout->setContentsMargins(0,0,50,100); // 这是布局管理器到桌面widget的距离  左 上 右 下

//    setLayout(layout);


//    QGridLayout *layout = new QGridLayout;
//    //添加部件，从0行0列开始 占据1行2列
//    layout->addWidget(ui->fontComboBox,0,0,1,2);
//    layout->addWidget(ui->pushButton,0,2,1,1);
//    layout->addWidget(ui->textEdit,1,0,1,3);

//    setLayout(layout);




}

MyWidget::~MyWidget()
{
    delete ui;
}
