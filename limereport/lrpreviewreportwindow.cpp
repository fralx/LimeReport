/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2021 by Alexander Arin                                  *
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
#include "items/editors/lrfonteditorwidget.h"
#include "items/editors/lrtextalignmenteditorwidget.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QScrollBar>
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
#include <QDesktopWidget>
#else
#include <QScreen>
#endif
#include <QLabel>
#include <QMessageBox>
#include <QToolButton>

namespace LimeReport{

PreviewReportWindow::PreviewReportWindow(ReportEngine *report, QWidget *parent, QSettings *settings, Qt::WindowFlags flags) :
    QMainWindow(parent,flags),
    ui(new Ui::PreviewReportWindow), m_settings(settings), m_ownedSettings(false),
    m_scalePercentChanging(false)
{
    ui->setupUi(this);

    m_progressWidget = new QWidget(ui->statusbar);
    QHBoxLayout* progressLayout = new QHBoxLayout();
    progressLayout->setContentsMargins(0, 0, 0, 0);
    progressLayout->addWidget(new QLabel(tr("Printing")));
    m_progressBar = new QProgressBar(ui->statusbar);
    m_progressBar->setMaximumWidth(100);
    m_progressBar->setMaximumHeight(ui->statusbar->fontMetrics().height());
    progressLayout->addWidget(m_progressBar);
    QToolButton* tbCancel = new QToolButton();
    tbCancel->setIcon(QIcon(":/report/images/closebox"));
    tbCancel->setAutoRaise(true);
    connect(tbCancel, SIGNAL(clicked(bool)), this, SLOT(slotCancelPrinting(bool)));
    progressLayout->addWidget(tbCancel);
    progressLayout->setSizeConstraint(QLayout::SetFixedSize);
    m_progressWidget->setLayout(progressLayout);
    m_progressWidget->setVisible(false);
    ui->statusbar->addPermanentWidget(m_progressWidget);

    setWindowTitle("Lime Report Preview");
    m_pagesNavigator = new QSpinBox(this);
    m_pagesNavigator->setMaximum(10000000);
    m_pagesNavigator->setPrefix(tr("Page: "));
    m_pagesNavigator->setMinimumWidth(120);
    ui->toolBar->insertWidget(ui->actionNextPage,m_pagesNavigator);
    ui->editModeTools->hide();
    ui->actionShowMessages->setVisible(false);

    connect(m_pagesNavigator,SIGNAL(valueChanged(int)),this,SLOT(slotPageNavigatorChanged(int)));
    m_previewReportWidget = new PreviewReportWidget(report,this);
    setCentralWidget(m_previewReportWidget);
    layout()->setContentsMargins(1,1,1,1);
    connect(m_previewReportWidget, SIGNAL(pageChanged(int)),
            this, SLOT(slotPageChanged(int)) );
    connect(m_previewReportWidget->d_ptr->m_previewPage, SIGNAL(selectionChanged()),
            this, SLOT(slotSelectionChanged()));
    connect(m_pagesNavigator, SIGNAL(valueChanged(int)),
            this, SLOT(slotPageNavigatorChanged(int)));
    connect(m_previewReportWidget, SIGNAL(onSave(bool&, LimeReport::IPreparedPages*)),
            this, SIGNAL(onSave(bool&, LimeReport::IPreparedPages*)));

    connect(m_previewReportWidget->d_ptr->m_report, SIGNAL(printingStarted(int)), this, SLOT(slotPrintingStarted(int)));
    connect(m_previewReportWidget->d_ptr->m_report, SIGNAL(pagePrintingFinished(int)), this, SLOT(slotPagePrintingFinished(int)));
    connect(m_previewReportWidget->d_ptr->m_report, SIGNAL(printingFinished()), this, SLOT(slotPrintingFinished()));

    m_fontEditor = new FontEditorWidgetForPage(m_previewReportWidget->d_ptr->m_previewPage,tr("Font"),this);
    m_fontEditor->setObjectName("fontTools");
    m_fontEditor->setIconSize(ui->toolBar->iconSize());
    m_textAlignmentEditor = new TextAlignmentEditorWidgetForPage(m_previewReportWidget->d_ptr->m_previewPage,tr("Text align"),this);
    m_textAlignmentEditor->setObjectName("textAlignmentTools");
    m_textAlignmentEditor->setIconSize(ui->toolBar->iconSize());
    addToolBar(Qt::TopToolBarArea,m_fontEditor);
    addToolBar(Qt::TopToolBarArea,m_textAlignmentEditor);

    m_scalePercent = new QComboBox(this);
    m_scalePercent->setEditable(true);
    ui->toolBar->insertWidget(ui->actionZoomOut, m_scalePercent);
    initPercentCombobox();
    
    connect(ui->actionShowMessages, SIGNAL(triggered()), this, SLOT(slotShowErrors()));
    connect(m_previewReportWidget, SIGNAL(scalePercentChanged(int)), this, SLOT(slotScalePercentChanged(int)));
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 3)
    connect(m_scalePercent, SIGNAL(currentTextChanged(QString)), this, SLOT(scaleComboboxChanged(QString)));
#else
    connect(m_scalePercent, SIGNAL(currentIndexChanged(QString)), this, SLOT(scaleComboboxChanged(QString)));
#endif
    connect(m_previewReportWidget, SIGNAL(pageChanged(int)), this, SLOT(slotCurrentPageChanged(int)));
    connect(m_previewReportWidget, SIGNAL(itemInserted(LimeReport::PageDesignIntf*, QPointF, QString)),
            this, SLOT(slotItemInserted(LimeReport::PageDesignIntf*, QPointF, QString)));

    restoreSetting();
    selectStateIcon();
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
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
        QDesktopWidget *desktop = QApplication::desktop();

        int screenWidth = desktop->screenGeometry().width();
        int screenHeight = desktop->screenGeometry().height();
#else
        QScreen *screen = QGuiApplication::primaryScreen();

        int screenWidth = screen->geometry().width();
        int screenHeight = screen->geometry().height();
#endif
        int x = static_cast<int>(screenWidth*0.1);
        int y = static_cast<int>(screenHeight*0.1);

        resize(
            static_cast<int>(screenWidth*0.8),
            static_cast<int>(screenHeight*0.8)
        );
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

void PreviewReportWindow::reloadPreview()
{
   m_previewReportWidget->refreshPages();
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

void PreviewReportWindow::setToolBarVisible(bool value)
{
    ui->toolBar->setHidden(value);
    selectStateIcon();
}

void PreviewReportWindow::setStatusBarVisible(bool value)
{
    ui->statusbar->setVisible(value);
}

void PreviewReportWindow::setMenuVisible(bool value)
{
    ui->menubar->setVisible(value);
}

void PreviewReportWindow::setHideResultEditButton(bool value)
{
    ui->actionEdit_Mode->setVisible(value);
    if (!value && ui->editModeTools) {
        delete ui->editModeTools;
        ui->editModeTools = 0;
    }
}

void PreviewReportWindow::setHidePrintButton(bool value)
{
    ui->actionPrint->setVisible(value);
}

void PreviewReportWindow::setHideSaveToFileButton(bool value)
{
    ui->actionSaveToFile->setVisible(value);
}

void PreviewReportWindow::setHidePrintToPdfButton(bool value)
{
    ui->actionPrint_To_PDF->setVisible(value);
}

void PreviewReportWindow::setEnablePrintMenu(bool value)
{
    ui->menuReport->setEnabled(value);
}

QSettings*PreviewReportWindow::settings()
{
    if (m_settings){
        return m_settings;
    } else {
        m_settings = new QSettings("LimeReport",QCoreApplication::applicationName());
        m_ownedSettings = true;
        return m_settings;
    }
}

void PreviewReportWindow::setReportReader(ItemsReaderIntf::Ptr /*reader*/)
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

void PreviewReportWindow::setDefaultPrinter(QPrinter *printer)
{
    m_previewReportWidget->setDefaultPrinter(printer);
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

void PreviewReportWindow::closeEvent(QCloseEvent* e)
{
    if (m_progressBar->isVisible()){
        QMessageBox::critical(this, tr("Attention"), tr("The printing is in process"));
        e->setAccepted(false);
    }
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

void PreviewReportWindow::showEvent(QShowEvent *)
{
    m_fontEditor->setVisible(ui->actionEdit_Mode->isChecked());
    if (ui->editModeTools)
        ui->editModeTools->setVisible(false);
    m_textAlignmentEditor->setVisible(ui->actionEdit_Mode->isChecked());
    switch (m_previewScaleType) {
    case FitWidth:
        m_previewReportWidget->fitWidth();
        break;
    case FitPage:
        m_previewReportWidget->fitPage();
        break;
    case OneToOne:
        m_previewReportWidget->setScalePercent(100);
        break;
    case Percents:
        m_previewReportWidget->setScalePercent(m_previewScalePercent);

    }
}

void PreviewReportWindow::selectStateIcon()
{
    if (ui->toolBar->isHidden()){
        ui->actionShow_Toolbar->setIcon(QIcon(":/report/images/not_checked.png"));
    } else {
        ui->actionShow_Toolbar->setIcon(QIcon(":/report/images/checked.png"));
    }
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

void PreviewReportWindow::on_actionEdit_Mode_triggered(bool checked)
{
    m_previewReportWidget->d_ptr->m_previewPage->setItemMode((checked)?ItemModes(DesignMode):PreviewMode);
    m_textAlignmentEditor->setVisible(checked);
    m_fontEditor->setVisible(checked);
    if (checked)
        ui->editModeTools->show();
    else ui->editModeTools->hide();
}

void PreviewReportWindow::slotSelectionChanged()
{
    QGraphicsScene* page=dynamic_cast<QGraphicsScene*>(sender());
    if (page){
        if (page->selectedItems().count()==1){
            BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(page->selectedItems().at(0));
            if (item) {
                m_fontEditor->setItem(item);
                m_textAlignmentEditor->setItem(item);
            }
        }
    }
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

ScaleType PreviewReportWindow::previewScaleType() const
{
    return m_previewScaleType;
}

void PreviewReportWindow::setPreviewScaleType(const ScaleType &previewScaleType, int percent)
{
    m_previewScaleType = previewScaleType;
    m_previewScalePercent = percent;
    m_previewReportWidget->setScaleType(previewScaleType, percent);
}

QColor PreviewReportWindow::previewPageBackgroundColor()
{
    return m_previewReportWidget->previewPageBackgroundColor();
}

void PreviewReportWindow::setPreviewPageBackgroundColor(QColor color)
{
    m_previewReportWidget->setPreviewPageBackgroundColor(color);
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

void PreviewReportWindow::slotInsertNewTextItem()
{
    m_previewReportWidget->startInsertTextItem();
    ui->actionSelection_Mode->setChecked(false);
}

void PreviewReportWindow::slotActivateItemSelectionMode()
{
    m_previewReportWidget->activateItemSelectionMode();
    ui->actionSelection_Mode->setChecked(true);
    ui->actionInsertTextItem->setChecked(false);
}

void PreviewReportWindow::slotDeleteSelectedItems()
{
    m_previewReportWidget->deleteSelectedItems();
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
    if (m_scalePercentChanging) return;
    m_scalePercentChanging = true;
    m_previewReportWidget->setScalePercent(text.remove(text.count()-1,1).toInt());
    m_scalePercentChanging = false;
}

void PreviewReportWindow::slotScalePercentChanged(int percent)
{
    if (m_scalePercentChanging) return;
    m_scalePercentChanging = true;
    if (m_scalePercent->findText(QString("%1%").arg(percent)) == -1)
        m_scalePercent->setCurrentIndex(-1);
    m_scalePercent->setEditText(QString("%1%").arg(percent));
    m_scalePercentChanging = false;
}

void PreviewReportWindow::on_actionShowMessages_toggled(bool value)
{
   m_previewReportWidget->setErrorsMesagesVisible(value);
}

void PreviewReportWindow::on_actionShow_Toolbar_triggered()
{
    setToolBarVisible(!ui->toolBar->isHidden());
    writeSetting();
}

void PreviewReportWindow::slotCurrentPageChanged(int /*page*/)
{
    slotActivateItemSelectionMode();
}

void PreviewReportWindow::slotItemInserted(PageDesignIntf *, QPointF, const QString&)
{
    slotActivateItemSelectionMode();
}

void PreviewReportWindow::slotPrintingStarted(int pageCount)
{
    m_progressBar->setMinimum(1);
    m_progressBar->setMaximum(pageCount);
    m_progressWidget->setVisible(true);
}

void PreviewReportWindow::slotPagePrintingFinished(int pageIndex)
{
    m_progressBar->setValue(pageIndex);
}

void PreviewReportWindow::slotPrintingFinished()
{
    m_progressWidget->setVisible(false);
}

void PreviewReportWindow::slotCancelPrinting(bool)
{
    m_previewReportWidget->d_ptr->m_report->cancelPrinting();
}

}// namespace LimeReport

