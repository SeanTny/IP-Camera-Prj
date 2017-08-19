#include <QCoreApplication>
#include "QFileInfo"
#include "QStringList"
#include "QDateTime"
#include "QDebug"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //以只写的方式打开，如果文件不存在，那么就会创建该文件
    QFile file("myfile.txt");
    if(! file.open(QIODevice::WriteOnly | QIODevice::Text ))
        qDebug()<<file.errorString();
    file.write("Hello Qt \nyafeilinux");
    file.close();

    //获取文件信息
    QFileInfo info(file);
    qDebug()<<"绝对路径:"<<info.absoluteFilePath()<<endl
           <<"文件名:"<<info.fileName()<<endl;

    if(! file.open(QIODevice::ReadOnly | QIODevice::Text ))
        qDebug()<<file.errorString();
    qDebug()<<"文件内容:"<<endl<<file.readAll();
    qDebug()<<"当前位置:"<<file.pos();

    file.seek(0);
    QByteArray array;
    array=file.read(5);
    qDebug()<<"前5个字符是："<<array
           <<"当前位置："<<file.pos();

    file.seek(15);
    array=file.read(5);
    qDebug()<<"第15——20个字符是："<<array;
    file.close();


    return a.exec();
}
