QT += core gui script sql
greaterThan(QT_MAJOR_VERSION, 4){
    QT += widgets printsupport
    DEFINES += HAVE_QT5
}
CONFIG  -= app_bundle
TARGET = LRDemo
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += $$PWD/../include
DEPENDPATH  += $$PWD/../include

RESOURCES += \
    r1.qrc

EXTRA_DIR += $$PWD/demo_reports/*

CONFIG(release, debug|release){
    message(Release)
    BUILD_TYPE = release
}else{
    message(Debug)
    BUILD_TYPE = debug
}

unix{
    UNIX_DIR       = $$PWD/../build/unix
    DEST_DIR       = $${UNIX_DIR}/$${BUILD_TYPE}/demo
    REPORTS_DIR  = $${DEST_DIR}/demo_reports
    MOC_DIR        = $${OUT_PWD}/moc
    UI_DIR         = $${OUT_PWD}//ui
    UI_HEADERS_DIR = $${OUT_PWD}//ui
    UI_SOURCES_DIR = $${OUT_PWD}//ui
    OBJECTS_DIR    = $${OUT_PWD}//obj
    RCC_DIR        = $${OUT_PWD}//rcc

    LIBS += -L$$PWD/../build/unix/$${BUILD_TYPE}/lib -llimereport
    DESTDIR = $$DEST_DIR
    QMAKE_POST_LINK += mkdir -p $$quote($$REPORTS_DIR) | $$QMAKE_COPY_DIR $$quote($$EXTRA_DIR) $$quote($$REPORTS_DIR) $$escape_expand(\n\t)
    QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN
    QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/lib
    QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../lib
    QMAKE_LFLAGS_RPATH += #. .. ./libs
    target.path = $${DEST_DIR}
}

win32 {
    WIN32_DIR = $$PWD/../build/win32
    EXTRA_DIR ~= s,/,\\,g
    DEST_DIR       = $${WIN32_DIR}/$${BUILD_TYPE}/demo
    DEST_DIR      ~= s,/,\\,g
    REPORTS_DIR  = $${DEST_DIR}/demo_reports
    REPORTS_DIR ~= s,/,\\,g

    MOC_DIR        = $${OUT_PWD}/moc
    UI_DIR         = $${OUT_PWD}/ui
    UI_HEADERS_DIR = $${OUT_PWD}/ui
    UI_SOURCES_DIR = $${OUT_PWD}/ui
    OBJECTS_DIR    = $${OUT_PWD}/obj
    RCC_DIR        = $${OUT_PWD}/rcc

    DESTDIR = $$DEST_DIR
    RC_FILE += mainicon.rc

    QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$EXTRA_DIR) $$quote($$REPORTS_DIR) $$escape_expand(\\n\\t)
    LIBS += -L$$PWD/../build/win32/$${BUILD_TYPE}/lib -llimereport
}

unix{
    INSTALLS = target
}
