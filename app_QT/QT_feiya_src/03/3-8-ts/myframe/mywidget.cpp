#include "mywidget.h"
#include "ui_mywidget.h"
#include <QFont>
#include <QBitmap>
#include <QMovie>

MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget)
{

    ui->setupUi(this);

    QFont font;
    font.setFamily("华文行楷");
    font.setPointSize(20);
    font.setBold(true);
    font.setItalic(true);
    ui->label->setFont(font);

    QString string =tr("标题太长，需要省略！");
    QString str=ui->label->fontMetrics().elidedText(string,Qt::ElideMiddle,180);
    ui->label->setText(str);

    ui->label->setPixmap(QPixmap("K:/IP-Camera-Prj/app_QT/QT_feiya_src/03/3-8-ts/logo.png"));


    QMovie *movie =new QMovie("K:/IP-Camera-Prj/app_QT/QT_feiya_src/03/3-8-ts/donghua.gif");
    ui->label->setMovie(movie);
    movie->start();





}

MyWidget::~MyWidget()
{
    delete ui;
}
