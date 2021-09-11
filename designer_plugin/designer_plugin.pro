QT += core gui

include(../common.pri)
include(../limereport/limereport.pri)
include(../limereport/designer.pri)

CONFIG(release) {
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
    CONFIG += lib_bundle
    CONFIG += -dll
}

DESTDIR = $${DEST_LIBS}
unix {
    target.path = $${DESTDIR}
    INSTALLS = target
}


CONFIG(zint) {
    message(zint)
    INCLUDEPATH += $$ZINT_PATH/backend $$ZINT_PATH/backend_qt
    DEPENDPATH += $$ZINT_PATH/backend $$ZINT_PATH/backend_qt
    LIBS += -L$${DEST_LIBS}
    CONFIG(debug, debug|release) {
        LIBS += -lQtZintd
    } else {
        LIBS += -lQtZint
    }
}
