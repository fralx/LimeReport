#ifndef AXISPROPITEM_H
#define AXISPROPITEM_H

#include <QPushButton>
#include <QHBoxLayout>
#include <lrobjectpropitem.h>
#include <lrchartitem.h>


namespace LimeReport {

class AxisPropEditor : public QWidget
{
    Q_OBJECT
public:
    AxisPropEditor(ChartItem* chart, bool isXAxis, QWidget *parent = 0);
signals:
    void editingFinished();
private slots:
    void slotButtonClicked();
private:
    QPushButton* m_button;
    ChartItem* m_chart;
    bool m_isXAxis;
};

class AxisPropItem: public LimeReport::ObjectPropItem
{
    Q_OBJECT
public:
    AxisPropItem():ObjectPropItem(){}
    AxisPropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value, ObjectPropItem* parent, bool readonly, bool isXAxis)
        :ObjectPropItem(object, objects, name, displayName, value, parent, readonly), m_isXAxis(isXAxis){}
    QWidget* createProperyEditor(QWidget *parent) const;
    QString displayValue() const;

private:
    bool m_isXAxis = false;
};

} // namespace LimeReport

#endif // AXISPROPITEM_H
