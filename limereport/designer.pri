include(../common.pri)

DEFINES += HAVE_REPORT_DESIGNER

CONFIG(dialogdesigner) {
    include($$REPORT_PATH/dialogdesigner/dialogdesigner.pri)
}

INCLUDEPATH += $$REPORT_PATH/objectinspector \
               $$REPORT_PATH/databrowser

SOURCES += \
    $$REPORT_PATH/databrowser/lrdatabrowser.cpp \
    $$REPORT_PATH/databrowser/lrsqleditdialog.cpp \
    $$REPORT_PATH/databrowser/lrconnectiondialog.cpp \
    $$REPORT_PATH/databrowser/lrvariabledialog.cpp \
    $$REPORT_PATH/databrowser/lrdatabrowsertree.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrstringpropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrrectproptem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrintpropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrenumpropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrboolpropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrflagspropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrfontpropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrimagepropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrqrealpropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrcolorpropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrdatasourcepropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrgroupfieldpropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrcontentpropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrmarginpropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lrseriespropitem.cpp \
    $$REPORT_PATH/objectinspector/propertyItems/lraxispropitem.cpp \
    $$REPORT_PATH/objectinspector/editors/lrtextitempropertyeditor.cpp \
    $$REPORT_PATH/objectinspector/editors/lrcomboboxeditor.cpp \
    $$REPORT_PATH/objectinspector/editors/lrcheckboxeditor.cpp \
    $$REPORT_PATH/objectinspector/editors/lrbuttonlineeditor.cpp \
    $$REPORT_PATH/objectinspector/editors/lrfonteditor.cpp \
    $$REPORT_PATH/objectinspector/editors/lrimageeditor.cpp \
    $$REPORT_PATH/objectinspector/editors/lrcoloreditor.cpp \
    $$REPORT_PATH/objectinspector/lrbasedesignobjectmodel.cpp \
    $$REPORT_PATH/objectinspector/lrobjectinspectorwidget.cpp \
    $$REPORT_PATH/objectinspector/lrobjectitemmodel.cpp \
    $$REPORT_PATH/objectinspector/lrobjectpropitem.cpp \
    $$REPORT_PATH/objectinspector/lrpropertydelegate.cpp \
    $$REPORT_PATH/objectsbrowser/lrobjectbrowser.cpp \
    $$REPORT_PATH/scriptbrowser/lrscriptbrowser.cpp \
    $$REPORT_PATH/items/lrsubitemparentpropitem.cpp \
    $$REPORT_PATH/items/lralignpropitem.cpp \
    $$REPORT_PATH/items/editors/lritemsaligneditorwidget.cpp \
    $$REPORT_PATH/translationeditor/translationeditor.cpp \
    $$REPORT_PATH/translationeditor/languageselectdialog.cpp \
    $$REPORT_PATH/lrreportdesignwidget.cpp \
    $$REPORT_PATH/lrreportdesignwindow.cpp

CONFIG(svg) {
    SOURCES += \
        $$REPORT_PATH/objectinspector/editors/lrsvgeditor.cpp \
        $$REPORT_PATH/objectinspector/propertyItems/lrsvgpropitem.cpp
}

HEADERS += \
    $$REPORT_PATH/databrowser/lrdatabrowser.h \
    $$REPORT_PATH/databrowser/lrsqleditdialog.h \
    $$REPORT_PATH/databrowser/lrconnectiondialog.h \
    $$REPORT_PATH/databrowser/lrvariabledialog.h \
    $$REPORT_PATH/databrowser/lrdatabrowsertree.h \
    $$REPORT_PATH/scriptbrowser/lrscriptbrowser.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrstringpropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrrectproptem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrdatasourcepropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrfontpropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrimagepropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrintpropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrenumpropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrboolpropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrflagspropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrgroupfieldpropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrcontentpropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrqrealpropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrcolorpropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrmarginpropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lrseriespropitem.h \
    $$REPORT_PATH/objectinspector/propertyItems/lraxispropitem.h \
    $$REPORT_PATH/objectinspector/editors/lrtextitempropertyeditor.h \
    $$REPORT_PATH/objectinspector/editors/lrcomboboxeditor.h \
    $$REPORT_PATH/objectinspector/editors/lrcheckboxeditor.h \
    $$REPORT_PATH/objectinspector/editors/lrbuttonlineeditor.h \
    $$REPORT_PATH/objectinspector/editors/lrimageeditor.h \
    $$REPORT_PATH/objectinspector/editors/lrcoloreditor.h \
    $$REPORT_PATH/objectinspector/editors/lrfonteditor.h \
    $$REPORT_PATH/objectinspector/lrbasedesignobjectmodel.h \
    $$REPORT_PATH/objectinspector/lrobjectinspectorwidget.h \
    $$REPORT_PATH/objectinspector/lrobjectitemmodel.h \
    $$REPORT_PATH/objectinspector/lrobjectpropitem.h \
    $$REPORT_PATH/objectinspector/lrpropertydelegate.h \
    $$REPORT_PATH/objectsbrowser/lrobjectbrowser.h \
    $$REPORT_PATH/translationeditor/translationeditor.h \    
    $$REPORT_PATH/translationeditor/languageselectdialog.h \
    $$REPORT_PATH/items/editors/lritemsaligneditorwidget.h \
    $$REPORT_PATH/items/lrsubitemparentpropitem.h \
    $$REPORT_PATH/items/lralignpropitem.h \
    $$REPORT_PATH/lrreportdesignwidget.h \
    $$REPORT_PATH/lrreportdesignwindow.h

contains(CONFIG, svg) {
    HEADERS += \
        $$REPORT_PATH/objectinspector/editors/lrsvgeditor.h \
        $$REPORT_PATH/objectinspector/propertyItems/lrsvgpropitem.h
}

FORMS += \
    $$REPORT_PATH/databrowser/lrsqleditdialog.ui \
    $$REPORT_PATH/databrowser/lrconnectiondialog.ui \
    $$REPORT_PATH/databrowser/lrdatabrowser.ui \
    $$REPORT_PATH/databrowser/lrvariabledialog.ui \
    $$REPORT_PATH/objectinspector/editors/ltextitempropertyeditor.ui \
    $$REPORT_PATH/scriptbrowser/lrscriptbrowser.ui \
    $$REPORT_PATH/translationeditor/translationeditor.ui \
    $$REPORT_PATH/translationeditor/languageselectdialog.ui

RESOURCES += \
    $$REPORT_PATH/objectinspector/lobjectinspector.qrc \
    $$REPORT_PATH/databrowser/lrdatabrowser.qrc \
    $$REPORT_PATH/scriptbrowser/lrscriptbrowser.qrc \
    $$REPORT_PATH/translationeditor/translationeditor.qrc
