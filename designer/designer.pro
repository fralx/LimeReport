include(../common.pri)
QT += core gui

CONFIG(release) {
    TARGET = LRDesigner
} else {
    TARGET = LRDesignerd
}

TEMPLATE = app

HEADERS += \
           designersettingmanager.h

SOURCES += main.cpp \
           designersettingmanager.cpp

INCLUDEPATH += $$PWD/../include
DEPENDPATH  += $$PWD/../include

RESOURCES += $$PWD/../3rdparty/dark_style_sheet/qdarkstyle/style.qrc
RESOURCES += $$PWD/../3rdparty/light_style_sheet/qlightstyle/lightstyle.qrc

DEST_DIR       = $${DEST_BINS}
REPORTS_DIR    = $${DEST_DIR}

macx{
    CONFIG  += app_bundle
}

unix:{
    DESTDIR = $$DEST_DIR
linux{
    #Link share lib to ../lib rpath
    QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN
    QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/lib
    QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../lib
    QMAKE_LFLAGS_RPATH += #. .. ./libs
}
    target.path = $${DEST_DIR}
    INSTALLS = target
}

win32 {
    EXTRA_DIR ~= s,/,\\,g
    DEST_DIR ~= s,/,\\,g
    REPORTS_DIR ~= s,/,\\,g

    DESTDIR = $$DEST_DIR
    RC_FILE += mainicon.rc
}

LIBS += -L$${DEST_LIBS}
CONFIG(debug, debug|release) {
    LIBS += -llimereportd
} else {
    LIBS += -llimereport
}

!CONFIG(static_build) : CONFIG(zint) {
    CONFIG(debug, debug|release) {
        LIBS += -L$${DEST_LIBS} -lQtZintd
    } else {
        LIBS += -L$${DEST_LIBS} -lQtZint
    }
}
