#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QMainWindow>
#include <QSoundEffect>

namespace Ui {
class MyMainWindow;
}

class QSoundEffect; //类的前置声明

class MyMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyMainWindow(QWidget *parent = 0);
    ~MyMainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_spinBox_valueChanged(int arg1);

private:
    Ui::MyMainWindow *ui;
    QSoundEffect *effect;
};

#endif // MYMAINWINDOW_H
