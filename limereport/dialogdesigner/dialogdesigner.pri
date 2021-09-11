include(../../common.pri)
include($$PWD/3rdparty/qtcreator/designerintegrationv2/designerintegration.pri)

INCLUDEPATH *= $$PWD/3rdparty/designer

equals(QT_MAJOR_VERSION, 4) : CONFIG(uitools) {
    DEFINES += HAVE_QTDESIGNER_INTEGRATION
}

if(equals(QT_MAJOR_VERSION, 5) | equals(QT_MAJOR_VERSION, 6)) : contains(QT, uitools) {
    DEFINES += HAVE_QTDESIGNER_INTEGRATION
}

equals(QT_MAJOR_VERSION, 4) {
    CONFIG *= designer
    qtAddLibrary(QtDesignerComponents)
}

equals(QT_MAJOR_VERSION, 5) | equals(QT_MAJOR_VERSION, 6) {
    QT *= designer designercomponents-private
}

SOURCES += $$PWD/lrdialogdesigner.cpp
HEADERS += $$PWD/lrdialogdesigner.h

RESOURCES += \
           $$PWD/dialogdesigner.qrc
