#-------------------------------------------------
#
# Project created by QtCreator 2018-03-09T16:01:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RDCServer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        RDCMainWindow.cpp \
    RDCPreferencesDialog.cpp \
    RDCTcpSocket.cpp \
    RDCClientInfo.cpp \
    RDCClientInfoList.cpp \
    RDCServer.cpp \
    RDCMessage.cpp \
    RDCMessagePool.cpp \
    RDCConfiguration.cpp \
    RDCHostInfo.cpp

HEADERS += \
        RDCMainWindow.h \
    RDCPreferencesDialog.h \
    RDCTcpSocket.h \
    RDCClientInfo.h \
    RDCClientInfoList.h \
    RDCServer.h \
    RDCMessage.h \
    RDCServiceCommand.h \
    RDCMessagePool.h \
    RDCConfiguration.h \
    RDCTcpSocketEventHandler.h \
    RDCHostInfo.h

FORMS += \
        RDCMainWindow.ui \
    RDCPreferencesDialog.ui

INCLUDEPATH += \
              /usr/local/include

LIBS += -lz \
        -L /usr/local/lib/libevent.a    \
        -L /usr/local/lib/libevent_pthreads.a   \
        -L /usr/local/lib/libevent_core.a
