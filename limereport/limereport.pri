include(../common.pri)

DEFINES += INSPECT_BASEDESIGN

INCLUDEPATH += \
    $$REPORT_PATH/ \
    $$REPORT_PATH/items \
    $$REPORT_PATH/bands \
    $$REPORT_PATH/base \
    $$REPORT_PATH/objectinspector \
    $$REPORT_PATH/databrowser

SOURCES += \
    $$REPORT_PATH/bands/lrpageheader.cpp \
    $$REPORT_PATH/bands/lrpagefooter.cpp \
    $$REPORT_PATH/bands/lrreportheader.cpp \
    $$REPORT_PATH/bands/lrreportfooter.cpp \
    $$REPORT_PATH/bands/lrdataband.cpp \
    $$REPORT_PATH/bands/lrgroupbands.cpp \
    $$REPORT_PATH/bands/lrsubdetailband.cpp \
    $$REPORT_PATH/bands/lrtearoffband.cpp \
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
    $$REPORT_PATH/databrowser/lrdatabrowser.cpp \
    $$REPORT_PATH/databrowser/lrsqleditdialog.cpp \
    $$REPORT_PATH/databrowser/lrconnectiondialog.cpp \
    $$REPORT_PATH/databrowser/lrvariabledialog.cpp \
    $$REPORT_PATH/databrowser/lrdatabrowsertree.cpp \
    $$REPORT_PATH/serializators/lrxmlqrectserializator.cpp \
    $$REPORT_PATH/serializators/lrxmlbasetypesserializators.cpp \
    $$REPORT_PATH/serializators/lrxmlreader.cpp \
    $$REPORT_PATH/serializators/lrxmlwriter.cpp \
    $$REPORT_PATH/items/lrsubitemparentpropitem.cpp \
    $$REPORT_PATH/items/lralignpropitem.cpp \
    $$REPORT_PATH/items/lrhorizontallayout.cpp \
    $$REPORT_PATH/items/editors/lritemeditorwidget.cpp \
    $$REPORT_PATH/items/editors/lrfonteditorwidget.cpp \
    $$REPORT_PATH/items/editors/lrtextalignmenteditorwidget.cpp \
    $$REPORT_PATH/items/editors/lritemsaligneditorwidget.cpp \
    $$REPORT_PATH/items/editors/lritemsborderseditorwidget.cpp \
    $$REPORT_PATH/items/lrsimpletagparser.cpp \
    $$REPORT_PATH/items/lrimageitem.cpp \
    $$REPORT_PATH/items/lrtextitemeditor.cpp \
    $$REPORT_PATH/items/lrshapeitem.cpp \
    $$REPORT_PATH/items/lrtextitem.cpp \
    $$REPORT_PATH/lrbanddesignintf.cpp \
    $$REPORT_PATH/lrpageitemdesignintf.cpp \
    $$REPORT_PATH/lrpagedesignintf.cpp \
    $$REPORT_PATH/lrbandsmanager.cpp \
    $$REPORT_PATH/lrglobal.cpp \
    $$REPORT_PATH/lritemdesignintf.cpp \
    $$REPORT_PATH/lrdatadesignintf.cpp \
    $$REPORT_PATH/lrreportdesignwidget.cpp \
    $$REPORT_PATH/lrbasedesignintf.cpp \
    $$REPORT_PATH/lrreportengine.cpp \
    $$REPORT_PATH/lrdatasourcemanager.cpp \
    $$REPORT_PATH/lrreportdesignwindow.cpp \
    $$REPORT_PATH/lrreportrender.cpp \
    $$REPORT_PATH/lrscriptenginemanager.cpp \
    $$REPORT_PATH/lrpreviewreportwindow.cpp \
    $$REPORT_PATH/lrpreviewreportwidget.cpp \
    $$REPORT_PATH/lrgraphicsviewzoom.cpp \
    $$REPORT_PATH/lrvariablesholder.cpp \
    $$REPORT_PATH/lrgroupfunctions.cpp \
    $$REPORT_PATH/lrsimplecrypt.cpp \    
    $$REPORT_PATH/lraboutdialog.cpp \
    $$REPORT_PATH/lrsettingdialog.cpp \
    $$REPORT_PATH/scriptbrowser/lrscriptbrowser.cpp \
    $$REPORT_PATH/lritemscontainerdesignitf.cpp
    
contains(CONFIG, zint){
    SOURCES += $$REPORT_PATH/items/lrbarcodeitem.cpp
}

HEADERS += \
    $$REPORT_PATH/base/lrsingleton.h \
    $$REPORT_PATH/base/lrsimpleabstractfactory.h \
    $$REPORT_PATH/base/lrattribsabstractfactory.h \
    $$REPORT_PATH/bands/lrpageheader.h \
    $$REPORT_PATH/bands/lrpagefooter.h \
    $$REPORT_PATH/bands/lrreportheader.h \
    $$REPORT_PATH/bands/lrreportfooter.h \
    $$REPORT_PATH/bands/lrdataband.h \
    $$REPORT_PATH/bands/lrtearoffband.h \
    $$REPORT_PATH/bands/lrsubdetailband.h \
    $$REPORT_PATH/bands/lrgroupbands.h \
    $$REPORT_PATH/databrowser/lrdatabrowser.h \
    $$REPORT_PATH/databrowser/lrsqleditdialog.h \
    $$REPORT_PATH/databrowser/lrconnectiondialog.h \
    $$REPORT_PATH/databrowser/lrvariabledialog.h \
    $$REPORT_PATH/databrowser/lrdatabrowsertree.h \
    $$REPORT_PATH/serializators/lrserializatorintf.h \
    $$REPORT_PATH/serializators/lrstorageintf.h \
    $$REPORT_PATH/serializators/lrxmlqrectserializator.h \
    $$REPORT_PATH/serializators/lrxmlserializatorsfactory.h \
    $$REPORT_PATH/serializators/lrxmlbasetypesserializators.h \
    $$REPORT_PATH/serializators/lrxmlreader.h \
    $$REPORT_PATH/serializators/lrxmlwriter.h \
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
    $$REPORT_PATH/items/editors/lritemeditorwidget.h \
    $$REPORT_PATH/items/editors/lrfonteditorwidget.h \
    $$REPORT_PATH/items/editors/lrtextalignmenteditorwidget.h \
    $$REPORT_PATH/items/editors/lritemsaligneditorwidget.h \
    $$REPORT_PATH/items/editors/lritemsborderseditorwidget.h \
    $$REPORT_PATH/items/lrtextitem.h \
    $$REPORT_PATH/items/lrsubitemparentpropitem.h \
    $$REPORT_PATH/items/lralignpropitem.h \
    $$REPORT_PATH/items/lrhorizontallayout.h \
    $$REPORT_PATH/items/lrtextitemeditor.h \
    $$REPORT_PATH/items/lrshapeitem.h \
    $$REPORT_PATH/items/lrimageitem.h \
    $$REPORT_PATH/items/lrsimpletagparser.h \
    $$REPORT_PATH/lrbanddesignintf.h \
    $$REPORT_PATH/lrpageitemdesignintf.h \
    $$REPORT_PATH/lrbandsmanager.h \
    $$REPORT_PATH/lrglobal.h \
    $$REPORT_PATH/lrdatadesignintf.h \
    $$REPORT_PATH/lrcollection.h \
    $$REPORT_PATH/lrpagedesignintf.h \
    $$REPORT_PATH/lrreportdesignwidget.h \
    $$REPORT_PATH/lrreportengine_p.h \
    $$REPORT_PATH/lrdatasourcemanager.h \
    $$REPORT_PATH/lrreportdesignwindow.h \
    $$REPORT_PATH/lrreportrender.h \
    $$REPORT_PATH/lrpreviewreportwindow.h \
    $$REPORT_PATH/lrpreviewreportwidget.h \
    $$REPORT_PATH/lrpreviewreportwidget_p.h \
    $$REPORT_PATH/lrgraphicsviewzoom.h \
    $$REPORT_PATH/lrbasedesignintf.h \
    $$REPORT_PATH/lritemdesignintf.h \
    $$REPORT_PATH/lrdesignelementsfactory.h \
    $$REPORT_PATH/lrscriptenginemanager.h \
    $$REPORT_PATH/lrvariablesholder.h \
    $$REPORT_PATH/lrgroupfunctions.h \
    $$REPORT_PATH/lrreportengine.h \
    $$REPORT_PATH/lrdatasourcemanagerintf.h \
    $$REPORT_PATH/lrscriptenginemanagerintf.h \
    $$REPORT_PATH/lrsimplecrypt.h \    
    $$REPORT_PATH/lraboutdialog.h \
    $$REPORT_PATH/lrcallbackdatasourceintf.h \
    $$REPORT_PATH/lrsettingdialog.h \
    $$REPORT_PATH/lrpreviewreportwidget_p.h \
    $$REPORT_PATH/scriptbrowser/lrscriptbrowser.h \ 
    $$REPORT_PATH/lritemscontainerdesignitf.h
    
contains(CONFIG,zint){
    HEADERS += $$REPORT_PATH/items/lrbarcodeitem.h
}

FORMS += \
    $$REPORT_PATH/databrowser/lrsqleditdialog.ui \
    $$REPORT_PATH/databrowser/lrconnectiondialog.ui \
    $$REPORT_PATH/databrowser/lrdatabrowser.ui \
    $$REPORT_PATH/databrowser/lrvariabledialog.ui \
    $$REPORT_PATH/objectinspector/editors/ltextitempropertyeditor.ui \
    $$REPORT_PATH/lrpreviewreportwindow.ui \
    $$REPORT_PATH/lrpreviewreportwidget.ui \
    $$REPORT_PATH/items/lrtextitemeditor.ui \
    $$REPORT_PATH/lraboutdialog.ui \
    $$REPORT_PATH/lrsettingdialog.ui \
    $$REPORT_PATH/scriptbrowser/lrscriptbrowser.ui \



RESOURCES += \
    $$REPORT_PATH/objectinspector/lobjectinspector.qrc \
    $$REPORT_PATH/databrowser/lrdatabrowser.qrc \
    $$REPORT_PATH/report.qrc \
    $$REPORT_PATH/items/items.qrc \
    $$REPORT_PATH/scriptbrowser/lrscriptbrowser.qrc

