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
#ifndef LRREPORTRENDER_H
#define LRREPORTRENDER_H
#include <QObject>
#include "lrcollection.h"
#include "lrdatasourcemanager.h"
#include "lrpageitemdesignintf.h"
#include "lrscriptenginemanager.h"
#include "serializators/lrstorageintf.h"

namespace LimeReport{

class PageDesignIntf;
class BandDesignIntf;
class ContentItemDesignIntf;

class GroupBandsHolder: public QList<BandDesignIntf*>{
public:
    GroupBandsHolder(bool tryToKeepTogether):QList<BandDesignIntf*>(),m_tryToKeepTogether(tryToKeepTogether),
        m_dataGroup(true), m_footerGroup(false){}
    bool tryToKeepTogether(){return m_tryToKeepTogether;}
    void setTryToKeepTogether(bool value){m_tryToKeepTogether=value;}
    bool isDataGroup(){return m_dataGroup;}
    bool isFooterGroup(){ return m_footerGroup;}
    void setIsFooterGroup(){m_footerGroup=true;m_dataGroup=false;}
private:
    bool m_tryToKeepTogether;
    bool m_dataGroup;
    bool m_footerGroup;
};


struct PagesRange{
    int firstPage;
    int lastPage;
};

class ReportRender: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* datasourcesManager READ datasources())
public:
    enum DataRenderMode {StartNewPageAsNeeded, NotStartNewPage, ForcedStartPage};
    enum BandPrintMode {PrintAlwaysPrintable, PrintNotAlwaysPrintable };
    enum ResetPageNuberType{BandReset, PageReset};
    typedef QSharedPointer<ReportRender> Ptr;    
    ~ReportRender();
    ReportRender(QObject *parent = 0);
    void setDatasources(DataSourceManager* value);
    void setScriptContext(ScriptEngineContext* scriptContext);
    DataSourceManager*  datasources(){return m_datasources;}
    int     pageCount();
    PageItemDesignIntf::Ptr pageAt(int index);
    QString renderPageToString(PageDesignIntf *patternPage);
    ReportPages renderPageToPages(PageDesignIntf *patternPage);
    void    secondRenderPass(ReportPages renderedPages);
signals:
    void    pageRendered(int renderedPageCount);
public slots:
    void    cancelRender();
private:
    void    baseDesignIntfToScript(BaseDesignIntf* item);
    void    renderPage(PageDesignIntf *patternPage);
    void    initDatasources();
    void    initDatasource(const QString &name);
    void    initRenderPage();
#ifdef HAVE_UI_LOADER
    void    initDialogs();
#endif
    void    initVariables();
    bool    runInitScript();
    void    clearPageMap();
    BandDesignIntf*    renderBand(BandDesignIntf *patternBand, BandDesignIntf *bandData, DataRenderMode mode = NotStartNewPage, bool isLast = false);
    void    renderDataBand(BandDesignIntf* dataBand);
    void    renderPageHeader(PageItemDesignIntf* patternPage);
    void    renderPageFooter(PageItemDesignIntf* patternPage);
    void    moveTearOffBand();
    void    renderPageItems(PageItemDesignIntf* patternPage);
    qreal   calcPageFooterHeight(PageItemDesignIntf* patternPage);
    qreal   calcSlicePercent(qreal height);
    void    renderChildHeader(BandDesignIntf* parent, BandPrintMode printMode);
    void    renderChildFooter(BandDesignIntf* parent, BandPrintMode printMode);
    void    renderChildBands(BandDesignIntf* parentBand);
    void    renderGroupHeader(BandDesignIntf* parentBand, IDataSource* dataSource, bool firstTime);
    void    renderGroupFooter(BandDesignIntf* parentBand);
    void    initGroups();
    void    extractGroupFuntionsFromItem(ContentItemDesignIntf* contentItem, BandDesignIntf* band);
    void    extractGroupFunctionsFromContainer(BaseDesignIntf* baseItem, BandDesignIntf* band);
    void    extractGroupFunctions(BandDesignIntf* band);
    void    replaceGroupFunctionsInItem(ContentItemDesignIntf* contentItem, BandDesignIntf* band);
    void    replaceGroupFunctionsInContainer(BaseDesignIntf* baseItem, BandDesignIntf* band);
    void    replaceGroupsFunction(BandDesignIntf* band);

    void    popPageFooterGroupValues(BandDesignIntf* dataBand);
    void    pushPageFooterGroupValues(BandDesignIntf* dataBand);

    enum    GroupType{DATA,FOOTER};
    void    closeGroup(BandDesignIntf* band);
    void    openDataGroup(BandDesignIntf* band);
    void    closeDataGroup(BandDesignIntf* band);
    void    openFooterGroup(BandDesignIntf* band);
    void    closeFooterGroup(BandDesignIntf* band);
    void    cutGroups();
    void    checkFooterGroup(BandDesignIntf* groupBand);
    void    pasteGroups();
    void    checkLostHeadersOnPrevPage();

    BandDesignIntf* findEnclosingGroup();
    bool    registerBand(BandDesignIntf* band, bool registerInChildren=true);
    BandDesignIntf *sliceBand(BandDesignIntf* band, BandDesignIntf *patternBand, bool isLast);

    BandDesignIntf* saveUppperPartReturnBottom(BandDesignIntf *band, int height, BandDesignIntf *patternBand);
    BandDesignIntf* renderData(BandDesignIntf* patternBand);
    void    startNewColumn();
    void    startNewPage();
    void    resetPageNumber(ResetPageNuberType resetType);
    int     findLastPageNumber(int currentPage);
    void    savePage(bool isLast = false);
    QString toString();
    void initColumns();
    bool isNeedToRearrangeColumnsItems();
    BandDesignIntf* lastColumnItem(int columnIndex);
    void rearrangeColumnsItems();
    int  columnItemsCount(int columnIndex);
    qreal columnHeigth(int columnIndex);
    qreal maxColumnHeight();
    void renameChildItems(BaseDesignIntf *item);
    void renderGroupFooterByHeader(BandDesignIntf *groupHeader);
private:
    DataSourceManager* m_datasources;
    ScriptEngineContext* m_scriptEngineContext;
    PageItemDesignIntf* m_renderPageItem;
    PageItemDesignIntf* m_patternPageItem;
    QList<PageItemDesignIntf::Ptr> m_renderedPages;
    QMultiMap< BandDesignIntf*, GroupBandsHolder* > m_childBands;
    QList<BandDesignIntf*> m_reprintableBands;
//    QList<BandDesignIntf*> m_lastRenderedHeaders;

    //int m_maxHeightByColumn[0];
    //int m_currentStartDataPos;
    int m_currentIndex;
    int m_pageCount;

    QMap<QString,QVariant> m_popupedValues;
    QMultiMap<BandDesignIntf*,QString> m_popupedExpression;

    qreal           m_pageFooterHeight;
    qreal           m_dataAreaSize;
    qreal           m_reportFooterHeight;
    int             m_renderedDataBandCount;
    BandDesignIntf* m_lastDataBand;
    BandDesignIntf* m_lastRenderedFooter;
    bool            m_renderCanceled;
    QVector<qreal>  m_maxHeightByColumn;
    QVector<qreal>  m_currentStartDataPos;
    int             m_currentColumn;
    QList<PagesRange> m_ranges;
    QVector<BandDesignIntf*> m_columnedBandItems;
    unsigned long long m_curentNameIndex;
    bool m_newPageStarted;


};
} // namespace LimeReport
#endif // LRREPORTRENDER_H
