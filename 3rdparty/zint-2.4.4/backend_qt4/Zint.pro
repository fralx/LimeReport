DEFINES += NO_PNG
TEMPLATE = lib

contains(CONFIG, static_build){
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

#VERSION = 2.4.4

INCLUDEPATH += $$PWD/../backend
DEFINES +=  _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS ZINT_VERSION=\\\"$$VERSION\\\"
contains(CONFIG,release) {
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


INCLUDEPATH += zint zint/backend zint/backend_qt4

HEADERS += \
    $$PWD/qzint.h \
    $$PWD/qzint_global.h \
    $$PWD/../backend/aztec.h \
    $$PWD/../backend/code1.h \
    $$PWD/../backend/code49.h \
    $$PWD/../backend/common.h \
    $$PWD/../backend/composite.h \
    $$PWD/../backend/dmatrix.h \
    $$PWD/../backend/font.h \
    $$PWD/../backend/gb2312.h \
    $$PWD/../backend/gridmtx.h \
    $$PWD/../backend/gs1.h \
    $$PWD/../backend/large.h \
    $$PWD/../backend/maxicode.h \
    $$PWD/../backend/maxipng.h \
    $$PWD/../backend/ms_stdint.h \
    $$PWD/../backend/pdf417.h \
    $$PWD/../backend/qr.h \
    $$PWD/../backend/reedsol.h \
    $$PWD/../backend/rss.h \
    $$PWD/../backend/sjis.h \
    $$PWD/../backend/zint.h

SOURCES += \
    $$PWD/qzint.cpp \
    $$PWD/../backend/2of5.c \
    $$PWD/../backend/auspost.c \
    $$PWD/../backend/aztec.c \
    $$PWD/../backend/code.c \
    $$PWD/../backend/code1.c \
    $$PWD/../backend/code16k.c \
    $$PWD/../backend/code49.c \
    $$PWD/../backend/code128.c \
    $$PWD/../backend/common.c \
    $$PWD/../backend/composite.c \
    $$PWD/../backend/dllversion.c \
    $$PWD/../backend/dmatrix.c \
    $$PWD/../backend/gridmtx.c \
    $$PWD/../backend/gs1.c \
    $$PWD/../backend/imail.c \
    $$PWD/../backend/large.c \
    $$PWD/../backend/library.c \
    $$PWD/../backend/maxicode.c \
    $$PWD/../backend/medical.c \
    $$PWD/../backend/pdf417.c \
    $$PWD/../backend/plessey.c \
    $$PWD/../backend/png.c \
    $$PWD/../backend/postal.c \
    $$PWD/../backend/ps.c \
    $$PWD/../backend/qr.c \
    $$PWD/../backend/reedsol.c \
    $$PWD/../backend/render.c \
    $$PWD/../backend/rss.c \
    $$PWD/../backend/svg.c \
    $$PWD/../backend/telepen.c \
    $$PWD/../backend/upcean.c


DESTDIR        = $${DEST_LIBS}
#DLLDESTDIR     = $${DESTDIR}
unix {
    target.path = $${DESTDIR}
    INSTALLS = target
}
