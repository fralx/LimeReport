include(../common.pri)
QT += core gui

contains(CONFIG,release) {
	TARGET = LRDesigner
} else {
	TARGET = LRDesignerd
}
TEMPLATE = app

SOURCES += main.cpp

INCLUDEPATH += $$PWD/../include
DEPENDPATH  += $$PWD/../include

DEST_DIR       = $${DEST_BINS}
REPORTS_DIR    = $${DEST_DIR}

macx{
    CONFIG  += app_bundle
}

unix:{
    LIBS += -L$${DEST_LIBS} -llimereport
    !contains(CONFIG, static_build){
        contains(CONFIG,zint){
            LIBS += -L$${DEST_LIBS} -lQtZint
        }
    }
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
    !contains(CONFIG, static_build){
        contains(CONFIG,zint){
            LIBS += -L$${DEST_LIBS} -lQtZint
        }
    }
    LIBS += -L$${DEST_LIBS}
	contains(CONFIG,release) {
		LIBS += -llimereport
	} else {
		LIBS += -llimereportd
	}
}

