/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2015 by Alexander Arin                                  *
 *   arin_a@bk.ru                                                          *
 *                                                                         *
 **                   GNU General Public License Usage                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 **                  GNU Lesser General Public License                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library.                                      *
 *   If not, see <http://www.gnu.org/licenses/>.                           *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ****************************************************************************/
#include "lrpreviewreportwindow.h"
#include "ui_lrpreviewreportwindow.h"
#include "serializators/lrxmlreader.h"
#include "serializators/lrxmlwriter.h"
#include "lrreportengine_p.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QScrollBar>
#include <QDesktopWidget>

PreviewReportWindow::PreviewReportWindow(ReportEnginePrivate *report,QWidget *parent, QSettings *settings, Qt::WindowFlags flags) :
    QMainWindow(parent,flags),
    ui(new Ui::PreviewReportWindow), m_currentPage(1), m_changingPage(false), m_settings(settings), m_ownedSettings(false),
    m_priorScrolValue(0)
{
    ui->setupUi(this);
    setWindowTitle("Lime Report Preview");
    m_previewPage = report->createPreviewPage();
    m_previewPage->setItemMode( LimeReport::PreviewMode );
    m_pagesNavigator = new QSpinBox(this);
    m_pagesNavigator->setMaximum(10000000);
    m_pagesNavigator->setPrefix("Page: ");
    m_pagesNavigator->setMinimumWidth(120);
    ui->toolBar->insertWidget(ui->actionNextPage,m_pagesNavigator);
    m_simpleScene = new QGraphicsScene(this);
    ui->actionShowMessages->setVisible(false);
    ui->errorsView->setVisible(false);
    connect(m_pagesNavigator,SIGNAL(valueChanged(int)),this,SLOT(slotPageNavigatorChanged(int)));
    connect(ui->graphicsView->verticalScrollBar(),SIGNAL(valueChanged(int)), this, SLOT(slotSliderMoved(int)));
    connect(ui->actionShowMessages, SIGNAL(triggered()), this, SLOT(slotShowErrors()));
    restoreSetting();
}

void PreviewReportWindow::writeSetting()
{
    settings()->beginGroup("PreviewWindow");
    settings()->setValue("Geometry",saveGeometry());
    settings()->setValue("State",saveState());
    settings()->endGroup();
}

void PreviewReportWindow::restoreSetting()
{
    settings()->beginGroup("PreviewWindow");
    QVariant v = settings()->value("Geometry");
    if (v.isValid()){
        restoreGeometry(v.toByteArray());
    } else {
        QDesktopWidget *desktop = QApplication::desktop();

        int screenWidth = desktop->width();
        int screenHeight = desktop->height();

        int x = screenWidth*0.1;
        int y = screenHeight*0.1;

        resize(screenWidth*0.8, screenHeight*0.8);
        move(x, y);
    }
    v = settings()->value("State");
    if (v.isValid()){
        restoreState(v.toByteArray());
    }
    settings()->endGroup();
}


PreviewReportWindow::~PreviewReportWindow()
{
    if (m_ownedSettings)
        delete m_settings;
    delete m_previewPage;
    delete ui;
}

void PreviewReportWindow::initPreview(int pagesCount)
{
    m_pagesNavigator->setSuffix(tr(" of %1").arg(pagesCount));
    m_pagesNavigator->setMinimum(1);
    m_pagesNavigator->setMaximum(pagesCount);
    ui->graphicsView->setScene(m_previewPage);
    ui->graphicsView->centerOn(0, 0);
    ui->graphicsView->scale(0.5,0.5);
    m_currentPage=1;
    m_pagesNavigator->setValue(m_currentPage);
}

void PreviewReportWindow::setSettings(QSettings* value)
{
    if (m_ownedSettings)
        delete m_settings;
    m_settings=value;
    m_ownedSettings=false;
    restoreSetting();
}

void PreviewReportWindow::setErrorMessages(const QStringList &value){
    ui->actionShowMessages->setVisible(true);
    foreach (QString line, value) {
        ui->errorsView->append(line);
    }
}

QSettings*PreviewReportWindow::settings()
{
    if (m_settings){
        return m_settings;
    } else {
        m_settings = new QSettings("LimeReport",QApplication::applicationName());
        m_ownedSettings = true;
        return m_settings;
    }
}

void PreviewReportWindow::setReportReader(ItemsReaderIntf::Ptr reader)
{
    m_reader=reader;
    if (!reader.isNull()){
        if (reader->first()) reader->readItem(m_previewPage->pageItem());
        int pagesCount = reader->firstLevelItemsCount();
        m_previewPage->pageItem()->setItemMode(PreviewMode);
        initPreview(pagesCount);
    }
}

void PreviewReportWindow::setPages(ReportPages pages)
{
    m_reportPages = pages;
    if (!m_reportPages.isEmpty()){
        //m_previewPage->setPageItem(m_reportPages.at(0));
        m_previewPage->setPageItems(m_reportPages);
        m_changingPage = true;
        initPreview(m_reportPages.count());
        m_currentPage = 1;
        if (pages.at(0)) pages.at(0)->setSelected(true);
        m_changingPage = false;
    }
}

void PreviewReportWindow::exec()
{
    bool deleteOnClose = testAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_DeleteOnClose,false);
    setAttribute(Qt::WA_ShowModal,true);
    show();
    m_eventLoop.exec();
    if (deleteOnClose) delete this;
}

void PreviewReportWindow::closeEvent(QCloseEvent *)
{
#ifdef Q_OS_WIN
    writeSetting();
#endif
#ifdef Q_OS_MAC
    writeSetting();
#endif
    m_eventLoop.exit();
}

void PreviewReportWindow::resizeEvent(QResizeEvent* e)
{
#ifdef Q_OS_UNIX
    if (e->oldSize()!=e->size()){
        writeSetting();
    }
#else
    Q_UNUSED(e)
#endif
}

void PreviewReportWindow::moveEvent(QMoveEvent* e)
{
#ifdef Q_OS_UNIX
    if (e->oldPos()!=e->pos()){
        writeSetting();
    }
#else
    Q_UNUSED(e)
#endif
}

void PreviewReportWindow::slotPrint()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer,QApplication::activeWindow());
    if (dialog.exec()==QDialog::Accepted){
        if (!m_reportPages.isEmpty())
            ReportEnginePrivate::printReport(
                m_reportPages,
                printer,
                PrintRange(dialog.printRange(),dialog.fromPage(),dialog.toPage())
            );
        else ReportEnginePrivate::printReport(m_reader,printer);
        foreach(PageItemDesignIntf::Ptr pageItem, m_reportPages){
            m_previewPage->reactivatePageItem(pageItem);
        }

    }
}

void PreviewReportWindow::slotPriorPage()
{
    m_changingPage=true;
    if ((!m_reportPages.isEmpty())&&(m_currentPage>1)){
        m_currentPage--;
        //m_previewPage->setPageItem(m_reportPages.at(m_currentPage-1));
        ui->graphicsView->ensureVisible(
                    calcPageShift(m_reportPages.at(m_currentPage-1)), 0, 0
        );
        m_pagesNavigator->setValue(m_currentPage);
    } else {
        if (reader() && reader()->prior()){
            m_previewPage->removeAllItems();
            reader()->readItem(m_previewPage->pageItem());
            m_currentPage--;
            m_pagesNavigator->setValue(m_currentPage);
        }
    }
    m_changingPage=false;
}

void PreviewReportWindow::slotNextPage()
{
    m_changingPage=true;
    if ((!m_reportPages.isEmpty())&&(m_reportPages.count()>(m_currentPage))){
        m_currentPage++;
        //m_previewPage->setPageItem(m_reportPages.at(m_currentPage-1));
        ui->graphicsView->ensureVisible(
                    calcPageShift(m_reportPages.at(m_currentPage-1)), 0, 0
        );
        m_pagesNavigator->setValue(m_currentPage);
    } else {
        if (reader() && reader()->next()){
            m_previewPage->removeAllItems();
            reader()->readItem(m_previewPage->pageItem());
            m_currentPage++;
            m_pagesNavigator->setValue(m_currentPage);
        }
    }
    m_changingPage=false;
}

void PreviewReportWindow::slotZoomIn()
{
    ui->graphicsView->scale(1.2,1.2);
}

void PreviewReportWindow::slotZoomOut()
{
    ui->graphicsView->scale(1/1.2,1/1.2);
}

void PreviewReportWindow::slotPageNavigatorChanged(int value)
{
    if (m_changingPage) return;
    m_changingPage = true;
    if ((!m_reportPages.isEmpty())&&(m_reportPages.count() >= value)){
        m_currentPage = value;
        //m_previewPage->setPageItem(m_reportPages.at(m_currentPage-1));
        ui->graphicsView->ensureVisible(
                    calcPageShift(m_reportPages.at(m_currentPage-1)), 0, 0
        );
    } else {
        if (reader()){
            int direction = (m_currentPage>value)?-1:1;
            while (m_currentPage != value){
                if (direction == 1) reader()->next();
                else reader()->prior();
                m_currentPage += direction;
            }
            m_previewPage->removeAllItems();
            reader()->readItem(m_previewPage->pageItem());
            m_pagesNavigator->setValue(m_currentPage);
        }
    }
    m_changingPage=false;
}

void PreviewReportWindow::slotShowErrors()
{
    ui->errorsView->setVisible(ui->actionShowMessages->isChecked());
}

ItemsReaderIntf *PreviewReportWindow::reader()
{
    return m_reader.data();
}

bool PreviewReportWindow::pageIsVisible(PageItemDesignIntf::Ptr page)
{
    return page->mapToScene(page->rect()).boundingRect().intersects(
                ui->graphicsView->mapToScene(ui->graphicsView->viewport()->geometry()).boundingRect()
                );
}

QRectF PreviewReportWindow::calcPageShift(PageItemDesignIntf::Ptr page)
{
    qreal pageHeight = page->mapToScene(page->boundingRect()).boundingRect().height();
    qreal viewHeight = ui->graphicsView->mapToScene(
                0,ui->graphicsView->viewport()->height()
                ).y()-ui->graphicsView->mapToScene(0,0).y();
    viewHeight = (pageHeight<viewHeight)?pageHeight:viewHeight;
    QRectF pageStartPos =  m_reportPages.at(m_currentPage-1)->mapRectToScene(
                m_reportPages.at(m_currentPage-1)->rect()
                );
    return QRectF(0,pageStartPos.y(),0,viewHeight);
}

void PreviewReportWindow::on_actionSaveToFile_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Report file name"));
    if (!fileName.isEmpty()){
        QScopedPointer< ItemsWriterIntf > writer(new XMLWriter());
        foreach (PageItemDesignIntf::Ptr page, m_reportPages){
            writer->putItem(page.data());
        }
        writer->saveToFile(fileName);
    }
}

void PreviewReportWindow::slotFirstPage()
{
    m_changingPage=true;
    if ((!m_reportPages.isEmpty())&&(m_currentPage>1)){
        m_currentPage=1;
        //m_previewPage->setPageItem(m_reportPages.at(m_currentPage-1));
        ui->graphicsView->ensureVisible(
                    calcPageShift(m_reportPages.at(m_currentPage-1)), 0, 0
        );
        m_pagesNavigator->setValue(m_currentPage);
    } else {
        if (reader() && reader()->prior()){
            while (reader()->prior()) {
            }
            m_previewPage->removeAllItems();
            reader()->readItem(m_previewPage->pageItem());
            m_currentPage=1;
            m_pagesNavigator->setValue(m_currentPage);
        }
    }
    m_changingPage=false;
}

void PreviewReportWindow::slotLastPage()
{
    m_changingPage=true;
    if ((!m_reportPages.isEmpty())&&(m_reportPages.count()>(m_currentPage))){
        m_currentPage=m_reportPages.count();
        //m_previewPage->setPageItem(m_reportPages.at(m_currentPage-1));
        ui->graphicsView->ensureVisible(
                    calcPageShift(m_reportPages.at(m_currentPage-1)), 0, 0
        );
        m_pagesNavigator->setValue(m_currentPage);
    } else {

        if (reader() && reader()->next()){
            m_currentPage++;
            while (reader()->next()) {
                m_currentPage++;
            }
            m_previewPage->removeAllItems();
            reader()->readItem(m_previewPage->pageItem());
            m_pagesNavigator->setValue(m_currentPage);
        }
    }
    m_changingPage=false;
}

void PreviewReportWindow::slotPrintToPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("PDF file name"),"","PDF(*.pdf)" );
       qDebug()<<fileName;
        if (!fileName.isEmpty()){
            QPrinter printer;
            printer.setOutputFileName(fileName);
            printer.setOutputFormat(QPrinter::PdfFormat);

            if (!m_reportPages.isEmpty()){
                ReportEnginePrivate::printReport(m_reportPages,printer,PrintRange());
            } else {
                ReportEnginePrivate::printReport(m_reader,printer);
            }
            foreach(PageItemDesignIntf::Ptr pageItem, m_reportPages){
                m_previewPage->reactivatePageItem(pageItem);
            }
        }
}

void PreviewReportWindow::slotSliderMoved(int value)
{
    if (ui->graphicsView->verticalScrollBar()->minimum()==value){
        m_currentPage = 1;
    } else if (ui->graphicsView->verticalScrollBar()->maximum()==value){
        m_currentPage = m_reportPages.count();
    }

    if (!pageIsVisible(m_reportPages.at(m_currentPage-1))){
        if (value>m_priorScrolValue){
            m_currentPage++;
        } else {
            m_currentPage--;
        }
    }

    m_changingPage = true;
    m_pagesNavigator->setValue(m_currentPage);
    m_changingPage = false;
    m_priorScrolValue = value;

}

