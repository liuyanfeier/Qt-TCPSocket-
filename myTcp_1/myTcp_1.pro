#-------------------------------------------------
#
# Project created by QtCreator 2016-08-17T11:32:44
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myTcp_1
TEMPLATE = app


SOURCES += main.cpp \
    client.cpp \
    server.cpp

HEADERS  += \
    client.h \
    server.h

FORMS    += \
    client.ui \
    server.ui
