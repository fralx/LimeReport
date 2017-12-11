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
#ifndef LRLIMERENDER_H
#define LRLIMERENDER_H

#include <QObject>
#include <QSettings>
#include <QPrintDialog>
//#include <QJSEngine>

#include "lrreportrender.h"
#include "limerender_p.h"
#include "lrglobal.h"
#include "lrdatasourcemanagerintf.h"
#include "lrscriptenginemanagerintf.h"
#include "lrpreviewreportwidget.h"

class QPrinter;

namespace LimeReport {

//class PrintRange{
//public:
//    int fromPage() const { return m_fromPage;}
//    int toPage() const { return m_toPage;}
//    QPrintDialog::PrintRange rangeType() const { return m_rangeType;}
//    PrintRange(QAbstractPrintDialog::PrintRange rangeType=QPrintDialog::AllPages, int fromPage=0, int toPage=0);
//    void setRangeType(QAbstractPrintDialog::PrintRange rangeType){ m_rangeType=rangeType;}
//    void setFromPage(int fromPage){ m_fromPage = fromPage;}
//    void setToPage(int toPage){ m_toPage = toPage;}
//private:
//    QPrintDialog::PrintRange m_rangeType;
//    int m_fromPage;
//    int m_toPage;
//};

class DataSourceManager;
class LimeRenderPrivate;
class PageDesignIntf;
class PageItemDesignIntf;
class PreviewReportWidget;

typedef QList< QSharedPointer<PageItemDesignIntf> > ReportPages;

class LIMEREPORT_EXPORT LimeRender : public QObject{
    Q_OBJECT
    friend class PreviewReportWidget;
public:
    static void setSettings(QSettings *value){m_settings=value;}
public:
    explicit LimeRender(QObject *parent = 0);
    ~LimeRender();
    bool    printReport(QPrinter *printer=0);
    bool    printPages(ReportPages pages, QPrinter *printer);
    void    printToFile(const QString& fileName);
    PageDesignIntf *createPreviewScene(QObject *parent = 0);
    bool    printToPDF(const QString& fileName);
    void    previewReport(PreviewHints hints = PreviewBarsUserSetting);
    IDataSourceManager* dataManager();
    IScriptEngineManager* scriptManager();
    bool    loadFromFile(const QString& fileName, bool autoLoadPreviewOnChange = false);
    bool    loadFromByteArray(QByteArray *data);
    bool    loadFromString(const QString& data);
    QString lastError();
    PreviewReportWidget *createPreviewWidget(QWidget *parent = 0);
    void setPreviewWindowTitle(const QString& title);
    void setPreviewWindowIcon(const QIcon& icon);
    void setResultEditable(bool value);
    bool resultIsEditable();
    bool isBusy();
    void setPassPharse(QString& passPharse);
    QList<QLocale::Language> aviableLanguages();
    bool setReportLanguage(QLocale::Language language);
    Qt::LayoutDirection previewLayoutDirection();
    void setPreviewLayoutDirection(const Qt::LayoutDirection& previewLayoutDirection);
signals:
    void renderStarted();
    void renderFinished();
    void renderPageFinished(int renderedPageCount);
    void onLoad(bool& loaded);
public slots:
    void cancelRender();
protected:
    LimeRenderPrivate * const d_ptr;
    LimeRender(LimeRenderPrivate &dd, QObject * parent=0);
private:
    Q_DECLARE_PRIVATE(LimeRender)
    static QSettings* m_settings;
};

} // namespace LimeReport

#endif // LRLIMERENDER_H
