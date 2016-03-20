DEFINES += WITH_ZINT
contains (DEFINES,WITH_ZINT){
    CONFIG +=zint
    include(3rdparty/3rdparty.pro)
}

include(report-lib.pri)
TEMPLATE = lib
QT += core xml sql script
TARGET = limereport

DEFINES += LIMEREPORT_EXPORTS

CONFIG(release, debug|release){
    message(Release)
    BUILD_TYPE = release
}else{
    message(Debug)
    BUILD_TYPE = debug
}
CONFIG += create_prl
CONFIG += link_prl

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
    MOC_DIR = $${OUT_PWD}/unix/$${BUILD_TYPE}/moc
    UI_DIR = $${OUT_PWD}/unix/$${BUILD_TYPE}/ui
    UI_HEADERS_DIR = $${OUT_PWD}/unix/$${BUILD_TYPE}/ui
    UI_SOURCES_DIR = $${OUT_PWD}/unix/$${BUILD_TYPE}/ui
    OBJECTS_DIR = $${OUT_PWD}/unix/$${BUILD_TYPE}/obj
    RCC_DIR = $${OUT_PWD}/unix/$${BUILD_TYPE}/rcc
    DESTDIR = $$PWD/lib/unix/$${BUILD_TYPE}
    for(FILE,EXTRA_FILES){
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DESTDIR/include) $$escape_expand(\\n\\t)
    }

}

win32 {
    EXTRA_FILES ~= s,/,\\,g
    DEST_DIR ~= s,/,\\,g
    for(FILE,EXTRA_FILES){
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DEST_DIR) $$escape_expand(\\n\\t)
    }
    MOC_DIR = $${OUT_PWD}/win32/$${BUILD_TYPE}/moc
    UI_DIR = $${OUT_PWD}/win32/$${BUILD_TYPE}/ui
    UI_HEADERS_DIR = $${OUT_PWD}/win32/$${BUILD_TYPE}/ui
    UI_SOURCES_DIR = $${OUT_PWD}/win32/$${BUILD_TYPE}/ui
    OBJECTS_DIR = $${OUT_PWD}/win32/$${BUILD_TYPE}/obj
    RCC_DIR = $${OUT_PWD}/win32/$${BUILD_TYPE}/rcc
    DESTDIR = $$PWD/lib/win32/$${BUILD_TYPE}

}

DEPENDPATH += report report/bands report/base report/databrowser report/items report/objectinspector 
INCLUDEPATH += report report/bands report/base report/databrowser report/items report/objectinspector

contains(DEFINES,WITH_ZINT){

    LIBS += -L$${DEST_DIR} -lQtZint
}

INSTALLS += target
