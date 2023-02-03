#include "lrsvgitem.h"
#include "lrdesignelementsfactory.h"
#include "lrimageitemeditor.h"
#include "lrpagedesignintf.h"
#include <QtSvg>

namespace{
    const QString xmlTag = "SVGItem";
    LimeReport::BaseDesignIntf * createSVGItem(QObject* owner, LimeReport::BaseDesignIntf*  parent){
        return new LimeReport::SVGItem(owner,parent);
    }
    bool VARIABLE_IS_NOT_USED registred = LimeReport::DesignElementsFactory::instance().registerCreator(
        xmlTag, LimeReport::ItemAttribs(QObject::tr("SVG Item"),"Item"), createSVGItem
    );
}

namespace LimeReport{
SVGItem::SVGItem(QObject *owner, QGraphicsItem *parent)
    :ItemDesignIntf(xmlTag,owner,parent)
{
}

void SVGItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    if (isSelected()) painter->setOpacity(Const::SELECTION_OPACITY);
    else painter->setOpacity(qreal(opacity())/100);
    if (m_image.isNull() && itemMode() == DesignMode){
        QString text;
        painter->setFont(transformToSceneFont(QFont("Arial",10)));
        painter->setPen(Qt::black);
        if (!datasource().isEmpty() && !field().isEmpty())
            text = datasource()+"."+field();
        else text = tr("SVG Image");
        painter->drawText(rect().adjusted(4,4,-4,-4), Qt::AlignCenter, text );
    }
    else if (!m_image.isEmpty()){
        QSvgRenderer render;
        render.load(m_image);
        render.render(painter, option->rect);
    }
    ItemDesignIntf::paint(painter,option,widget);
    painter->restore();
}

QByteArray SVGItem::imageAsByteArray() const
{
    return m_image;
}

void SVGItem::setImageAsByteArray(QByteArray image)
{
    setImage(image);
}

QString SVGItem::fileFilter() const
{
    return tr("SVG (*.svg)");
}

void SVGItem::preparePopUpMenu(QMenu &menu)
{
    QAction* editAction = menu.addAction(QIcon(":/report/images/edit_pecil2.png"),tr("Edit"));
    menu.insertAction(menu.actions().at(0),editAction);
    menu.insertSeparator(menu.actions().at(1));

    menu.addSeparator();
    QAction* action = menu.addAction(tr("Watermark"));
    action->setCheckable(true);
    action->setChecked(isWatermark());
}

void SVGItem::processPopUpAction(QAction *action)
{
    if (action->text().compare(tr("Watermark")) == 0){
        page()->setPropertyToSelectedItems("watermark",action->isChecked());
    }
    if (action->text().compare(tr("Edit")) == 0){
        this->showEditorDialog();
    }
    ItemDesignIntf::processPopUpAction(action);
}

QWidget *SVGItem::defaultEditor()
{
    ImageItemEditor* editor = new ImageItemEditor(this);
    editor->setAttribute(Qt::WA_DeleteOnClose);
    return editor;
};

BaseDesignIntf* SVGItem::createSameTypeItem(QObject *owner, QGraphicsItem *parent){
    return new SVGItem(owner, parent);
}

void SVGItem::updateItemSize(DataSourceManager *dataManager, RenderPass pass, int maxHeight)
{
    Q_UNUSED(maxHeight)
    if (m_image.isEmpty()){
        if (!m_datasource.isEmpty() && !m_field.isEmpty()){
            IDataSource* ds = dataManager->dataSource(m_datasource);
            if (ds) {
                QVariant data = ds->data(m_field);
                m_image = data.value<QByteArray>();
            }
        } else if (!m_resourcePath.isEmpty()){
            m_resourcePath = expandUserVariables(m_resourcePath, pass, NoEscapeSymbols, dataManager);
            m_resourcePath = expandDataFields(m_resourcePath, NoEscapeSymbols, dataManager);
            m_image = imageFromResource(m_resourcePath);
        } else if (!m_variable.isEmpty()){
            //TODO: Migrate to QMetaType
            QVariant data = dataManager->variable(m_variable);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            if (data.typeId() == QMetaType::QString){
#else
            if (data.type() == QVariant::String){
#endif
                m_image = imageFromResource(data.toString());
            } else {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                if (data.typeId() == QMetaType::QByteArray) {
#else
                if (data.type() == QVariant::ByteArray) {
#endif
                    m_image = data.value<QByteArray>() ;
                }
            }
        }
    }
}

QByteArray SVGItem::imageFromResource(QString resourcePath)
{
    QFile file(resourcePath);
    if (file.open(QIODevice::ReadOnly)){
        return file.readAll();
    }
    return  QByteArray();
}

QString SVGItem::variable() const
{
    return m_variable;
}

void SVGItem::setVariable(const QString &variable)
{
    if (m_variable != variable){
        QString oldValue = m_variable;
        m_variable = variable;
        update();
        notify("variable", oldValue, m_variable);
    }
    m_variable = variable;
}

bool SVGItem::isNeedUpdateSize(RenderPass) const { return m_image.isNull() ; }

QString SVGItem::resourcePath() const
{
    return m_resourcePath;
}

void SVGItem::setResourcePath(const QString &resourcePath)
{
    if (m_resourcePath != resourcePath){
        QString oldValue = m_resourcePath;
        m_resourcePath = resourcePath;
        QFile file(resourcePath);
        if (file.open(QIODevice::ReadOnly)){
            m_image = file.readAll();
        }
        update();
        notify("resourcePath", oldValue, resourcePath);
    }
}

QByteArray SVGItem::image() const
{
    return m_image;
}

void SVGItem::setImage(const QByteArray &image)
{
    if (m_image != image){
        QByteArray oldValue = m_image;
        m_image = image;
        update();
        notify("image", oldValue, image);
    }
}

QString SVGItem::datasource() const
{
    return m_datasource;
}

void SVGItem::setDatasource(const QString &datasource)
{
    m_datasource = datasource;
}

QString SVGItem::field() const
{
    return m_field;
}

void SVGItem::setField(const QString &field)
{
    m_field = field;
};
} // namespace LimeReport
