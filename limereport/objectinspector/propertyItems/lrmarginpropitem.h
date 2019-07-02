#ifndef LRMARGINPROPITEM_H
#define LRMARGINPROPITEM_H

#include "lrobjectpropitem.h"

namespace LimeReport {

class MarginPropItem : public ObjectPropItem
{
    Q_OBJECT
public:
    MarginPropItem():ObjectPropItem(){}
    MarginPropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value,ObjectPropItem* parent, bool readonly)
        :ObjectPropItem(object, objects, name, displayName, value, parent, readonly){}
    QString displayValue() const;
    QWidget* createProperyEditor(QWidget *parent) const;
    void setPropertyEditorData(QWidget * propertyEditor, const QModelIndex &) const;
    void setModelData(QWidget * propertyEditor, QAbstractItemModel * model, const QModelIndex & index);
private:
    qreal valueInUnits(qreal value) const;
    qreal valueInReportUnits(qreal value) const;
    QString unitShortName() const;
};

} // namespace LimeReport


#endif // LRMARGINPROPITEM_H
