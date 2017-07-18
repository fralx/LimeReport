include(../common.pri)
QT += core gui

contains(CONFIG,release) {
	TARGET = LRDemo_r1
} else {
	TARGET = LRDemo_r1d
}

TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += $$PWD/../include
DEPENDPATH  += $$PWD/../include

RESOURCES += \
    r1.qrc

EXTRA_DIR     += $$PWD/demo_reports
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
#    QMAKE_POST_LINK += mkdir -p $$quote($$REPORTS_DIR) |
    QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$EXTRA_DIR) $$quote($$REPORTS_DIR) $$escape_expand(\n\t)
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

    QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_quote($$EXTRA_DIR\\*) $$shell_quote($$REPORTS_DIR\\demo_reports) $$escape_expand(\\n\\t)
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


