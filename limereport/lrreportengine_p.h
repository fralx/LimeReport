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
#ifndef LRREPORTDESIGNINTF_P_H
#define LRREPORTDESIGNINTF_P_H

#include <QObject>
#include <QSharedPointer>
#include <QMainWindow>
#include "lrreportengine.h"
#include "lrcollection.h"
#include "lrglobal.h"
#include "lrdatasourcemanager.h"
#include "lrbanddesignintf.h"
#include "lrreportrender.h"
#include "serializators/lrstorageintf.h"
#include "lrscriptenginemanager.h"

class QFileSystemWatcher;

namespace LimeReport{

class PageDesignIntf;
class PrintRange;
class ReportDesignWindow;

//TODO: Add on render callback

class ReportEnginePrivate : public QObject, public ICollectionContainer
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(ReportEngine)
    Q_PROPERTY(ACollectionProperty pages READ fakeCollectionReader())
    Q_PROPERTY(QObject* datasourcesManager READ dataManager)
    Q_PROPERTY(QObject* scriptContext READ scriptContext)
    Q_PROPERTY(bool suppressFieldAndVarError READ suppressFieldAndVarError WRITE setSuppressFieldAndVarError)
    friend class PreviewReportWidget;
public:
    static void printReport(ItemsReaderIntf::Ptr reader, QPrinter &printer);
    static void printReport(ReportPages pages, QPrinter &printer);
public:
    explicit ReportEnginePrivate(QObject *parent = 0);
    virtual ~ReportEnginePrivate();

    PageDesignIntf*      appendPage(const QString& pageName="");
    bool deletePage(PageDesignIntf *page);
    PageDesignIntf*      createPreviewPage();
    PageDesignIntf*      pageAt(int index){return (index<=(m_pages.count()-1)) ? m_pages.at(index):0;}
    int                  pageCount() {return m_pages.count();}
    DataSourceManager*   dataManager(){return m_datasources;}
    ScriptEngineContext* scriptContext(){return m_scriptEngineContext;}
    ScriptEngineManager* scriptManager(){return &ScriptEngineManager::instance();}
    IDataSourceManager*  dataManagerIntf(){return m_datasources;}

    IScriptEngineManager* scriptManagerIntf(){
        ScriptEngineManager::instance().setDataManager(dataManager());
        return &ScriptEngineManager::instance();
    }

    void    clearReport();
    bool    printReport(QPrinter *printer=0);
    bool    printPages(ReportPages pages, QPrinter *printer);
    void    printToFile(const QString& fileName);
    bool    printToPDF(const QString& fileName);
    void    previewReport(PreviewHints hints = PreviewBarsUserSetting);
    void    designReport();
    void    setSettings(QSettings* value);
    void    setShowProgressDialog(bool value){m_showProgressDialog = value;}
    QSettings*  settings();
    bool    loadFromFile(const QString& fileName, bool autoLoadPreviewOnChange);
    bool    loadFromByteArray(QByteArray *data, const QString& name = "");
    bool    loadFromString(const QString& report, const QString& name = "");
    QString reportFileName(){return m_fileName;}
    void    setReportFileName(const QString& reportFileName){ m_fileName = reportFileName;}
    bool    saveToFile();
    bool    saveToFile(const QString& fileName);
    QByteArray  saveToByteArray();
    QString saveToString();
    bool    isNeedToSave();
    QString lastError();
    ReportEngine * q_ptr;
    void emitSaveReport();
    bool emitLoadReport();
    void emitSaveFinished();
    bool isSaved();
    void setCurrentReportsDir(const QString& dirName);
    QString currentReportsDir(){ return m_reportsDir;}
    void setReportName(const QString& reportName){ m_reportName=reportName;}
    QString reportName(){ return m_reportName;}
    bool hasActivePreview(){return m_activePreview;}
    PageDesignIntf *createPreviewScene(QObject *parent);
    PreviewReportWidget *createPreviewWidget(QWidget *parent);
    QIcon previewWindowIcon() const;
    void setPreviewWindowIcon(const QIcon &previewWindowIcon);
    QString previewWindowTitle() const;
    void setPreviewWindowTitle(const QString &previewWindowTitle);

    bool suppressFieldAndVarError() const;
    void setSuppressFieldAndVarError(bool suppressFieldAndVarError);
    bool isBusy();
    bool resultIsEditable() const;
    void setResultEditable(bool value);

    void setPassPhrase(const QString &passPhrase);
    void reorderPages(const QList<PageDesignIntf *> &reorderedPages);
    void clearSelection();
    Qt::LayoutDirection previewLayoutDirection();
    void setPreviewLayoutDirection(const Qt::LayoutDirection& previewLayoutDirection);
    ScaleType previewScaleType();
    int       previewScalePercent();
    void setPreviewScaleType(const ScaleType &previewScaleType, int percent = 0);

signals:
    void    pagesLoadFinished();
    void    datasourceCollectionLoadFinished(const QString& collectionName);
    void    cleared();
    void    renderStarted();
    void    renderFinished();
    void    renderPageFinished(int renderedPageCount);
    void    onLoad(bool& loaded);
    void    onSave();
    void    saveFinished();
public slots:
    bool    slotLoadFromFile(const QString& fileName);
    void    cancelRender();
protected:
    PageDesignIntf* createPage(const QString& pageName="");
protected slots:
    void    slotDataSourceCollectionLoaded(const QString& collectionName);
private slots:
    void slotPreviewWindowDestroyed(QObject *window);
private:
    //ICollectionContainer
    virtual QObject*    createElement(const QString&,const QString&);
    virtual int         elementsCount(const QString&);
    virtual QObject*    elementAt(const QString&, int index);
    virtual void        collectionLoadFinished(const QString&);
    void    saveError(QString message);
    void    showError(QString message);
    //ICollectionContainer
    ReportPages renderToPages();
    QString renderToString();
private:
    QList<PageDesignIntf*> m_pages;
    DataSourceManager* m_datasources;
    ScriptEngineContext* m_scriptEngineContext;
    ReportRender::Ptr m_reportRender;
    QString m_fileName;
    QString m_lastError;
    QSettings* m_settings;
    bool m_ownedSettings;
    QScopedPointer<QPrinter> m_printer;
    bool m_printerSelected;
    bool m_showProgressDialog;
    QString m_reportsDir;
    QString m_reportName;
    QMainWindow* m_activePreview;
    QIcon m_previewWindowIcon;
    QString m_previewWindowTitle;
    QPointer<ReportDesignWindow> m_designerWindow;
    ReportSettings m_reportSettings;
    bool m_reportRendering;
    bool m_resultIsEditable;
    QString m_passPhrase;
    QFileSystemWatcher  *m_fileWatcher;
    Qt::LayoutDirection m_previewLayoutDirection;
    ScaleType m_previewScaleType;
    int m_previewScalePercent;
};

}
#endif // LRREPORTDESIGNINTF_P_H
