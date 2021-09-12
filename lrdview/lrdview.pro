#-------------------------------------------------
#
# Project created by QtCreator 2014-03-24T20:29:50
#
#-------------------------------------------------

QT  += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets gui-private

CONFIG += uitools
include(../common.pri)
include(../limereport.pri)

TARGET = lrdview
TEMPLATE = app

win32 {
  RC_FILE += mainicon.rc
}

INCLUDEPATH += $$PWD/../include
DEPENDPATH  += $$PWD/../include

SOURCES += main.cpp\
        MainWindow.cpp \
    XmlModel.cpp \
    SettingDialog.cpp

HEADERS  += MainWindow.h \
    XmlModel.h \
    SettingDialog.h

FORMS    += MainWindow.ui \
    SettingDialog.ui

RESOURCES += \
    MainWindow.qrc

#win32:CONFIG(release, debug|release): {
#    LIBS += -LD:/Work/C++/Projects/LRDView/libs -ladateeditorplugin
#    QTPLUGIN += adateeditorplugin
#    DEFINES += DEBUG
#}

