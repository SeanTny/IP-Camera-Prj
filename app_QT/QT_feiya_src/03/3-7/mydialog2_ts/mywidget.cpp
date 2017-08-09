#include "mywidget.h"
#include "ui_mywidget.h"
#include "QColor"
#include "QColorDialog"
#include "QDebug"
#include "QFileDialog"
#include "QFontDialog"
#include "QInputDialog"
#include "QMessageBox"
#include "QProgressDialog"
#include "QErrorMessage"





myWidget::myWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::myWidget)
{
    ui->setupUi(this);
    errordlg=new QErrorMessage(this);
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



//字体对话框
void myWidget::on_pushButton_5_clicked()
{
    bool ok;
    //获取字符串
    QString string=QInputDialog::getText(this,
                                         tr("输入字符串对话框 "),
                                         tr("请输入用户名："),
                                         QLineEdit::Normal,
                                         tr("admin"),
                                         &ok
                                         );
    if(ok)
    {
        qDebug()<<"string:"<<string;
    }



    //获取整数
    int valuel=QInputDialog::getInt(this,
                                    "输入整数对话框",
                                    "请输入-1000到1000之间的整数",
                                    100,
                                    -1000,
                                    1000,
                                    10,
                                    &ok
                                    );
    if(ok)
    {
     qDebug()<<"value:"<<valuel;
    }


    //获取浮点数
    double valuel_double=QInputDialog::getDouble(this,
                                                 "输入浮点数对话框",
                                                 "请输入-1000到1000之间的数值",
                                                 0.00,
                                                 -1000.00,
                                                 100.00,
                                                 20.00,
                                                 &ok
                                                 );
    if(ok)
    {
        qDebug()<<"value_double:"<<valuel_double;
    }


    QStringList items;
    items<<"条目1"<<"条目2";
    //获取条目
    QString item=QInputDialog::getItem(this,
                                       tr("输入条目对话框"),
                                       "请选择或输入一个条目",
                                       items,
                                       0,
                                       true,
                                       &ok
                                       );

if(ok)
{
    qDebug()<<"item:"<<item;
}




}





void myWidget::on_pushButton_4_clicked()
{
    //问题对话框
    int ret1=QMessageBox::question(this,
                                   tr("问题对话框"),
                                   "你了解QT吗",
                                   QMessageBox::Yes,
                                   QMessageBox::No
                                   );
    if(ret1==QMessageBox::Yes)
    {
        qDebug()<<"问题！yes";
    }


    int ret2=QMessageBox::information(this,
                                      "提示对话框",
                                      "这是QT书籍",
                                      QMessageBox::Ok
                                      );
    if(ret2==QMessageBox::Ok)
    {
        qDebug()<<"提示";
    }


    int ret3=QMessageBox::warning(this,
                                  "警告对话框",
                                  "不能提前结束",
                                  QMessageBox::Abort
                                  );
    if(ret3==QMessageBox::Abort)
    {
        qDebug()<<"警告!";
    }


    int ret4=QMessageBox::critical(this,
                                  "严重错误对话框",
                                   "发现一个严重错误，现在需要关闭所有文件",
                                   QMessageBox::YesAll
                                  );
    if(ret4==QMessageBox::YesAll)
    {
        qDebug()<<"错误";
    }


    //关于对话框
    QMessageBox::about(this,
                       "关于对话框",
                       "yafeilinux致力于QT的普及工作"
                       );



}

void myWidget::on_pushButton_6_clicked()
{
    QProgressDialog dialog("文件复制进度",
                           "取消",
                           0,
                           50000,
                           this
                           );
    dialog.setWindowTitle("进度对话框");
    dialog.setWindowModality(Qt::WindowModal); //将对话框设置为模态对话框
    dialog.show();
    for(int i=0;i<50000;i++)
    {
        dialog.setValue(i);
        QCoreApplication::processEvents();    //避免界面冻结
        if(dialog.wasCanceled())
            break;
    }
    dialog.setValue(50000);
    qDebug()<<"复制成功";

}








void myWidget::on_pushButton_7_clicked()
{

    errordlg->setWindowTitle("错误消息对画框");
    errordlg->showMessage("这里是出错信息");


}




QWizardPage *myWidget::createPage1()
{
    QWizardPage* page =new QWizardPage;
    page->setTitle("介绍");
    return page;
}

QWizardPage *myWidget::createPage2()
{
    QWizardPage* page =new QWizardPage;
    page->setTitle("用户信息选择");
    return page;
}

QWizardPage *myWidget::createPage3()
{
    QWizardPage* page =new QWizardPage;
    page->setTitle("结束");
    return page;

}

void myWidget::on_pushButton_8_clicked()
{
    QWizard wizard(this);
    wizard.setWindowTitle("向导对话框");
    wizard.addPage(createPage1());
    wizard.addPage(createPage2());
    wizard.addPage(createPage3());
    wizard.exec();
}
