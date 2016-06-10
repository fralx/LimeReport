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


namespace LimeReport {

class ReportEnginePrivate;
class DataBrowser;
class ReportDesignWindow;

class ReportDesignWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QObject* datasourcesManager READ dataManager())
    friend class ReportDesignWindow;
public:
    ~ReportDesignWidget();
//    static ReportDesignWidget* instance(){return m_instance;}
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
//    void setDatabrowser(DataBrowser* databrowser);
    ReportEnginePrivate* report(){return m_report;}
    QString reportFileName();
    bool isNeedToSave();
    bool emitLoadReport();
    void saveState(QSettings *settings);
    void loadState(QSettings *settings);
    void applySettings();
    void applyUseGrid();
    bool useGrid(){ return m_useGrid;}
    bool useMagnet() const;
    void setUseMagnet(bool useMagnet);

public slots:
    void saveToFile(const QString&);
    bool save();
    bool loadFromFile(const QString&);
    void deleteSelectedItems();
    void connectPage(PageDesignIntf* page);
    void undo();
    void redo();
    void copy();
    void paste();
    void cut();
    void brinToFront();
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
    void setFont(const QFont &font);
    void setTextAlign(const bool &horizontalAlign, const Qt::AlignmentFlag &alignment);
    void setBorders(const BaseDesignIntf::BorderLines& borders);
    void editSetting();
    void setUseGrid(bool value);
    void previewReport();
    void printReport();
    void addPage();
    void deleteCurrentPage();
private slots:
    void slotItemSelected(LimeReport::BaseDesignIntf *item);
    void slotSelectionChanged();
    void slotPagesLoadFinished();
    void slotDatasourceCollectionLoaded(const QString&);
    void slotSceneRectChanged(QRectF);
    void slotCurrentTabChanged(int index);
signals:
    void insertModeStarted();
    void itemInserted(LimeReport::PageDesignIntf*,QPointF,const QString&);
    void itemInsertCanceled(const QString&);
    void itemSelected(LimeReport::BaseDesignIntf *item);
    void itemPropertyChanged(const QString& objectName, const QString& propertyName, const QVariant& oldValue, const QVariant& newValue);
    void multiItemSelected();
    void commandHistoryChanged();
    void cleared();
    void loaded();
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
private:
    bool eventFilter(QObject *target, QEvent *event);
    ReportDesignWidget(ReportEnginePrivate* report,QMainWindow *mainWindow,QWidget *parent = 0);
private:
    ReportEnginePrivate* m_report;
    QGraphicsView *m_view;
    QTextEdit* m_scriptEditor;
    QMainWindow *m_mainWindow;
    QTabWidget* m_tabWidget;
    GraphicsViewZoomer* m_zoomer;
    QFont m_defaultFont;
    int m_verticalGridStep;
    int m_horizontalGridStep;
    bool m_useGrid;
    bool m_useMagnet;
//    static ReportDesignWidget* m_instance;
};

}
#endif // LRREPORTDESIGNWIDGET_H
