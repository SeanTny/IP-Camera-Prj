#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QErrorMessage>
#include <QWizard>


namespace Ui {
class myWidget;
}



class myWidget : public QWidget
{
    Q_OBJECT

public:
    explicit myWidget(QWidget *parent = 0);
    ~myWidget();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

private:

    Ui::myWidget *ui;
    QErrorMessage *errordlg;

    QWizardPage *createPage1(); //声明了一个函数 函数返回 QwizarPage 类对象指针
    QWizardPage *createPage2();
    QWizardPage *createPage3();


};

#endif // MYWIDGET_H
