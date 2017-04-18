include(../../common.pri)
include($$PWD/3rdparty/qtcreator/designerintegrationv2/designerintegration.pri)
INCLUDEPATH *= $$PWD/3rdparty/designer
greaterThan(QT_MAJOR_VERSION, 4) {
    contains(QT,uitools){
        DEFINES += HAVE_QTDESIGNER_INTEGRATION
    }
}
lessThan(QT_MAJOR_VERSION, 5){
    contains(CONFIG,uitools){
        DEFINES += HAVE_QTDESIGNER_INTEGRATION
    }
}
QT += designer designercomponents-private

SOURCES += $$PWD/lrdialogdesigner.cpp
HEADERS += $$PWD/lrdialogdesigner.h

RESOURCES += \
    $$PWD/dialogdesigner.qrc
