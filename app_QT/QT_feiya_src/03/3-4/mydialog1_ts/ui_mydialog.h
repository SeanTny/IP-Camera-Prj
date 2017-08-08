/********************************************************************************
** Form generated from reading UI file 'mydialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYDIALOG_H
#define UI_MYDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_myDialog
{
public:
    QDialogButtonBox *buttonBox;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *myDialog)
    {
        if (myDialog->objectName().isEmpty())
            myDialog->setObjectName(QStringLiteral("myDialog"));
        myDialog->resize(400, 200);
        buttonBox = new QDialogButtonBox(myDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 130, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        pushButton = new QPushButton(myDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(100, 60, 75, 23));
        pushButton_2 = new QPushButton(myDialog);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(280, 70, 75, 23));

        retranslateUi(myDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), myDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), myDialog, SLOT(reject()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), myDialog, SLOT(close()));

        QMetaObject::connectSlotsByName(myDialog);
    } // setupUi

    void retranslateUi(QDialog *myDialog)
    {
        myDialog->setWindowTitle(QApplication::translate("myDialog", "Dialog", 0));
        pushButton->setText(QApplication::translate("myDialog", "\350\277\233\345\205\245\344\270\273\347\225\214\351\235\242", 0));
        pushButton_2->setText(QApplication::translate("myDialog", "\351\200\200\345\207\272\347\250\213\345\272\217", 0));
    } // retranslateUi

};

namespace Ui {
    class myDialog: public Ui_myDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYDIALOG_H
