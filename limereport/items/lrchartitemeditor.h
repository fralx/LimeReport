#ifndef CHARITEMEDITOR_H
#define CHARITEMEDITOR_H

#include <QWidget>
#include "lrchartitem.h"
#include "lrcolorindicator.h"
#include <QTableWidgetItem>
#include <QToolButton>

namespace Ui {
class ChartItemEditor;
}

class ChartItemEditor : public QWidget
{
    Q_OBJECT
public:
    ChartItemEditor(LimeReport::ChartItem* item, LimeReport::PageDesignIntf* page,
                    QSettings* settings=0, QWidget *parent = 0);
    ~ChartItemEditor();
public:
    QSettings *settings();
    void rebuildTable();
    
protected:
    void resizeEvent(QResizeEvent *);
    void moveEvent(QMoveEvent *);

signals:
    void editingFinished();

private slots:
    void on_splitter_splitterMoved(int, int);
    void on_pbOk_clicked();
    void slotAddSeries();
    void slotDeleteSeries();
    void on_tableWidget_itemSelectionChanged();
    void on_seriesNameLineEdit_textChanged(const QString &arg1);
    void on_valuesFieldComboBox_currentTextChanged(const QString &arg1);
    void on_labelsFieldComboBox_currentTextChanged(const QString &arg1);
    void slotChangeSeriesColor();
    void on_seriesTypeComboBox_currentIndexChanged(const QString &arg1);
    void on_xAxisFieldComboBox_currentTextChanged(const QString &arg1);
    void on_tableWidget_itemChanged(QTableWidgetItem *item);

private:
    void readSetting();
    void writeSetting();
    void init();
    void enableSeriesEditor();
    void disableSeriesEditor();
    LimeReport::SeriesItem* currentSeries();
private:
    Ui::ChartItemEditor *ui;
    LimeReport::ChartItem* m_charItem;
    LimeReport::PageDesignIntf* m_page;
    QSettings* m_settings;
    bool m_ownedSettings;
    bool m_isReadingSetting;
    QToolButton* m_colorButton;
    ColorIndicator* m_colorIndicator;
    bool m_initing;
};

#endif // CHARITEMEDITOR_H
