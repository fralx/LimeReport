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
#include "lrpreviewreportwidget.h"
#include "lrpreviewreportwidget_p.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QScrollBar>
#include <QDesktopWidget>

namespace LimeReport{

PreviewReportWindow::PreviewReportWindow(ReportEnginePrivate *report,QWidget *parent, QSettings *settings, Qt::WindowFlags flags) :
    QMainWindow(parent,flags),
    ui(new Ui::PreviewReportWindow), m_settings(settings), m_ownedSettings(false)
{
    ui->setupUi(this);
    setWindowTitle("Lime Report Preview");
    m_pagesNavigator = new QSpinBox(this);
    m_pagesNavigator->setMaximum(10000000);
    m_pagesNavigator->setPrefix(tr("Page: "));
    m_pagesNavigator->setMinimumWidth(120);
    ui->toolBar->insertWidget(ui->actionNextPage,m_pagesNavigator);
    ui->actionShowMessages->setVisible(false);

    connect(m_pagesNavigator,SIGNAL(valueChanged(int)),this,SLOT(slotPageNavigatorChanged(int)));
    m_previewReportWidget = new PreviewReportWidget(report,this);
    setCentralWidget(m_previewReportWidget);
    layout()->setContentsMargins(1,1,1,1);
    connect(m_previewReportWidget,SIGNAL(pageChanged(int)), this,SLOT(slotPageChanged(int)) );


    m_scalePercent = new QComboBox(this);
    m_scalePercent->setEditable(true);
    ui->toolBar->insertWidget(ui->actionZoomOut, m_scalePercent);
    initPercentCombobox();
    connect(m_previewReportWidget, SIGNAL(scalePercentChanged(int)), this, SLOT(slotScalePercentChanged(int)));
    connect(m_scalePercent, SIGNAL(currentIndexChanged(QString)), this, SLOT(scaleComboboxChanged(QString)));
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

        int screenWidth = desktop->screenGeometry().width();
        int screenHeight = desktop->screenGeometry().height();

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
    //delete m_previewPage;
    delete ui;
}

void PreviewReportWindow::initPreview(int pagesCount)
{
    m_pagesNavigator->setSuffix(tr(" of %1").arg(pagesCount));
    m_pagesNavigator->setMinimum(1);
    m_pagesNavigator->setMaximum(pagesCount);
    m_pagesNavigator->setValue(1);
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
    m_previewReportWidget->setErrorMessages(value);
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
//    m_reader=reader;
//    if (!reader.isNull()){
//        if (reader->first()) reader->readItem(m_previewPage->pageItem());
//        int pagesCount = reader->firstLevelItemsCount();
//        m_previewPage->pageItem()->setItemMode(PreviewMode);
//        initPreview(pagesCount);
//    }
}

void PreviewReportWindow::setPages(ReportPages pages)
{
    m_previewReportWidget->d_ptr->setPages(pages);
    if (!pages.isEmpty()){
        initPreview(pages.count());
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
    m_previewReportWidget->print();
}

void PreviewReportWindow::slotPriorPage()
{
    m_previewReportWidget->priorPage();
}

void PreviewReportWindow::slotNextPage()
{
    m_previewReportWidget->nextPage();
}

void PreviewReportWindow::slotZoomIn()
{
    m_previewReportWidget->zoomIn();
}

void PreviewReportWindow::slotZoomOut()
{
    m_previewReportWidget->zoomOut();
}

void PreviewReportWindow::slotPageNavigatorChanged(int value)
{
    m_previewReportWidget->pageNavigatorChanged(value);
}

void PreviewReportWindow::slotShowErrors()
{
    m_previewReportWidget->setErrorsMesagesVisible(ui->actionShowMessages->isChecked());
}

ItemsReaderIntf *PreviewReportWindow::reader()
{
    return m_reader.data();
}

void PreviewReportWindow::initPercentCombobox()
{
    for (int i = 10; i<310; i+=10){
        m_scalePercent->addItem(QString("%1%").arg(i));
    }
    m_scalePercent->setCurrentIndex(4);
}

void PreviewReportWindow::on_actionSaveToFile_triggered()
{
    m_previewReportWidget->saveToFile();
}

void PreviewReportWindow::slotFirstPage()
{
    m_previewReportWidget->firstPage();
}

void PreviewReportWindow::slotLastPage()
{
    m_previewReportWidget->lastPage();
}

void PreviewReportWindow::slotPrintToPDF()
{
    m_previewReportWidget->printToPDF();
}

void PreviewReportWindow::slotPageChanged(int pageIndex)
{
    m_pagesNavigator->setValue(pageIndex);
}

void PreviewReportWindow::on_actionFit_page_width_triggered()
{
    m_previewReportWidget->fitWidth();
}

void PreviewReportWindow::on_actionFit_page_triggered()
{
    m_previewReportWidget->fitPage();
}

void PreviewReportWindow::on_actionOne_to_one_triggered()
{
    m_previewReportWidget->setScalePercent(100);
}

void PreviewReportWindow::scaleComboboxChanged(QString text)
{
    m_previewReportWidget->setScalePercent(text.remove(text.count()-1,1).toInt());
}

void PreviewReportWindow::slotScalePercentChanged(int percent)
{
    m_scalePercent->setEditText(QString("%1%").arg(percent));
}

void PreviewReportWindow::on_actionShowMessages_toggled(bool value)
{
   m_previewReportWidget->setErrorsMesagesVisible(value);
}

}// namespace LimeReport