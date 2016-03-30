#include "lrpreviewreportwidget.h"
#include "ui_lrpreviewreportwidget.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QScrollBar>
#include <QFileDialog>

#include "lrpagedesignintf.h"
#include "lrreportrender.h"
#include "lrreportengine_p.h"
#include "lrpreviewreportwidget_p.h"
#include "serializators/lrxmlwriter.h"

namespace LimeReport {

bool PreviewReportWidgetPrivate::pageIsVisible(){
    QGraphicsView* view = q_ptr->ui->graphicsView;
    PageItemDesignIntf::Ptr page = m_reportPages.at(m_currentPage-1);
    return page->mapToScene(page->rect()).boundingRect().intersects(
                view->mapToScene(view->viewport()->geometry()).boundingRect()
                );
}

QRectF PreviewReportWidgetPrivate::calcPageShift(){
    QGraphicsView *view = q_ptr->ui->graphicsView;
    PageItemDesignIntf::Ptr page = m_reportPages.at(m_currentPage-1);
    qreal pageHeight = page->mapToScene(page->boundingRect()).boundingRect().height();
    qreal viewHeight = view->mapToScene(
                0, view->viewport()->height()
                ).y() - view->mapToScene(0,0).y();
    viewHeight = (pageHeight<viewHeight)?pageHeight:viewHeight;
    QRectF pageStartPos =  m_reportPages.at(m_currentPage-1)->mapRectToScene(
                m_reportPages.at(m_currentPage-1)->rect()
                );
    return QRectF(0,pageStartPos.y(),0,viewHeight);
}

void PreviewReportWidgetPrivate::setPages(ReportPages pages)
{
    m_reportPages = pages;
    if (!m_reportPages.isEmpty()){
        m_previewPage->setPageItems(m_reportPages);
        m_changingPage = true;
        m_currentPage = 1;
        if (pages.at(0)) pages.at(0)->setSelected(true);
        m_changingPage = false;
        q_ptr->initPreview();
    }
}

PreviewReportWidget::PreviewReportWidget(ReportEnginePrivate *report, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreviewReportWidget), d_ptr(new PreviewReportWidgetPrivate(this))
{
    ui->setupUi(this);
    d_ptr->m_previewPage = report->createPreviewPage();
    d_ptr->m_previewPage->setItemMode( LimeReport::PreviewMode );
    d_ptr->m_report = report;

    ui->errorsView->setVisible(false);
    connect(ui->graphicsView->verticalScrollBar(),SIGNAL(valueChanged(int)), this, SLOT(slotSliderMoved(int)));
    connect(d_ptr->m_report, SIGNAL(destroyed(QObject*)), this, SLOT(reportEngineDestroyed(QObject*)));
    d_ptr->m_zoomer = new GraphicsViewZoomer(ui->graphicsView);
}

PreviewReportWidget::~PreviewReportWidget()
{
    delete d_ptr->m_previewPage;
    d_ptr->m_previewPage = 0;
    delete d_ptr->m_zoomer;
    delete d_ptr;
    delete ui;
}

void PreviewReportWidget::initPreview()
{
    ui->graphicsView->setScene(d_ptr->m_previewPage);
    ui->graphicsView->centerOn(0, 0);
    ui->graphicsView->scale(0.5,0.5);
}

void PreviewReportWidget::setErrorsMesagesVisible(bool visible)
{
    ui->errorsView->setVisible(visible);
}

void PreviewReportWidget::slotZoomIn()
{
    ui->graphicsView->scale(1.2,1.2);
}

void PreviewReportWidget::slotZoomOut()
{
    ui->graphicsView->scale(1/1.2,1/1.2);
}

void PreviewReportWidget::slotFirstPage()
{
    d_ptr->m_changingPage=true;
    if ((!d_ptr->m_reportPages.isEmpty())&&(d_ptr->m_currentPage>1)){
        d_ptr->m_currentPage=1;
        ui->graphicsView->ensureVisible(d_ptr->calcPageShift(), 0, 0);
        emit pageChanged(d_ptr->m_currentPage);
    }
    d_ptr->m_changingPage=false;
}

void PreviewReportWidget::slotPriorPage()
{
    d_ptr->m_changingPage=true;
    if ((!d_ptr->m_reportPages.isEmpty())&&(d_ptr->m_currentPage>1)){
       d_ptr->m_currentPage--;
       ui->graphicsView->ensureVisible(d_ptr->calcPageShift(), 0, 0);
       emit pageChanged(d_ptr->m_currentPage);
    }
   d_ptr->m_changingPage=false;
}

void PreviewReportWidget::slotNextPage()
{
    d_ptr->m_changingPage=true;
    if ((!d_ptr->m_reportPages.isEmpty())&&(d_ptr->m_reportPages.count()>(d_ptr->m_currentPage))){
        d_ptr->m_currentPage++;
        ui->graphicsView->ensureVisible(d_ptr->calcPageShift(), 0, 0);
        emit pageChanged(d_ptr->m_currentPage);
    }
    d_ptr->m_changingPage=false;
}

void PreviewReportWidget::slotLastPage()
{
    d_ptr->m_changingPage=true;
    if ((!d_ptr->m_reportPages.isEmpty())&&(d_ptr->m_reportPages.count()>(d_ptr->m_currentPage))){
        d_ptr->m_currentPage=d_ptr->m_reportPages.count();
        ui->graphicsView->ensureVisible(d_ptr->calcPageShift(), 0, 0);
        emit pageChanged(d_ptr->m_currentPage);
    }
    d_ptr->m_changingPage=false;
}

void PreviewReportWidget::slotPrint()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer,QApplication::activeWindow());
    if (dialog.exec()==QDialog::Accepted){
        if (!d_ptr->m_reportPages.isEmpty())
            ReportEnginePrivate::printReport(
                d_ptr->m_reportPages,
                printer,
                PrintRange(dialog.printRange(),dialog.fromPage(),dialog.toPage())
            );
    }
}

void PreviewReportWidget::slotPrintToPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("PDF file name"),"","PDF(*.pdf)" );
    if (!fileName.isEmpty()){
        QPrinter printer;
        printer.setOutputFileName(fileName);
        printer.setOutputFormat(QPrinter::PdfFormat);
        if (!d_ptr->m_reportPages.isEmpty()){
            ReportEnginePrivate::printReport(d_ptr->m_reportPages,printer,PrintRange());
        }
        foreach(PageItemDesignIntf::Ptr pageItem, d_ptr->m_reportPages){
            d_ptr->m_previewPage->reactivatePageItem(pageItem);
        }
    }
}

void PreviewReportWidget::slotPageNavigatorChanged(int value)
{
    if (d_ptr->m_changingPage) return;
    d_ptr->m_changingPage = true;
    if ((!d_ptr->m_reportPages.isEmpty())&&(d_ptr->m_reportPages.count() >= value) && value>0){
        d_ptr->m_currentPage = value;
        ui->graphicsView->ensureVisible(d_ptr->calcPageShift(), 0, 0);
    }
    d_ptr->m_changingPage=false;
}

void PreviewReportWidget::slotSaveToFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Report file name"));
    if (!fileName.isEmpty()){
        QScopedPointer< ItemsWriterIntf > writer(new XMLWriter());
        foreach (PageItemDesignIntf::Ptr page, d_ptr->m_reportPages){
            writer->putItem(page.data());
        }
        writer->saveToFile(fileName);
    }
}

void PreviewReportWidget::setErrorMessages(const QStringList &value)
{
    foreach (QString line, value) {
        ui->errorsView->append(line);
    }
}

void PreviewReportWidget::refreshPages()
{
    if (d_ptr->m_report){
        try{
            d_ptr->m_report->dataManager()->setDesignTime(false);
            ReportPages pages = d_ptr->m_report->renderToPages();
            d_ptr->m_report->dataManager()->setDesignTime(true);
            if (pages.count()>0){
                d_ptr->m_reportPages = pages;
                if (!d_ptr->m_reportPages.isEmpty()){
                    d_ptr->m_previewPage->setPageItems(d_ptr->m_reportPages);
                    d_ptr->m_changingPage = true;
                    d_ptr->m_currentPage = 1;
                    if (pages.at(0)) pages.at(0)->setSelected(true);
                    d_ptr->m_changingPage = false;
                }
                ui->graphicsView->centerOn(0, 0);
            }
        } catch (ReportError &exception){
            d_ptr->m_report->saveError(exception.what());
            d_ptr->m_report->showError(exception.what());
       }
    }
}

void PreviewReportWidget::slotSliderMoved(int value)
{
    if (ui->graphicsView->verticalScrollBar()->minimum()==value){
        d_ptr->m_currentPage = 1;
    } else if (ui->graphicsView->verticalScrollBar()->maximum()==value){
        d_ptr->m_currentPage = d_ptr->m_reportPages.count();
    }

    if (!d_ptr->pageIsVisible()){
        if (value>d_ptr->m_priorScrolValue){
            d_ptr->m_currentPage++;
        } else {
            d_ptr->m_currentPage--;
        }
    }

    d_ptr->m_changingPage = true;
    emit pageChanged(d_ptr->m_currentPage);

    d_ptr->m_changingPage = false;
    d_ptr->m_priorScrolValue = value;
}

void PreviewReportWidget::reportEngineDestroyed(QObject *object)
{
    if (object == d_ptr->m_report){
        d_ptr->m_report = 0;
    }
}



}
