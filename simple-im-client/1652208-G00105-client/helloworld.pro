#-------------------------------------------------
#
# Project created by QtCreator 2018-12-16T18:36:43
#
#-------------------------------------------------

QT       += core gui
QT       += network

INCLUDEPATH +="D:\Qt\Qt5.12.0\5.12.0\msvc2017_64\include\QtNetwork"

CONFIG += release

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = helloworld
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

CONFIG += c++11

SOURCES += \
        main.cpp \
    mainwindow.cpp \
    main_form.cpp \
    load_msg.cpp \
    repsw.cpp \
    repswfail.cpp \
    f_offline.cpp \
    setting.cpp \
    group_form.cpp

HEADERS += \
    mainwindow.h \
    main_form.h \
    client.h \
    load_msg.h \
    repsw.h \
    repswfail.h \
    f_offline.h \
    setting.h \
    group_form.h

FORMS += \
    mainwindow.ui \
    main_form.ui \
    load_msg.ui \
    repsw.ui \
    repswfail.ui \
    f_offline.ui \
    setting.ui \
    group_form.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS=client.ico
