#ifndef LRPREVIEWREPORTWIDGET_H
#define LRPREVIEWREPORTWIDGET_H

#include <QWidget>
#include <QTimer>
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
    ScaleType scaleType() const;
    int  scalePercent() const;
    void setScaleType(const ScaleType &scaleType, int percent = 0);
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
protected:
    void resizeEvent(QResizeEvent *);
signals:
    void pageChanged(int page);
    void scalePercentChanged(int percent);
    void pagesSet(int pageCount);
private slots:
    void slotSliderMoved(int value);
    void reportEngineDestroyed(QObject* object);
    void slotZoomed(double);
    void resizeDone();
private:
    void initPreview();
    void setErrorsMesagesVisible(bool visible);
    void setErrorMessages(const QStringList &value);
    void emitPageSet();
private:
    Ui::PreviewReportWidget *ui;
    PreviewReportWidgetPrivate* d_ptr;
    ScaleType m_scaleType;
    int       m_scalePercent;
    QTimer m_resizeTimer;
};

} // namespace LimeReport
#endif // LRPREVIEWREPORTWIDGET_H
