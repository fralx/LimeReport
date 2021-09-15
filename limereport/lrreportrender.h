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
    bool isTOC;
};

class PagesRanges{
public:
    PagesRanges(): m_TOCRangeIndex(-1) {}
    int findLastPageNumber(int index);
    int findPageNumber(int index);
    PagesRange& currentRange(bool isTOC);
    void startNewRange(bool isTOC = false);
    void addTOCMarker(bool addNewRange);
    void addPage();
    void addTOCPage();
    void clear();
private:
    void shiftRangesNextToTOC();
private:
    QVector<PagesRange> m_ranges;
    int m_TOCRangeIndex;
};


class ReportRender: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* datasourcesManager READ datasources)
public:
    enum DataRenderMode {StartNewPageAsNeeded, NotStartNewPage, ForcedStartPage};
    enum BandPrintMode {PrintAlwaysPrintable, PrintNotAlwaysPrintable };
    enum ResetPageNuberType{BandReset, PageReset};
    enum PageRenderStage{BeforePageHeader, AfterPageHeader};
    typedef QSharedPointer<ReportRender> Ptr;    
    ~ReportRender();
    ReportRender(QObject *parent = 0);
    void setDatasources(DataSourceManager* value);
    void setScriptContext(ScriptEngineContext* scriptContext);
    DataSourceManager*  datasources(){return m_datasources;}
    int     pageCount();
    PageItemDesignIntf::Ptr pageAt(int index);
    QString renderPageToString(PageItemDesignIntf *patternPage);
    ReportPages renderPageToPages(PageItemDesignIntf *patternPage);
    ReportPages renderTOC(PageItemDesignIntf *patternPage, bool first, bool resetPages);
    void    secondRenderPass(ReportPages renderedPages);
    void    createTOCMarker(bool startNewRange);
signals:
    void    pageRendered(int renderedPageCount);
public slots:
    void    cancelRender();
private:
    void    analizeContainer(BaseDesignIntf *item, BandDesignIntf *band);
    void    analizeItem(ContentItemDesignIntf *item, BandDesignIntf *band);
    void    analizePage(PageItemDesignIntf *patternPage);

    void    initDatasources();
    void    initDatasource(const QString &name);
    void    initRenderPage();
    void    initVariables();
    void    initGroups();
    void    clearPageMap();

    void    renderPage(PageItemDesignIntf *patternPage, bool isTOC = false, bool isFirst = false, bool = false);
    BandDesignIntf*    renderBand(BandDesignIntf *patternBand, BandDesignIntf *bandData, DataRenderMode mode = NotStartNewPage, bool isLast = false);
    void    renderDataBand(BandDesignIntf* dataBand);
    void    renderPageHeader(PageItemDesignIntf* patternPage);
    void    renderReportHeader(PageItemDesignIntf* patternPage, PageRenderStage stage);
    void    renderPageFooter(PageItemDesignIntf* patternPage);
    void    renderPageItems(PageItemDesignIntf* patternPage);
    void    renderChildHeader(BandDesignIntf* parent, BandPrintMode printMode);
    void    renderChildFooter(BandDesignIntf* parent, BandPrintMode printMode);
    void    renderChildBands(BandDesignIntf* parentBand);
    void    recalcIfNeeded(BandDesignIntf *band);
    void    renderDataHeader(BandDesignIntf* header);
    void    renderGroupHeader(BandDesignIntf* parentBand, IDataSource* dataSource, bool firstTime);
    void    renderGroupFooter(BandDesignIntf* parentBand);
    void    moveTearOffBand();
    qreal   calcPageFooterHeight(PageItemDesignIntf* patternPage);
    qreal   calcSlicePercent(qreal height);

    bool	containsGroupFunctions(BaseDesignIntf *container);
    void	extractGroupFuntionsFromItem(ContentItemDesignIntf* contentItem, BandDesignIntf* band);
    void    extractGroupFunctionsFromContainer(BaseDesignIntf* baseItem, BandDesignIntf* band);
    void    extractGroupFunctions(BandDesignIntf* band);
    void    replaceGroupFunctionsInItem(ContentItemDesignIntf* contentItem, BandDesignIntf* band);
    void    replaceGroupFunctionsInContainer(BaseDesignIntf* baseItem, BandDesignIntf* band);
    void    replaceGroupsFunction(BandDesignIntf* band);

    BandDesignIntf *findRecalcableBand(BandDesignIntf *patternBand);

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
    void    checkLostHeadersInPrevColumn();

    BandDesignIntf* findEnclosingGroup();
    bool    registerBand(BandDesignIntf* band, bool registerInChildren=true);
    BandDesignIntf *sliceBand(BandDesignIntf* band, BandDesignIntf *patternBand, bool isLast);

    BandDesignIntf* saveUppperPartReturnBottom(BandDesignIntf *band, int height, BandDesignIntf *patternBand);
    BandDesignIntf* renderData(BandDesignIntf* patternBand, bool emitBeforeRender = true);
    void    startNewColumn();
    void    startNewPage(bool isFirst = false);
    void    resetPageNumber(ResetPageNuberType resetType);
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
    void updateTOC(BaseDesignIntf* item, int pageNumber);
    void placeBandOnPage(BandDesignIntf *band, int columnIndex);
    QColor makeBackgroundColor(BandDesignIntf *band);
private:
    DataSourceManager* m_datasources;
    ScriptEngineContext* m_scriptEngineContext;
    PageItemDesignIntf* m_renderPageItem;
    PageItemDesignIntf* m_patternPageItem;
    QList<PageItemDesignIntf::Ptr> m_renderedPages;
    QMultiMap< BandDesignIntf*, GroupBandsHolder* > m_childBands;
    QList<BandDesignIntf*> m_reprintableBands;
    QList<BandDesignIntf*> m_recalcBands;
    QMap<QString, QVector<QString> > m_groupfunctionItems;
    int m_currentIndex;
    int m_pageCount;

    QMap<QString,QVariant> m_popupedValues;
    QMultiMap<BandDesignIntf*,QString> m_popupedExpression;

    qreal           m_pageFooterHeight;
    qreal           m_dataAreaSize;
    qreal           m_reportFooterHeight;
    int             m_renderedDataBandCount;
    BandDesignIntf* m_lastRenderedHeader;
    BandDesignIntf* m_lastDataBand;
    BandDesignIntf* m_lastRenderedFooter;
    BandDesignIntf* m_lastRenderedBand;
    bool            m_renderCanceled;
    QVector<qreal>  m_maxHeightByColumn;
    QVector<qreal>  m_currentStartDataPos;
    int             m_currentColumn;
    PagesRanges     m_pagesRanges;
    QVector<BandDesignIntf*> m_columnedBandItems;
    unsigned long long m_currentNameIndex;
    bool            m_newPageStarted;
    bool            m_lostHeadersMoved;

};
} // namespace LimeReport
#endif // LRREPORTRENDER_H
