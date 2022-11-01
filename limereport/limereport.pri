include(../common.pri)

contains(CONFIG, embedded_designer) {
    include(designer.pri)
    message(embedded designer)
}

DEFINES += INSPECT_BASEDESIGN

INCLUDEPATH += \
    $$REPORT_PATH/ \
    $$REPORT_PATH/items \
    $$REPORT_PATH/bands \
    $$REPORT_PATH/base \
    $$REPORT_PATH/scripteditor

SOURCES += \
    $$PWD/scripteditor/lrcompletermodel.cpp \
    $$REPORT_PATH/bands/lrpageheader.cpp \
    $$REPORT_PATH/bands/lrpagefooter.cpp \
    $$REPORT_PATH/bands/lrreportheader.cpp \
    $$REPORT_PATH/bands/lrreportfooter.cpp \
    $$REPORT_PATH/bands/lrdataband.cpp \
    $$REPORT_PATH/bands/lrgroupbands.cpp \
    $$REPORT_PATH/bands/lrsubdetailband.cpp \
    $$REPORT_PATH/bands/lrtearoffband.cpp \
    $$REPORT_PATH/serializators/lrxmlqrectserializator.cpp \
    $$REPORT_PATH/serializators/lrxmlbasetypesserializators.cpp \
    $$REPORT_PATH/serializators/lrxmlreader.cpp \
    $$REPORT_PATH/serializators/lrxmlwriter.cpp \
    $$REPORT_PATH/scripteditor/lrscripteditor.cpp \
    $$REPORT_PATH/scripteditor/lrcodeeditor.cpp \
    $$REPORT_PATH/scripteditor/lrscripthighlighter.cpp \
    $$REPORT_PATH/items/lrhorizontallayout.cpp \
    $$REPORT_PATH/items/editors/lritemeditorwidget.cpp \
    $$REPORT_PATH/items/editors/lrfonteditorwidget.cpp \
    $$REPORT_PATH/items/editors/lrtextalignmenteditorwidget.cpp \
    $$REPORT_PATH/items/editors/lritemsborderseditorwidget.cpp \
#    $$REPORT_PATH/items/lrsimpletagparser.cpp \
    $$REPORT_PATH/items/lrimageitem.cpp \
    $$REPORT_PATH/items/lrimageitemeditor.cpp \
    $$REPORT_PATH/items/lrtextitemeditor.cpp \
    $$REPORT_PATH/items/lrshapeitem.cpp \
    $$REPORT_PATH/items/lrtextitem.cpp \
    $$REPORT_PATH/items/lrverticallayout.cpp \
    $$REPORT_PATH/items/lrlayoutmarker.cpp \
    $$REPORT_PATH/items/lrabstractlayout.cpp \
    $$REPORT_PATH/items/lrchartitem.cpp \
    $$REPORT_PATH/items/lrchartitemeditor.cpp \
    $$REPORT_PATH/items/lrchartaxiseditor.cpp \
    $$REPORT_PATH/items/charts/lrhorizontalbarchart.cpp \
    $$REPORT_PATH/items/charts/lrlineschart.cpp \
    $$REPORT_PATH/items/charts/lrgridlineschart.cpp \
    $$REPORT_PATH/items/charts/lrpiechart.cpp \
    $$REPORT_PATH/items/charts/lrverticalbarchart.cpp \
    $$REPORT_PATH/lrbanddesignintf.cpp \
    $$REPORT_PATH/lrpageitemdesignintf.cpp \
    $$REPORT_PATH/lrpagedesignintf.cpp \
    $$REPORT_PATH/lrbandsmanager.cpp \
    $$REPORT_PATH/lrglobal.cpp \
    $$REPORT_PATH/lritemdesignintf.cpp \
    $$REPORT_PATH/lrdatadesignintf.cpp \
    $$REPORT_PATH/lrbasedesignintf.cpp \
    $$REPORT_PATH/lrreportengine.cpp \
    $$REPORT_PATH/lrdatasourcemanager.cpp \
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
    $$REPORT_PATH/lritemscontainerdesignitf.cpp \
    $$REPORT_PATH/lrcolorindicator.cpp \
    $$REPORT_PATH/lrreporttranslation.cpp \
    $$REPORT_PATH/exporters/lrpdfexporter.cpp \
    $$REPORT_PATH/lraxisdata.cpp \
    $$REPORT_PATH/lrpreparedpages.cpp \
    $$REPORT_PATH/items/lrpageeditor.cpp \
    $$REPORT_PATH/items/lrborderframeeditor.cpp \
    $$REPORT_PATH/items/lrbordereditor.cpp

CONFIG(staticlib) {
    SOURCES += $$REPORT_PATH/lrfactoryinitializer.cpp
}

CONFIG(zint) {
    SOURCES += $$REPORT_PATH/items/lrbarcodeitem.cpp
}

CONFIG(svg) {
    SOURCES += $$REPORT_PATH/items/lrsvgitem.cpp \
}

HEADERS += \
    $$PWD/items/lreditableimageitemintf.h \
    $$PWD/scripteditor/lrcompletermodel.h \
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
    $$REPORT_PATH/serializators/lrserializatorintf.h \
    $$REPORT_PATH/serializators/lrstorageintf.h \
    $$REPORT_PATH/serializators/lrxmlqrectserializator.h \
    $$REPORT_PATH/serializators/lrxmlserializatorsfactory.h \
    $$REPORT_PATH/serializators/lrxmlbasetypesserializators.h \
    $$REPORT_PATH/serializators/lrxmlreader.h \
    $$REPORT_PATH/serializators/lrxmlwriter.h \
    $$REPORT_PATH/scripteditor/lrscripteditor.h \
    $$REPORT_PATH/scripteditor/lrcodeeditor.h \
    $$REPORT_PATH/scripteditor/lrscripthighlighter.h \
    $$REPORT_PATH/items/editors/lritemeditorwidget.h \
    $$REPORT_PATH/items/editors/lrfonteditorwidget.h \
    $$REPORT_PATH/items/editors/lrtextalignmenteditorwidget.h \
    $$REPORT_PATH/items/editors/lritemsborderseditorwidget.h \
    $$REPORT_PATH/items/lrtextitem.h \
    $$REPORT_PATH/items/lrhorizontallayout.h \
    $$REPORT_PATH/items/lrtextitemeditor.h \
    $$REPORT_PATH/items/lrshapeitem.h \
    $$REPORT_PATH/items/lrimageitem.h \
    $$REPORT_PATH/items/lrimageitemeditor.h \
#    $$REPORT_PATH/items/lrsimpletagparser.h \
    $$REPORT_PATH/items/lrverticallayout.h \
    $$REPORT_PATH/items/lrlayoutmarker.h \
    $$REPORT_PATH/items/lrabstractlayout.h \
    $$REPORT_PATH/items/lrchartitem.h \
    $$REPORT_PATH/items/lrchartitemeditor.h \
    $$REPORT_PATH/items/lrchartaxiseditor.h \
    $$REPORT_PATH/items/charts/lrhorizontalbarchart.h \
    $$REPORT_PATH/items/charts/lrlineschart.h \
    $$REPORT_PATH/items/charts/lrgridlineschart.h \
    $$REPORT_PATH/items/charts/lrpiechart.h \
    $$REPORT_PATH/items/charts/lrverticalbarchart.h \
    $$REPORT_PATH/lrbanddesignintf.h \
    $$REPORT_PATH/lrpageitemdesignintf.h \
    $$REPORT_PATH/lrbandsmanager.h \
    $$REPORT_PATH/lrglobal.h \
    $$REPORT_PATH/lrdatadesignintf.h \
    $$REPORT_PATH/lrcollection.h \
    $$REPORT_PATH/lrpagedesignintf.h \
    $$REPORT_PATH/lrreportengine_p.h \
    $$REPORT_PATH/lrdatasourcemanager.h \
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
    $$REPORT_PATH/lrdatasourceintf.h \
    $$REPORT_PATH/lrdatasourcemanagerintf.h \
    $$REPORT_PATH/lrscriptenginemanagerintf.h \
    $$REPORT_PATH/lrsimplecrypt.h \
    $$REPORT_PATH/lraboutdialog.h \
    $$REPORT_PATH/lrcallbackdatasourceintf.h \
    $$REPORT_PATH/lrsettingdialog.h \
    $$REPORT_PATH/lrpreviewreportwidget_p.h \
    $$REPORT_PATH/lritemscontainerdesignitf.h \
    $$REPORT_PATH/lrcolorindicator.h \
    $$REPORT_PATH/lrreporttranslation.h \
    $$REPORT_PATH/lrreportdesignwindowintrerface.h \
    $$REPORT_PATH/lrexporterintf.h \
    $$REPORT_PATH/lrexportersfactory.h \
    $$REPORT_PATH/exporters/lrpdfexporter.h \
    $$REPORT_PATH/lrpreparedpages.h \
    $$REPORT_PATH/lraxisdata.h \
    $$REPORT_PATH/lrpreparedpagesintf.h \
    $$REPORT_PATH/items/lrpageeditor.h \
    $$REPORT_PATH/items/lrborderframeeditor.h \
    $$REPORT_PATH/items/lrbordereditor.h

CONFIG(staticlib) {
    HEADERS += $$REPORT_PATH/lrfactoryinitializer.h
}

CONFIG(zint) {
    HEADERS += $$REPORT_PATH/items/lrbarcodeitem.h
}

CONFIG(svg) {
    HEADERS += $$REPORT_PATH/items/lrsvgitem.h
}

FORMS += \
    $$REPORT_PATH/lrpreviewreportwindow.ui \
    $$REPORT_PATH/lrpreviewreportwidget.ui \
    $$REPORT_PATH/items/lrtextitemeditor.ui \
    $$REPORT_PATH/lraboutdialog.ui \
    $$REPORT_PATH/lrsettingdialog.ui \
    $$REPORT_PATH/items/lrchartitemeditor.ui \
    $$REPORT_PATH/items/lrchartaxiseditor.ui \
    $$REPORT_PATH/items/lrimageitemeditor.ui \
    $$REPORT_PATH/scripteditor/lrscripteditor.ui \
    $$REPORT_PATH/items/lrpageeditor.ui \
    $$REPORT_PATH/items/lrborderframeeditor.ui \
    $$REPORT_PATH/items/lrbordereditor.ui

RESOURCES += \
    $$REPORT_PATH/report.qrc \
    $$REPORT_PATH/items/items.qrc
