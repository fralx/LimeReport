#CONFIG +=zint
#QMAKE_CFLAGS += -std=c99
#ZINT_PATH = $$PWD/../zint-2.4.3
#ZINT_VERSION = 2.4.3
#include(qzint.pri)
include(report-lib.pri)

TEMPLATE = lib
QT += core xml sql script
TARGET = limereport

DEFINES += LIMEREPORT_EXPORTS

EXTRA_FILES += \
    $$PWD/src/lrglobal.cpp \
    $$PWD/src/lrglobal.h \
    $$PWD/src/lrdatasourcemanagerintf.h \
    $$PWD/src/lrreportengine.h \
    $$PWD/src/lrscriptenginemanagerintf.h \
    $$PWD/src/lrcallbackdatasourceintf.h

DEST_DIR = $$PWD/include/

unix {
    for(FILE,EXTRA_FILES){
        QMAKE_POST_LINK += $$quote($$QMAKE_COPY $${FILE} $${DEST_DIR}$$escape_expand(\n\t))
    }
}

win32 {
    EXTRA_FILES ~= s,/,\\,g
    DEST_DIR ~= s,/,\\,g
    for(FILE,EXTRA_FILES){
                QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DEST_DIR) $$escape_expand(\\n\\t)
    }
}

DESTDIR = $$PWD/lib

DEPENDPATH += report report/bands report/base report/databrowser report/items report/objectinspector 
INCLUDEPATH += report report/bands report/base report/databrowser report/items report/objectinspector

CONFIG(release, debug|release): DESTDIR = $$PWD/lib/release/
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/lib/debug/

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../zint-2.4.3/build-backend_dll-Desktop_Qt_5_5_0_MSVC2010_32bit-Release/release/ -lQtZint22
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../zint-2.4.3/build-backend_dll-Desktop_Qt_5_5_0_MSVC2010_32bit-Release/debug/ -lQtZint22

#INCLUDEPATH += $$PWD/../zint-2.4.3/backend $$PWD/../zint-2.4.3/backend_qt4
#DEPENDPATH += $$PWD/../zint-2.4.3/backend $$PWD/../zint-2.4.3/backend_qt4
