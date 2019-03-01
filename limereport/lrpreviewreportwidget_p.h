#ifndef LRPREVIEWREPORTWIDGET_P_H
#define LRPREVIEWREPORTWIDGET_P_H

#include "lrpagedesignintf.h"
#include "lrreportrender.h"
#include "lrgraphicsviewzoom.h"

namespace LimeReport{

class PreviewReportWidget;

class PreviewReportWidgetPrivate
{
public:
    PreviewReportWidgetPrivate(PreviewReportWidget* previewReportWidget):
      m_previewPage(NULL), m_report(NULL), m_zoomer(NULL),
      m_currentPage(1), m_changingPage(false), m_priorScrolValue(0), m_scalePercent(50),
      q_ptr(previewReportWidget), m_previePageColor(Qt::white) {}
    bool pageIsVisible();
    QRectF calcPageShift();
    void setPages( ReportPages pages);
    PageItemDesignIntf::Ptr currentPage();
    QList<QString> aviableExporters();
    void startInsertTextItem();
    void activateItemSelectionMode();
    void deleteSelectedItems();
public:
    PageDesignIntf* m_previewPage;
    ReportPages     m_reportPages;
    ReportEnginePrivate* m_report;
    GraphicsViewZoomer* m_zoomer;
    int m_currentPage;
    bool m_changingPage;
    int m_priorScrolValue;
    int m_scalePercent;
    PreviewReportWidget* q_ptr;
    QColor m_previePageColor;
};

}
#endif // LRPREVIEWREPORTWIDGET_P_H
