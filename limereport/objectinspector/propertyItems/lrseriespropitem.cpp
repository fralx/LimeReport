#include "lrseriespropitem.h"

#include <QToolButton>

#include <lrchartitemeditor.h>
#include <lrpagedesignintf.h>
#include <lrreportengine_p.h>

namespace{
LimeReport::ObjectPropItem * createSeriesPropItem(
    QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::SeriesPropItem(object, objects, name, displayName, data, parent, readonly);
}
bool VARIABLE_IS_NOT_USED registredSeriesProp = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("series", "LimeReport::ChartItem"), QObject::tr("series"), createSeriesPropItem);
}

namespace LimeReport {

QWidget *SeriesPropItem::createProperyEditor(QWidget *parent) const
{
    return new SeriesPropEditor(qobject_cast<ChartItem*>(object()), parent);
}

QString SeriesPropItem::displayValue() const
{
    return QObject::tr("Series");
}

SeriesPropEditor::SeriesPropEditor(ChartItem *chart, QWidget *parent)
    : QWidget(parent), m_button(new QPushButton(this)), m_chart(chart)
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

void SeriesPropEditor::slotButtonClicked()
{
    m_chart->showEditorDialog();
    emit editingFinished();
}


}
