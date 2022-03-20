#ifndef CHARTAXISEDITOR_H
#define CHARTAXISEDITOR_H

#include <QWidget>
#include "lrchartitem.h"

namespace Ui {
class ChartAxisEditor;
}

class ChartAxisEditor : public QWidget
{
    Q_OBJECT
    public:
    ChartAxisEditor(LimeReport::ChartItem* item, LimeReport::PageDesignIntf* page, bool isXAxis,
                    QSettings* settings=0, QWidget *parent = 0);
    ~ChartAxisEditor();

    QSettings *settings();
private slots:
    void on_minimumCheckBox_stateChanged(int arg1);
    void on_maximumCheckBox_stateChanged(int arg1);
    void on_stepCheckBox_stateChanged(int arg1);
    void on_pushButtonOk_clicked();
    void on_enableScaleCalculation_checkbox_stateChanged(int arg1);

    void on_cancelButton_clicked();

private:
    void readSetting();
    void writeSetting();
    void init();

    Ui::ChartAxisEditor *ui;
    LimeReport::ChartItem* m_chartItem;
    LimeReport::PageDesignIntf* m_page;
    QSettings* m_settings;
    bool m_isXAxis;
};

#endif // CHARTAXISEDITOR_H
