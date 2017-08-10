#include "mymainwindow.h"
#include "ui_mymainwindow.h"

MyMainWindow::MyMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyMainWindow)
{
    ui->setupUi(this);

    effect=new QSoundEffect(this);
    effect->setSource(QUrl::fromLocalFile("../mysoundeffect/sound.wav"));
    effect->setVolume(0.25f);
}

MyMainWindow::~MyMainWindow()
{
    delete ui;
}

void MyMainWindow::on_pushButton_clicked()
{
    effect->play();
}

void MyMainWindow::on_pushButton_2_clicked()
{
    effect->stop();
}

void MyMainWindow::on_spinBox_valueChanged(int arg1)
{
    effect->setLoopCount(arg1);
}
