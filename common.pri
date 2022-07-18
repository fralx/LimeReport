# uncomment to disable translations
#CONFIG += no_build_translations

# uncomment to disable zint
#CONFIG += no_zint

# uncomment to disable svg
#CONFIG += no_svg

# uncomment to enable easy_profiler
#CONFIG *= easy_profiler

isEmpty(BINARY_RESULT_DIR) {
    BINARY_RESULT_DIR = $${PWD}
}

!CONFIG(no_build_translations) {
    CONFIG *= build_translations
}

!CONFIG(no_zint) {
    CONFIG *= zint
}

!CONFIG(no_svg) {
    QT *= svg
    CONFIG *= svg
    DEFINES *= HAVE_SVG
}

CONFIG(easy_profiler) {
    message(EasyProfiler)
    INCLUDEPATH *= $$PWD/3rdparty/easyprofiler/easy_profiler_core/include
    DEPENDPATH *= $$PWD/3rdparty/easyprofiler/easy_profiler_core/include
    unix|win32: LIBS *= -L$$PWD/3rdparty/easyprofiler/build/bin/ -leasy_profiler    
    equals(QT_MAJOR_VERSION, 5) | equals(QT_MAJOR_VERSION, 6) {
        DEFINES *= BUILD_WITH_EASY_PROFILER
    }
}

!CONFIG(qtscriptengine) {
    equals(QT_MAJOR_VERSION, 4) {
        CONFIG *= qtscriptengine
    }
    equals(QT_MAJOR_VERSION, 5) : lessThan(QT_MINOR_VERSION, 6) {
        CONFIG *= qtscriptengine
    }
    equals(QT_MAJOR_VERSION, 5) : greaterThan(QT_MINOR_VERSION, 5) {
        CONFIG *= qjsengine
    }
    equals(QT_MAJOR_VERSION, 6) {
        CONFIG *= qjsengine
    }
}

CONFIG(qtscriptengine) {
    CONFIG -= qjsengine
    QT *= script
    DEFINES *= USE_QTSCRIPTENGINE
    message(qtscriptengine)
}

!CONFIG(no_formdesigner) {
    CONFIG *= dialogdesigner
}

!CONFIG(no_embedded_designer) {
    CONFIG *= embedded_designer
    DEFINES *= HAVE_REPORT_DESIGNER
    message(embedded designer)
}

ZINT_PATH = $$PWD/3rdparty/zint-2.10.0
CONFIG(zint) {
    DEFINES *= HAVE_ZINT
}

equals(QT_MAJOR_VERSION, 4) {
    CONFIG *= uitools
}

equals(QT_MAJOR_VERSION, 5) | equals(QT_MAJOR_VERSION, 6) {
    QT *= uitools
}

CONFIG(release, debug|release) {
    message(Release)
    BUILD_TYPE = release
}else{
    message(Debug)
    BUILD_TYPE = debug
}

BUILD_DIR = $${BINARY_RESULT_DIR}/build/$${QT_VERSION}

DEST_INCLUDE_DIR = $$PWD/include

unix{
    ARCH_DIR       = $${OUT_PWD}/unix
    ARCH_TYPE      = unix

    macx{
        ARCH_DIR   = $${OUT_PWD}/macx
        ARCH_TYPE  = macx
    }

    linux{
        !contains(QT_ARCH, x86_64) {
            message("Compiling for 32bit system")
            ARCH_DIR  = $${OUT_PWD}/linux32
            ARCH_TYPE = linux32
        }else{
            message("Compiling for 64bit system")
            ARCH_DIR  = $${OUT_PWD}/linux64
            ARCH_TYPE = linux64
        }
    }
}

win32 {
    !contains(QT_ARCH, x86_64) {
        message("Compiling for 32bit system")
        ARCH_DIR  = $${OUT_PWD}/win32
        ARCH_TYPE = win32
    } else {
        message("Compiling for 64bit system")
        ARCH_DIR  = $${OUT_PWD}/win64
        ARCH_TYPE = win64
    }
}

DEST_LIBS      = $${BUILD_DIR}/$${ARCH_TYPE}/$${BUILD_TYPE}/lib
DEST_BINS      = $${BUILD_DIR}/$${ARCH_TYPE}/$${BUILD_TYPE}/$${TARGET}

MOC_DIR        = $${ARCH_DIR}/$${BUILD_TYPE}/moc
UI_DIR         = $${ARCH_DIR}/$${BUILD_TYPE}/ui
UI_HEADERS_DIR = $${ARCH_DIR}/$${BUILD_TYPE}/ui
UI_SOURCES_DIR = $${ARCH_DIR}/$${BUILD_TYPE}/ui
OBJECTS_DIR    = $${ARCH_DIR}/$${BUILD_TYPE}/obj
RCC_DIR        = $${ARCH_DIR}/$${BUILD_TYPE}/rcc

LIMEREPORT_VERSION_MAJOR = 1
LIMEREPORT_VERSION_MINOR = 6
LIMEREPORT_VERSION_RELEASE = 8

LIMEREPORT_VERSION = '$${LIMEREPORT_VERSION_MAJOR}.$${LIMEREPORT_VERSION_MINOR}.$${LIMEREPORT_VERSION_RELEASE}'
DEFINES *= LIMEREPORT_VERSION_STR=\\\"$${LIMEREPORT_VERSION}\\\"

QT *= xml sql

REPORT_PATH = $$PWD/limereport
TRANSLATIONS_PATH = $$PWD/translations

equals(QT_MAJOR_VERSION, 4) {
    DEFINES *= HAVE_QT4
    CONFIG(uitools) {
        message(uitools)
        DEFINES *= HAVE_UI_LOADER
    }
}

equals(QT_MAJOR_VERSION, 5) | equals(QT_MAJOR_VERSION, 6) {
    DEFINES *= HAVE_QT5
    QT *= printsupport widgets
    contains(QT, uitools) {
        message(uitools)
        DEFINES *= HAVE_UI_LOADER
    }
    CONFIG(qjsengine) {
        message(qjsengine)
        DEFINES *= USE_QJSENGINE
        QT *= qml
    }
}
