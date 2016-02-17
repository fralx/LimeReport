QT += core gui script sql

EXTRA_DIR += $$PWD/demo_reports/*

win32:CONFIG(release, debug|release): DEST_DIR = $$OUT_PWD/demo_reports/
win32:CONFIG(debug, debug|release): DEST_DIR = $$OUT_PWD/demo_reports/

unix{
    DEST_DIR = $$OUT_PWD/demo_reports/
    QMAKE_POST_LINK += mkdir -p $$quote($$DEST_DIR) | $$QMAKE_COPY_DIR $$quote($$EXTRA_DIR) $$quote($$DEST_DIR) $$escape_expand(\n\t)
}

win32 {
    EXTRA_DIR ~= s,/,\\,g
    DEST_DIR ~= s,/,\\,g
    QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$EXTRA_DIR) $$quote($$DEST_DIR) $$escape_expand(\\n\\t)
    CONFIG(release, debug|release) {
        DEST_DIR = $$OUT_PWD/release/demo_reports/
        DEST_DIR ~= s,/,\\,g
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$EXTRA_DIR) $$quote($$DEST_DIR) $$escape_expand(\\n\\t)
    }
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = LRDemo
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../lib/release -llimereport
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../lib/debug -llimereport
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../lib/debug -llimereport
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../lib/release -llimereport

INCLUDEPATH += $$PWD/../../include
DEPENDPATH += $$PWD/../../include

RESOURCES += \
    r1.qrc

win32 {
  RC_FILE += mainicon.rc
}
