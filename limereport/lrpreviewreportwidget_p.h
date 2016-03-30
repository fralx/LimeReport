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
    PreviewReportWidgetPrivate(PreviewReportWidget* previewReportWidget): q_ptr(previewReportWidget),
      m_currentPage(1), m_changingPage(false), m_priorScrolValue(0){}
    bool pageIsVisible();
    QRectF calcPageShift();
    void setPages( ReportPages pages);
public:
    PageDesignIntf* m_previewPage;
    ReportPages     m_reportPages;
    ReportEnginePrivate* m_report;
    GraphicsViewZoomer* m_zoomer;
    int m_currentPage;
    bool m_changingPage;
    int m_priorScrolValue;
    PreviewReportWidget* q_ptr;

};

}
#endif // LRPREVIEWREPORTWIDGET_P_H
