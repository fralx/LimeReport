#include "lrpreviewreportwidget.h"
#include "ui_lrpreviewreportwidget.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QPrinterInfo>
#include <QScrollBar>
#include <QFileDialog>

#include "lrpagedesignintf.h"
#include "lrreportrender.h"
#include "lrreportengine_p.h"
#include "lrpreviewreportwidget_p.h"
#include "serializators/lrxmlwriter.h"
#include "lrpreparedpages.h"

#include "lrexportersfactory.h"


namespace LimeReport {

bool PreviewReportWidgetPrivate::pageIsVisible(){
    QGraphicsView* view = q_ptr->ui->graphicsView;
    if ( m_currentPage-1 >= m_reportPages.size() || m_currentPage <= 0 )
        return false;
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
        q_ptr->emitPageSet();
        q_ptr->activateCurrentPage();
    }
}

PageItemDesignIntf::Ptr PreviewReportWidgetPrivate::currentPage()
{
    if (m_reportPages.count()>0 && m_reportPages.count() >= m_currentPage && m_currentPage > 0)
        return m_reportPages.at(m_currentPage-1);
    else return PageItemDesignIntf::Ptr(0);
}

QList<QString> PreviewReportWidgetPrivate::aviableExporters()
{
    return ExportersFactory::instance().map().keys();
}

void PreviewReportWidgetPrivate::startInsertTextItem()
{
    m_previewPage->startInsertMode("TextItem");
}

void PreviewReportWidgetPrivate::activateItemSelectionMode()
{
    m_previewPage->startEditMode();
}

void PreviewReportWidgetPrivate::deleteSelectedItems()
{
    m_previewPage->deleteSelected();
}

PreviewReportWidget::PreviewReportWidget(ReportEngine *report, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreviewReportWidget), d_ptr(new PreviewReportWidgetPrivate(this)),
    m_scaleType(FitWidth), m_scalePercent(0), m_previewPageBackgroundColor(Qt::white),
    m_defaultPrinter(0), m_scaleChanging(false)
{
    ui->setupUi(this);
    d_ptr->m_report = report->d_ptr;
    d_ptr->m_previewPage = d_ptr->m_report->createPreviewPage();
    d_ptr->m_previewPage->setItemMode( LimeReport::PreviewMode );
    m_resizeTimer.setSingleShot(true);

    ui->errorsView->setVisible(false);
    connect(ui->graphicsView->verticalScrollBar(),SIGNAL(valueChanged(int)), this, SLOT(slotSliderMoved(int)));
    connect(d_ptr->m_report, SIGNAL(destroyed(QObject*)), this, SLOT(reportEngineDestroyed(QObject*)));
    d_ptr->m_zoomer = new GraphicsViewZoomer(ui->graphicsView);
    connect(d_ptr->m_zoomer, SIGNAL(zoomed(double)), this, SLOT(slotZoomed(double)));
    connect(&m_resizeTimer, SIGNAL(timeout()), this, SLOT(resizeDone()));

}

PreviewReportWidget::~PreviewReportWidget()
{
    delete d_ptr->m_previewPage;
    d_ptr->m_previewPage = 0;
    delete d_ptr->m_zoomer;
    delete d_ptr;
    delete ui;
}

QList<QString> PreviewReportWidget::aviableExporters()
{
    return d_ptr->aviableExporters();
}

bool PreviewReportWidget::exportReport(QString exporterName, const QMap<QString, QVariant> &params)
{
    if (ExportersFactory::instance().map().contains(exporterName)){

        ReportExporterInterface* e = ExportersFactory::instance().objectCreator(exporterName)(d_ptr->m_report);
        QString defaultFileName = d_ptr->m_report->reportName().split(".")[0];
        QString filter = QString("%1 (*.%2)").arg(e->exporterName()).arg(e->exporterFileExt());
        QString fileName = QFileDialog::getSaveFileName(this,tr("%1 file name").arg(e->exporterName()), defaultFileName, filter);
        if (!fileName.isEmpty()){
            QFileInfo fi(fileName);
            if (fi.suffix().isEmpty())
                fileName += QString(".%1").arg(e->exporterFileExt());
            bool result = e->exportPages(d_ptr->m_reportPages, fileName, params);
            delete e;
            return result;
        }
    }
    return false;
}

void PreviewReportWidget::initPreview()
{
    if (ui->graphicsView->scene()!=d_ptr->m_previewPage)
        ui->graphicsView->setScene(d_ptr->m_previewPage);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    ui->graphicsView->resetMatrix();
#else
    ui->graphicsView->resetTransform();
#endif
    ui->graphicsView->centerOn(0, 0);
    ui->graphicsView->scene()->setBackgroundBrush(QColor(m_previewPageBackgroundColor));
    setScalePercent(d_ptr->m_scalePercent);
    PageDesignIntf* page = dynamic_cast<PageDesignIntf*>(ui->graphicsView->scene());
    if (page)
        connect(page, SIGNAL(itemInserted(LimeReport::PageDesignIntf*, QPointF, QString)),
                this, SIGNAL(itemInserted(LimeReport::PageDesignIntf*, QPointF, QString)));
}

void PreviewReportWidget::setErrorsMesagesVisible(bool visible)
{
    ui->errorsView->setVisible(visible);
}

void PreviewReportWidget::zoomIn()
{
    d_ptr->m_scalePercent =  (d_ptr->m_scalePercent / 10) * 10 + 10;
    setScalePercent(d_ptr->m_scalePercent);
}

void PreviewReportWidget::zoomOut()
{
    if (d_ptr->m_scalePercent>0)
        d_ptr->m_scalePercent = (d_ptr->m_scalePercent / 10) * 10 - 10;
    setScalePercent(d_ptr->m_scalePercent);
}

void PreviewReportWidget::firstPage()
{
    d_ptr->m_changingPage=true;
    if ((!d_ptr->m_reportPages.isEmpty())&&(d_ptr->m_currentPage>1)){
        d_ptr->m_currentPage=1;
        ui->graphicsView->ensureVisible(d_ptr->calcPageShift(), 0, 0);
        emit pageChanged(d_ptr->m_currentPage);
        activateCurrentPage();
    }
    d_ptr->m_changingPage=false;
}

void PreviewReportWidget::priorPage()
{
    d_ptr->m_changingPage=true;
    if ((!d_ptr->m_reportPages.isEmpty())&&(d_ptr->m_currentPage>1)){
       d_ptr->m_currentPage--;
       ui->graphicsView->ensureVisible(d_ptr->calcPageShift(), 0, 0);
       emit pageChanged(d_ptr->m_currentPage);
       activateCurrentPage();
    }
   d_ptr->m_changingPage=false;
}

void PreviewReportWidget::nextPage()
{
    d_ptr->m_changingPage=true;
    if ((!d_ptr->m_reportPages.isEmpty())&&(d_ptr->m_reportPages.count()>(d_ptr->m_currentPage))){
        d_ptr->m_currentPage++;
        ui->graphicsView->ensureVisible(d_ptr->calcPageShift(), 0, 0);
        emit pageChanged(d_ptr->m_currentPage);
        activateCurrentPage();
    }
    d_ptr->m_changingPage=false;
}

void PreviewReportWidget::lastPage()
{
    d_ptr->m_changingPage=true;
    if ((!d_ptr->m_reportPages.isEmpty())&&(d_ptr->m_reportPages.count()>(d_ptr->m_currentPage))){
        d_ptr->m_currentPage=d_ptr->m_reportPages.count();
        ui->graphicsView->ensureVisible(d_ptr->calcPageShift(), 0, 0);
        emit pageChanged(d_ptr->m_currentPage);
        activateCurrentPage();
    }
    d_ptr->m_changingPage=false;
}

void PreviewReportWidget::printPages(QPrinter* printer)
{
    if (!d_ptr->m_reportPages.isEmpty())
        d_ptr->m_report->printPages(
            d_ptr->m_reportPages,
            printer
        );
    foreach(PageItemDesignIntf::Ptr pageItem, d_ptr->m_reportPages){
        d_ptr->m_previewPage->reactivatePageItem(pageItem);
    }
}

void PreviewReportWidget::print()
{
    QPrinterInfo pi;
    QPrinter lp(QPrinter::HighResolution);

    if (!pi.defaultPrinter().isNull()){
#if QT_VERSION >= 0x050300
            lp.setPrinterName(pi.defaultPrinterName());
#else
            lp.setPrinterName(pi.defaultPrinter().printerName());
#endif
    }

    QPrinter* printer = m_defaultPrinter ? m_defaultPrinter : &lp;

    QPrintDialog dialog(printer,QApplication::activeWindow());
    if (dialog.exec()==QDialog::Accepted){
        printPages(printer);
    }

}

void PreviewReportWidget::printToPDF()
{
    if (!d_ptr->m_reportPages.isEmpty()){
        exportReport("PDF");
        foreach(PageItemDesignIntf::Ptr pageItem, d_ptr->m_reportPages){
            d_ptr->m_previewPage->reactivatePageItem(pageItem);
        }
    }
}

void PreviewReportWidget::pageNavigatorChanged(int value)
{
    if (d_ptr->m_changingPage) return;
    d_ptr->m_changingPage = true;
    if ((!d_ptr->m_reportPages.isEmpty())&&(d_ptr->m_reportPages.count() >= value) && value>0){
        d_ptr->m_currentPage = value;
        activateCurrentPage();
        ui->graphicsView->ensureVisible(d_ptr->calcPageShift(), 0, 0);
    }
    d_ptr->m_changingPage=false;
}

void PreviewReportWidget::saveToFile()
{
    bool saved = false;
    PreparedPages pagesManager = PreparedPages(&d_ptr->m_reportPages);
    emit onSave(saved, &pagesManager);
    if (!saved){
        QString fileName = QFileDialog::getSaveFileName(this,tr("Report file name"));
        if (!fileName.isEmpty()){
            QScopedPointer< ItemsWriterIntf > writer(new XMLWriter());
            foreach (PageItemDesignIntf::Ptr page, d_ptr->m_reportPages){
                writer->putItem(page.data());
            }
            writer->saveToFile(fileName);
        }
    }
}

void PreviewReportWidget::setScalePercent(int percent)
{
    m_scaleChanging = true;
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    ui->graphicsView->resetMatrix();
#else
    ui->graphicsView->resetTransform();
#endif
    d_ptr->m_scalePercent = percent;
    qreal scaleSize = percent/100.0;
    ui->graphicsView->scale(scaleSize, scaleSize);
    emit scalePercentChanged(percent);
    if (percent == 100){
        m_scaleType = OneToOne;
    } else {
        m_scaleType = Percents;
        m_scalePercent = percent;
    }
    m_scaleChanging = false;
}

void PreviewReportWidget::fitWidth()
{
    if (d_ptr->currentPage()){
        resize(m_scaleType = FitWidth);
    }
}

void PreviewReportWidget::fitPage()
{
    if (d_ptr->currentPage()){
        resize(m_scaleType = FitPage);
    }
}

void PreviewReportWidget::resizeEvent(QResizeEvent *)
{
    m_resizeTimer.start(100);
}

void PreviewReportWidget::setErrorMessages(const QStringList &value)
{
    foreach (QString line, value) {
        ui->errorsView->append(line);
    }
}

void PreviewReportWidget::emitPageSet()
{
    emit pagesSet(d_ptr->m_reportPages.count());
}

QPrinter *PreviewReportWidget::defaultPrinter() const
{
    return m_defaultPrinter;
}

void PreviewReportWidget::setDefaultPrinter(QPrinter *defaultPrinter)
{
    m_defaultPrinter = defaultPrinter;
}

ScaleType PreviewReportWidget::scaleType() const
{
    return m_scaleType;
}

void PreviewReportWidget::startInsertTextItem()
{
    d_ptr->startInsertTextItem();
}

void PreviewReportWidget::activateItemSelectionMode()
{
    d_ptr->activateItemSelectionMode();
}

void PreviewReportWidget::deleteSelectedItems()
{
    d_ptr->deleteSelectedItems();
}

int PreviewReportWidget::scalePercent() const
{
    return m_scalePercent;
}

void PreviewReportWidget::setScaleType(const ScaleType &scaleType, int percent)
{
    m_scaleType = scaleType;
    m_scalePercent = percent;
}

void PreviewReportWidget::setPreviewPageBackgroundColor(QColor color)
{
    m_previewPageBackgroundColor = color;
}

QColor PreviewReportWidget::previewPageBackgroundColor()
{
    return  m_previewPageBackgroundColor;
}

void PreviewReportWidget::refreshPages()
{
    if (d_ptr->m_report){
        try{
            d_ptr->m_report->dataManager()->setDesignTime(false);
            ReportPages pages = d_ptr->m_report->renderToPages();
            d_ptr->m_report->dataManager()->setDesignTime(true);
            if (pages.count()>0){
                d_ptr->setPages(pages);
            }
        } catch (ReportError &exception){
            d_ptr->m_report->saveError(exception.what());
            d_ptr->m_report->showError(exception.what());
       }
    }
}

void PreviewReportWidget::activateCurrentPage()
{
    PageDesignIntf* page = dynamic_cast<PageDesignIntf*>(ui->graphicsView->scene());
    if (page)
        page->setCurrentPage(d_ptr->currentPage().data());
}

void PreviewReportWidget::resize(ScaleType scaleType, int percent)
{
    switch (scaleType) {
    case FitWidth:
            setScalePercent(ui->graphicsView->viewport()->width() / ui->graphicsView->scene()->width()*100);
        break;
    case FitPage:
            setScalePercent(qMin(
                ui->graphicsView->viewport()->width() / ui->graphicsView->scene()->width(),
                ui->graphicsView->viewport()->height() / d_ptr->currentPage()->height()
            ) * 100);
        break;
    case OneToOne:
        setScalePercent(100);
        break;
    case Percents:
        setScalePercent(percent);
        break;
    }

}

void PreviewReportWidget::slotSliderMoved(int value)
{
    if (m_scaleChanging) return;
    int curPage = d_ptr->m_currentPage;
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

    if (curPage != d_ptr->m_currentPage){
        d_ptr->m_changingPage = true;
        emit pageChanged(d_ptr->m_currentPage);
        activateCurrentPage();
        d_ptr->m_changingPage = false;
    }

    d_ptr->m_priorScrolValue = value;
}

void PreviewReportWidget::reportEngineDestroyed(QObject *object)
{
    if (object == d_ptr->m_report){
        d_ptr->m_report = 0;
    }
}

void PreviewReportWidget::slotZoomed(double )
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    d_ptr->m_scalePercent = ui->graphicsView->matrix().m11()*100;
#else
    d_ptr->m_scalePercent = ui->graphicsView->transform().m11()*100;
#endif
    emit scalePercentChanged(d_ptr->m_scalePercent);
}

void PreviewReportWidget::resizeDone()
{
    switch (m_scaleType) {
    case FitPage:
        fitPage();
        break;
    case FitWidth:
        fitWidth();
        break;
    case OneToOne:
        setScalePercent(100);
        break;
    case Percents:
        setScalePercent(m_scalePercent);
        break;
    }
}


}
