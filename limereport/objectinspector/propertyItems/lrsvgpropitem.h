#ifndef SVGPROPITEM_H
#define SVGPROPITEM_H

#include "lrobjectpropitem.h"
namespace LimeReport{

class SvgPropItem : public ObjectPropItem
{
    Q_OBJECT
public:
    SvgPropItem():ObjectPropItem(){}
    SvgPropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value,ObjectPropItem* parent, bool readonly)
        :ObjectPropItem(object, objects, name, displayName, value, parent, readonly){}
    QWidget* createProperyEditor(QWidget *parent) const;
    QString displayValue() const;
    void setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const;
    void setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index);
    virtual QIcon iconValue() const;
};

} // namespace LimeReport

#endif // SVGPROPITEM_H
