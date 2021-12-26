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
#include <QToolBar>
#include <QAction>
#include <QDebug>
#include <QMessageBox>
#include <QToolButton>
#include <QDockWidget>
#include <QStatusBar>
#include <QFileDialog>
#include <QApplication>
#include <QMenuBar>
#include <QCheckBox>
#include <QVBoxLayout>
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
#include <QDesktopWidget>
#else
#include <QScreen>
#endif
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QPushButton>

#include "lrreportdesignwindow.h"
#include "lrbandsmanager.h"
#include "lrobjectinspectorwidget.h"
#include "lrbasedesignobjectmodel.h"
#include "lrreportdesignwidget.h"
#include "lrdatabrowser.h"
#include "scriptbrowser/lrscriptbrowser.h"
#include "lrbasedesignintf.h"
#include "lrpagedesignintf.h"

#include "lrpreviewreportwindow.h"
#include "serializators/lrstorageintf.h"
#include "serializators/lrxmlreader.h"
#include "objectsbrowser/lrobjectbrowser.h"
#include "lraboutdialog.h"


namespace LimeReport{

ReportDesignWindow* ReportDesignWindow::m_instance=0;

void ReportDesignWindow::createProgressBar()
{
    m_progressWidget = new QWidget(m_statusBar);
    QHBoxLayout* progressLayout = new QHBoxLayout();
    progressLayout->setContentsMargins(0, 0, 0, 0);
    m_progressLabel = new QLabel(tr("Rendered %1 pages").arg(0));
    progressLayout->addWidget(m_progressLabel);
    m_progressBar = new QProgressBar(m_statusBar);
    m_progressBar->setFormat("%v pages");
    m_progressBar->setAlignment(Qt::AlignCenter);
    m_progressBar->setMaximumWidth(100);
    m_progressBar->setMaximumHeight(m_statusBar->fontMetrics().height());
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(0);
    m_progressBar->setTextVisible(true);
    progressLayout->addWidget(m_progressBar);
    QToolButton* tbCancel = new QToolButton();
    tbCancel->setToolTip(tr("Cancel report rendering"));
    tbCancel->setIcon(QIcon(":/report/images/closebox"));
    tbCancel->setAutoRaise(true);
    connect(tbCancel, SIGNAL(clicked(bool)), this, SLOT(slotCancelRendering(bool)));
    progressLayout->addWidget(tbCancel);
    progressLayout->setSizeConstraint(QLayout::SetFixedSize);
    m_progressWidget->setLayout(progressLayout);
    m_progressWidget->setVisible(false);
    m_statusBar->addPermanentWidget(m_progressWidget);

    connect(dynamic_cast<QObject*>(m_reportDesignWidget->report()), SIGNAL(renderStarted()), this, SLOT(renderStarted()));
    connect(dynamic_cast<QObject*>(m_reportDesignWidget->report()), SIGNAL(renderPageFinished(int)), this, SLOT(renderPageFinished(int)));
    connect(dynamic_cast<QObject*>(m_reportDesignWidget->report()), SIGNAL(renderFinished()), this, SLOT(renderFinished()));
}

ReportDesignWindow::ReportDesignWindow(ReportEnginePrivateInterface* report, QWidget *parent, QSettings* settings) :
    ReportDesignWindowInterface(parent), m_textAttibutesIsChanging(false), m_settings(settings), m_ownedSettings(false),
    m_progressDialog(0), m_showProgressDialog(true), m_editorTabType(ReportDesignWidget::Page), m_reportItemIsLocked(false)
{
    initReportEditor(report);
    createActions();
    createMainMenu();
    createToolBars();
    createObjectInspector();
    createDataWindow();
    createScriptWindow();
    createObjectsBrowser();

#ifdef HAVE_QTDESIGNER_INTEGRATION
    createDialogWidgetBox();
    createDialogPropertyEditor();
    createDialogObjectInspector();
    createDialogActionEditor();
    createDialogSignalSlotEditor();
    createDialogResourceEditor();
    createDialogDesignerToolBar();
#endif
    m_instance=this;
    m_statusBar=new QStatusBar(this);
    m_lblReportName = new QLabel(report->reportFileName(),this);
    m_statusBar->insertWidget(0,m_lblReportName);
    setStatusBar(m_statusBar);

    QString windowTitle = "Lime Report Designer";
    if (!report->reportName().isEmpty())
        windowTitle = report->reportName() + " - " + windowTitle;
    setWindowTitle(windowTitle);
    showDefaultEditors();
    showDefaultToolBars();
    restoreSetting();
    m_hideLeftPanel->setChecked(isDockAreaVisible(Qt::LeftDockWidgetArea));
    m_hideRightPanel->setChecked(isDockAreaVisible(Qt::RightDockWidgetArea));
    createProgressBar();
    m_deletePageAction->setEnabled(report->pageCount()>1);
}

ReportDesignWindow::~ReportDesignWindow()
{
    m_instance=0;
    delete m_validator;
    if (m_ownedSettings&&m_settings) delete m_settings;
}

void ReportDesignWindow::createActions()
{
    m_newReportAction = new QAction(tr("New Report"),this);
    m_newReportAction->setIcon(QIcon(":/report/images/newReport"));
    m_newReportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    connect(m_newReportAction,SIGNAL(triggered()),this,SLOT(slotNewReport()));

    m_newPageAction = new QAction(tr("New Report Page"),this);
    m_newPageAction->setIcon(QIcon(":/report/images/addPage"));
    connect(m_newPageAction,SIGNAL(triggered()),this,SLOT(slotNewPage()));

    m_deletePageAction = new QAction(tr("Delete Report Page"),this);
    m_deletePageAction->setIcon(QIcon(":/report/images/deletePage"));
    connect(m_deletePageAction,SIGNAL(triggered()),this,SLOT(slotDeletePage()));
    m_deletePageAction->setEnabled(false);

    m_editModeAction = new QAction(tr("Edit Mode"),this);
    m_editModeAction->setIcon(QIcon(":/report/images/editMode"));
    m_editModeAction->setCheckable(true);
    m_editModeAction->setChecked(true);
    //m_editModeAction->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(m_editModeAction,SIGNAL(triggered()),this,SLOT(slotEditMode()));

    m_undoAction = new QAction(tr("Undo"),this);
    m_undoAction->setIcon(QIcon(":/report/images/undo"));
    m_undoAction->setEnabled(false);
    m_undoAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z));
    connect(m_undoAction,SIGNAL(triggered()),this,SLOT(slotUndo()));

    m_redoAction = new QAction(tr("Redo"),this);
    m_redoAction->setIcon(QIcon(":/report/images/redo"));
    m_redoAction->setEnabled(false);
    m_redoAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Z));
    connect(m_redoAction,SIGNAL(triggered()),this,SLOT(slotRedo()));

    m_copyAction = new QAction(tr("Copy"),this);
    m_copyAction->setIcon(QIcon(":/report/images/copy"));
    m_copyAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    connect(m_copyAction,SIGNAL(triggered()),this,SLOT(slotCopy()));

    m_pasteAction = new QAction(tr("Paste"),this);
    m_pasteAction->setIcon(QIcon(":/report/images/paste"));
    m_pasteAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_V));
    connect(m_pasteAction,SIGNAL(triggered()),this,SLOT(slotPaste()));

    m_cutAction = new QAction(tr("Cut"),this);
    m_cutAction->setIcon(QIcon(":/report/images/cut"));
    m_cutAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_X));
    connect(m_cutAction,SIGNAL(triggered()),this,SLOT(slotCut()));

    m_settingsAction = new QAction(tr("Settings"),this);
    m_settingsAction->setIcon(QIcon(":/report/images/settings"));
    connect(m_settingsAction,SIGNAL(triggered()),this,SLOT(slotEditSettings()));

    m_useGridAction = new QAction(tr("Use grid"),this);
    m_useGridAction->setIcon(QIcon(":/report/images/grid"));
    m_useGridAction->setCheckable(true);
    m_useGridAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    connect(m_useGridAction,SIGNAL(toggled(bool)),this,SLOT(slotUseGrid(bool)));

    m_useMagnetAction = new QAction(tr("Use magnet"),this);
    m_useMagnetAction->setIcon(QIcon(":/report/images/magnet"));
    m_useMagnetAction->setCheckable(true);
    m_useMagnetAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_M));
    connect(m_useMagnetAction,SIGNAL(toggled(bool)),this,SLOT(slotUseMagnet(bool)));

    
    m_newTextItemAction = new QAction(tr("Text Item"),this);
    m_newTextItemAction->setIcon(QIcon(":/items/TextItem"));
    m_actionMap.insert("TextItem",m_newTextItemAction);
    connect(m_newTextItemAction,SIGNAL(triggered()),this,SLOT(slotNewTextItem()));

    m_saveReportAction = new QAction(tr("Save Report"),this);
    m_saveReportAction->setIcon(QIcon(":/report/images/save"));
    m_saveReportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    connect(m_saveReportAction,SIGNAL(triggered()),this,SLOT(slotSaveReport()));

    m_saveReportAsAction = new QAction(tr("Save Report As"),this);
    m_saveReportAsAction->setIcon(QIcon(":/report/images/saveas"));
    m_saveReportAsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    connect(m_saveReportAsAction,SIGNAL(triggered()),this,SLOT(slotSaveReportAs()));

    m_loadReportAction = new QAction(tr("Load Report"),this);
    m_loadReportAction->setIcon(QIcon(":/report/images/folder"));
    m_loadReportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    connect(m_loadReportAction,SIGNAL(triggered()),this,SLOT(slotLoadReport()));

    m_deleteItemAction = new QAction(tr("Delete item"),this);
    m_deleteItemAction->setShortcut(QKeySequence("Del"));
    m_deleteItemAction->setIcon(QIcon(":/report/images/delete"));
    connect(m_deleteItemAction,SIGNAL(triggered()),this,SLOT(slotDelete()));

    m_zoomInReportAction = new QAction(tr("Zoom In"),this);
    m_zoomInReportAction->setIcon(QIcon(":/report/images/zoomIn"));
    connect(m_zoomInReportAction,SIGNAL(triggered()),this,SLOT(slotZoomIn()));

    m_zoomOutReportAction = new QAction(tr("Zoom Out"),this);
    m_zoomOutReportAction->setIcon(QIcon(":/report/images/zoomOut"));
    connect(m_zoomOutReportAction,SIGNAL(triggered()),this,SLOT(slotZoomOut()));

    m_previewReportAction = new QAction(tr("Render Report"),this);
    m_previewReportAction->setIcon(QIcon(":/report/images/render"));
    m_previewReportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
    connect(m_previewReportAction,SIGNAL(triggered()),this,SLOT(slotPreviewReport()));

    m_testAction = new QAction("test",this);
    m_testAction->setIcon(QIcon(":/report/images/pin"));
    connect(m_testAction,SIGNAL(triggered()),this,SLOT(slotTest()));

    m_editLayoutMode = new QAction(tr("Edit layouts mode"),this);
    m_editLayoutMode->setIcon(QIcon(":/report/images/editlayout"));
    m_editLayoutMode->setCheckable(true);
    connect(m_editLayoutMode,SIGNAL(triggered()),this,SLOT(slotEditLayoutMode()));

    m_addHLayout = new QAction(tr("Horizontal layout"),this);
    m_addHLayout->setIcon(QIcon(":/report/images/hlayout"));
    connect(m_addHLayout,SIGNAL(triggered()),this,SLOT(slotHLayout()));

    m_addVLayout = new QAction(tr("Vertical layout"),this);
    m_addVLayout->setIcon(QIcon(":/report/images/vlayout"));
    connect(m_addVLayout,SIGNAL(triggered()),this,SLOT(slotVLayout()));

    m_aboutAction = new QAction(tr("About"),this);
    m_aboutAction->setIcon(QIcon(":/report/images/copyright"));
    connect(m_aboutAction,SIGNAL(triggered()),this,SLOT(slotShowAbout()));

    m_hideLeftPanel = new QAction(tr("Hide left panel | Alt+L"),this);
    m_hideLeftPanel->setCheckable(true);
    m_hideLeftPanel->setIcon(QIcon(":/report/images/hideLeftPanel"));
    m_hideLeftPanel->setShortcut(QKeySequence(Qt::ALT | Qt::Key_L));
    connect(m_hideLeftPanel,SIGNAL(toggled(bool)), this, SLOT(slotHideLeftPanel(bool)));

    m_hideRightPanel = new QAction(tr("Hide right panel | Alt+R"),this);
    m_hideRightPanel->setCheckable(true);
    m_hideRightPanel->setIcon(QIcon(":/report/images/hideRightPanel"));
    m_hideRightPanel->setShortcut(QKeySequence(Qt::ALT | Qt::Key_R));
    connect(m_hideRightPanel,SIGNAL(toggled(bool)), this, SLOT(slotHideRightPanel(bool)));
#ifdef HAVE_QTDESIGNER_INTEGRATION
    m_deleteDialogAction = new QAction(tr("Delete dialog"), this);
    m_deleteDialogAction->setIcon(QIcon(":/report//images/deleteDialog"));
    connect(m_deleteDialogAction, SIGNAL(triggered()), this, SLOT(slotDeleteDialog()));

    m_addNewDialogAction = new QAction(tr("Add new dialog"), this);
    m_addNewDialogAction->setIcon(QIcon(":/report//images/addDialog"));
    connect(m_addNewDialogAction, SIGNAL(triggered()), this, SLOT(slotAddNewDialog()));
#endif

    m_lockSelectedItemsAction = new QAction(tr("Lock selected items"), this);
    m_lockSelectedItemsAction->setIcon(QIcon(":/report/images/lock"));
    m_lockSelectedItemsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    connect(m_lockSelectedItemsAction, SIGNAL(triggered()),
            this, SLOT(slotLockSelectedItems()));

    m_unlockSelectedItemsAction = new QAction(tr("Unlock selected items"), this);
    m_unlockSelectedItemsAction->setIcon(QIcon(":/report/images/unlock"));
    m_unlockSelectedItemsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_L));
    connect(m_unlockSelectedItemsAction, SIGNAL(triggered()),
            this, SLOT(slotUnlockSelectedItems()));

    m_selectOneLevelItems = new QAction(tr("Select one level items"), this);
    //m_unlockSelectedItemsAction->setIcon(QIcon(":/report/images/unlock"));
    m_selectOneLevelItems->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_A));
    connect(m_selectOneLevelItems, SIGNAL(triggered()),
            this, SLOT(slotSelectOneLevelItems()));
}

void ReportDesignWindow::createReportToolBar()
{
    m_reportToolBar = new QToolBar(tr("Report Tools"),this);
    m_reportToolBar->setIconSize(QSize(24,24));
    m_reportToolBar->addAction(m_editModeAction);
    m_reportToolBar->addWidget(m_newBandButton);
    m_reportToolBar->addAction(m_newTextItemAction);
    m_reportToolBar->setObjectName("reportTools");
    createItemsActions();
    m_reportToolBar->addSeparator();
    m_reportToolBar->addAction(m_addHLayout);
    m_reportToolBar->addAction(m_addVLayout);
    m_reportToolBar->addSeparator();
    m_reportToolBar->addAction(m_deleteItemAction);

    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_reportToolBar->addWidget(empty);

    m_reportToolBar->addAction(m_hideLeftPanel);
    m_reportToolBar->addAction(m_hideRightPanel);
    addToolBar(Qt::LeftToolBarArea,m_reportToolBar);

}

void ReportDesignWindow::createToolBars()
{
    createBandsButton();

    m_mainToolBar = addToolBar(tr("Main Tools"));
    m_mainToolBar->setIconSize(QSize(16,16));
    m_mainToolBar->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea | Qt::TopToolBarArea );
    m_mainToolBar->setFloatable(false);
    m_mainToolBar->setObjectName("mainTools");

    m_mainToolBar->addAction(m_newReportAction);
    m_mainToolBar->addAction(m_loadReportAction);
    m_mainToolBar->addAction(m_saveReportAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_settingsAction);
    m_mainToolBar->addSeparator();

    m_mainToolBar->addAction(m_newPageAction);
    m_mainToolBar->addAction(m_deletePageAction);
#ifdef HAVE_QTDESIGNER_INTEGRATION
    m_mainToolBar->addAction(m_addNewDialogAction);
#endif
    m_mainToolBar->addSeparator();

    m_mainToolBar->addAction(m_copyAction);
    m_mainToolBar->addAction(m_pasteAction);
    m_mainToolBar->addAction(m_cutAction);
    m_mainToolBar->addAction(m_undoAction);
    m_mainToolBar->addAction(m_redoAction);
    m_mainToolBar->addSeparator();

    m_mainToolBar->addAction(m_zoomInReportAction);
    m_mainToolBar->addAction(m_zoomOutReportAction); 
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_previewReportAction);
    //m_mainToolBar->addSeparator();
    //m_mainToolBar->addAction(m_useGridAction);

    //m_mainToolBar->addAction(m_printReportAction);

    m_fontEditorBar = new FontEditorWidgetForDesigner(m_reportDesignWidget,tr("Font"),this);
    m_fontEditorBar->setIconSize(m_mainToolBar->iconSize());
    m_fontEditorBar->setObjectName("fontTools");
    addToolBar(m_fontEditorBar);
    m_textAlignmentEditorBar = new TextAlignmentEditorWidgetForDesigner(m_reportDesignWidget,tr("Text alignment"),this);
    m_textAlignmentEditorBar->setIconSize(m_mainToolBar->iconSize());
    m_textAlignmentEditorBar->setObjectName("textAlignmentTools");
    addToolBar(m_textAlignmentEditorBar);
    m_itemsAlignmentEditorBar = new ItemsAlignmentEditorWidget(m_reportDesignWidget,tr("Items alignment"),this);
    m_itemsAlignmentEditorBar->setIconSize(m_mainToolBar->iconSize());
    m_itemsAlignmentEditorBar->setObjectName("itemsAlignmentTools");
    m_itemsAlignmentEditorBar->insertAction(m_itemsAlignmentEditorBar->actions().at(0),m_useGridAction);
    m_itemsAlignmentEditorBar->insertAction(m_itemsAlignmentEditorBar->actions().at(1),m_useMagnetAction);
    m_itemsAlignmentEditorBar->insertSeparator(m_itemsAlignmentEditorBar->actions().at(2));
    addToolBar(m_itemsAlignmentEditorBar);
    m_itemsBordersEditorBar = new ItemsBordersEditorWidgetForDesigner(m_reportDesignWidget,tr("Borders"),this);
    m_itemsBordersEditorBar->setIconSize(m_mainToolBar->iconSize());
    m_itemsBordersEditorBar->setObjectName("itemsBorderTools");
    addToolBar(m_itemsBordersEditorBar);

    createReportToolBar();

    m_pageTools << m_mainToolBar << m_reportToolBar << m_fontEditorBar
                << m_textAlignmentEditorBar << m_itemsAlignmentEditorBar
                << m_itemsBordersEditorBar;

}

void ReportDesignWindow::createItemsActions()
{
    foreach(ItemAttribs items,DesignElementsFactory::instance().attribsMap().values()){
        if (items.m_tag.compare("Item",Qt::CaseInsensitive)==0){
            QAction* tmpAction = new QAction(QObject::tr(items.m_alias.toLatin1()),this);
            tmpAction->setWhatsThis(DesignElementsFactory::instance().attribsMap().key(items));
            tmpAction->setIcon(QIcon(":/items/"+tmpAction->whatsThis()));
            connect(tmpAction,SIGNAL(triggered()),this,SLOT(slotItemActionCliked()));
            m_reportToolBar->addAction(tmpAction);
            m_actionMap.insert(tmpAction->whatsThis(),tmpAction);
        }
    }
}

void ReportDesignWindow::createBandsButton()
{
    m_newBandButton = new QToolButton(this);
    m_newBandButton->setPopupMode(QToolButton::InstantPopup);
    m_newBandButton->setIcon(QIcon(":/report/images/addBand"));
    m_newBandButton->setToolTip(tr("Report bands"));

    m_bandsAddSignalsMap = new QSignalMapper(this);

    m_newReportHeader=new QAction(QIcon(),tr("Report Header"),this);
    connect(m_newReportHeader,SIGNAL(triggered()),m_bandsAddSignalsMap,SLOT(map()));
    m_bandsAddSignalsMap->setMapping(m_newReportHeader,BandDesignIntf::ReportHeader);
    m_newBandButton->addAction(m_newReportHeader);

    m_newReportFooter=new QAction(QIcon(),tr("Report Footer"),this);
    connect(m_newReportFooter,SIGNAL(triggered()),m_bandsAddSignalsMap,SLOT(map()));
    m_bandsAddSignalsMap->setMapping(m_newReportFooter,BandDesignIntf::ReportFooter);
    m_newBandButton->addAction(m_newReportFooter);

    m_newPageHeader=new QAction(QIcon(),tr("Page Header"),this);
    connect(m_newPageHeader,SIGNAL(triggered()),m_bandsAddSignalsMap,SLOT(map()));
    m_bandsAddSignalsMap->setMapping(m_newPageHeader,BandDesignIntf::PageHeader);
    m_newBandButton->addAction(m_newPageHeader);

    m_newPageFooter=new QAction(QIcon(),tr("Page Footer"),this);
    connect(m_newPageFooter,SIGNAL(triggered()),m_bandsAddSignalsMap,SLOT(map()));
    m_bandsAddSignalsMap->setMapping(m_newPageFooter,BandDesignIntf::PageFooter);
    m_newBandButton->addAction(m_newPageFooter);

    m_newData=new QAction(QIcon(),tr("Data"),this);
    connect(m_newData,SIGNAL(triggered()),m_bandsAddSignalsMap,SLOT(map()));
    m_bandsAddSignalsMap->setMapping(m_newData,BandDesignIntf::Data);
    m_newBandButton->addAction(m_newData);

    m_newDataHeader=new QAction(QIcon(),tr("Data Header"),this);
    m_newDataHeader->setEnabled(false);
    connect(m_newDataHeader,SIGNAL(triggered()),m_bandsAddSignalsMap,SLOT(map()));
    m_bandsAddSignalsMap->setMapping(m_newDataHeader,BandDesignIntf::DataHeader);
    m_newBandButton->addAction(m_newDataHeader);

    m_newDataFooter=new QAction(QIcon(),tr("Data Footer"),this);
    m_newDataFooter->setEnabled(false);
    connect(m_newDataFooter,SIGNAL(triggered()),m_bandsAddSignalsMap,SLOT(map()));
    m_bandsAddSignalsMap->setMapping(m_newDataFooter,BandDesignIntf::DataFooter);
    m_newBandButton->addAction(m_newDataFooter);

    m_newSubDetail=new QAction(QIcon(),tr("SubDetail"),this);
    m_newSubDetail->setEnabled(false);
    connect(m_newSubDetail,SIGNAL(triggered()),m_bandsAddSignalsMap,SLOT(map()));
    m_bandsAddSignalsMap->setMapping(m_newSubDetail,BandDesignIntf::SubDetailBand);
    m_newBandButton->addAction(m_newSubDetail);

    m_newSubDetailHeader=new QAction(QIcon(),tr("SubDetailHeader"),this);
    m_newSubDetailHeader->setEnabled(false);
    connect(m_newSubDetailHeader,SIGNAL(triggered()),m_bandsAddSignalsMap,SLOT(map()));
    m_bandsAddSignalsMap->setMapping(m_newSubDetailHeader,BandDesignIntf::SubDetailHeader);
    m_newBandButton->addAction(m_newSubDetailHeader);

    m_newSubDetailFooter=new QAction(QIcon(),tr("SubDetailFooter"),this);
    m_newSubDetailFooter->setEnabled(false);
    connect(m_newSubDetailFooter,SIGNAL(triggered()),m_bandsAddSignalsMap,SLOT(map()));
    m_bandsAddSignalsMap->setMapping(m_newSubDetailFooter,BandDesignIntf::SubDetailFooter);
    m_newBandButton->addAction(m_newSubDetailFooter);

    m_newGroupHeader=new QAction(QIcon(),tr("GroupHeader"),this);
    m_newGroupHeader->setEnabled(false);
    connect(m_newGroupHeader,SIGNAL(triggered()),m_bandsAddSignalsMap,SLOT(map()));
    m_bandsAddSignalsMap->setMapping(m_newGroupHeader,BandDesignIntf::GroupHeader);
    m_newBandButton->addAction(m_newGroupHeader);

    m_newGroupFooter=new QAction(QIcon(),tr("GroupFooter"),this);
    m_newGroupFooter->setEnabled(false);
    connect(m_newGroupFooter,SIGNAL(triggered()),m_bandsAddSignalsMap,SLOT(map()));
    m_bandsAddSignalsMap->setMapping(m_newGroupFooter,BandDesignIntf::GroupFooter);
    m_newBandButton->addAction(m_newGroupFooter);

    m_newTearOffBand=new QAction(QIcon(),tr("Tear-off Band"),this);
    connect(m_newTearOffBand,SIGNAL(triggered()),m_bandsAddSignalsMap,SLOT(map()));
    m_bandsAddSignalsMap->setMapping(m_newTearOffBand,BandDesignIntf::TearOffBand);
    m_newBandButton->addAction(m_newTearOffBand);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
    connect(m_bandsAddSignalsMap,SIGNAL(mappedInt(int)),this,SLOT(slotNewBand(int)));
#else
    connect(m_bandsAddSignalsMap,SIGNAL(mapped(int)),this,SLOT(slotNewBand(int)));
#endif
}

void ReportDesignWindow::createMainMenu()
{
    m_fileMenu = menuBar()->addMenu(tr("File"));
    m_fileMenu->addAction(m_newReportAction);
    m_fileMenu->addAction(m_loadReportAction);
    m_fileMenu->addAction(m_saveReportAction);
    m_fileMenu->addAction(m_saveReportAsAction);
    m_fileMenu->addAction(m_previewReportAction);
    //m_fileMenu->addAction(m_printReportAction);
    m_editMenu = menuBar()->addMenu(tr("Edit"));
    m_editMenu->addAction(m_redoAction);
    m_editMenu->addAction(m_undoAction);
    m_editMenu->addAction(m_copyAction);
    m_editMenu->addAction(m_pasteAction);
    m_editMenu->addAction(m_cutAction);
    m_editMenu->addAction(m_settingsAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_lockSelectedItemsAction);
    m_editMenu->addAction(m_unlockSelectedItemsAction);
    m_editMenu->addAction(m_selectOneLevelItems);
    m_infoMenu = menuBar()->addMenu(tr("Info"));
    m_infoMenu->addAction(m_aboutAction);
    m_recentFilesMenu = m_fileMenu->addMenu(tr("Recent Files"));
    m_recentFilesSignalMap = new QSignalMapper(this);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
    connect(m_recentFilesSignalMap,SIGNAL(mappedString(QString)),this,SLOT(slotLoadRecentFile(QString)));
#else
    connect(m_recentFilesSignalMap,SIGNAL(mapped(QString)),this,SLOT(slotLoadRecentFile(QString)));
#endif
    m_recentFilesMenu->setDisabled(m_recentFiles.isEmpty());
}

void ReportDesignWindow::initReportEditor(ReportEnginePrivateInterface* report)
{
    m_reportDesignWidget=new ReportDesignWidget(report, m_settings, this,this);
    setCentralWidget(m_reportDesignWidget);
    connect(m_reportDesignWidget,SIGNAL(itemSelected(LimeReport::BaseDesignIntf*)),
            this,SLOT(slotItemSelected(LimeReport::BaseDesignIntf*)));
    connect(m_reportDesignWidget,SIGNAL(itemPropertyChanged(QString,QString,QVariant,QVariant)),
            this,SLOT(slotItemPropertyChanged(QString,QString,QVariant,QVariant)));
    connect(m_reportDesignWidget,SIGNAL(insertModeStarted()),this,SLOT(slotInsertModeStarted()));
    connect(m_reportDesignWidget,SIGNAL(multiItemSelected()),this,SLOT(slotMultiItemSelected()));
    connect(m_reportDesignWidget,SIGNAL(itemInserted(LimeReport::PageDesignIntf*,QPointF,QString)),
            this,SLOT(slotItemInserted(LimeReport::PageDesignIntf*,QPointF,QString)));
    connect(m_reportDesignWidget,SIGNAL(itemInsertCanceled(QString)),this,SLOT(slotItemInsertCanceled(QString)));
    connect(dynamic_cast<QObject*>(report), SIGNAL(datasourceCollectionLoadFinished(QString)),this,SLOT(slotUpdateDataBrowser(QString)));
    connect(m_reportDesignWidget,SIGNAL(commandHistoryChanged()),this,SLOT(slotCommandHistoryChanged()));
    connect(m_reportDesignWidget,SIGNAL(activePageChanged()),this,SLOT(slotActivePageChanged()));
    connect(m_reportDesignWidget, SIGNAL(bandAdded(LimeReport::PageDesignIntf*,LimeReport::BandDesignIntf*)),
            this, SLOT(slotBandAdded(LimeReport::PageDesignIntf*,LimeReport::BandDesignIntf*)));
    connect(m_reportDesignWidget, SIGNAL(bandDeleted(LimeReport::PageDesignIntf*,LimeReport::BandDesignIntf*)),
            this, SLOT(slotBandDeleted(LimeReport::PageDesignIntf*,LimeReport::BandDesignIntf*)));
    connect(m_reportDesignWidget, SIGNAL(pageAdded(PageDesignIntf*)), this, SLOT(slotPageAdded(PageDesignIntf*)));
    connect(m_reportDesignWidget, SIGNAL(pageDeleted()), this, SLOT(slotPageDeleted()));
}

void ReportDesignWindow::createObjectInspector()
{
    m_objectInspector = new ObjectInspectorWidget(this);
    m_validator = new ObjectNameValidator();
    m_objectInspector->setValidator(m_validator);
    m_objectInspector->setSubclassesAsLevel(false);
    m_objectInspector->setTranslateProperties(true);
    m_objectInspector->setAlternatingRowColors(true);
    m_objectInspector->setRootIsDecorated(!m_objectInspector->subclassesAsLevel());
    QDockWidget *objectDoc = new QDockWidget(this);
    QWidget* w = new QWidget(objectDoc);
    QVBoxLayout* l = new QVBoxLayout(w);
    l->addWidget(m_objectInspector);
    l->setContentsMargins(0, 0, 0, 0);
    w->setLayout(l);
    objectDoc->setWindowTitle(tr("Object Inspector"));
    objectDoc->setWidget(w);
    objectDoc->setObjectName("objectInspector");
    m_pageEditors.append(objectDoc);
    addDockWidget(Qt::LeftDockWidgetArea,objectDoc);
}

void ReportDesignWindow::createObjectsBrowser()
{
    QDockWidget *doc = new QDockWidget(this);
    doc->setWindowTitle(tr("Report structure"));
    m_objectsBrowser = new ObjectBrowser(doc);
    doc->setWidget(m_objectsBrowser);
    doc->setObjectName("structureDoc");
    addDockWidget(Qt::RightDockWidgetArea,doc);
    m_objectsBrowser->setMainWindow(this);
    m_pageEditors.append(doc);
    m_objectsBrowser->setReportEditor(m_reportDesignWidget);
}

#ifdef HAVE_QTDESIGNER_INTEGRATION

void ReportDesignWindow::createDialogWidgetBox()
{
    QDockWidget *doc = new QDockWidget(this);
    doc->setWindowTitle(tr("Widget Box"));
    doc->setWidget(m_reportDesignWidget->toolWindow(ReportDesignWidget::WidgetBox));
    doc->setObjectName("WidgetBox");
    addDockWidget(Qt::LeftDockWidgetArea,doc);
    m_dialogEditors.append(doc);
}

void ReportDesignWindow::createDialogPropertyEditor()
{
    QDockWidget *doc = new QDockWidget(this);
    doc->setWindowTitle(tr("Property Editor"));
    doc->setWidget(m_reportDesignWidget->toolWindow(ReportDesignWidget::PropertyEditor));
    doc->setObjectName("PropertyEditor");
    addDockWidget(Qt::RightDockWidgetArea,doc);
    m_dialogEditors.append(doc);
}

void ReportDesignWindow::createDialogObjectInspector()
{
    QDockWidget *dock = new QDockWidget(this);
    dock->setWindowTitle(tr("Object Inspector"));
    dock->setWidget(m_reportDesignWidget->toolWindow(ReportDesignWidget::ObjectInspector));
    dock->setObjectName("ObjectInspector");
    addDockWidget(Qt::RightDockWidgetArea,dock);
    m_dialogEditors.append(dock);
}

void ReportDesignWindow::createDialogActionEditor()
{
    QDockWidget *dock = new QDockWidget(this);
    dock->setWindowTitle(tr("Action Editor"));
    dock->setWidget(m_reportDesignWidget->toolWindow(ReportDesignWidget::ActionEditor));
    dock->setObjectName("ActionEditor");
    addDockWidget(Qt::BottomDockWidgetArea,dock);
    m_dialogEditors.append(dock);
    m_docksToTabify.append(dock);
}

void ReportDesignWindow::createDialogResourceEditor()
{
    QDockWidget *dock = new QDockWidget(this);
    dock->setWindowTitle(tr("Resource Editor"));
    dock->setWidget(m_reportDesignWidget->toolWindow(ReportDesignWidget::ResourceEditor));
    dock->setObjectName("ResourceEditor");
    addDockWidget(Qt::BottomDockWidgetArea,dock);
    m_dialogEditors.append(dock);
    m_docksToTabify.append(dock);
}

void ReportDesignWindow::createDialogSignalSlotEditor()
{
    QDockWidget *dock = new QDockWidget(this);
    dock->setWindowTitle(tr("SignalSlot Editor"));
    dock->setWidget(m_reportDesignWidget->toolWindow(ReportDesignWidget::SignalSlotEditor));
    dock->setObjectName("SignalSlotEditor");
    addDockWidget(Qt::BottomDockWidgetArea,dock);
    m_dialogEditors.append(dock);
    m_docksToTabify.append(dock);
}

void ReportDesignWindow::createDialogDesignerToolBar()
{
    m_dialogDesignerToolBar = addToolBar(tr("Dialog Designer Tools"));
    m_dialogDesignerToolBar->setObjectName("DialogDesignerTools");
    m_dialogDesignerToolBar->addAction(m_saveReportAction);
    m_dialogDesignerToolBar->addAction(m_previewReportAction);
    m_dialogDesignerToolBar->addSeparator();
    m_dialogDesignerToolBar->addAction(m_deleteDialogAction);
    m_dialogDesignerToolBar->addSeparator();
    m_reportDesignWidget->initDialogDesignerToolBar(m_dialogDesignerToolBar);
    m_dialogTools << m_dialogDesignerToolBar;
}

#endif

void ReportDesignWindow::createDataWindow()
{
    QDockWidget *dataDoc = new QDockWidget(this);
    dataDoc->setWindowTitle(tr("Data Browser"));
    m_dataBrowser=new DataBrowser(dataDoc);
    dataDoc->setWidget(m_dataBrowser);
    dataDoc->setObjectName("dataDoc");
    addDockWidget(Qt::LeftDockWidgetArea,dataDoc);
    m_dataBrowser->setSettings(settings());
    m_dataBrowser->setMainWindow(this);
    m_pageEditors.append(dataDoc);
    m_dataBrowser->setReportEditor(m_reportDesignWidget);
}

void ReportDesignWindow::createScriptWindow()
{
    QDockWidget *dataDoc = new QDockWidget(this);
    dataDoc->setWindowTitle(tr("Script Browser"));
    m_scriptBrowser=new ScriptBrowser(dataDoc);
    dataDoc->setWidget(m_scriptBrowser);
    dataDoc->setObjectName("scriptDoc");
    addDockWidget(Qt::LeftDockWidgetArea,dataDoc);
    m_scriptBrowser->setReportEditor(m_reportDesignWidget);
    m_pageEditors.append(dataDoc);
#ifdef HAVE_UI_LOADER
    m_scriptBrowser->updateDialogsTree();
#endif
}

void ReportDesignWindow::updateRedoUndo()
{
    m_undoAction->setEnabled(m_reportDesignWidget->isCanUndo());
    m_redoAction->setEnabled(m_reportDesignWidget->isCanRedo());
}

void ReportDesignWindow::startNewReport()
{
    m_reportDesignWidget->saveState();
    m_reportDesignWidget->clear();
    m_reportDesignWidget->createStartPage();
    m_reportDesignWidget->createTabs();
    m_lblReportName->setText("");
    updateRedoUndo();
    m_reportDesignWidget->slotPagesLoadFinished();
    m_newPageHeader->setEnabled(true);
    m_newPageFooter->setEnabled(true);
    m_newReportHeader->setEnabled(true);
    m_newReportFooter->setEnabled(true);
    m_editorTabType = ReportDesignWidget::Page;
    showDefaultToolBars();
    m_reportDesignWidget->report()->dataManager()->dropChanges();
    m_reportDesignWidget->report()->scriptContext()->dropChanges();
    m_reportDesignWidget->loadState();

}

void ReportDesignWindow::writePosition()
{
    settings()->beginGroup("DesignerWindow");
    settings()->setValue("Geometry",saveGeometry());
    settings()->endGroup();
}

void ReportDesignWindow::setDocWidgetsVisibility(bool visible)
{
    if (!m_hideLeftPanel->isChecked())
        hideDockWidgets(Qt::LeftDockWidgetArea,!visible);
    if (!m_hideRightPanel->isChecked())
        hideDockWidgets(Qt::RightDockWidgetArea,!visible);
}

void ReportDesignWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_Escape){m_editModeAction->trigger();}
}

void ReportDesignWindow::writeState()
{
    settings()->beginGroup("DesignerWindow");

    setDocWidgetsVisibility(true);

    m_editorsStates[m_editorTabType] = saveState();
    settings()->setValue("PageEditorsState",         m_editorsStates[ReportDesignWidget::Page]);
    settings()->setValue("DialogEditorsState",       m_editorsStates[ReportDesignWidget::Dialog]);
    settings()->setValue("ScriptEditorsState",       m_editorsStates[ReportDesignWidget::Script]);
    settings()->setValue("TranslationEditorsState",  m_editorsStates[ReportDesignWidget::Translations]);
    settings()->setValue("InspectorFirsColumnWidth", m_objectInspector->columnWidth(0));
    settings()->setValue("InspectorTranslateProperties", m_objectInspector->translateProperties());
    settings()->endGroup();
    settings()->beginGroup("RecentFiles");
    settings()->setValue("filesCount",m_recentFiles.count());
    QMap<QString, QDateTime>::const_iterator it = m_recentFiles.constBegin();
    int count = 0;
    while (it != m_recentFiles.constEnd()) {
        settings()->setValue("fileName"+QString::number(count),it.key());
        settings()->setValue("fileDate"+QString::number(count),it.value());
        ++count;
        ++it;
    }
    settings()->endGroup();
    m_reportDesignWidget->saveState();
}

void ReportDesignWindow::createRecentFilesMenu()
{
    if (m_recentFilesMenu){
        m_recentFilesMenu->clear();
        removeNotExistedRecentFiles();
        foreach(QString fileName, m_recentFiles.keys()){
            QAction* tmpAction = new QAction(QIcon(":/report/images/newReport"),fileName,this);
            connect(tmpAction,SIGNAL(triggered()), m_recentFilesSignalMap, SLOT(map()));
            m_recentFilesSignalMap->setMapping(tmpAction,fileName);
            m_recentFilesMenu->addAction(tmpAction);
        }
        m_recentFilesMenu->setDisabled(m_recentFiles.isEmpty());
    }
}

void ReportDesignWindow::removeNotExistedRecentFiles()
{
    QMap<QString,QDateTime>::iterator it = m_recentFiles.begin();
    while (it!=m_recentFiles.end()){
        if (!QFile::exists(it.key())){
            it = m_recentFiles.erase(it);
        } else {
            ++it;
        }
    }
}

void ReportDesignWindow::removeNotExistedRecentFilesFromMenu(const QString &fileName)
{
    if (m_recentFilesMenu){
        foreach(QAction* action, m_recentFilesMenu->actions()){
            if (action->text().compare(fileName)==0){
                m_recentFilesMenu->removeAction(action);
                break;
            }
        }
    }
}

void ReportDesignWindow::addRecentFile(const QString &fileName)
{
    if (!m_recentFiles.contains(fileName)){
        if (m_recentFiles.count()==10){
            QMap<QString, QDateTime>::const_iterator it = m_recentFiles.constBegin();
            QDateTime minDate = QDateTime::currentDateTime();
            while (it != m_recentFiles.constEnd()) {
                if (minDate>it.value()) minDate = it.value();
                ++it;
            }
            m_recentFiles.remove(m_recentFiles.key(minDate));
        }
        m_recentFiles.insert(fileName,QDateTime::currentDateTime());
    } else {
        m_recentFiles[fileName] = QDateTime::currentDateTime();
    }
    createRecentFilesMenu();
}

void ReportDesignWindow::restoreSetting()
{
    settings()->beginGroup("DesignerWindow");
    QVariant v = settings()->value("Geometry");
    if (v.isValid()){
        restoreGeometry(v.toByteArray());
    } else {
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 3)
        QScreen *screen = QGuiApplication::primaryScreen();

        int screenWidth = screen->geometry().width();
        int screenHeight = screen->geometry().height();
#else
        QDesktopWidget *desktop = QApplication::desktop();

        int screenWidth = desktop->screenGeometry().width();
        int screenHeight = desktop->screenGeometry().height();        
#endif
        int x = screenWidth * 0.1;
        int y = screenHeight * 0.1;

        resize(screenWidth * 0.8, screenHeight * 0.8);
        move(x, y);
    }
    v = settings()->value("PageEditorsState");
    if (v.isValid()){
        m_editorsStates[ReportDesignWidget::Page] = v.toByteArray();
        m_editorTabType = ReportDesignWidget::Page;
    }
    v = settings()->value("DialogEditorsState");
    if (v.isValid()){
        m_editorsStates[ReportDesignWidget::Dialog] = v.toByteArray();
    }
    v = settings()->value("ScriptEditorsState");
    if (v.isValid()){
        m_editorsStates[ReportDesignWidget::Script] = v.toByteArray();
    }

    v = settings()->value("TranslationEditorsState");
    if (v.isValid()){
        m_editorsStates[ReportDesignWidget::Translations] = v.toByteArray();
    }

    v = settings()->value("InspectorFirsColumnWidth");
    if (v.isValid()){
        m_objectInspector->setColumnWidth(0,v.toInt());
    }

    v = settings()->value("InspectorTranslateProperties");
    if (v.isValid()){
        m_objectInspector->setTranslateProperties(v.toBool());
    }

    settings()->endGroup();

    settings()->beginGroup("RecentFiles");
    int filesCount = settings()->value("filesCount").toInt();
    QStringList keys = settings()->childKeys();
    for(int i=0;i<filesCount;i++){
        if (keys.contains("fileName"+QString::number(i))){
            m_recentFiles[settings()->value("fileName"+QString::number(i)).toString()] =
                    settings()->value("fileData"+QString::number(i)).toDateTime();

        } else break;

    }
    settings()->endGroup();

    m_reportDesignWidget->loadState();
    m_useGridAction->setChecked(m_reportDesignWidget->useGrid());
    createRecentFilesMenu();
}

bool ReportDesignWindow::checkNeedToSave()
{
    if (m_reportDesignWidget->isNeedToSave()){
        QMessageBox::StandardButton button = QMessageBox::question(
            this,"",tr("Report has been modified! Do you want save the report?"),
            QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes
        );
        switch (button) {
        case QMessageBox::Cancel:
            break;
        case QMessageBox::Yes:
            if (!m_reportDesignWidget->save()) break;
        default:
            return true;
        }
        return false;
    }
    return true;
}

void ReportDesignWindow::showModal()
{
    bool deleteOnClose = testAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_DeleteOnClose,false);
    setAttribute(Qt::WA_ShowModal,true);
    restoreSetting();
    show();
    m_eventLoop.exec();
    if (deleteOnClose) delete this;
}

void ReportDesignWindow::showNonModal()
{
    restoreSetting();
    show();
}

void ReportDesignWindow::setSettings(QSettings* value)
{
    if (m_ownedSettings)
        delete m_settings;
    m_settings=value;
    m_ownedSettings=false;
    restoreSetting();
}

QSettings*ReportDesignWindow::settings()
{
    if (m_settings){
        return m_settings;
    } else {
        m_settings = new QSettings("LimeReport",QApplication::applicationName());
        m_ownedSettings = true;
        return m_settings;
    }
}

void ReportDesignWindow::slotNewReport()
{    
    if (checkNeedToSave()) {
        m_lblReportName->setText("");
        startNewReport();
        m_deletePageAction->setEnabled(false);
        setWindowTitle("Lime Report Designer");
    }
}

void ReportDesignWindow::slotNewPage(){
    if (m_reportDesignWidget){
        m_reportDesignWidget->addPage();
    }
}

void ReportDesignWindow::slotDeletePage()
{
    if (m_reportDesignWidget && m_reportDesignWidget->report()->pageCount()>1){
        m_reportDesignWidget->deleteCurrentPage();
    }
}

void ReportDesignWindow::slotNewTextItem()
{
    if (m_newTextItemAction->isChecked()) {m_newTextItemAction->setCheckable(false);return;}
    if (m_reportDesignWidget) {
        m_reportItemIsLocked = QApplication::keyboardModifiers() == Qt::SHIFT;
        m_reportDesignWidget->startInsertMode("TextItem");
        m_newTextItemAction->setCheckable(true);
        m_newTextItemAction->setChecked(true);
    }
}

void ReportDesignWindow::slotNewBand(const QString& bandType)
{
    if(m_reportDesignWidget) m_reportDesignWidget->addBand(bandType);
}

void ReportDesignWindow::slotNewBand(int bandType)
{
    if(m_reportDesignWidget) m_reportDesignWidget->addBand(BandDesignIntf::BandsType(bandType));
}

void ReportDesignWindow::slotItemSelected(LimeReport::BaseDesignIntf *item)
{
    if (m_objectInspector->object()!=item){

        m_newSubDetail->setEnabled(false);
        m_newSubDetailHeader->setEnabled(false);
        m_newSubDetailFooter->setEnabled(false);
        m_newGroupHeader->setEnabled(false);
        m_newGroupFooter->setEnabled(false);
        m_newDataHeader->setEnabled(false);
        m_newDataFooter->setEnabled(false);

        m_objectInspector->commitActiveEditorData();
        m_objectInspector->setObject(item);

        if (m_objectInspector->subclassesAsLevel())
          m_objectInspector->expandToDepth(0);

        QSet<BandDesignIntf::BandsType> bs;
        bs<<BandDesignIntf::Data<<BandDesignIntf::SubDetailBand;
        BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(item);
        if (band){
            if (bs.contains(band->bandType())){
                m_newSubDetail->setEnabled(true);
            }
            if ((band->bandType()==BandDesignIntf::Data)||
                (band->bandType()==BandDesignIntf::SubDetailBand)
               )
            {
                m_newGroupHeader->setEnabled(true);
            }
            if (band->bandType()==BandDesignIntf::GroupHeader){
                m_newGroupFooter->setEnabled(!band->isConnectedToBand(BandDesignIntf::GroupFooter));
                m_newGroupHeader->setEnabled(!band->isConnectedToBand(BandDesignIntf::GroupHeader));
            }
            if (band->bandType()==BandDesignIntf::SubDetailBand){
                m_newSubDetailHeader->setEnabled(!band->isConnectedToBand(BandDesignIntf::SubDetailHeader));
                m_newSubDetailFooter->setEnabled(!band->isConnectedToBand(BandDesignIntf::SubDetailFooter));
            }
            if (band->bandType()==BandDesignIntf::Data){
                m_newDataHeader->setEnabled(!band->isConnectedToBand(BandDesignIntf::DataHeader));
                m_newDataFooter->setEnabled(!band->isConnectedToBand(BandDesignIntf::DataFooter));
                m_newGroupHeader->setEnabled(!band->isConnectedToBand(BandDesignIntf::GroupHeader));
            }
        }

        m_fontEditorBar->setItem(item);
        m_textAlignmentEditorBar->setItem(item);
        m_itemsBordersEditorBar->setItem(item);
    } else {m_objectInspector->clearObjectsList();}
}

void ReportDesignWindow::slotItemPropertyChanged(const QString &objectName, const QString &propertyName, const QVariant& oldValue, const QVariant& newValue )
{
    Q_UNUSED(oldValue)
    Q_UNUSED(newValue)

    if (m_objectInspector->object()&&(m_objectInspector->object()->objectName()==objectName)){
        m_objectInspector->updateProperty(propertyName);
    }
}

void ReportDesignWindow::slotMultiItemSelected()
{
    m_objectInspector->commitActiveEditorData();

    QList<QObject*> selectionList;
    foreach (QGraphicsItem* gi, m_reportDesignWidget->activePage()->selectedItems()) {
        QObject* oi = dynamic_cast<QObject*>(gi);
        if (oi) selectionList.append(oi);
    }
    m_objectInspector->setMultiObjects(&selectionList);
    if (m_objectInspector->subclassesAsLevel())
       m_objectInspector->expandToDepth(0);
}

void ReportDesignWindow::slotInsertModeStarted()
{
    m_editModeAction->setChecked(false);
}

void ReportDesignWindow::slotItemInserted(PageDesignIntf *, QPointF, const QString &ItemType)
{
    if (!m_reportItemIsLocked){
        m_editModeAction->setChecked(true);
        if (m_actionMap.value(ItemType))
            m_actionMap.value(ItemType)->setCheckable(false);
    } else {
        m_reportDesignWidget->startInsertMode(ItemType);
    }
}

void ReportDesignWindow::slotItemInsertCanceled(const QString &ItemType)
{
    m_editModeAction->setChecked(true);
    m_reportItemIsLocked = false;
    if (m_actionMap.value(ItemType))
        m_actionMap.value(ItemType)->setCheckable(false);
}

void ReportDesignWindow::slotUpdateDataBrowser(const QString &collectionName)
{
    if (collectionName.compare("connections",Qt::CaseInsensitive)==0){
        if (m_dataBrowser) m_dataBrowser->initConnections();
    }
    if (collectionName.compare("queries",Qt::CaseInsensitive)==0){
        if (m_dataBrowser) m_dataBrowser->updateDataTree();
    }
    if (collectionName.compare("subqueries",Qt::CaseInsensitive)==0){
        if (m_dataBrowser) m_dataBrowser->updateDataTree();
    }
    if (collectionName.compare("subproxies",Qt::CaseInsensitive)==0){
        if (m_dataBrowser) m_dataBrowser->updateDataTree();
    }
    if (collectionName.compare("variables",Qt::CaseInsensitive)==0){
        if (m_dataBrowser) m_dataBrowser->updateVariablesTree();
    }
}

void ReportDesignWindow::slotCommandHistoryChanged()
{
    updateRedoUndo();
}

void ReportDesignWindow::slotSaveReport()
{
    if (m_reportDesignWidget->emitSaveReport()) return; // report save as'd via signal

    m_reportDesignWidget->save();

    QString filename = m_reportDesignWidget->reportFileName();
    m_lblReportName->setText(filename);
    if(!filename.isEmpty()) addRecentFile(filename);
    setWindowTitle(m_reportDesignWidget->report()->reportName() + " - Lime Report Designer");
}

void ReportDesignWindow::slotSaveReportAs()
{
    if (m_reportDesignWidget->emitSaveReportAs()) return; // report save as'd via signal

    QString fileName = QFileDialog::getSaveFileName(this,tr("Report file name"),"","Report files(*.lrxml);; All files(*)");
    if (!fileName.isEmpty()){
        m_reportDesignWidget->saveToFile(fileName);
        m_lblReportName->setText(m_reportDesignWidget->reportFileName());
        addRecentFile(fileName);
        setWindowTitle(m_reportDesignWidget->report()->reportName() + " - Lime Report Designer");
    }
}

void ReportDesignWindow::slotLoadReport()
{
    if (!checkNeedToSave()) return; // don't need to save

    if (m_reportDesignWidget->emitLoadReport()) return; // report loaded via signal

    QString fileName = QFileDialog::getOpenFileName(
                this,tr("Report file name"),
                m_reportDesignWidget->report()->currentReportsDir(),
                "Report files(*.lrxml);; All files(*)"
                );
    if (!fileName.isEmpty()) {
        QApplication::processEvents();
        setCursor(Qt::WaitCursor);
        m_reportDesignWidget->clear();
        if (m_reportDesignWidget->loadFromFile(fileName)){
            m_lblReportName->setText(fileName);
            m_objectInspector->setObject(0);
            updateRedoUndo();
            setWindowTitle(m_reportDesignWidget->report()->reportName() + " - Lime Report Designer");
            if (!m_recentFiles.contains(fileName)){
                if (m_recentFiles.count()==10){
                    QMap<QString, QDateTime>::const_iterator it = m_recentFiles.constBegin();
                    QDateTime minDate = QDateTime::currentDateTime();
                    while (it != m_recentFiles.constEnd()) {
                        if (minDate>it.value()) minDate = it.value();
                        ++it;
                    }
                    m_recentFiles.remove(m_recentFiles.key(minDate));
                }
                m_recentFiles.insert(fileName,QDateTime::currentDateTime());
            } else {
                m_recentFiles[fileName] = QDateTime::currentDateTime();
            }
            createRecentFilesMenu();
            m_deletePageAction->setEnabled(m_reportDesignWidget->report()->pageCount()>1);
        } else {
            slotNewReport();
        }
        unsetCursor();
        setWindowTitle(m_reportDesignWidget->report()->reportName() + " - Lime Report Designer");
        addRecentFile(fileName);
        m_editorTabType = ReportDesignWidget::Page;
    }

}

void ReportDesignWindow::slotZoomIn()
{
    m_reportDesignWidget->scale(1.2,1.2);
}

void ReportDesignWindow::slotZoomOut()
{
    m_reportDesignWidget->scale(1/1.2,1/1.2);
}

void ReportDesignWindow::slotEditMode()
{
    m_editModeAction->setChecked(true);
    m_reportDesignWidget->startEditMode();
}

void ReportDesignWindow::slotUndo()
{
    m_reportDesignWidget->undo();
    updateRedoUndo();

}

void ReportDesignWindow::slotRedo()
{
    m_reportDesignWidget->redo();
    updateRedoUndo();
}

void ReportDesignWindow::slotCopy()
{
    m_reportDesignWidget->copy();
}

void ReportDesignWindow::slotPaste()
{
    m_reportDesignWidget->paste();
}

void ReportDesignWindow::slotCut()
{
    m_reportDesignWidget->cut();
}

void ReportDesignWindow::slotDelete()
{
    m_reportDesignWidget->deleteSelectedItems();
}

void ReportDesignWindow::slotEditLayoutMode()
{
    m_reportDesignWidget->editLayoutMode(m_editLayoutMode->isChecked());
}

void ReportDesignWindow::slotHLayout()
{
    m_reportDesignWidget->addHLayout();
}

void ReportDesignWindow::slotVLayout()
{
    m_reportDesignWidget->addVLayout();
}

void ReportDesignWindow::slotTest()
{
}

void ReportDesignWindow::slotPrintReport()
{
    setCursor(Qt::WaitCursor);
    m_reportDesignWidget->printReport();
    setCursor(Qt::ArrowCursor);
}

void ReportDesignWindow::slotPreviewReport()
{
    m_previewReportAction->setDisabled(true);
    m_reportDesignWidget->previewReport();
    m_previewReportAction->setDisabled(false);
}

void ReportDesignWindow::slotItemActionCliked()
{
    QAction* action=dynamic_cast<QAction*>(sender());
    action->setCheckable(true);
    action->setChecked(true);
    m_reportItemIsLocked = QApplication::keyboardModifiers() == Qt::SHIFT;
    m_reportDesignWidget->startInsertMode(action->whatsThis());
}

void ReportDesignWindow::slotBandAdded(PageDesignIntf *, BandDesignIntf * band)
{
    if (band->isUnique()){
        switch (band->bandType()) {
        case BandDesignIntf::PageHeader:
            m_newPageHeader->setDisabled(true);
            break;
        case BandDesignIntf::PageFooter:
            m_newPageFooter->setDisabled(true);
            break;
        case BandDesignIntf::ReportHeader:
            m_newReportHeader->setDisabled(true);
            break;
        case BandDesignIntf::ReportFooter:
            m_newReportFooter->setDisabled(true);
            break;
        case BandDesignIntf::TearOffBand:
            m_newTearOffBand->setDisabled(true);
            break;
        default:
            break;
        }
    }
}

void ReportDesignWindow::slotBandDeleted(PageDesignIntf *, BandDesignIntf *band)
{
    if (band->isUnique()){
        switch (band->bandType()) {
        case BandDesignIntf::PageHeader:
            m_newPageHeader->setEnabled(true);
            break;
        case BandDesignIntf::PageFooter:
            m_newPageFooter->setEnabled(true);
            break;
        case BandDesignIntf::ReportHeader:
            m_newReportHeader->setEnabled(true);
            break;
        case BandDesignIntf::ReportFooter:
            m_newReportFooter->setEnabled(true);
        case BandDesignIntf::TearOffBand:
            m_newTearOffBand->setEnabled(true);
        default:
            break;
        }
    }
}


void ReportDesignWindow::updateAvaibleBands(){

    if (!m_reportDesignWidget || !m_reportDesignWidget->activePage()) return;
    m_newPageHeader->setEnabled(true);
    m_newPageFooter->setEnabled(true);
    m_newReportHeader->setEnabled(true);
    m_newReportFooter->setEnabled(true);
    m_newTearOffBand->setEnabled(true);

    foreach(BandDesignIntf* band, m_reportDesignWidget->activePage()->pageItem()->bands()){
        switch (band->bandType()) {
        case BandDesignIntf::PageHeader:
            m_newPageHeader->setEnabled(false);
            break;
        case BandDesignIntf::PageFooter:
            m_newPageFooter->setEnabled(false);
            break;
        case BandDesignIntf::ReportHeader:
            m_newReportHeader->setEnabled(false);
            break;
        case BandDesignIntf::ReportFooter:
            m_newReportFooter->setEnabled(false);
        case BandDesignIntf::TearOffBand:
            m_newTearOffBand->setEnabled(false);
        default:
            break;
        }
    }
}

void ReportDesignWindow::showDefaultToolBars(){
    foreach (QToolBar* tb, m_pageTools){
        tb->setVisible(m_editorTabType != ReportDesignWidget::Dialog);
    }
    foreach (QToolBar* tb, m_dialogTools){
        tb->setVisible(m_editorTabType == ReportDesignWidget::Dialog);
    }
}

void ReportDesignWindow::showDefaultEditors(){
    foreach (QDockWidget* w, m_pageEditors) {
        w->setVisible(m_editorTabType != ReportDesignWidget::Dialog);
    }
#ifdef HAVE_QTDESIGNER_INTEGRATION
    foreach (QDockWidget* w, m_dialogEditors) {
        w->setVisible(m_editorTabType == ReportDesignWidget::Dialog);
    }
    for ( int i = 0; i < m_docksToTabify.size() - 1; ++i){
        tabifyDockWidget(m_docksToTabify.at(i),m_docksToTabify.at(i+1));
    }
    m_docksToTabify.at(0)->raise();
#endif
}

void ReportDesignWindow::slotActivePageChanged()
{
    m_objectInspector->setObject(0);
    updateRedoUndo();
    updateAvaibleBands();

    if (m_editorTabType == ReportDesignWidget::Dialog){
#ifdef HAVE_UI_LOADER
        m_scriptBrowser->updateDialogsTree();
#endif
    }

    setDocWidgetsVisibility(true);

    m_editorsStates[m_editorTabType] = saveState();
    m_editorTabType = m_reportDesignWidget->activeTabType();

    if (!m_editorsStates[m_editorTabType].isEmpty()){
        restoreState(m_editorsStates[m_editorTabType]);
    } else {
        showDefaultEditors();
        showDefaultToolBars();
    }

    setDocWidgetsVisibility(false);
}

void ReportDesignWindow::renderStarted()
{
    m_progressWidget->setVisible(true);
}

void ReportDesignWindow::renderPageFinished(int renderedPageCount)
{
    m_progressLabel->setText(tr("Rendered %1 pages").arg(renderedPageCount));
}

void ReportDesignWindow::renderFinished()
{
    m_progressWidget->setVisible(false);
}

void ReportDesignWindow::slotShowAbout()
{
    AboutDialog* about = new AboutDialog(this);
    about->exec();
}

bool ReportDesignWindow::isDockAreaVisible(Qt::DockWidgetArea area){
    QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();
    foreach (QDockWidget* dw, dockWidgets){
        if ((dockWidgetArea(dw) == area) && !dw->isHidden())
            return true;
    }
    return false;
}

void ReportDesignWindow::hideDockWidgets(Qt::DockWidgetArea area, bool value){
    QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();
    QMap<QDockWidget*, bool>* currentDocState = 0;

    switch (area) {
    case Qt::LeftDockWidgetArea:
        if (value)
            m_leftDocVisibleState.clear();
        currentDocState = &m_leftDocVisibleState;
        break;
    case Qt::RightDockWidgetArea:
        if (value)
            m_rightDocVisibleState.clear();
        currentDocState = &m_rightDocVisibleState;
    default:
        break;
    }

    foreach (QDockWidget* dw, dockWidgets) {
        if (dockWidgetArea(dw) == area){
            if (!value){
                if (currentDocState->value(dw)) dw->show();
            } else {
                currentDocState->insert(dw, dw->isVisible());
                dw->hide();
            }
        }
    }
}

void ReportDesignWindow::slotHideLeftPanel(bool value)
{
    hideDockWidgets(Qt::LeftDockWidgetArea,!value);
}

void ReportDesignWindow::slotHideRightPanel(bool value)
{
    hideDockWidgets(Qt::RightDockWidgetArea,!value);
}

void ReportDesignWindow::slotEditSettings()
{
    m_reportDesignWidget->editSetting();
}

void ReportDesignWindow::slotUseGrid(bool value)
{
    m_reportDesignWidget->setUseGrid(value);
}

void ReportDesignWindow::slotUseMagnet(bool value)
{
    m_reportDesignWidget->setUseMagnet(value);
}

void ReportDesignWindow::slotLoadRecentFile(const QString fileName)
{
    if (checkNeedToSave()){
        QApplication::processEvents();
        if (QFile::exists(fileName)){
            setCursor(Qt::WaitCursor);
            m_reportDesignWidget->clear();
            m_reportDesignWidget->loadFromFile(fileName);
            m_lblReportName->setText(fileName);
            m_objectInspector->setObject(0);
            updateRedoUndo();
            unsetCursor();
            setWindowTitle(m_reportDesignWidget->report()->reportName() + " - Lime Report Designer");
            m_recentFiles[fileName] = QDateTime::currentDateTime();
        } else {
            m_recentFiles.remove(fileName);
            removeNotExistedRecentFilesFromMenu(fileName);
            QMessageBox::information(this,tr("Warning"),tr("File \"%1\" not found!").arg(fileName));
        }
    }
}

void ReportDesignWindow::slotPageAdded(PageDesignIntf *)
{
    m_deletePageAction->setEnabled(m_reportDesignWidget->report()->pageCount()>1);
}

void ReportDesignWindow::slotPageDeleted()
{
    m_deletePageAction->setEnabled(m_reportDesignWidget->report()->pageCount()>1);
}

void ReportDesignWindow::slotFilterTextChanged(const QString& filter)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    m_filterModel->setFilterRegExp(QRegExp(filter, Qt::CaseInsensitive, QRegExp::FixedString));
#else
    m_filterModel->setFilterRegularExpression(QRegularExpression(filter, QRegularExpression::CaseInsensitiveOption));
#endif
}

#ifdef HAVE_QTDESIGNER_INTEGRATION
void ReportDesignWindow::slotDeleteDialog()
{
    if ( m_editorTabType == ReportDesignWidget::Dialog ){
        m_reportDesignWidget->report()->scriptContext()->deleteDialog(m_reportDesignWidget->activeDialogName());
    }
}

void ReportDesignWindow::slotAddNewDialog()
{
    m_reportDesignWidget->addNewDialog();
}

#endif

void ReportDesignWindow::slotLockSelectedItems()
{
    m_reportDesignWidget->lockSelectedItems();
}

void ReportDesignWindow::slotUnlockSelectedItems()
{
    m_reportDesignWidget->unlockSelectedItems();
}

void ReportDesignWindow::slotSelectOneLevelItems()
{
    m_reportDesignWidget->selectOneLevelItems();
}

void ReportDesignWindow::slotCancelRendering(bool)
{
    m_reportDesignWidget->report()->cancelRender();
}

void ReportDesignWindow::closeEvent(QCloseEvent * event)
{
    if (m_progressWidget->isVisible()){
        QMessageBox::critical(this, tr("Attention"), tr("The rendering is in process"));
        event->ignore();
        return;
    }
    if (checkNeedToSave()){    
        m_dataBrowser->closeAllDataWindows();
        writeState();
#ifdef Q_OS_WIN
        writePosition();
#endif
#ifdef Q_OS_MAC
        writePosition();
#endif
        m_eventLoop.exit();
        event->accept();
    } else event->ignore();
}

void ReportDesignWindow::resizeEvent(QResizeEvent*)
{
#ifdef Q_OS_UNIX
    writePosition();
#endif
}

void ReportDesignWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);

    if (!m_editorsStates[m_editorTabType].isEmpty()){
        restoreState(m_editorsStates[m_editorTabType]);
    } else {
        showDefaultEditors();
        showDefaultToolBars();
    }

}

void ReportDesignWindow::moveEvent(QMoveEvent*)
{
#ifdef Q_OS_UNIX
    writePosition();
#endif
}

bool ObjectNameValidator::validate(const QString &propName, const QVariant &propValue, QObject *object, QString &msg)
{
    if (propName.compare("objectName")==0){
        BaseDesignIntf* bd = dynamic_cast<BaseDesignIntf*>(object);
        if (bd){
            if (bd->page()->reportItemByName(propValue.toString())){
                msg = QString(QObject::tr("Object with name %1 already exists!").arg(propValue.toString()));
                return false;
            } else (bd->emitObjectNamePropertyChanged(object->objectName(),propValue.toString()));
        }
    }
    return true;
}

}

