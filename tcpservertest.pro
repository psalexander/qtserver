#-------------------------------------------------
#
# Project created by QtCreator 2014-01-10T21:58:01
#
#-------------------------------------------------

QT       += core gui network
QT += x11extras
QT += multimedia
QT += multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tcpservertest
TEMPLATE = app
LIBS += -lX11

SOURCES += main.cpp\
        dialog.cpp \
    simulationmouseevent.cpp

HEADERS  += dialog.h \
    simulationmouseevent.h
