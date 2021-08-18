#include "lrmarginpropitem.h"
#include <QDoubleSpinBox>
#include <limits>
#include "lrbasedesignintf.h"

namespace  {
    LimeReport::ObjectPropItem * createMarginPropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
    {
        return new LimeReport::MarginPropItem(object, objects, name, displayName, data, parent, readonly);
    }
    bool VARIABLE_IS_NOT_USED  registredTopMargin = LimeReport::ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("topMargin","LimeReport::PageItemDesignIntf"),
                QObject::tr("margin"),createMarginPropItem
    );
    bool VARIABLE_IS_NOT_USED  registredRightMargin = LimeReport::ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("rightMargin","LimeReport::PageItemDesignIntf"),
                QObject::tr("margin"),createMarginPropItem
    );
    bool VARIABLE_IS_NOT_USED  registredBottomMargin = LimeReport::ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("bottomMargin","LimeReport::PageItemDesignIntf"),
                QObject::tr("margin"),createMarginPropItem
    );
    bool VARIABLE_IS_NOT_USED  registredLeftMargin = LimeReport::ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("leftMargin","LimeReport::PageItemDesignIntf"),
                QObject::tr("margin"),createMarginPropItem
    );
}

namespace LimeReport{


QString MarginPropItem::displayValue() const
{
    LimeReport::BaseDesignIntf * item = dynamic_cast<LimeReport::BaseDesignIntf*>(object());
    switch (item->unitType()) {
    case LimeReport::BaseDesignIntf::Millimeters:

        return  QString("%1 %2").arg(propertyValue().toDouble(), 0, 'f', 2)
                                .arg(QObject::tr("mm"));
    case LimeReport::BaseDesignIntf::Inches:
        return QString("%1 %2").arg((propertyValue().toDouble() * Const::mmFACTOR) / (item->unitFactor() * 10), 0, 'f', 2)
                               .arg(QObject::tr("''"));
    }
    return QString();
}

QWidget *MarginPropItem::createProperyEditor(QWidget *parent) const
{
    QDoubleSpinBox *editor= new QDoubleSpinBox(parent);
    editor->setMaximum(std::numeric_limits<qreal>::max());
    editor->setMinimum(std::numeric_limits<qreal>::max()*-1);
    editor->setSuffix(" "+unitShortName());
    return editor;
}

void MarginPropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const
{
    QDoubleSpinBox *editor =qobject_cast<QDoubleSpinBox*>(propertyEditor);
    editor->setValue(valueInUnits(propertyValue().toReal()));
}

void MarginPropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    model->setData(index, valueInReportUnits(qobject_cast<QDoubleSpinBox*>(propertyEditor)->value()));
    setValueToObject(propertyName(), propertyValue());
}

qreal MarginPropItem::valueInUnits(qreal value) const
{
    BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(object());
    switch (item->unitType()) {
    case LimeReport::BaseDesignIntf::Millimeters:
        return value;
    case LimeReport::BaseDesignIntf::Inches:
        return (value * Const::mmFACTOR) / (item->unitFactor() * 10);
    }
    return 0;
}

qreal MarginPropItem::valueInReportUnits(qreal value) const
{
    BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(object());
    switch (item->unitType()) {
    case LimeReport::BaseDesignIntf::Millimeters:
        return value;
    case LimeReport::BaseDesignIntf::Inches:
        return (value * (item->unitFactor() * 10)) / Const::mmFACTOR;
    }
    return 0;
}

QString MarginPropItem::unitShortName() const
{
    BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(object());
    switch (item->unitType()) {
    case LimeReport::BaseDesignIntf::Millimeters:
        return QObject::tr("mm");
    case LimeReport::BaseDesignIntf::Inches:
        return QObject::tr("''");
    }
    return QString();
}

} // namespace LimeReport
