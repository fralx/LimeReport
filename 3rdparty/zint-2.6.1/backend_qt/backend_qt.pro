DEFINES += NO_PNG
TEMPLATE = lib

contains(CONFIG, static_build){
    message(Static Build)
    CONFIG += staticlib
    DEFINES += HAVE_STATIC_BUILD
}

!contains(CONFIG, staticlib){
    CONFIG += dll
    DEFINES += QZINT_LIBRARY
}

include(../../../common.pri)

macx{
    CONFIG -= dll
    CONFIG += lib_bundle
}

unix{
    CONFIG += plugin
}

INCLUDEPATH += $$PWD/../backend
DEFINES +=  _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS ZINT_VERSION=\\\"$$VERSION\\\"
CONFIG(release, debug|release){
        TARGET = QtZint
} else {
        TARGET = QtZintd
}

!contains(DEFINES, NO_PNG) {
    SOURCES += $$PWD/../backend/png.c
    LIBS += -lpng
}


win32-msvc* {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    #QMAKE_CFLAGS += /TP /wd4018 /wd4244 /wd4305
    #QMAKE_CXXFLAGS += /TP /wd4018 /wd4244 /wd4305
}


INCLUDEPATH += zint zint/backend zint/backend_qt

HEADERS +=  $$PWD/../backend/aztec.h \
            $$PWD/../backend/bmp.h \
            $$PWD/../backend/code49.h \
            $$PWD/../backend/common.h \
            $$PWD/../backend/composite.h \
            $$PWD/../backend/dmatrix.h \
            $$PWD/../backend/eci.h \
            $$PWD/../backend/font.h \
            $$PWD/../backend/gridmtx.h \
            $$PWD/../backend/gs1.h \
            $$PWD/../backend/hanxin.h \
            $$PWD/../backend/large.h \
            $$PWD/../backend/maxicode.h \
            $$PWD/../backend/pcx.h \
            $$PWD/../backend/pdf417.h \
            $$PWD/../backend/reedsol.h \
            $$PWD/../backend/rss.h \
            $$PWD/../backend/sjis.h \
            $$PWD/../backend/stdint_msvc.h \
            $$PWD/../backend/zint.h \
            $$PWD/qzint.h \
            $$PWD/qzint_global.h

SOURCES += $$PWD/../backend/2of5.c \
           $$PWD/../backend/auspost.c \
           $$PWD/../backend/aztec.c \
           $$PWD/../backend/bmp.c \
           $$PWD/../backend/codablock.c \
           $$PWD/../backend/code.c \
           $$PWD/../backend/code1.c \
           $$PWD/../backend/code128.c \
           $$PWD/../backend/code16k.c \
           $$PWD/../backend/code49.c \
           $$PWD/../backend/common.c \
           $$PWD/../backend/composite.c \
           $$PWD/../backend/dllversion.c \
           $$PWD/../backend/dmatrix.c \
           $$PWD/../backend/dotcode.c \
           $$PWD/../backend/eci.c \
           $$PWD/../backend/emf.c \
           $$PWD/../backend/gif.c \
           $$PWD/../backend/gridmtx.c \
           $$PWD/../backend/gs1.c \
           $$PWD/../backend/hanxin.c \
           $$PWD/../backend/imail.c \
           $$PWD/../backend/large.c \
           $$PWD/../backend/library.c \
           $$PWD/../backend/maxicode.c \
           $$PWD/../backend/medical.c \
           $$PWD/../backend/pcx.c \
           $$PWD/../backend/pdf417.c \
           $$PWD/../backend/plessey.c \
           $$PWD/../backend/png.c \
           $$PWD/../backend/postal.c \
           $$PWD/../backend/ps.c \
           $$PWD/../backend/qr.c \
           $$PWD/../backend/raster.c \
           $$PWD/../backend/reedsol.c \
           $$PWD/../backend/render.c \
           $$PWD/../backend/rss.c \
           $$PWD/../backend/svg.c \
           $$PWD/../backend/telepen.c \
           $$PWD/../backend/tif.c \
           $$PWD/../backend/upcean.c \
           $$PWD/qzint.cpp

DESTDIR        = $${DEST_LIBS}
#DLLDESTDIR     = $${DESTDIR}
unix {
    target.path = $${DESTDIR}
    INSTALLS = target
}
