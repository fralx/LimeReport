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
#ifndef LRREPORTEDITORWINDOW_H
#define LRREPORTEDITORWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include <QToolButton>
#include <QLabel>
#include <QFontComboBox>
#include <QSpinBox>
#include <QProgressDialog>
#include "lrbanddesignintf.h"
#include "items/editors/lrfonteditorwidget.h"
#include "items/editors/lrtextalignmenteditorwidget.h"
#include "items/editors/lritemsaligneditorwidget.h"
#include "items/editors/lritemsborderseditorwidget.h"
#include "lrobjectitemmodel.h"
#include "lrreportdesignwindowintrerface.h"

namespace LimeReport{

class ObjectInspectorWidget;
class QObjectPropertyModel;
class ReportDesignWidget;
class DataBrowser;
class ScriptBrowser;
class BaseDesignIntf;
class PageDesignIntf;
class ObjectBrowser;

class ReportDesignWindow : public ReportDesignWindowInterface
{
    Q_OBJECT
public:
    explicit ReportDesignWindow(ReportEnginePrivateInterface *report, QWidget *parent = 0, QSettings* settings=0);
    ~ReportDesignWindow();
    static ReportDesignWindow* instance(){return m_instance;}

    bool checkNeedToSave();
    void showModal();
    void showNonModal();
    void setSettings(QSettings* value);
    QSettings* settings();
    void restoreSetting();
    void setShowProgressDialog(bool value){m_showProgressDialog = value;}
private slots:
    void slotNewReport();
    void slotNewPage();
    void slotDeletePage();
    void slotNewTextItem();
    void slotNewBand(const QString& bandType);
    void slotNewBand(int bandType);
    void slotSaveReport();
    void slotSaveReportAs();
    void slotLoadReport();
    void slotZoomIn();
    void slotZoomOut();
    void slotEditMode();
    void slotUndo();
    void slotRedo();
    void slotCopy();
    void slotPaste();
    void slotCut();
    void slotDelete();
    void slotEditLayoutMode();
    void slotHLayout();
    void slotVLayout();
    void slotItemSelected(LimeReport::BaseDesignIntf *item);
    void slotItemPropertyChanged(const QString& objectName, const QString& propertyName, const QVariant &oldValue, const QVariant &newValue);
    void slotMultiItemSelected();
    void slotInsertModeStarted();
    void slotItemInserted(LimeReport::PageDesignIntf* , QPointF, const QString& ItemType);
    void slotItemInsertCanceled(const QString& ItemType);
    void slotUpdateDataBrowser(const QString& collectionName);
    void slotCommandHistoryChanged();
    void slotTest();
    void slotPrintReport();
    void slotPreviewReport();
    void slotItemActionCliked();
    void slotBandAdded(LimeReport::PageDesignIntf*, LimeReport::BandDesignIntf*band);
    void slotBandDeleted(LimeReport::PageDesignIntf*, LimeReport::BandDesignIntf*band);
    void slotActivePageChanged();
    void renderStarted();
    void renderPageFinished(int renderedPageCount);
    void renderFinished();
    void slotShowAbout();
    void slotHideLeftPanel(bool value);
    void slotHideRightPanel(bool value);
    void slotEditSettings();
    void slotUseGrid(bool value);
    void slotUseMagnet(bool value);
    void slotLoadRecentFile(const QString fileName);
    void slotPageAdded(PageDesignIntf* );
    void slotPageDeleted();
    void slotFilterTextChanged(const QString& filter);
#ifdef HAVE_QTDESIGNER_INTEGRATION
    void slotDeleteDialog();
    void slotAddNewDialog();
#endif
    void slotLockSelectedItems();
    void slotUnlockSelectedItems();
    void slotSelectOneLevelItems();
    void slotCancelRendering(bool);

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent* event);
    void moveEvent(QMoveEvent *);
    void hideDockWidgets(Qt::DockWidgetArea area, bool value);
    bool isDockAreaVisible(Qt::DockWidgetArea area);
    void setDocWidgetsVisibility(bool visible);
    void keyPressEvent(QKeyEvent *event);

private:
    void initReportEditor(ReportEnginePrivate* report);
    void createActions();
    void createBandsButton();
    void createMainMenu();
    void createToolBars();
    void createReportToolBar();
    void createItemsActions();
    void createObjectInspector();
    void createObjectsBrowser();
    void initReportEditor(ReportEnginePrivateInterface* report);
    void createDataWindow();
    void createScriptWindow();
#ifdef HAVE_QTDESIGNER_INTEGRATION
    void createDialogWidgetBox();
    void createDialogPropertyEditor();
    void createDialogObjectInspector();
    void createDialogActionEditor();
    void createDialogResourceEditor();
    void createDialogSignalSlotEditor();
    void createDialogDesignerToolBar();
#endif
    void updateRedoUndo();
    void updateAvaibleBands();
    void startNewReport();
    void writePosition();
    void writeState();
    void createRecentFilesMenu();
    void removeNotExistedRecentFiles();
    void removeNotExistedRecentFilesFromMenu(const QString& fileName);
    void addRecentFile(const QString& fileName);
    void showDefaultToolBars();
    void showDefaultEditors();
private:
    static ReportDesignWindow* m_instance;
    QStatusBar* m_statusBar;
    QToolBar* m_mainToolBar;
    QToolBar* m_fontToolBar;
    QToolBar* m_reportToolBar;
    QToolBar* m_alignToolBar;
#ifdef HAVE_QTDESIGNER_INTEGRATION
    QToolBar* m_dialogDesignerToolBar;
#endif
    QToolButton* m_newBandButton;
    QMenuBar* m_mainMenu;
    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_infoMenu;

    QAction* m_newReportAction;
    QAction* m_newTextItemAction;
    QAction* m_deleteItemAction;
    QAction* m_saveReportAction;
    QAction* m_saveReportAsAction;
    QAction* m_loadReportAction;
    QAction* m_zoomInReportAction;
    QAction* m_zoomOutReportAction;
    QAction* m_previewReportAction;
    QAction* m_closeRenderViewAction;
    QAction* m_nextPageAction;
    QAction* m_priorPageAction;
    QAction* m_testAction;
    QAction* m_printReportAction;
    QAction* m_editModeAction;
    QAction* m_undoAction;
    QAction* m_redoAction;
    QAction* m_copyAction;
    QAction* m_pasteAction;
    QAction* m_cutAction;
    QAction* m_settingsAction;
    QAction* m_useGridAction;
    QAction* m_useMagnetAction;
    QAction* m_newPageAction;
    QAction* m_deletePageAction;

    QAction* m_newPageHeader;
    QAction* m_newPageFooter;
    QAction* m_newReportHeader;
    QAction* m_newReportFooter;
    QAction* m_newData;
    QAction* m_newDataHeader;
    QAction* m_newDataFooter;
    QAction* m_newSubDetail;
    QAction* m_newSubDetailHeader;
    QAction* m_newSubDetailFooter;
    QAction* m_newGroupHeader;
    QAction* m_newGroupFooter;
    QAction* m_newTearOffBand;
    QAction* m_aboutAction;
    QAction* m_editLayoutMode;
    QAction* m_addHLayout;
    QAction* m_addVLayout;
    QAction* m_hideLeftPanel;
    QAction* m_hideRightPanel;
#ifdef HAVE_QTDESIGNER_INTEGRATION
    QAction* m_deleteDialogAction;
    QAction* m_addNewDialogAction;
#endif

    QAction* m_lockSelectedItemsAction;
    QAction* m_unlockSelectedItemsAction;
    QAction* m_selectOneLevelItems;

    QMenu*   m_recentFilesMenu;

    QSignalMapper* m_bandsAddSignalsMap;
    QSignalMapper* m_recentFilesSignalMap;

    ObjectInspectorWidget* m_objectInspector;
    ReportDesignWidget* m_reportDesignWidget;
    DataBrowser * m_dataBrowser;
    ScriptBrowser* m_scriptBrowser;

    ObjectBrowser* m_objectsBrowser;

    QHash<QString,QAction*> m_actionMap;
    QLabel* m_lblReportName;
    QEventLoop m_eventLoop;
    QFontComboBox* m_fontNameEditor;
    QComboBox* m_fontSizeEditor;
    QStringListModel m_fontSizeModel;
    bool m_textAttibutesIsChanging;
    BaseDesignIntf const * m_fontItem;
    FontEditorWidget* m_fontEditorBar;
    TextAlignmentEditorWidget* m_textAlignmentEditorBar;
    ItemsAlignmentEditorWidget* m_itemsAlignmentEditorBar;
    ItemsBordersEditorWidget* m_itemsBordersEditorBar;
    QSettings* m_settings;
    bool m_ownedSettings;
    ValidatorIntf* m_validator;
    QProgressDialog* m_progressDialog;
    bool m_showProgressDialog;
    QMap<QString,QDateTime> m_recentFiles;
    QVector<QDockWidget*> m_pageEditors;
    QVector<QDockWidget*> m_dialogEditors;
    QVector<QDockWidget*> m_docksToTabify;
    ReportDesignWidget::EditorTabType m_editorTabType;
    QByteArray         m_editorsStates[ReportDesignWidget::TabTypeCount];
    QVector<QToolBar*> m_pageTools;
    QVector<QToolBar*> m_dialogTools;
    bool m_reportItemIsLocked;
    QMap<QDockWidget*, bool> m_leftDocVisibleState;
    QMap<QDockWidget*, bool> m_rightDocVisibleState;
    QSortFilterProxyModel* m_filterModel;

    QWidget* m_progressWidget;
    QProgressBar* m_progressBar;
    QLabel* m_progressLabel;

    void createProgressBar();
};

class ObjectNameValidator : public ValidatorIntf{
    bool validate(const QString &propName, const QVariant &propValue, QObject *object, QString &msg);
};

}
#endif // LRREPORTEDITORWINDOW_H
