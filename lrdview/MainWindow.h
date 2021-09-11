#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPrinterInfo>
#include "lrreportengine.h"
#include "lrpagedesignintf.h"
#include "XmlModel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionEdit_triggered();
    void on_actionOpen_triggered();
    void on_actionDesign_triggered();
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_actionPrint_triggered();
    void on_actionSetting_triggered();

protected:
     void closeEvent(QCloseEvent *event);
private:
    void loadReport(QString fileName);
    void writeSetting();
    void readSetting();
    QPrinterInfo getPrinterInfo(QString printerName);
private slots:
    void settingAccepted();
    void on_actionDelete_triggered();

    void on_action_PDF_triggered();

private:
    Ui::MainWindow *ui;
    LimeReport::PageDesignIntf*  m_previewScene;
    LimeReport::ReportEngine m_report;
    XmlModel m_model;
    QByteArray m_reportData;
    QSettings* m_settings;
    QString m_certPrinterName;
    QString m_otherPrinterName;
    bool m_isCert;
    bool m_deleteLastFile;
    QString m_lastReport;
};

#endif // MAINWINDOW_H
