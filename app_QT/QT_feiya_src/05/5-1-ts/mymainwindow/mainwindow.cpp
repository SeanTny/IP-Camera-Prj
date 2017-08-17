#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMenu"
#include "QToolButton"
#include "QSpinBox"
#include "QTextEdit"
#include "QMdiSubWindow"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMenu *editMenu=ui->menuBar->addMenu("编辑(&E)");
    QAction *action_Open=editMenu->addAction(QIcon(":/image/image/open.png"),"打开文件(&O)");
    action_Open->setShortcut(QKeySequence("Ctrl+O"));


    ui->mainToolBar->addAction(action_Open);




    //动作组
    QActionGroup *grop=new QActionGroup(this);      //建立动作组
    QAction *action_L=grop->addAction("左对齐(&L)"); //向动作组中添加动作
             action_L->setCheckable(true);          //设置选中框
    QAction *action_R=grop->addAction("右对齐(&R)");
             action_R->setCheckable(true);
    QAction *action_C=grop->addAction("中心对齐(&C)");
             action_C->setCheckable(true);



   action_L->setChecked(true); //指定这个action为默认选中
   editMenu->addSeparator();
   editMenu->addAction(action_L);
   editMenu->addAction(action_R);
   editMenu->addAction(action_C);



   QToolButton *toolBtn=new QToolButton(this);
   toolBtn->setText("颜色");

   QMenu *colorMenu=new QMenu(this);  //创建一个菜单
   colorMenu->addAction("红色");
   colorMenu->addAction("绿色");

   toolBtn->setMenu(colorMenu);       //添加菜单
   toolBtn->setPopupMode(QToolButton::MenuButtonPopup);

    ui->mainToolBar->addWidget(toolBtn);
    QSpinBox *spinBox=new QSpinBox(this);
    ui->mainToolBar->addWidget(spinBox);




}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_New_triggered()
{
    //新建文本编辑器部件
    QTextEdit *edit=new QTextEdit(this);

    QMdiSubWindow *child =ui->mdiArea->addSubWindow(edit);
    child->setWindowTitle("多文当编辑器子窗口");
    child->show();

}




