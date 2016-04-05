#ifndef LRPREVIEWREPORTWIDGET_H
#define LRPREVIEWREPORTWIDGET_H

#include <QWidget>
#include "lrglobal.h"

namespace LimeReport {

namespace Ui {
class PreviewReportWidget;
}

class PreviewReportWidgetPrivate;
class ReportEnginePrivate;

class LIMEREPORT_EXPORT PreviewReportWidget : public QWidget
{
    Q_OBJECT
    friend class ReportEnginePrivate;
    friend class PreviewReportWindow;
    friend class PreviewReportWidgetPrivate;
public:
    explicit PreviewReportWidget(ReportEnginePrivate *report, QWidget *parent = 0);
    ~PreviewReportWidget();    
public slots:
    void refreshPages();
    void zoomIn();
    void zoomOut();

    void firstPage();
    void priorPage();
    void nextPage();
    void lastPage();

    void print();
    void printToPDF();
    void pageNavigatorChanged(int value);
    void saveToFile();
    void setScalePercent(int percent);
    void fitWidth();
    void fitPage();
signals:
    void pageChanged(int page);
    void scalePercentChanged(int percent);
    void pagesSet(int pageCount);
private slots:
    void slotSliderMoved(int value);
    void reportEngineDestroyed(QObject* object);
    void slotZoomed(double);
private:
    void initPreview();
    void setErrorsMesagesVisible(bool visible);
    void setErrorMessages(const QStringList &value);
    void emitPageSet();
private:
    Ui::PreviewReportWidget *ui;
    PreviewReportWidgetPrivate* d_ptr;
};

} // namespace LimeReport
#endif // LRPREVIEWREPORTWIDGET_H
