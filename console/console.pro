include(../common.pri)
QT += core
QT -= gui

TARGET = limereport
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app
SOURCES += main.cpp

DEFINES += QT_DEPRECATED_WARNINGS

DESTDIR = $${DEST_BINS}

INCLUDEPATH += $$PWD/../include
DEPENDPATH  += $$PWD/../include

LIBS += -L$${DEST_LIBS}

CONFIG(debug, debug|release) {
    LIBS += -llimereportd
} else {
    LIBS += -llimereport
}

!CONFIG(static_build) : CONFIG(zint) {
    LIBS += -L$${DEST_LIBS}
    CONFIG(debug, debug|release) {
        LIBS += -lQtZintd
    } else {
        LIBS += -lQtZint
    }
}
