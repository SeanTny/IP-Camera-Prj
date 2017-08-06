#-------------------------------------------------
#
# Project created by QtCreator 2017-08-05T17:04:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = helloworld
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    led.cpp

HEADERS  += widget.h \
    led.h

FORMS    += widget.ui
