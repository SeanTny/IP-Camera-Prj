#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>

namespace Ui {
class MyWidget;
}



class MyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = 0);
    ~MyWidget();

private:
    Ui::MyWidget *ui;
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
};

#endif // MYWIDGET_H
