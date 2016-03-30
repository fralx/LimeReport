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
    void initPreview();
    void setErrorsMesagesVisible(bool visible);
    void setErrorMessages(const QStringList &value);
    void refreshPages();
public slots:
    void slotZoomIn();
    void slotZoomOut();

    void slotFirstPage();
    void slotPriorPage();
    void slotNextPage();
    void slotLastPage();

    void slotPrint();
    void slotPrintToPDF();
    void slotPageNavigatorChanged(int value);
    void slotSaveToFile();
signals:
    void pageChanged(int page);
private slots:
    void slotSliderMoved(int value);
    void reportEngineDestroyed(QObject* object);
private:
    Ui::PreviewReportWidget *ui;
    PreviewReportWidgetPrivate* d_ptr;
};

} // namespace LimeReport
#endif // LRPREVIEWREPORTWIDGET_H
