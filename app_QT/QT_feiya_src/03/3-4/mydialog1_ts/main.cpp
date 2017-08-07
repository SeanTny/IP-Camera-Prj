#include "widget.h"
#include <QApplication>
#include "mydialog.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;
    myDialog dialog;

    if(dialog.exec()==QDialog::Accepted)
    {
        w.show();
        return a.exec();
    }
    else
    {
        return 0;
    }

}
