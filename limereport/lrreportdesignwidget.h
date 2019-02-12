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
#ifndef LRREPORTDESIGNWIDGET_H
#define LRREPORTDESIGNWIDGET_H

#include <QObject>
#include <QGraphicsView>
#include <QMainWindow>
#include <QTextEdit>

#include "lrpagedesignintf.h"
#include "lrdatadesignintf.h"
#include "lrdatasourcemanager.h"
#include "lrcollection.h"
#include "lrreportengine_p.h"
#include "lrgraphicsviewzoom.h"

#ifdef HAVE_QT4
QT_BEGIN_NAMESPACE
class LimeReportTabWidget: public QTabWidget{
    Q_OBJECT
public:
    explicit LimeReportTabWidget(QWidget *parent = 0):QTabWidget(parent){}
    QTabBar* tabBar() const{ return QTabWidget::tabBar();}
};
QT_END_NAMESPACE
#endif

namespace LimeReport {

class ReportEnginePrivate;
class DataBrowser;
class ReportDesignWindow;
class DialogDesignerManager;
class DialogDesigner;
class TranslationEditor;
class ScriptEditor;


class ReportDesignWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QObject* datasourcesManager READ dataManager())
public:
    enum ToolWindowType{
        WidgetBox = 1,
        ObjectInspector = 2,
        ActionEditor = 3,
        SignalSlotEditor = 4,
        PropertyEditor = 5,
        ResourceEditor = 6
    };
    enum EditorTabType{
        Page,
        Dialog,
        Script,
        Translations,
        TabTypeCount
    };
    ReportDesignWidget(ReportEnginePrivateInterface* report, QSettings* settings,
                       QMainWindow *mainWindow, QWidget *parent = 0);
    ~ReportDesignWidget();
    void createStartPage();
    void clear();
    DataSourceManager* dataManager();
    ScriptEngineManager* scriptManager();
    ScriptEngineContext* scriptContext();
    void removeDatasource(const QString& datasourceName);
    void addBand(const QString& bandType);
    void addBand(BandDesignIntf::BandsType bandType);
    void startInsertMode(const QString& itemType);
    void startEditMode();
    void updateSize();
    bool isCanUndo();
    bool isCanRedo();
    void deleteItem(QGraphicsItem *item);
    PageDesignIntf* activePage();
    QGraphicsView* activeView();
    QList<QGraphicsItem *> selectedItems();
    QStringList datasourcesNames();
    void scale( qreal sx, qreal sy);
    ReportEnginePrivateInterface* report(){return m_report;}
    QString reportFileName();
    bool isNeedToSave();
    bool emitSaveReport();
    bool emitSaveReportAs();
    bool emitLoadReport();
    void saveState();
    void loadState();
    void applySettings();
    void applyUseGrid();
    bool useGrid(){ return m_useGrid;}
    bool useMagnet() const;
    void setUseMagnet(bool useMagnet);
    EditorTabType activeTabType();
#ifdef HAVE_QTDESIGNER_INTEGRATION
    void initDialogDesignerToolBar(QToolBar* toolBar);
    void updateDialogs();
    DialogDesignerManager *dialogDesignerManager() const;
    QString activeDialogName();
    DialogDesigner* activeDialogPage();
    QWidget* toolWindow(ToolWindowType windowType);
#endif
public slots:
    bool saveToFile(const QString&);
    bool save();
    bool loadFromFile(const QString&);
    void deleteSelectedItems();
    void connectPage(PageDesignIntf* page);
    void undo();
    void redo();
    void copy();
    void paste();
    void cut();
    void bringToFront();
    void sendToBack();
    void alignToLeft();
    void alignToRight();
    void alignToVCenter();
    void alignToTop();
    void alignToBottom();
    void alignToHCenter();
    void sameHeight();
    void sameWidth();
    void editLayoutMode(bool value);
    void addHLayout();
    void addVLayout();
    void setFont(const QFont &font);
    void setTextAlign(const bool &horizontalAlign, const Qt::AlignmentFlag &alignment);
    void setBorders(const BaseDesignIntf::BorderLines& borders);
    void editSetting();
    void setUseGrid(bool value);
    void previewReport();
    void printReport();
    void addPage();
    void deleteCurrentPage();
    void slotPagesLoadFinished();
    void slotDialogDeleted(QString dialogName);
#ifdef HAVE_QTDESIGNER_INTEGRATION
    void addNewDialog();
#endif
private slots:
    void slotItemSelected(LimeReport::BaseDesignIntf *item);
    void slotSelectionChanged();
    void slotDatasourceCollectionLoaded(const QString&);
    void slotSceneRectChanged(QRectF);
    void slotCurrentTabChanged(int index);
    void slotReportLoaded();
    void slotScriptTextChanged();
#ifdef HAVE_QTDESIGNER_INTEGRATION
    void slotDialogChanged(QString);
    void slotDialogNameChanged(QString oldName, QString newName);
#endif
    void slotPagePropertyObjectNameChanged(const QString& oldValue, const QString& newValue);
    void slotTabMoved(int from, int to);
signals:
    void insertModeStarted();
    void itemInserted(LimeReport::PageDesignIntf*,QPointF,const QString&);
    void itemInsertCanceled(const QString&);
    void itemSelected(LimeReport::BaseDesignIntf *item);
    void itemPropertyChanged(const QString& objectName, const QString& propertyName, const QVariant& oldValue, const QVariant& newValue);
    void multiItemSelected();
    void commandHistoryChanged();
    void cleared();
    void loadFinished();
    void activePageChanged();
    void activePageUpdated(LimeReport::PageDesignIntf*);
    void bandAdded(LimeReport::PageDesignIntf*, LimeReport::BandDesignIntf*);
    void bandDeleted(LimeReport::PageDesignIntf*, LimeReport::BandDesignIntf*);
    void itemAdded(LimeReport::PageDesignIntf*, LimeReport::BaseDesignIntf*);
    void itemDeleted(LimeReport::PageDesignIntf*, LimeReport::BaseDesignIntf*);
    void pageAdded(PageDesignIntf* page);
    void pageDeleted();
protected:
    void createTabs();
#ifdef HAVE_QTDESIGNER_INTEGRATION
    void createNewDialogTab(const QString& dialogName,const QByteArray& description);
#endif
private:
    bool eventFilter(QObject *target, QEvent *event);
    void prepareReport();
private:
    ReportEnginePrivateInterface* m_report;
    QGraphicsView *m_view;
    ScriptEditor* m_scriptEditor;
    TranslationEditor* m_traslationEditor;
#ifdef HAVE_QTDESIGNER_INTEGRATION
    DialogDesignerManager* m_dialogDesignerManager;
#endif
    QMainWindow *m_mainWindow;
#ifdef HAVE_QT4
    LimeReportTabWidget* m_tabWidget;
#endif
#ifdef HAVE_QT5
    QTabWidget* m_tabWidget;
#endif
    GraphicsViewZoomer* m_zoomer;
    QFont m_defaultFont;
    int m_verticalGridStep;
    int m_horizontalGridStep;
    bool m_useGrid;
    bool m_useMagnet;
    bool m_dialogChanged;
    bool m_useDarkTheme;
    QSettings* m_settings;
};

} // namespace LimeReport
#endif // LRREPORTDESIGNWIDGET_H
