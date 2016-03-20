QT += core gui script sql
greaterThan(QT_MAJOR_VERSION, 4){
    QT += widgets printsupport
    DEFINES += HAVE_QT5
}
TARGET = LRDemo
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += $$PWD/../../include
DEPENDPATH  += $$PWD/../../include

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
CONFIG  -= app_bundle

unix{
    UNIX_DIR       = $$OUT_PWD/../build/unix
    DEST_DIR       = $${UNIX_DIR}/$${BUILD_TYPE}/demo_reports
    MOC_DIR        = $${DEST_DIR}/moc
    UI_DIR         = $${DEST_DIR}/ui
    UI_HEADERS_DIR = $${DEST_DIR}/ui
    UI_SOURCES_DIR = $${DEST_DIR}/ui
    OBJECTS_DIR    = $${DEST_DIR}/obj
    RCC_DIR        = $${DEST_DIR}/rcc

    LIBS += -L$$PWD/../../lib/$${BUILD_TYPE} -llimereport
    DESTDIR = $$DEST_DIR/bin
    QMAKE_POST_LINK += mkdir -p $$quote($$DESTDIR) | $$QMAKE_COPY_DIR $$quote($$EXTRA_DIR) $$quote($$DESTDIR) $$escape_expand(\n\t)
}

win32 {
    WIN32_DIR = $$OUT_PWD/../build/win32
    EXTRA_DIR ~= s,/,\\,g

    DEST_DIR       = $${WIN32_DIR}/$${BUILD_TYPE}/demo_reports/
    DEST_DIR      ~= s,/,\\,g
    MOC_DIR        = $${DEST_DIR}/moc
    UI_DIR         = $${DEST_DIR}/ui
    UI_HEADERS_DIR = $${DEST_DIR}/ui
    UI_SOURCES_DIR = $${DEST_DIR}/ui
    OBJECTS_DIR    = $${DEST_DIR}/obj
    RCC_DIR        = $${DEST_DIR}/rcc

    DESTDIR = $$DEST_DIR/bin
    RC_FILE += mainicon.rc

    QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$EXTRA_DIR) $$quote($$DESTDIR) $$escape_expand(\\n\\t)
    LIBS += -L$$PWD/../../lib/$${BUILD_TYPE} -llimereport
}
