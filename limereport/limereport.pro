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

macx{
    CONFIG  -= dll
    CONFIG  += lib_bundle
}

DEFINES += LIMEREPORT_EXPORTS

EXTRA_FILES += \
    $$PWD/lrglobal.cpp \
    $$PWD/lrglobal.h \
    $$PWD/lrdatasourcemanagerintf.h \
    $$PWD/lrreportengine.h \
    $$PWD/lrscriptenginemanagerintf.h \
    $$PWD/lrcallbackdatasourceintf.h

include(limereport.pri)

DEST_DIR = $$PWD/../include/

unix {
    UNIX_DIR      = $${OUT_PWD}/unix
    MOC_DIR        = $${UNIX_DIR}/moc/$${BUILD_TYPE}
    UI_DIR         = $${UNIX_DIR}/ui/$${BUILD_TYPE}
    UI_HEADERS_DIR = $${UNIX_DIR}/ui/$${BUILD_TYPE}
    UI_SOURCES_DIR = $${UNIX_DIR}/ui/$${BUILD_TYPE}
    OBJECTS_DIR    = $${UNIX_DIR}/obj/$${BUILD_TYPE}
    RCC_DIR        = $${UNIX_DIR}/rcc/$${BUILD_TYPE}
    DESTDIR        = $${BUILD_DIR}/lib/$${BUILD_TYPE}

    QMAKE_POST_LINK += mkdir -p $$quote($${BUILD_DIR}/lib/include) $$escape_expand(\\n\\t)

    for(FILE,EXTRA_FILES){
        QMAKE_POST_LINK += $$quote($$QMAKE_COPY $${FILE} $${DEST_DIR}) $$escape_expand(\\n\\t)
    }
    for(FILE,EXTRA_FILES){
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($${BUILD_DIR}/lib/include/) $$escape_expand(\\n\\t)
    }
}

win32 {
    EXTRA_FILES ~= s,/,\\,g
    DEST_DIR ~= s,/,\\,g
    BUILD_DIR ~= s,/,\\,g

    WIN32_DIR      = $${OUT_PWD}/win32
    MOC_DIR        = $${WIN32_DIR}/moc/$${BUILD_TYPE}
    UI_DIR         = $${WIN32_DIR}/ui/$${BUILD_TYPE}
    UI_HEADERS_DIR = $${WIN32_DIR}/ui/$${BUILD_TYPE}
    UI_SOURCES_DIR = $${WIN32_DIR}/ui/$${BUILD_TYPE}
    OBJECTS_DIR    = $${WIN32_DIR}/obj/$${BUILD_TYPE}
    RCC_DIR        = $${WIN32_DIR}/rcc/$${BUILD_TYPE}
    DESTDIR        = $${BUILD_DIR}/lib/$${BUILD_TYPE}

    for(FILE,EXTRA_FILES){
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DEST_DIR) $$escape_expand(\\n\\t)
    }
}

contains(CONFIG,zint){
    message(zint)
    INCLUDEPATH += $$ZINT_PATH/backend $$ZINT_PATH/backend_qt4
    DEPENDPATH += $$ZINT_PATH/backend $$ZINT_PATH/backend_qt4
    LIBS += -L$${DESTDIR} -lQtZint
}

#######
####Automatically build required translation files (*.qm)

contains(CONFIG,build_translations){
    LANGUAGES = ru es_ES

    defineReplace(prependAll) {
        for(a,$$1):result += $$2$${a}$$3
        return($$result)
    }

    TRANSLATIONS = $$prependAll(LANGUAGES, $$TRANSLATIONS_PATH/limereport_,.ts)

    qtPrepareTool(LUPDATE, lupdate)
    ts.commands = $$LUPDATE $$PWD -ts $$TRANSLATIONS

    TRANSLATIONS_FILES =
    qtPrepareTool(LRELEASE, lrelease)
    for(tsfile, TRANSLATIONS) {
        qmfile = $$tsfile
        qmfile ~= s,.ts$,.qm,
        qm.commands += $$LRELEASE -removeidentical $$tsfile -qm $$qmfile $$escape_expand(\\n\\t)
        tmp_command = $$LRELEASE -removeidentical $$tsfile -qm $$qmfile $$escape_expand(\\n\\t)
        TRANSLATIONS_FILES += $$qmfile
    }
    qm.depends = ts

    QMAKE_EXTRA_TARGETS += qm ts
    POST_TARGETDEPS +=  qm
}

#### EN AUTOMATIC TRANSLATIONS


