#include "lrsvgpropitem.h"
#include "editors/lrsvgeditor.h"

namespace{
LimeReport::ObjectPropItem * createSvgPropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::SvgPropItem(object, objects, name, displayName, data, parent, readonly);
}
bool VARIABLE_IS_NOT_USED registredImageProp = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("image","LimeReport::SVGItem"),QObject::tr("image"),createSvgPropItem);
}

namespace LimeReport{

QWidget* SvgPropItem::createProperyEditor(QWidget *parent) const
{
    return new SvgEditor(parent);
}

QString SvgPropItem::displayValue() const
{
    return (propertyValue().isNull()) ? "" : QObject::tr("image");
}

void SvgPropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const
{
    SvgEditor *editor = qobject_cast<SvgEditor*>(propertyEditor);
    editor->setImage(propertyValue().value<QByteArray>());
}

void SvgPropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    model->setData(index,qobject_cast<SvgEditor*>(propertyEditor)->image());
    object()->setProperty(propertyName().toLatin1(),propertyValue());
}

QIcon SvgPropItem::iconValue() const
{
    return QIcon(QPixmap::fromImage(propertyValue().value<QImage>()));
}

}
