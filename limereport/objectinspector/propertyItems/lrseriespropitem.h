#ifndef SERIESPROPITEM_H
#define SERIESPROPITEM_H

#include <QPushButton>
#include <QHBoxLayout>
#include <lrobjectpropitem.h>
#include <lrchartitem.h>

namespace LimeReport {

class SeriesPropEditor : public QWidget
{
    Q_OBJECT
public:
    SeriesPropEditor(ChartItem* chart, QWidget *parent = 0);
signals:
    void editingFinished();
private slots:
    void slotButtonClicked();
private:
    QPushButton* m_button;
    ChartItem* m_chart;
};

class SeriesPropItem : public LimeReport::ObjectPropItem{
    Q_OBJECT
public:
    SeriesPropItem():ObjectPropItem(){}
    SeriesPropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value, ObjectPropItem* parent, bool readonly)
        :ObjectPropItem(object, objects, name, displayName, value, parent, readonly){}
    QWidget* createProperyEditor(QWidget *parent) const;
    QString displayValue() const;
};

} // namespace LimeReport





#endif // SERIESPROPITEM_H
