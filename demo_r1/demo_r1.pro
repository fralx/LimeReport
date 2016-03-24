include(../common.pri)
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4){
    QT += widgets printsupport
    DEFINES += HAVE_QT5
}

macx{
    CONFIG  += app_bundle
}

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
DEST_DIR       = $${BUILD_DIR}/demo/$${BUILD_TYPE}
REPORTS_DIR    = $${DEST_DIR}/demo_reports/

CONFIG(release, debug|release){
    message(Release)
    BUILD_TYPE = release
}else{
    message(Debug)
    BUILD_TYPE = debug
}

unix{    
    MOC_DIR        = $${OUT_PWD}/moc
    UI_DIR         = $${OUT_PWD}/ui
    UI_HEADERS_DIR = $${OUT_PWD}/ui
    UI_SOURCES_DIR = $${OUT_PWD}/ui
    OBJECTS_DIR    = $${OUT_PWD}/obj
    RCC_DIR        = $${OUT_PWD}/rcc

    LIBS += -L$${BUILD_DIR}/lib/$${BUILD_TYPE} -llimereport
contains(CONFIG,zint){
    LIBS += -L$${BUILD_DIR}/lib/$${BUILD_TYPE} -lQtZint
}
    DESTDIR = $$DEST_DIR
    QMAKE_POST_LINK += mkdir -p $$quote($$REPORTS_DIR) | $$QMAKE_COPY_DIR $$quote($$EXTRA_DIR) $$quote($$REPORTS_DIR) $$escape_expand(\n\t)
    target.path = $${DEST_DIR}
}

win32 {
    EXTRA_DIR ~= s,/,\\,g
    DEST_DIR ~= s,/,\\,g
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
    LIBS += -L$${BUILD_DIR}/lib/$${BUILD_TYPE} -llimereport
}

unix{
    INSTALLS = target
}
