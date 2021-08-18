include(../common.pri)
QT += core gui

CONFIG(release, debug|release) {
    TARGET = LRDemo_r1
} else {
    TARGET = LRDemo_r1d
}

TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp

HEADERS += mainwindow.h

FORMS   += mainwindow.ui

INCLUDEPATH += $$PWD/../include
DEPENDPATH  += $$PWD/../include

RESOURCES += \
             r1.qrc

EXTRA_DIR     += $$PWD/demo_reports
DEST_DIR       = $${DEST_BINS}
REPORTS_DIR    = $${DEST_DIR}

macx{
    CONFIG += app_bundle
}

unix:{
    DESTDIR = $$DEST_DIR
    #    QMAKE_POST_LINK += mkdir -p $$quote($$REPORTS_DIR) |
    QMAKE_POST_LINK += $$QMAKE_COPY_DIR \"$$EXTRA_DIR\" \"$$REPORTS_DIR\" $$escape_expand(\n\t)
		
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
    DESTDIR = $$DEST_DIR
    contains(QMAKE_HOST.os, Linux) {
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR \"$$EXTRA_DIR\" \"$$REPORTS_DIR\" $$escape_expand(\n\t)
    } else {
        EXTRA_DIR ~= s,/,\\,g
        DEST_DIR ~= s,/,\\,g
	REPORTS_DIR ~= s,/,\\,g
	RC_FILE += mainicon.rc
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR \"$$EXTRA_DIR\" \"$$REPORTS_DIR\\demo_reports\" $$escape_expand(\\n\\t)
    }
}

LIBS += -L$${DEST_LIBS}
CONFIG(debug, debug|release) {
    LIBS += -llimereportd
} else {
    LIBS += -llimereport
}
message($$LIBS)

!CONFIG(static_build) : CONFIG(zint) {
    LIBS += -L$${DEST_LIBS}
    CONFIG(debug, debug|release) {
        LIBS += -lQtZintd
    } else {
        LIBS += -lQtZint
    }
}
