#include "bands/lrdataband.h"
#include "bands/lrgroupbands.h"
#include "bands/lrpagefooter.h"
#include "bands/lrpageheader.h"
#include "bands/lrreportheader.h"
#include "bands/lrreportfooter.h"
#include "bands/lrsubdetailband.h"
#include "bands/lrtearoffband.h"


#include "items/lrtextitem.h"
#ifdef HAVE_ZINT
#include "items/lrbarcodeitem.h"
#endif
#include "items/lrhorizontallayout.h"
#include "items/lrimageitem.h"
#include "items/lrshapeitem.h"
#include "items/lrchartitem.h"
#include "lrdesignelementsfactory.h"
#ifdef HAVE_SVG
#include "items/lrsvgitem.h"
#include "objectinspector/propertyItems/lrsvgpropitem.h"
#endif


#ifdef HAVE_REPORT_DESIGNER
#include "objectinspector/lrobjectpropitem.h"
#include "objectinspector/propertyItems/lrboolpropitem.h"
#include "objectinspector/propertyItems/lrcolorpropitem.h"
#include "objectinspector/propertyItems/lrcontentpropitem.h"
#include "objectinspector/propertyItems/lrdatasourcepropitem.h"
#include "objectinspector/propertyItems/lrenumpropitem.h"
#include "objectinspector/propertyItems/lrflagspropitem.h"
#include "objectinspector/propertyItems/lrfontpropitem.h"
#include "objectinspector/propertyItems/lrgroupfieldpropitem.h"
#include "objectinspector/propertyItems/lrimagepropitem.h"
#include "objectinspector/propertyItems/lrintpropitem.h"
#include "objectinspector/propertyItems/lrqrealpropitem.h"
#include "objectinspector/propertyItems/lrrectproptem.h"
#include "objectinspector/propertyItems/lrstringpropitem.h"
#include "items/lralignpropitem.h"
#include "items/lrsubitemparentpropitem.h"
#endif

#include "serializators/lrxmlbasetypesserializators.h"
#include "serializators/lrxmlqrectserializator.h"
#include "serializators/lrxmlserializatorsfactory.h"

#include "lrexportersfactory.h"
#include "lrexporterintf.h"
#include "exporters/lrpdfexporter.h"

void initResources(){
    Q_INIT_RESOURCE(report);
#ifdef HAVE_REPORT_DESIGNER
    Q_INIT_RESOURCE(lobjectinspector);
    Q_INIT_RESOURCE(lrdatabrowser);
    Q_INIT_RESOURCE(items);
    Q_INIT_RESOURCE(lrscriptbrowser);
    Q_INIT_RESOURCE(translationeditor);
    Q_INIT_RESOURCE(dialogdesigner);
#endif
}

namespace LimeReport{

BaseDesignIntf * createDataBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::DataBand(owner,parent);
}
BaseDesignIntf * createHeaderDataBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::DataHeaderBand(owner,parent);
}
BaseDesignIntf * createFooterDataBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::DataFooterBand(owner,parent);
}

BaseDesignIntf* createGroupHeaderBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::GroupBandHeader(owner,parent);
}

BaseDesignIntf * createGroupFooterBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::GroupBandFooter(owner,parent);
}

BaseDesignIntf * createPageHeaderBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::PageHeader(owner,parent);
}

BaseDesignIntf * createPageFooterBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::PageFooter(owner,parent);
}

BaseDesignIntf * createSubDetailBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::SubDetailBand(owner,parent);
}

BaseDesignIntf * createSubDetailHeaderBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::SubDetailHeaderBand(owner,parent);
}

BaseDesignIntf * createSubDetailFooterBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::SubDetailFooterBand(owner,parent);
}

BaseDesignIntf * createTearOffBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::TearOffBand(owner,parent);
}

BaseDesignIntf * createTextItem(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::TextItem(owner,parent);
}

#ifdef HAVE_ZINT
BaseDesignIntf * createBarcodeItem(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new BarcodeItem(owner,parent);
}
#endif

#ifdef HAVE_SVG
BaseDesignIntf* createSVGItem(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new SVGItem(owner,parent);
}
#endif

BaseDesignIntf* createHLayout(QObject *owner, LimeReport::BaseDesignIntf  *parent)
{
    return new HorizontalLayout(owner, parent);
}

BaseDesignIntf* createImageItem(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new ImageItem(owner,parent);
}

BaseDesignIntf* createShapeItem(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new ShapeItem(owner,parent);
}

BaseDesignIntf* createChartItem(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new ChartItem(owner,parent);
}

void initReportItems(){
    initResources();
    DesignElementsFactory::instance().registerCreator(
                "TextItem",
                LimeReport::ItemAttribs(QObject::tr("Text Item"),"TextItem"),
                createTextItem
    );
#ifdef HAVE_ZINT
    DesignElementsFactory::instance().registerCreator(
                "BarcodeItem",
                LimeReport::ItemAttribs(QObject::tr("Barcode Item"),"Item"),
                createBarcodeItem
    );
#endif

    DesignElementsFactory::instance().registerCreator(
                "HLayout",
                LimeReport::ItemAttribs(QObject::tr("HLayout"), LimeReport::Const::bandTAG),
                createHLayout
    );
    DesignElementsFactory::instance().registerCreator(
                         "ImageItem", LimeReport::ItemAttribs(QObject::tr("Image Item"),"Item"), createImageItem
    );

#ifdef HAVE_SVG
    DesignElementsFactory::instance().registerCreator(
        "SVGItem",
        LimeReport::ItemAttribs(QObject::tr("SVG Item"),"Item"),
        createSVGItem
        );
#endif

    DesignElementsFactory::instance().registerCreator(
                         "ShapeItem", LimeReport::ItemAttribs(QObject::tr("Shape Item"),"Item"), createShapeItem
    );
    DesignElementsFactory::instance().registerCreator(
                         "ChartItem", LimeReport::ItemAttribs(QObject::tr("Chart Item"),"Item"), createChartItem
    );
    DesignElementsFactory::instance().registerCreator(
            "Data",
            LimeReport::ItemAttribs(QObject::tr("Data"),LimeReport::Const::bandTAG),
            createDataBand
    );
    DesignElementsFactory::instance().registerCreator(
                "DataHeader",
                LimeReport::ItemAttribs(QObject::tr("DataHeader"),LimeReport::Const::bandTAG),
                createHeaderDataBand
    );
    DesignElementsFactory::instance().registerCreator(
                "DataFooter",
                LimeReport::ItemAttribs(QObject::tr("DataFooter"),LimeReport::Const::bandTAG),
                createFooterDataBand
    );
    DesignElementsFactory::instance().registerCreator(
           "GroupHeader",
            LimeReport::ItemAttribs(QObject::tr("GroupHeader"),LimeReport::Const::bandTAG),
            createGroupHeaderBand
    );
    DesignElementsFactory::instance().registerCreator(
            "GroupFooter",
            LimeReport::ItemAttribs(QObject::tr("GroupFooter"),LimeReport::Const::bandTAG),
            createGroupFooterBand
    );
    DesignElementsFactory::instance().registerCreator(
            "PageFooter",
            LimeReport::ItemAttribs(QObject::tr("Page Footer"),LimeReport::Const::bandTAG),
            createPageFooterBand
    );
    DesignElementsFactory::instance().registerCreator(
            "PageHeader",
            LimeReport::ItemAttribs(QObject::tr("Page Header"),LimeReport::Const::bandTAG),
            createPageHeaderBand
    );
    DesignElementsFactory::instance().registerCreator(
            "SubDetail",
            LimeReport::ItemAttribs(QObject::tr("SubDetail"),LimeReport::Const::bandTAG),
            createSubDetailBand
    );

    DesignElementsFactory::instance().registerCreator(
           "SubDetailHeader",
            LimeReport::ItemAttribs(QObject::tr("SubDetailHeader"),LimeReport::Const::bandTAG),
            createSubDetailHeaderBand
    );
    DesignElementsFactory::instance().registerCreator(
            "SubDetailFooter",
            LimeReport::ItemAttribs(QObject::tr("SubDetailFooter"),LimeReport::Const::bandTAG),
            createSubDetailFooterBand
    );
    DesignElementsFactory::instance().registerCreator(
            "TearOffBand",
            LimeReport::ItemAttribs(QObject::tr("Tear-off Band"),LimeReport::Const::bandTAG),
            createTearOffBand
    );

}

#ifdef HAVE_REPORT_DESIGNER

ObjectPropItem * createBoolPropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::BoolPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem * createColorPropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new ColorPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem * createContentPropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new ContentPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem * createDatasourcePropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::DatasourcePropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem* createFieldPropItem(QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly){
    return new LimeReport::FieldPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem * createEnumPropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::EnumPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem * createFlagsPropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::FlagsPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem * createFontPropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::FontPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem* createGroupFieldPropItem(QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly){
    return new LimeReport::GroupFieldPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem * createImagePropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::ImagePropItem(object, objects, name, displayName, data, parent, readonly);
}

#ifdef HAVE_SVG
ObjectPropItem * createSVGPropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::SvgPropItem(object, objects, name, displayName, data, parent, readonly);
}
#endif

ObjectPropItem * createIntPropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::IntPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem * createQRealPropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::QRealPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem * createReqtItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly
){
    return new LimeReport::RectPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem * createReqtMMItem(
    QObject*object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly
){
    return new LimeReport::RectUnitPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem * createStringPropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::StringPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem * createAlignItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly
){
    return new LimeReport::AlignmentPropItem(object, objects, name, displayName, data, parent, readonly);
}

ObjectPropItem * createLocationPropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::ItemLocationPropItem(object, objects, name, displayName, data, parent, readonly);
}

void initObjectInspectorProperties()
{
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("bool",""),QObject::tr("bool"),createBoolPropItem
    );
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("QColor",""),QObject::tr("QColor"),createColorPropItem
    );
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("content","LimeReport::TextItem"),QObject::tr("content"),createContentPropItem
    );
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("datasource","LimeReport::DataBandDesignIntf"),QObject::tr("datasource"),createDatasourcePropItem
    );
    ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("datasource","LimeReport::ImageItem"),QObject::tr("datasource"),createDatasourcePropItem
    );
    ObjectPropFactory::instance().registerCreator(
            LimeReport::APropIdent("field","LimeReport::ImageItem"),QObject::tr("field"),createFieldPropItem
    );
    ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("datasource","LimeReport::SVGItem"),QObject::tr("datasource"),createDatasourcePropItem
        );
    ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("field","LimeReport::SVGItem"),QObject::tr("field"),createFieldPropItem
        );
    ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("enum",""),QObject::tr("enum"),createEnumPropItem
    );
    ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("flags",""),QObject::tr("flags"),createFlagsPropItem
    );
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("QFont",""),QObject::tr("QFont"),createFontPropItem
    );
    ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("groupFieldName","LimeReport::GroupBandHeader"),QObject::tr("field"),createGroupFieldPropItem
    );
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("QImage",""),QObject::tr("QImage"),createImagePropItem
    );
#ifdef HAVE_SVG
    ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("image","LimeReport::SVGItem"),QObject::tr("image"),createSVGPropItem
    );
#endif
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("int",""),QObject::tr("int"),createIntPropItem
    );
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("qreal",""),QObject::tr("qreal"),createQRealPropItem
    );
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("double",""),QObject::tr("qreal"),createQRealPropItem
    );
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("QRect",""),QObject::tr("QRect"),createReqtItem
    );
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("QRectF",""),QObject::tr("QRectF"),createReqtItem
    );
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("geometry","LimeReport::BaseDesignIntf"),QObject::tr("geometry"),createReqtMMItem
    );
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("QString",""),QObject::tr("QString"),createStringPropItem
    );
    ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("alignment","LimeReport::TextItem"),QObject::tr("alignment"),createAlignItem
    );
    ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("itemLocation","LimeReport::ItemDesignIntf"),QObject::tr("itemLocation"),createLocationPropItem
    );

}
#endif
SerializatorIntf * createIntSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlIntSerializator(doc,node);
}

SerializatorIntf * createQRealSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlQRealSerializator(doc,node);
}

SerializatorIntf * createQStringSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlQStringSerializator(doc,node);
}

SerializatorIntf * createEnumAndFlagsSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlEnumAndFlagsSerializator(doc,node);
}

SerializatorIntf * createBoolSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlBoolSerializator(doc,node);
}

SerializatorIntf * createFontSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlFontSerializator(doc,node);
}

SerializatorIntf * createQSizeFSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlQSizeFSerializator(doc,node);
}

SerializatorIntf * createQImageSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlQImageSerializator(doc,node);
}

SerializatorIntf * createQColorSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlColorSerializator(doc,node);
}

SerializatorIntf* createQByteArraySerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlQByteArraySerializator(doc,node);
}

SerializatorIntf* createQVariantSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlQVariantSerializator(doc,node);
}

SerializatorIntf * createQRectSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XMLQRectSerializator(doc,node);
}

void initSerializators()
{
    XMLAbstractSerializatorFactory::instance().registerCreator("QString", createQStringSerializator);
    XMLAbstractSerializatorFactory::instance().registerCreator("int", createIntSerializator);
    XMLAbstractSerializatorFactory::instance().registerCreator("enumAndFlags",createEnumAndFlagsSerializator);
    XMLAbstractSerializatorFactory::instance().registerCreator("bool", createBoolSerializator);
    XMLAbstractSerializatorFactory::instance().registerCreator("QFont", createFontSerializator);
    XMLAbstractSerializatorFactory::instance().registerCreator("QSizeF", createQSizeFSerializator);
    XMLAbstractSerializatorFactory::instance().registerCreator("QImage", createQImageSerializator);
    XMLAbstractSerializatorFactory::instance().registerCreator("qreal", createQRealSerializator);
    XMLAbstractSerializatorFactory::instance().registerCreator("double", createQRealSerializator);
    XMLAbstractSerializatorFactory::instance().registerCreator("QColor", createQColorSerializator);
    XMLAbstractSerializatorFactory::instance().registerCreator("QByteArray", createQByteArraySerializator);
    XMLAbstractSerializatorFactory::instance().registerCreator("QVariant", createQVariantSerializator);
    XMLAbstractSerializatorFactory::instance().registerCreator("QRect", createQRectSerializator);
    XMLAbstractSerializatorFactory::instance().registerCreator("QRectF", createQRectSerializator);
}

LimeReport::ReportExporterInterface* createPDFExporter(ReportEnginePrivate* parent){
    return new LimeReport::PDFExporter(parent);
}

void initExporters()
{
    ExportersFactory::instance().registerCreator(
                "PDF",
                LimeReport::ExporterAttribs(QObject::tr("Export to PDF"), "PDFExporter"),
                createPDFExporter
    );
}

} //namespace LimeReport
