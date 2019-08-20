#-------------------------------------------------
#
# PIF IDE by PIF Project
# Licence: Nintersoft Open Source Licence
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pif_ide
TEMPLATE = app

win32 {
    VERSION = 0.8.02

    QMAKE_TARGET_COMPANY = PIF Project
    QMAKE_TARGET_PRODUCT = PIF IDE
    QMAKE_TARGET_DESCRIPTION = PIF IDE
    QMAKE_TARGET_COPYRIGHT = Copyright (c) 2019 - PIF Project

    RC_ICONS = images\logo\pif_logo_x64.ico
    RC_LANG = 0x0800
}
else {
    VERSION = 0.8.02
}

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

INCLUDEPATH += ./nmainwindow \
    ./pifeditor

SOURCES += \
        aboutwindow.cpp \
        highlighter/highlighter.cpp \
        main.cpp \
        nmainwindow/titlebar.cpp \
        nmainwindow/nmainwindow.cpp \
        editorwindow.cpp \
        pifeditor/pifeditor.cpp \
        settingswindow.cpp

HEADERS += \
        aboutwindow.h \
        highlighter/highlighter.h \
        nmainwindow/titlebar.h \
        nmainwindow/nmainwindow.h \
        editorwindow.h \
        pifeditor/pifeditor.h \
        settingswindow.h

FORMS += \
        aboutwindow.ui \
        nmainwindow/titlebar.ui \
        nmainwindow/nmainwindow.ui \
        editorwindow.ui \
        settingswindow.ui

TRANSLATIONS += lang/pif_ide_pt.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
