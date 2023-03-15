cmake_minimum_required(VERSION 3.14)
project(limereport)

set(LIMEREPORT_VERSION_MAJOR 1)
set(LIMEREPORT_VERSION_MINOR 6)
set(LIMEREPORT_VERSION_RELEASE 8)

option(ENABLE_ZINT "Enable libzint build for barcode support" OFF)
option(LIMEREPORT_STATIC "Build LimeReport as static library" OFF)
option(LIMEREPORT_DEMO "Build LimeReport demos" OFF)
option(USE_QT6 "Use Qt6" OFF)

if(USE_QT6)
    find_package(QT NAMES Qt6)
else()
    find_package(QT NAMES Qt5)
endif(USE_QT6)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets Sql Network Xml Svg Qml PrintSupport REQUIRED
  )

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS UiTools
  )
# Old Qt does not provide QT_VERSION_MAJOR
if (NOT QT_VERSION_MAJOR)
    string(SUBSTRING ${QT_VERSION} 0 1 QT_VERSION_MAJOR)
endif()

message(STATUS "Qt version: ${QT_VERSION_MAJOR}")
if (Qt${QT_VERSION_MAJOR}Widgets_FOUND)
    message(STATUS "QtGui found")
endif()
if (Qt${QT_VERSION_MAJOR}Widgets_FOUND)
    message(STATUS "QtWidgets found")
endif()

add_subdirectory(3rdparty)

if (LIMEREPORT_DEMO)
    add_subdirectory(designer)
    add_subdirectory(demo_r1)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)


set ( LIMEREPORT_SOURCES
${PROJECT_NAME}/bands/lrdataband.cpp
${PROJECT_NAME}/bands/lrgroupbands.cpp
${PROJECT_NAME}/bands/lrpagefooter.cpp
${PROJECT_NAME}/bands/lrpageheader.cpp
${PROJECT_NAME}/bands/lrreportfooter.cpp
${PROJECT_NAME}/bands/lrreportheader.cpp
${PROJECT_NAME}/bands/lrsubdetailband.cpp
${PROJECT_NAME}/bands/lrtearoffband.cpp
${PROJECT_NAME}/databrowser/lrconnectiondialog.cpp
${PROJECT_NAME}/databrowser/lrdatabrowser.cpp
${PROJECT_NAME}/databrowser/lrdatabrowsertree.cpp
${PROJECT_NAME}/databrowser/lrsqleditdialog.cpp
${PROJECT_NAME}/databrowser/lrvariabledialog.cpp
${PROJECT_NAME}/exporters/lrpdfexporter.cpp
${PROJECT_NAME}/items/charts/lrhorizontalbarchart.cpp
${PROJECT_NAME}/items/charts/lrlineschart.cpp
${PROJECT_NAME}/items/charts/lrgridlineschart.cpp
${PROJECT_NAME}/items/charts/lrpiechart.cpp
${PROJECT_NAME}/items/charts/lrverticalbarchart.cpp
${PROJECT_NAME}/items/editors/lrfonteditorwidget.cpp
${PROJECT_NAME}/items/editors/lritemeditorwidget.cpp
${PROJECT_NAME}/items/editors/lritemsaligneditorwidget.cpp
${PROJECT_NAME}/items/editors/lritemsborderseditorwidget.cpp
${PROJECT_NAME}/items/editors/lrtextalignmenteditorwidget.cpp
${PROJECT_NAME}/items/lrabstractlayout.cpp
${PROJECT_NAME}/items/lralignpropitem.cpp
${PROJECT_NAME}/items/lrborderframeeditor.cpp
${PROJECT_NAME}/items/lrbordereditor.cpp
${PROJECT_NAME}/items/lrchartitem.cpp
${PROJECT_NAME}/items/lrchartaxiseditor.cpp
${PROJECT_NAME}/items/lrchartitemeditor.cpp
${PROJECT_NAME}/items/lrhorizontallayout.cpp
${PROJECT_NAME}/items/lrimageitem.cpp
${PROJECT_NAME}/items/lrimageitemeditor.cpp
${PROJECT_NAME}/items/lrlayoutmarker.cpp
${PROJECT_NAME}/items/lrpageeditor.cpp
${PROJECT_NAME}/items/lrshapeitem.cpp
${PROJECT_NAME}/items/lrsubitemparentpropitem.cpp
${PROJECT_NAME}/items/lrsvgitem.cpp
${PROJECT_NAME}/items/lrtextitem.cpp
${PROJECT_NAME}/items/lrtextitemeditor.cpp
${PROJECT_NAME}/items/lrverticallayout.cpp
${PROJECT_NAME}/lraboutdialog.cpp
${PROJECT_NAME}/lraxisdata.cpp
${PROJECT_NAME}/lrbanddesignintf.cpp
${PROJECT_NAME}/lrbandsmanager.cpp
${PROJECT_NAME}/lrbasedesignintf.cpp
${PROJECT_NAME}/lrcolorindicator.cpp
${PROJECT_NAME}/lrdatadesignintf.cpp
${PROJECT_NAME}/lrdatasourcemanager.cpp
${PROJECT_NAME}/lrglobal.cpp
${PROJECT_NAME}/lrgraphicsviewzoom.cpp
${PROJECT_NAME}/lrgroupfunctions.cpp
${PROJECT_NAME}/lritemdesignintf.cpp
${PROJECT_NAME}/lritemscontainerdesignitf.cpp
${PROJECT_NAME}/lrpagedesignintf.cpp
${PROJECT_NAME}/lrpageitemdesignintf.cpp
${PROJECT_NAME}/lrpreparedpages.cpp
${PROJECT_NAME}/lrpreviewreportwidget.cpp
${PROJECT_NAME}/lrpreviewreportwindow.cpp
${PROJECT_NAME}/lrreportdesignwidget.cpp
${PROJECT_NAME}/lrreportdesignwindow.cpp
${PROJECT_NAME}/lrreportengine.cpp
${PROJECT_NAME}/lrreportrender.cpp
${PROJECT_NAME}/lrreporttranslation.cpp
${PROJECT_NAME}/lrscriptenginemanager.cpp
${PROJECT_NAME}/lrsettingdialog.cpp
${PROJECT_NAME}/lrsimplecrypt.cpp
${PROJECT_NAME}/lrvariablesholder.cpp
${PROJECT_NAME}/objectinspector/editors/lrbuttonlineeditor.cpp
${PROJECT_NAME}/objectinspector/editors/lrcheckboxeditor.cpp
${PROJECT_NAME}/objectinspector/editors/lrcoloreditor.cpp
${PROJECT_NAME}/objectinspector/editors/lrcomboboxeditor.cpp
${PROJECT_NAME}/objectinspector/editors/lrfonteditor.cpp
${PROJECT_NAME}/objectinspector/editors/lrimageeditor.cpp
${PROJECT_NAME}/objectinspector/editors/lrsvgeditor.cpp
${PROJECT_NAME}/objectinspector/editors/lrtextitempropertyeditor.cpp
${PROJECT_NAME}/objectinspector/lrbasedesignobjectmodel.cpp
${PROJECT_NAME}/objectinspector/lrobjectinspectorwidget.cpp
${PROJECT_NAME}/objectinspector/lrobjectitemmodel.cpp
${PROJECT_NAME}/objectinspector/lrobjectpropitem.cpp
${PROJECT_NAME}/objectinspector/lrpropertydelegate.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrboolpropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrcolorpropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrcontentpropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrdatasourcepropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrenumpropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrflagspropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrfontpropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrgroupfieldpropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrimagepropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrintpropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrmarginpropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrqrealpropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrrectproptem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrseriespropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrstringpropitem.cpp
${PROJECT_NAME}/objectinspector/propertyItems/lrsvgpropitem.cpp
${PROJECT_NAME}/objectsbrowser/lrobjectbrowser.cpp
${PROJECT_NAME}/scriptbrowser/lrscriptbrowser.cpp
${PROJECT_NAME}/scripteditor/lrcodeeditor.cpp
${PROJECT_NAME}/scripteditor/lrscripteditor.cpp
${PROJECT_NAME}/scripteditor/lrscripthighlighter.cpp
${PROJECT_NAME}/scripteditor/lrcompletermodel.cpp
${PROJECT_NAME}/serializators/lrxmlbasetypesserializators.cpp
${PROJECT_NAME}/serializators/lrxmlqrectserializator.cpp
${PROJECT_NAME}/serializators/lrxmlreader.cpp
${PROJECT_NAME}/serializators/lrxmlwriter.cpp
${PROJECT_NAME}/translationeditor/languageselectdialog.cpp
${PROJECT_NAME}/translationeditor/translationeditor.cpp


${PROJECT_NAME}/bands/lrdataband.h
${PROJECT_NAME}/bands/lrgroupbands.h
${PROJECT_NAME}/bands/lrpagefooter.h
${PROJECT_NAME}/bands/lrpageheader.h
${PROJECT_NAME}/bands/lrreportfooter.h
${PROJECT_NAME}/bands/lrreportheader.h
${PROJECT_NAME}/bands/lrsubdetailband.h
${PROJECT_NAME}/bands/lrtearoffband.h
${PROJECT_NAME}/base/lrattribsabstractfactory.h
${PROJECT_NAME}/base/lrsimpleabstractfactory.h
${PROJECT_NAME}/base/lrsingleton.h
${PROJECT_NAME}/databrowser/lrconnectiondialog.h
${PROJECT_NAME}/databrowser/lrdatabrowser.h
${PROJECT_NAME}/databrowser/lrdatabrowsertree.h
${PROJECT_NAME}/databrowser/lrsqleditdialog.h
${PROJECT_NAME}/databrowser/lrvariabledialog.h
${PROJECT_NAME}/exporters/lrpdfexporter.h
${PROJECT_NAME}/items/charts/lrhorizontalbarchart.h
${PROJECT_NAME}/items/charts/lrlineschart.h
${PROJECT_NAME}/items/charts/lrgridlineschart.h
${PROJECT_NAME}/items/charts/lrpiechart.h
${PROJECT_NAME}/items/charts/lrverticalbarchart.h
${PROJECT_NAME}/items/editors/lrfonteditorwidget.h
${PROJECT_NAME}/items/editors/lritemeditorwidget.h
${PROJECT_NAME}/items/editors/lritemsaligneditorwidget.h
${PROJECT_NAME}/items/editors/lritemsborderseditorwidget.h
${PROJECT_NAME}/items/editors/lrtextalignmenteditorwidget.h
${PROJECT_NAME}/items/lrabstractlayout.h
${PROJECT_NAME}/items/lralignpropitem.h
${PROJECT_NAME}/items/lrborderframeeditor.h
${PROJECT_NAME}/items/lrbordereditor.h
${PROJECT_NAME}/items/lrchartitem.h
${PROJECT_NAME}/items/lrchartaxiseditor.h
${PROJECT_NAME}/items/lrchartitemeditor.h
${PROJECT_NAME}/items/lreditableimageitemintf.h
${PROJECT_NAME}/items/lrhorizontallayout.h
${PROJECT_NAME}/items/lrimageitem.h
${PROJECT_NAME}/items/lrimageitemeditor.h
${PROJECT_NAME}/items/lrlayoutmarker.h
${PROJECT_NAME}/items/lrpageeditor.h
${PROJECT_NAME}/items/lrshapeitem.h
${PROJECT_NAME}/items/lrsubitemparentpropitem.h
${PROJECT_NAME}/items/lrsvgitem.h
${PROJECT_NAME}/items/lrtextitem.h
${PROJECT_NAME}/items/lrtextitemeditor.h
${PROJECT_NAME}/items/lrverticallayout.h
${PROJECT_NAME}/lraboutdialog.h
${PROJECT_NAME}/lraxisdata.h
${PROJECT_NAME}/lrbanddesignintf.h
${PROJECT_NAME}/lrbandsmanager.h
${PROJECT_NAME}/lrbasedesignintf.h
${PROJECT_NAME}/lrcollection.h
${PROJECT_NAME}/lrcolorindicator.h
${PROJECT_NAME}/lrdatadesignintf.h
${PROJECT_NAME}/lrdatasourcemanager.h
${PROJECT_NAME}/lrdesignelementsfactory.h
${PROJECT_NAME}/lrdesignerplugininterface.h
${PROJECT_NAME}/lrexporterintf.h
${PROJECT_NAME}/lrexportersfactory.h
${PROJECT_NAME}/lrgraphicsviewzoom.h
${PROJECT_NAME}/lrgroupfunctions.h
${PROJECT_NAME}/lritemdesignintf.h
${PROJECT_NAME}/lritemscontainerdesignitf.h
${PROJECT_NAME}/lrpagedesignintf.h
${PROJECT_NAME}/lrpageinitintf.h
${PROJECT_NAME}/lrpageitemdesignintf.h
${PROJECT_NAME}/lrpreparedpages.h
${PROJECT_NAME}/lrpreviewreportwidget_p.h
${PROJECT_NAME}/lrpreviewreportwindow.h
${PROJECT_NAME}/lrreportdesignwidget.h
${PROJECT_NAME}/lrreportdesignwindow.h
${PROJECT_NAME}/lrreportengine_p.h
${PROJECT_NAME}/lrreportrender.h
${PROJECT_NAME}/lrreporttranslation.h
${PROJECT_NAME}/lrscriptenginemanager.h
${PROJECT_NAME}/lrsettingdialog.h
${PROJECT_NAME}/lrsimplecrypt.h
${PROJECT_NAME}/lrvariablesholder.h
${PROJECT_NAME}/objectinspector/editors/lrbuttonlineeditor.h
${PROJECT_NAME}/objectinspector/editors/lrcheckboxeditor.h
${PROJECT_NAME}/objectinspector/editors/lrcoloreditor.h
${PROJECT_NAME}/objectinspector/editors/lrcomboboxeditor.h
${PROJECT_NAME}/objectinspector/editors/lrfonteditor.h
${PROJECT_NAME}/objectinspector/editors/lrimageeditor.h
${PROJECT_NAME}/objectinspector/editors/lrsvgeditor.h
${PROJECT_NAME}/objectinspector/editors/lrtextitempropertyeditor.h
${PROJECT_NAME}/objectinspector/lrbasedesignobjectmodel.h
${PROJECT_NAME}/objectinspector/lrobjectinspectorwidget.h
${PROJECT_NAME}/objectinspector/lrobjectitemmodel.h
${PROJECT_NAME}/objectinspector/lrobjectpropitem.h
${PROJECT_NAME}/objectinspector/lrpropertydelegate.h
${PROJECT_NAME}/objectinspector/propertyItems/lrboolpropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrcolorpropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrcontentpropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrdatasourcepropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrenumpropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrflagspropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrfontpropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrgroupfieldpropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrimagepropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrintpropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrmarginpropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrqrealpropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrrectproptem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrseriespropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrstringpropitem.h
${PROJECT_NAME}/objectinspector/propertyItems/lrsvgpropitem.h
${PROJECT_NAME}/objectsbrowser/lrobjectbrowser.h
${PROJECT_NAME}/scriptbrowser/lrscriptbrowser.h
${PROJECT_NAME}/scripteditor/lrcodeeditor.h
${PROJECT_NAME}/scripteditor/lrscripteditor.h
${PROJECT_NAME}/scripteditor/lrscripthighlighter.h
${PROJECT_NAME}/scripteditor/lrcompletermodel.h
${PROJECT_NAME}/serializators/lrserializatorintf.h
${PROJECT_NAME}/serializators/lrstorageintf.h
${PROJECT_NAME}/serializators/lrxmlbasetypesserializators.h
${PROJECT_NAME}/serializators/lrxmlqrectserializator.h
${PROJECT_NAME}/serializators/lrxmlreader.h
${PROJECT_NAME}/serializators/lrxmlserializatorsfactory.h
${PROJECT_NAME}/serializators/lrxmlwriter.h
${PROJECT_NAME}/translationeditor/languageselectdialog.h
${PROJECT_NAME}/translationeditor/translationeditor.h


${PROJECT_NAME}/databrowser/lrconnectiondialog.ui
${PROJECT_NAME}/databrowser/lrdatabrowser.ui
${PROJECT_NAME}/databrowser/lrsqleditdialog.ui
${PROJECT_NAME}/databrowser/lrvariabledialog.ui
${PROJECT_NAME}/dialogdesigner/templates/Dialog.ui
${PROJECT_NAME}/items/lrborderframeeditor.ui
${PROJECT_NAME}/items/lrbordereditor.ui
${PROJECT_NAME}/items/lrchartitemeditor.ui
${PROJECT_NAME}/items/lrchartaxiseditor.ui
${PROJECT_NAME}/items/lrimageitemeditor.ui
${PROJECT_NAME}/items/lrpageeditor.ui
${PROJECT_NAME}/items/lrtextitemeditor.ui
${PROJECT_NAME}/lraboutdialog.ui
${PROJECT_NAME}/lrpreviewreportwidget.ui
${PROJECT_NAME}/lrpreviewreportwindow.ui
${PROJECT_NAME}/lrsettingdialog.ui
${PROJECT_NAME}/objectinspector/editors/ltextitempropertyeditor.ui
${PROJECT_NAME}/scriptbrowser/lrscriptbrowser.ui
${PROJECT_NAME}/scripteditor/lrscripteditor.ui
${PROJECT_NAME}/translationeditor/languageselectdialog.ui
${PROJECT_NAME}/translationeditor/translationeditor.ui


${PROJECT_NAME}/databrowser/lrdatabrowser.qrc
${PROJECT_NAME}/dialogdesigner/dialogdesigner.qrc
${PROJECT_NAME}/items/items.qrc
${PROJECT_NAME}/objectinspector/lobjectinspector.qrc
${PROJECT_NAME}/report.qrc
${PROJECT_NAME}/scriptbrowser/lrscriptbrowser.qrc
${PROJECT_NAME}/translationeditor/translationeditor.qrc
)

if (ENABLE_ZINT)
    list(APPEND LIMEREPORT_SOURCES ${PROJECT_NAME}/items/lrbarcodeitem.cpp)
    list(APPEND LIMEREPORT_SOURCES ${PROJECT_NAME}/items/lrbarcodeitem.h)
endif(ENABLE_ZINT)

if (LIMEREPORT_STATIC)
    list(APPEND LIMEREPORT_SOURCES ${PROJECT_NAME}/lrfactoryinitializer.cpp)
    list(APPEND LIMEREPORT_SOURCES ${PROJECT_NAME}/lrfactoryinitializer.h)
endif(LIMEREPORT_STATIC)


set(EXTRA_FILES
   ${PROJECT_NAME}/lrglobal.h
   ${PROJECT_NAME}/lrdatasourcemanagerintf.h
   ${PROJECT_NAME}/lrdatasourceintf.h
   ${PROJECT_NAME}/lrreportengine.h
   ${PROJECT_NAME}/lrscriptenginemanagerintf.h
   ${PROJECT_NAME}/lrcallbackdatasourceintf.h
   ${PROJECT_NAME}/lrpreviewreportwidget.h
   ${PROJECT_NAME}/lrreportdesignwindowintrerface.h
   ${PROJECT_NAME}/lrpreparedpagesintf.h
)

configure_file(config.h.in config.h @ONLY)

set(GLOBAL_HEADERS
    ${PROJECT_NAME}/LimeReport
    ${PROJECT_NAME}/LRCallbackDS
    ${PROJECT_NAME}/LRDataManager
    ${PROJECT_NAME}/LRScriptManager
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    )

set(PROJECT_NAME ${PROJECT_NAME}-qt${QT_VERSION_MAJOR})

if (LIMEREPORT_STATIC)
    message(STATUS "STATIC LIBRARY")
    add_library(${PROJECT_NAME} STATIC  ${EXTRA_FILES} ${LIMEREPORT_SOURCES})
    target_compile_definitions( ${PROJECT_NAME} PUBLIC -DHAVE_STATIC_BUILD)
else()
    add_library(${PROJECT_NAME} SHARED  ${EXTRA_FILES} ${LIMEREPORT_SOURCES})
    target_compile_definitions( ${PROJECT_NAME} PRIVATE -DLIMEREPORT_EXPORTS)
    target_compile_definitions( ${PROJECT_NAME} INTERFACE -DLIMEREPORT_IMPORTS)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC -DCMAKE_CONFIG)

if(Qt${QT_VERSION_MAJOR}UiTools_FOUND)
    target_compile_definitions( ${PROJECT_NAME} PRIVATE -DHAVE_UI_LOADER)
    target_link_libraries( ${PROJECT_NAME} PUBLIC
        Qt${QT_VERSION_MAJOR}::UiTools)
endif()

target_link_libraries( ${PROJECT_NAME} PUBLIC
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Qml
	Qt${QT_VERSION_MAJOR}::Xml
	Qt${QT_VERSION_MAJOR}::Sql
	Qt${QT_VERSION_MAJOR}::PrintSupport
	Qt${QT_VERSION_MAJOR}::Svg)

if(ENABLE_ZINT)
    target_link_libraries( ${PROJECT_NAME} PRIVATE QZint)
endif(ENABLE_ZINT)

target_compile_definitions( ${PROJECT_NAME} PRIVATE -DHAVE_QT${QT_VERSION_MAJOR} -DHAVE_REPORT_DESIGNER -DUSE_QJSENGINE -D_CRT_SECURE_NO_WARNINGS)


target_include_directories( ${PROJECT_NAME} PRIVATE
	limereport/
	limereport/base
	limereport/bands
	limereport/databrowser
	limereport/items/editors
	limereport/items
	limereport/objectinspector
	limereport/scriptbrowser
	limereport/serializators
	limereport/scripteditor )

target_include_directories( ${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

list (APPEND GLOBAL_HEADERS ${EXTRA_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${GLOBAL_HEADERS}")

install(TARGETS
	${PROJECT_NAME}
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/limereport)

