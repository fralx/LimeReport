DEFINES +=  _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS ZINT_VERSION=\\\"$$ZINT_VERSION\\\" NO_PNG

!contains(DEFINES, NO_PNG) {
    LIBS += -lpng
}

INCLUDEPATH += \
    $$ZINT_PATH/backend \
    $$ZINT_PATH/backend_qt

HEADERS +=  $$ZINT_PATH/backend/aztec.h \
            $$ZINT_PATH/backend/big5.h \
            $$ZINT_PATH/backend/bmp.h \
            $$ZINT_PATH/backend/channel_precalcs.h \
            $$ZINT_PATH/backend/code1.h \
            $$ZINT_PATH/backend/code128.h \
            $$ZINT_PATH/backend/code49.h \
            $$ZINT_PATH/backend/common.h \
            $$ZINT_PATH/backend/composite.h \
            $$ZINT_PATH/backend/dmatrix.h \
            $$ZINT_PATH/backend/eci.h \
            $$ZINT_PATH/backend/eci_sb.h \
            $$ZINT_PATH/backend/emf.h \
            $$ZINT_PATH/backend/font.h \
            $$ZINT_PATH/backend/gb18030.h \
            $$ZINT_PATH/backend/gb2312.h \
            $$ZINT_PATH/backend/general_field.h \
            $$ZINT_PATH/backend/gridmtx.h \
            $$ZINT_PATH/backend/gs1.h \
            $$ZINT_PATH/backend/gs1_lint.h \
            $$ZINT_PATH/backend/hanxin.h \
            $$ZINT_PATH/backend/iso3166.h \
            $$ZINT_PATH/backend/iso4217.h \
            $$ZINT_PATH/backend/ksx1001.h \
            $$ZINT_PATH/backend/large.h \
            $$ZINT_PATH/backend/maxicode.h \
            $$ZINT_PATH/backend/ms_stdint.h \
            $$ZINT_PATH/backend/output.h \
            $$ZINT_PATH/backend/pcx.h \
            $$ZINT_PATH/backend/pdf417.h \
            $$ZINT_PATH/backend/qr.h \
            $$ZINT_PATH/backend/reedsol.h \
            $$ZINT_PATH/backend/reedsol_logs.h \
            $$ZINT_PATH/backend/rss.h \
            $$ZINT_PATH/backend/sjis.h \
            $$ZINT_PATH/backend/stdint_msvc.h \
            $$ZINT_PATH/backend/tif.h \
            $$ZINT_PATH/backend/tif_lzw.h \
            $$ZINT_PATH/backend/zfiletypes.h \
            $$ZINT_PATH/backend/zintconfig.h \
            $$ZINT_PATH/backend/zint.h \
            $$ZINT_PATH/backend_qt/qzint.h

SOURCES += $$ZINT_PATH/backend/2of5.c \
           $$ZINT_PATH/backend/auspost.c \
           $$ZINT_PATH/backend/aztec.c \
           $$ZINT_PATH/backend/bmp.c \
           $$ZINT_PATH/backend/codablock.c \
           $$ZINT_PATH/backend/code.c \
           $$ZINT_PATH/backend/code1.c \
           $$ZINT_PATH/backend/code128.c \
           $$ZINT_PATH/backend/code16k.c \
           $$ZINT_PATH/backend/code49.c \
           $$ZINT_PATH/backend/common.c \
           $$ZINT_PATH/backend/composite.c \
           $$ZINT_PATH/backend/dmatrix.c \
           $$ZINT_PATH/backend/dotcode.c \
           $$ZINT_PATH/backend/eci.c \
           $$ZINT_PATH/backend/emf.c \
           $$ZINT_PATH/backend/gb18030.c \
           $$ZINT_PATH/backend/gb2312.c \
           $$ZINT_PATH/backend/general_field.c \
           $$ZINT_PATH/backend/gif.c \
           $$ZINT_PATH/backend/gridmtx.c \
           $$ZINT_PATH/backend/gs1.c \
           $$ZINT_PATH/backend/hanxin.c \
           $$ZINT_PATH/backend/imail.c \
           $$ZINT_PATH/backend/large.c \
           $$ZINT_PATH/backend/library.c \
           $$ZINT_PATH/backend/mailmark.c \
           $$ZINT_PATH/backend/maxicode.c \
           $$ZINT_PATH/backend/medical.c \
           $$ZINT_PATH/backend/output.c \
           $$ZINT_PATH/backend/pcx.c \
           $$ZINT_PATH/backend/pdf417.c \
           $$ZINT_PATH/backend/plessey.c \
           $$ZINT_PATH/backend/png.c \
           $$ZINT_PATH/backend/postal.c \
           $$ZINT_PATH/backend/ps.c \
           $$ZINT_PATH/backend/qr.c \
           $$ZINT_PATH/backend/raster.c \
           $$ZINT_PATH/backend/reedsol.c \
           $$ZINT_PATH/backend/rss.c \
           $$ZINT_PATH/backend/sjis.c \
           $$ZINT_PATH/backend/svg.c \
           $$ZINT_PATH/backend/telepen.c \
           $$ZINT_PATH/backend/tif.c \
           $$ZINT_PATH/backend/ultra.c \
           $$ZINT_PATH/backend/upcean.c \
           $$ZINT_PATH/backend/vector.c \
           $$ZINT_PATH/backend/dllversion.c \
           $$ZINT_PATH/backend_qt/qzint.cpp
