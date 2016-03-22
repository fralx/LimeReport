TARGET = limereport
TEMPLATE = lib

CONFIG(release, debug|release){
    message(Release)
    BUILD_TYPE = release
}else{
    message(Debug)
    BUILD_TYPE = debug
}

CONFIG += lib
CONFIG += dll
CONFIG += create_prl
CONFIG += link_prl
CONFIG  -= app_bundle

DEFINES += LIMEREPORT_EXPORTS

EXTRA_FILES += \
    $$PWD/lrglobal.cpp \
    $$PWD/lrglobal.h \
    $$PWD/lrdatasourcemanagerintf.h \
    $$PWD/lrreportengine.h \
    $$PWD/lrscriptenginemanagerintf.h \
    $$PWD/lrcallbackdatasourceintf.h

DEST_DIR = $$PWD/../include/

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
    DESTDIR = $$PWD/../build/unix/$${BUILD_TYPE}/lib
    for(FILE,EXTRA_FILES){
        QMAKE_POST_LINK += mkdir -p $$quote($${DESTDIR}/include) | $$QMAKE_COPY $$quote($$FILE) $$quote($$DESTDIR/include/) $$escape_expand(\\n\\t)
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
    DESTDIR = $$PWD/../build/win32/$${BUILD_TYPE}/lib
}

include(limereport.pri)

contains(CONFIG,zint){
    message(zint)
    INCLUDEPATH += $$ZINT_PATH/backend $$ZINT_PATH/backend_qt4
    DEPENDPATH += $$ZINT_PATH/backend $$ZINT_PATH/backend_qt4
    LIBS += -L$${DESTDIR} -lQtZint
}

#######
####Automatically build required translation files (*.qm)


LANGUAGES = ru es_ES

defineReplace(prependAll) {
    for(a,$$1):result += $$2$${a}$$3
    return($$result)
}

TRANSLATIONS_PATH = $$PWD/../translations
TRANSLATIONS = $$prependAll(LANGUAGES, $$TRANSLATIONS_PATH/limereport_,.ts)

qtPrepareTool(LUPDATE, lupdate)
ts.commands = $$LUPDATE $$_PRO_FILE_

qtPrepareTool(LRELEASE, lrelease)
qm.commands = $$LRELEASE $$_PRO_FILE_
qm.depends = ts

QMAKE_EXTRA_TARGETS += qm ts
POST_TARGETDEPS +=  qm ts


#### EN AUTOMATIC TRANSLATIONS


