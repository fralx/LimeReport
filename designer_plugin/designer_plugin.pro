include(../common.pri)
include(limereport.pri)
QT += core gui

contains(CONFIG,release) {
        TARGET = designer_plugin
} else {
        TARGET = designer_plugind
}

TEMPLATE = lib
CONFIG += plugin

HEADERS += \
    lrdesignerplugin.h
SOURCES += \
    lrdesignerplugin.cpp

INCLUDEPATH += $$PWD/../include
DEPENDPATH  += $$PWD/../include

macx{
    CONFIG  += lib_bundle
    CONFIG  += -dll
}

DESTDIR        = $${DEST_LIBS}
unix {
    target.path = $${DESTDIR}
    INSTALLS = target
}


contains(CONFIG,zint){
    message(zint)
    INCLUDEPATH += $$ZINT_PATH/backend $$ZINT_PATH/backend_qt
    DEPENDPATH += $$ZINT_PATH/backend $$ZINT_PATH/backend_qt
        LIBS += -L$${DEST_LIBS}
        contains(CONFIG,release) {
                LIBS += -lQtZint
        } else {
                LIBS += -lQtZintd
        }
}
