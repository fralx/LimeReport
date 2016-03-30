CONFIG += build_translations
CONFIG += zint
ZINT_PATH = $$PWD/3rdparty/zint-2.4.4

CONFIG(release, debug|release){
    message(Release)
    BUILD_TYPE = release
}else{
    message(Debug)
    BUILD_TYPE = debug
}

BUILD_DIR = $$PWD/build/$${QT_VERSION}
DEST_INCLUDE_DIR = $$PWD/include/

unix:!macx {
    ARCH_DIR       = $${OUT_PWD}/unix
}
win32 {
    ARCH_DIR       = $${OUT_PWD}/win32
}
macx{
    ARCH_DIR       = $${OUT_PWD}/macx
}

MOC_DIR        = $${ARCH_DIR}/$${BUILD_TYPE}/moc
UI_DIR         = $${ARCH_DIR}/$${BUILD_TYPE}/ui
UI_HEADERS_DIR = $${ARCH_DIR}/$${BUILD_TYPE}/ui
UI_SOURCES_DIR = $${ARCH_DIR}/$${BUILD_TYPE}/ui
OBJECTS_DIR    = $${ARCH_DIR}/$${BUILD_TYPE}/obj
RCC_DIR        = $${ARCH_DIR}/$${BUILD_TYPE}/rcc

LIMEREPORT_VERSION_MAJOR = 1
LIMEREPORT_VERSION_MINOR = 3
LIMEREPORT_VERSION_RELEASE = 10

LIMEREPORT_VERSION = '\\"$${LIMEREPORT_VERSION_MAJOR}.$${LIMEREPORT_VERSION_MINOR}.$${LIMEREPORT_VERSION_RELEASE}\\"'
DEFINES += LIMEREPORT_VERSION_STR=\"$${LIMEREPORT_VERSION}\"
DEFINES += LIMEREPORT_VERSION=$${LIMEREPORT_VERSION}

QT += script xml sql
REPORT_PATH = $$PWD/limereport
TRANSLATIONS_PATH = $$PWD/translations

greaterThan(QT_MAJOR_VERSION, 4) {
    DEFINES+=HAVE_QT5
    QT+= printsupport widgets
    contains(QT,uitools){
        message(uitools)
        DEFINES += HAVE_UI_LOADER
    }
}

lessThan(QT_MAJOR_VERSION, 5){
    CONFIG(uitools){
        message(uitools)
        DEFINES += HAVE_UI_LOADER
    }
}
