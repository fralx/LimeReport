#include "lraxispropitem.h"

#include <QToolButton>

#include <lrchartitemeditor.h>
#include <lrpagedesignintf.h>
#include <lrreportengine_p.h>

namespace {
LimeReport::ObjectPropItem * createYAxisPropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::AxisPropItem(object, objects, name, displayName, data, parent, readonly, false);
}

LimeReport::ObjectPropItem * createXAxisPropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::AxisPropItem(object, objects, name, displayName, data, parent, readonly, true);
}
bool VARIABLE_IS_NOT_USED registredXAxisProp = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("xAxisSettings", "LimeReport::ChartItem"), QObject::tr("X axis"), createXAxisPropItem);
bool VARIABLE_IS_NOT_USED registredYAxisProp = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("yAxisSettings", "LimeReport::ChartItem"), QObject::tr("Y axis"), createYAxisPropItem);
}

namespace LimeReport {

QWidget *AxisPropItem::createProperyEditor(QWidget *parent) const
{
    return new AxisPropEditor(qobject_cast<ChartItem*>(object()), m_isXAxis, parent);
}

QString AxisPropItem::displayValue() const
{
    return QObject::tr("Axis");
}

AxisPropEditor::AxisPropEditor(ChartItem *chart, bool isXAxis, QWidget *parent)
    : QWidget(parent), m_button(new QPushButton(this)), m_chart(chart), m_isXAxis(isXAxis)
{
    m_button->setText("...");
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(m_button);
    layout->setSpacing(1);
    layout->setContentsMargins(1,0,1,1);
    setLayout(layout);
    setFocusProxy(m_button);
    setAutoFillBackground(true);
    connect(m_button,SIGNAL(clicked()),this,SLOT(slotButtonClicked()));
}

void AxisPropEditor::slotButtonClicked()
{
    m_chart->showAxisEditorDialog(m_isXAxis);
    emit editingFinished();
}

}
