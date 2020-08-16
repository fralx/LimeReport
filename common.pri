#BINARY_RESULT_DIR = $${TOP_BUILD_DIR}

isEmpty(BINARY_RESULT_DIR) {
    BINARY_RESULT_DIR = $${PWD}
}

message(TOP_BUILD_DIR: $$TOP_BUILD_DIR)

#!contains(CONFIG, config_build_dir){
#    TOP_BUILD_DIR = $${PWD}
#}

!contains(CONFIG, no_build_translations){
    CONFIG += build_translations
}
#CONFIG *= easy_profiler

!contains(CONFIG, no_zint){
    CONFIG *= zint
}

!contains(CONGIG, no_svg){
    QT *= svg
    CONFIG *= svg
    DEFINES += HAVE_SVG
}

INCLUDEPATH += $$PWD/3rdparty/easyprofiler/easy_profiler_core/include
DEPENDPATH += $$PWD/3rdparty/easyprofiler/easy_profiler_core/include

contains(CONFIG, easy_profiler){
    message(EasyProfiler)
    unix|win32: LIBS += -L$$PWD/3rdparty/easyprofiler/build/bin/ -leasy_profiler
    greaterThan(QT_MAJOR_VERSION, 4){
        DEFINES += BUILD_WITH_EASY_PROFILER
    }
}

!contains(CONFIG, qtscriptengine){
greaterThan(QT_MAJOR_VERSION, 4){
greaterThan(QT_MINOR_VERSION, 5){
    CONFIG *= qjsengine
}
lessThan(QT_MINOR_VERSION, 6){
    CONFIG *= qtscriptengine
}
}
lessThan(QT_MAJOR_VERSION, 5){
    CONFIG *= qtscriptengine
}
}

contains(CONFIG, qtscriptengine){
    CONFIG -= qjsengine
    QT *= script
    DEFINES *= USE_QTSCRIPTENGINE
    message(qtscriptengine)
}

!contains(CONFIG, no_formdesigner){
    CONFIG *= dialogdesigner
}

!contains(CONFIG, no_embedded_designer){
    CONFIG *= embedded_designer
    DEFINES += HAVE_REPORT_DESIGNER
}

ZINT_PATH = $$PWD/3rdparty/zint-2.6.1
contains(CONFIG,zint){
    DEFINES *= HAVE_ZINT
}

greaterThan(QT_MAJOR_VERSION, 4) {
    QT *= uitools
}

lessThan(QT_MAJOR_VERSION, 5){
    CONFIG *= uitools
}

CONFIG(release, debug|release){
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
        ARCH_DIR       = $${OUT_PWD}/macx
        ARCH_TYPE      = macx
    }
    linux{
        !contains(QT_ARCH, x86_64){
            message("Compiling for 32bit system")
            ARCH_DIR       = $${OUT_PWD}/linux32
            ARCH_TYPE      = linux32
        }else{
            message("Compiling for 64bit system")
            ARCH_DIR       = $${OUT_PWD}/linux64
            ARCH_TYPE      = linux64
        }
    }
}
win32 {
    !contains(QT_ARCH, x86_64) {
        message("Compiling for 32bit system")
        ARCH_DIR       = $${OUT_PWD}/win32
        ARCH_TYPE      = win32
    } else {
        message("Compiling for 64bit system")
        ARCH_DIR       = $${OUT_PWD}/win64
        ARCH_TYPE      = win64
    }
}

DEST_LIBS = $${BUILD_DIR}/$${ARCH_TYPE}/$${BUILD_TYPE}/lib
DEST_BINS = $${BUILD_DIR}/$${ARCH_TYPE}/$${BUILD_TYPE}/$${TARGET}

MOC_DIR        = $${ARCH_DIR}/$${BUILD_TYPE}/moc
UI_DIR         = $${ARCH_DIR}/$${BUILD_TYPE}/ui
UI_HEADERS_DIR = $${ARCH_DIR}/$${BUILD_TYPE}/ui
UI_SOURCES_DIR = $${ARCH_DIR}/$${BUILD_TYPE}/ui
OBJECTS_DIR    = $${ARCH_DIR}/$${BUILD_TYPE}/obj
RCC_DIR        = $${ARCH_DIR}/$${BUILD_TYPE}/rcc

LIMEREPORT_VERSION_MAJOR = 1
LIMEREPORT_VERSION_MINOR = 5
LIMEREPORT_VERSION_RELEASE = 68

LIMEREPORT_VERSION = '$${LIMEREPORT_VERSION_MAJOR}.$${LIMEREPORT_VERSION_MINOR}.$${LIMEREPORT_VERSION_RELEASE}'
DEFINES *= LIMEREPORT_VERSION_STR=\\\"$${LIMEREPORT_VERSION}\\\"

QT *= xml sql

REPORT_PATH = $$PWD/limereport
TRANSLATIONS_PATH = $$PWD/translations

greaterThan(QT_MAJOR_VERSION, 4) {
    DEFINES *= HAVE_QT5
    QT *= printsupport widgets
    contains(QT,uitools){
        message(uitools)
        DEFINES *= HAVE_UI_LOADER
    }
    contains(CONFIG, qjsengine){
        message(qjsengine)
        DEFINES *= USE_QJSENGINE
        QT *= qml
    }
}

lessThan(QT_MAJOR_VERSION, 5){
    DEFINES *= HAVE_QT4
    CONFIG(uitools){
        message(uitools)
        DEFINES *= HAVE_UI_LOADER
    }
}
