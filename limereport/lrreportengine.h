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
#ifndef LRREPORTDESIGNINTF_H
#define LRREPORTDESIGNINTF_H

#include <QObject>
#include <QSettings>
#include <QPrintDialog>

#include "lrglobal.h"
#include "lrdatasourcemanagerintf.h"
#include "lrscriptenginemanagerintf.h"
#include "lrpreviewreportwidget.h"

class QPrinter;

namespace LimeReport {

class PrintRange{
public:
    int fromPage() const { return m_fromPage;}
    int toPage() const { return m_toPage;}
    QPrintDialog::PrintRange rangeType() const { return m_rangeType;}
    PrintRange(QAbstractPrintDialog::PrintRange rangeType=QPrintDialog::AllPages, int fromPage=0, int toPage=0);
    void setRangeType(QAbstractPrintDialog::PrintRange rangeType){ m_rangeType=rangeType;}
    void setFromPage(int fromPage){ m_fromPage = fromPage;}
    void setToPage(int toPage){ m_toPage = toPage;}
private:
    QPrintDialog::PrintRange m_rangeType;
    int m_fromPage;
    int m_toPage;
};

class DataSourceManager;
class ReportEnginePrivate;
class PageDesignIntf;
class PageItemDesignIntf;

typedef QList< QSharedPointer<PageItemDesignIntf> > ReportPages;

class LIMEREPORT_EXPORT ReportEngine : public QObject{
    Q_OBJECT
public:
    static void setSettings(QSettings *value){m_settings=value;}
public:
    explicit ReportEngine(QObject *parent = 0);
    ~ReportEngine();
    bool    printReport(QPrinter *printer=0);
    bool    printPages(ReportPages pages, QPrinter *printer);
    void    printToFile(const QString& fileName);
    PageDesignIntf *createPreviewScene(QObject *parent = 0);
    bool    printToPDF(const QString& fileName);
    void    previewReport(PreviewHints hints = PreviewBarsUserSetting);
    void    designReport();
    void    setShowProgressDialog(bool value);
    IDataSourceManager* dataManager();
    IScriptEngineManager* scriptManager();
    bool    loadFromFile(const QString& fileName, bool autoLoadPreviewOnChange = false);
    bool    loadFromByteArray(QByteArray *data);
    bool    loadFromString(const QString& data);
    QString reportFileName();
    void    setReportFileName(const QString& fileName);
    bool    saveToFile();
    bool    saveToFile(const QString& fileName);
    QByteArray  saveToByteArray();
    QString saveToString();
    QString lastError();
    void setCurrentReportsDir(const QString& dirName);
    void setReportName(const QString& name);
    QString reportName();
    PreviewReportWidget *createPreviewWidget(QWidget *parent = 0);
    void setPreviewWindowTitle(const QString& title);
    void setPreviewWindowIcon(const QIcon& icon);
    void setResultEditable(bool value);
    bool resultIsEditable();
    bool isBusy();
    void setPassPharse(QString& passPharse);
    Qt::LayoutDirection previewLayoutDirection();
    void setPreviewLayoutDirection(const Qt::LayoutDirection& previewLayoutDirection);
signals:
    void renderStarted();
    void renderFinished();
    void renderPageFinished(int renderedPageCount);
    void onLoad(bool& loaded);
    void onSave();
    void saveFinished();
public slots:
    void cancelRender();
protected:
    ReportEnginePrivate * const d_ptr;
    ReportEngine(ReportEnginePrivate &dd, QObject * parent=0);
private:
    Q_DECLARE_PRIVATE(ReportEngine)
    static QSettings* m_settings;
};

} // namespace LimeReport

#endif // LRREPORTDESIGNINTF_H
