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
#include <stdexcept>
#include <QMessageBox>

#include "lrglobal.h"
#include "lrreportrender.h"
#include "lrpagedesignintf.h"
#include "lrbanddesignintf.h"
#include "lritemdesignintf.h"
#include "lrscriptenginemanager.h"

#include "serializators/lrxmlreader.h"
#include "serializators/lrxmlwriter.h"

namespace LimeReport{

void ReportRender::initColumns(){
    m_maxHeightByColumn.clear();
    m_currentStartDataPos.clear();
    m_maxHeightByColumn.append(0);
    m_currentStartDataPos.append(0);
    m_currentColumn = 0;
}

bool ReportRender::isNeedToRearrangeColumnsItems()
{
    if (m_columnedBandItems.size()<=1) return false;
    if (m_columnedBandItems[0]->columnsFillDirection()!=BandDesignIntf::VerticalUniform)
        return false;

    int avg = m_columnedBandItems.size()/m_columnedBandItems[0]->columnsCount();

    for (int i=0;i<m_maxHeightByColumn.size();++i){
        qreal maxHeight = 0;
        int maxHeightColumn = 0;
        if (m_maxHeightByColumn[i]>maxHeight){
            maxHeight = m_maxHeightByColumn[i];
            maxHeightColumn = i;
        }
        if (maxHeightColumn>0 && columnItemsCount(maxHeightColumn)<avg &&
                maxHeight> lastColumnItem(maxHeightColumn-1)->height()
        ){
          return true;
        }
    }
    return false;
}

BandDesignIntf *ReportRender::lastColumnItem(int columnIndex)
{
    if (columnIndex<0) return 0;
    for(int i=0;i<m_columnedBandItems.size();++i){
       if (m_columnedBandItems[i]->columnIndex()>columnIndex)
           return m_columnedBandItems[i-1];
    }
    return m_columnedBandItems.last();
}

void ReportRender::rearrangeColumnsItems()
{
    if (isNeedToRearrangeColumnsItems()){
        qreal startHeight = columnHeigth(0);
        int avg = m_columnedBandItems.size() / m_columnedBandItems[0]->columnsCount();
        for (int i = 1; i < m_columnedBandItems[0]->columnsCount(); ++i){
           if (columnItemsCount(i) < avg){
               int getCount = avg * (m_columnedBandItems[0]->columnsCount()-i) - columnItemsCount(i);
               for (int j = 0; j < getCount; ++j){
                   BandDesignIntf* band = lastColumnItem(i-1);
                   band->setPos(band->pos().x()+band->width(),m_columnedBandItems[0]->pos().y());
                   band->setColumnIndex(i);
               }

           }
        }
        m_renderPageItem->relocateBands();
        m_maxHeightByColumn[0]+=startHeight-maxColumnHeight();
        m_currentStartDataPos[0]-=startHeight-maxColumnHeight();
        m_columnedBandItems.clear();
    }
}

int ReportRender::columnItemsCount(int columnIndex)
{
    int result = 0;
    foreach(BandDesignIntf* band, m_columnedBandItems){
        if (band->columnIndex()==columnIndex)
            ++result;
        if (band->columnIndex()>columnIndex) break;
    }
    return result;
}

qreal ReportRender::columnHeigth(int columnIndex)
{
    qreal result = 0;
    for(int i=0;i<m_columnedBandItems.size();++i){
       if (m_columnedBandItems[i]->columnIndex()==columnIndex)
           result += m_columnedBandItems[i]->height();
       if (m_columnedBandItems[i]->columnIndex()>columnIndex) break;
    }
    return result;
}

qreal ReportRender::maxColumnHeight()
{
    qreal result = 0;
    for (int i=0;i<m_columnedBandItems[0]->columnsCount();++i){
        qreal curColumnHeight = columnHeigth(i);
        if (curColumnHeight>result) result = curColumnHeight;
    }
    return result;
}

void ReportRender::renameChildItems(BaseDesignIntf *item){
    foreach(BaseDesignIntf* child, item->childBaseItems()){
        if (!child->childBaseItems().isEmpty()) renameChildItems(child);
        child->setObjectName(child->metaObject()->className()+QString::number(++m_currentNameIndex));
    }
}

ReportRender::ReportRender(QObject *parent)
    :QObject(parent), m_renderPageItem(0), m_pageCount(0),
    m_lastRenderedHeader(0), m_lastDataBand(0), m_lastRenderedFooter(0),
    m_lastRenderedBand(0), m_currentColumn(0), m_newPageStarted(false),
    m_lostHeadersMoved(false)
{
    initColumns();
}

void ReportRender::setDatasources(DataSourceManager *value)
{
    m_datasources=value;
    initVariables();
    resetPageNumber(BandReset);
}

void ReportRender::setScriptContext(ScriptEngineContext* scriptContext)
{
    m_scriptEngineContext=scriptContext;
}

void ReportRender::initDatasources(){
    try{
        datasources()->setAllDatasourcesToFirst();
    } catch(ReportError &exception){
        //TODO possible should thow exeption
        QMessageBox::critical(0,tr("Error"),exception.what());
        return;
    }
}

void ReportRender::initDatasource(const QString& name){
    try{
        if (datasources()->containsDatasource(name)){
            IDataSource* ds = datasources()->dataSource(name);
            if (ds)
                ds->first();
        }
    } catch(ReportError &exception){
        QMessageBox::critical(0,tr("Error"),exception.what());
        return;
    }
}

void ReportRender::analizeItem(ContentItemDesignIntf* contentItem, BandDesignIntf* band){
    Q_UNUSED(band)
    if (contentItem){
        QString content = contentItem->content();
        QVector<QString> functions;
        foreach(const QString &functionName, m_datasources->groupFunctionNames()){
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
            QRegularExpression rx = getGroupFunctionRegEx(functionName);
            if(content.indexOf(rx)>=0){
                functions.append(functionName);
            }
#else
            QRegExp rx(QString(Const::GROUP_FUNCTION_RX).arg(functionName));
            rx.setMinimal(true);
            if (rx.indexIn(content)>=0){
                functions.append(functionName);
            }            
#endif
        }
        if (functions.size()>0)
            m_groupfunctionItems.insert(contentItem->patternName(), functions);
    }
}

void ReportRender::analizeContainer(BaseDesignIntf* item, BandDesignIntf* band){
    if (item){
        foreach(BaseDesignIntf* child, item->childBaseItems()){
            ContentItemDesignIntf* contentItem = dynamic_cast<ContentItemDesignIntf*>(child);
            if (contentItem) analizeItem(contentItem, band);
            else analizeContainer(child, band);
        }
    }
}

void ReportRender::analizePage(PageItemDesignIntf* patternPage){
    m_groupfunctionItems.clear();
    foreach(BandDesignIntf* band, patternPage->bands()){
        if (band->isFooter() || band->isHeader()){
            analizeContainer(band,band);
        }
    }
}

void ReportRender::renderPage(PageItemDesignIntf* patternPage, bool isTOC, bool /*isFirst*/, bool /*resetPageNumbers*/)
{
    m_currentNameIndex = 0;
    m_patternPageItem = patternPage;

    analizePage(patternPage);

    if (m_patternPageItem->resetPageNumber() && m_pageCount>0 && !isTOC) {
        resetPageNumber(PageReset);
    }

    if (m_patternPageItem->resetPageNumber() && !isTOC && m_pageCount == 0){
        m_pagesRanges.startNewRange();
    }

    m_renderCanceled = false;
    BandDesignIntf* reportFooter = m_patternPageItem->bandByType(BandDesignIntf::ReportFooter);
    m_reportFooterHeight = 0;
    if (reportFooter)
        m_reportFooterHeight = reportFooter->height();

    initGroups();
    clearPageMap();

    try{
        datasources()->setAllDatasourcesToFirst();
        datasources()->clearGroupFuntionsExpressions();
    } catch(ReportError &exception){
        //TODO possible should thow exeption
        QMessageBox::critical(0,tr("Error"),exception.what());
        return;
    }

    clearPageMap();
    startNewPage(true);

    renderReportHeader(m_patternPageItem, AfterPageHeader);

    BandDesignIntf* lastRenderedBand = 0;
    for (int i=0;i<m_patternPageItem->dataBandCount() && !m_renderCanceled; i++){
        lastRenderedBand = m_patternPageItem->dataBandAt(i);
        initDatasource(lastRenderedBand->datasourceName());
        renderDataBand(lastRenderedBand);
        if (i < m_patternPageItem->dataBandCount()-1) closeFooterGroup(lastRenderedBand);
    }

    if (reportFooter)
        renderBand(reportFooter, 0, StartNewPageAsNeeded);
    if (lastRenderedBand && lastRenderedBand->keepFooterTogether())
        closeFooterGroup(lastRenderedBand);

    BandDesignIntf* tearOffBand = m_patternPageItem->bandByType(BandDesignIntf::TearOffBand);
    if (tearOffBand)
        renderBand(tearOffBand, 0, StartNewPageAsNeeded);

    savePage(true);

}

int ReportRender::pageCount()
{
    return m_renderedPages.count();
}

PageItemDesignIntf::Ptr ReportRender::pageAt(int index)
{
    if ((index>m_renderedPages.count()-1)||(index<0)) throw ReportError(tr("page index out of range"));
    else return m_renderedPages.at(index);
}

QString ReportRender::renderPageToString(PageItemDesignIntf *patternPage)
{
    renderPage(patternPage);
    return toString();
}

ReportPages ReportRender::renderPageToPages(PageItemDesignIntf *patternPage)
{
    renderPage(patternPage);
    return m_renderedPages;
}

ReportPages ReportRender::renderTOC(PageItemDesignIntf* patternPage, bool first, bool resetPages){
    renderPage(patternPage, true, first, resetPages);
    return m_renderedPages;
}

void ReportRender::initRenderPage()
{
    if (!m_renderPageItem) {
        m_renderPageItem = new PageItemDesignIntf(m_patternPageItem->pageSize(), m_patternPageItem->pageRect());
        m_renderPageItem->initFromItem(m_patternPageItem);
        m_renderPageItem->setItemMode(PreviewMode);
        m_renderPageItem->setPatternName(m_patternPageItem->objectName());
        m_renderPageItem->setPatternItem(m_patternPageItem);

        ScriptValueType svCurrentPage;
        ScriptEngineType* se = ScriptEngineManager::instance().scriptEngine();

#ifdef USE_QJSENGINE
        svCurrentPage = getJSValue(*se, m_renderPageItem);
        se->globalObject().setProperty("currentPage", svCurrentPage);
#else
        svCurrentPage = se->globalObject().property("currentPage");
        if (svCurrentPage.isValid()){
            se->newQObject(svCurrentPage, m_renderPageItem);
        } else {
            svCurrentPage = se->newQObject(m_renderPageItem);
            se->globalObject().setProperty("currentPage", svCurrentPage);
        }
#endif


    }
}

void ReportRender::initVariables()
{
    m_datasources->setReportVariable("#PAGE",1);
    m_datasources->setReportVariable("#PAGE_COUNT",0);
    m_datasources->setReportVariable("#IS_LAST_PAGEFOOTER",false);
    m_datasources->setReportVariable("#IS_FIRST_PAGEFOOTER",false);
}

void ReportRender::clearPageMap()
{
    m_renderedPages.clear();
}


bool checkContentItem(ContentItemDesignIntf* item, DataSourceManager* datasources){
    QString content = item->content();
    foreach(QString functionName, datasources->groupFunctionNames()){
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
        QRegularExpression rx = getGroupFunctionRegEx(functionName);
        if(content.indexOf(rx)>=0){
            return true;
        }
#else
        QRegExp rx(QString(Const::GROUP_FUNCTION_RX).arg(functionName));
        if (rx.indexIn(content)>=0){
            return true;
        }
#endif
    }
    return false;
}

bool ReportRender::containsGroupFunctions(BaseDesignIntf *container){
    if (container){
        foreach (BaseDesignIntf* item, container->childBaseItems()) {
            ContentItemDesignIntf* contentItem = dynamic_cast<ContentItemDesignIntf*>(item);
            if (contentItem){
                if (checkContentItem(contentItem, m_datasources)) return true;
            } else {
                if (containsGroupFunctions(item)) return true;
            }
        }
    }
    return false;
}

void ReportRender::extractGroupFuntionsFromItem(ContentItemDesignIntf* contentItem, BandDesignIntf* band){
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
    if ( contentItem && contentItem->content().contains(getScriptRegEx())){
        foreach(const QString &functionName, m_datasources->groupFunctionNames()){

            QRegularExpression rx = getGroupFunctionRegEx(functionName);
            QRegularExpression rxName = getGroupFunctionNameRegEx(functionName);
            QRegularExpressionMatch match = rx.match(contentItem->content());

            if (match.hasMatch()){

                QRegularExpressionMatchIterator iter = rx.globalMatch(contentItem->content());
                while(iter.hasNext()){
                    match = iter.next();
                    QVector<QString> captures = normalizeCaptures(match);
                    if (captures.size() >= 3){
                        int dsIndex = captures.size() == 3 ? Const::DATASOURCE_INDEX - 1 : Const::DATASOURCE_INDEX;
                        BandDesignIntf* dataBand = m_patternPageItem->bandByName(captures.at(dsIndex));
                        if (dataBand){
                            GroupFunction* gf = datasources()->addGroupFunction(
                                functionName, captures.at(Const::VALUE_INDEX), band->objectName(), dataBand->objectName()
                            );
                            if (gf){
                                connect(dataBand, SIGNAL(bandRendered(BandDesignIntf*)),
                                        gf, SLOT(slotBandRendered(BandDesignIntf*)));
                                connect(dataBand, SIGNAL(bandReRendered(BandDesignIntf*, BandDesignIntf*)),
                                        gf, SLOT(slotBandReRendered(BandDesignIntf*, BandDesignIntf*)));
                            }
                        } else {
                            GroupFunction* gf = datasources()->addGroupFunction(
                                functionName, captures.at(Const::VALUE_INDEX), band->objectName(), captures.at(dsIndex)
                            );
                            gf->setInvalid(tr("Databand \"%1\" not found").arg(captures.at(dsIndex)));
                        }
                    }
                }

            } else if (contentItem->content().indexOf(rxName)>=0){
                match = rxName.match(contentItem->content());
                GroupFunction* gf = datasources()->addGroupFunction(functionName, match.captured(1), band->objectName(), "");
                gf->setInvalid(tr("Wrong using function %1").arg(functionName));
            }
        }
    }

#else
    if ( contentItem && contentItem->content().contains(QRegExp("\\$S\\s*\\{.*\\}"))){
        foreach(const QString &functionName, m_datasources->groupFunctionNames()){
            QRegExp rx(QString(Const::GROUP_FUNCTION_RX).arg(functionName));
            rx.setMinimal(true);
            QRegExp rxName(QString(Const::GROUP_FUNCTION_NAME_RX).arg(functionName));
            rxName.setMinimal(true);
            if (rx.indexIn(contentItem->content())>=0){
                int pos = 0;
                while ( (pos = rx.indexIn(contentItem->content(),pos)) != -1){
                    QVector<QString> captures = normalizeCaptures(rx);
                    if (captures.size()>=3){
                        int dsIndex = captures.size() == 3 ? Const::DATASOURCE_INDEX - 1 : Const::DATASOURCE_INDEX;
                        BandDesignIntf* dataBand = m_patternPageItem->bandByName(captures.at(dsIndex));
                        if (dataBand){
                            GroupFunction* gf = datasources()->addGroupFunction(functionName,captures.at(Const::VALUE_INDEX),band->objectName(),dataBand->objectName());
                            if (gf){
                                connect(dataBand, SIGNAL(bandRendered(BandDesignIntf*)),
                                        gf, SLOT(slotBandRendered(BandDesignIntf*)));
                                connect(dataBand, SIGNAL(bandReRendered(BandDesignIntf*, BandDesignIntf*)),
                                        gf, SLOT(slotBandReRendered(BandDesignIntf*, BandDesignIntf*)));
                            }
                        } else {
                            GroupFunction* gf = datasources()->addGroupFunction(functionName,captures.at(Const::VALUE_INDEX),band->objectName(),captures.at(dsIndex));
                            gf->setInvalid(tr("Databand \"%1\" not found").arg(captures.at(dsIndex)));
                        }
                    }
                    pos += rx.matchedLength();
                }
            } else if (rxName.indexIn(contentItem->content())>=0){
                GroupFunction* gf = datasources()->addGroupFunction(functionName,rxName.cap(1),band->objectName(),"");
                gf->setInvalid(tr("Wrong using function %1").arg(functionName));
            }
        }
    }    
#endif
}

void ReportRender::extractGroupFunctionsFromContainer(BaseDesignIntf* baseItem, BandDesignIntf* band){
    if (baseItem){
        foreach (BaseDesignIntf* item, baseItem->childBaseItems()) {
            ContentItemDesignIntf* contentItem = dynamic_cast<ContentItemDesignIntf*>(item);
            if (contentItem) extractGroupFuntionsFromItem(contentItem, band);
            else extractGroupFunctionsFromContainer(item, band);
        }
    }
}

void ReportRender::extractGroupFunctions(BandDesignIntf *band)
{
    extractGroupFunctionsFromContainer(band, band);
}

void ReportRender::replaceGroupFunctionsInItem(ContentItemDesignIntf* contentItem, BandDesignIntf* band){
    if (contentItem){
        if (m_groupfunctionItems.contains(contentItem->patternName())){
            QString content = contentItem->content();
            foreach(QString functionName, m_groupfunctionItems.value(contentItem->patternName())){
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
                QRegularExpression rx = getGroupFunctionRegEx(functionName);
                QRegularExpressionMatch match = rx.match(content);

                if (match.capturedStart() != -1){
                    int pos = 0;
                    while ( (pos = match.capturedStart()) != -1 ){
                        QVector<QString> captures = normalizeCaptures(match);
                        if (captures.size() >= 3){
                            QString expressionIndex = datasources()->putGroupFunctionsExpressions(captures.at(Const::VALUE_INDEX));
                            if (captures.size()<5){
                                content.replace(captures.at(0), QString("%1(%2,%3)")
                                    .arg(functionName).arg('"'+expressionIndex+'"').arg('"'+band->objectName()+'"'));
                            } else {
                                content.replace(captures.at(0), QString("%1(%2,%3,%4)").arg(
                                                    functionName,
                                                    '"'+expressionIndex+'"',
                                                    '"'+band->objectName()+'"',
                                                    captures.at(4)
                                                ));
                            }
                        }
                        match = rx.match(content, pos + match.capturedLength());
                    }
                }
#else
                QRegExp rx(QString(Const::GROUP_FUNCTION_RX).arg(functionName));
                rx.setMinimal(true);
                if (rx.indexIn(content)>=0){
                    int pos = 0;
                    while ( (pos = rx.indexIn(content,pos))!= -1 ){
                        QVector<QString> captures = normalizeCaptures(rx);
                        if (captures.size() >= 3){
                            QString expressionIndex = datasources()->putGroupFunctionsExpressions(captures.at(Const::VALUE_INDEX));
                            if (captures.size()<5){
                                content.replace(captures.at(0),QString("%1(%2,%3)").arg(functionName).arg('"'+expressionIndex+'"').arg('"'+band->objectName()+'"'));
                            } else {
                                content.replace(captures.at(0),QString("%1(%2,%3,%4)").arg(
                                                    functionName,
                                                    '"'+expressionIndex+'"',
                                                    '"'+band->objectName()+'"',
                                                    captures.at(4)
                                                ));
                            }
                        }
                        pos += rx.matchedLength();
                    }
                }

#endif
            }
            contentItem->setContent(content);
        }
    }
}

void ReportRender::replaceGroupFunctionsInContainer(BaseDesignIntf* baseItem, BandDesignIntf* band)
{
    if (baseItem){
        foreach(BaseDesignIntf* item, baseItem->childBaseItems()){
            ContentItemDesignIntf* contentItem = dynamic_cast<ContentItemDesignIntf*>(item);
            if (contentItem) replaceGroupFunctionsInItem(contentItem, band);
            else replaceGroupFunctionsInContainer(item, band);
        }
    }
}

void ReportRender::replaceGroupsFunction(BandDesignIntf *band)
{
    replaceGroupFunctionsInContainer(band, band);
}

QColor ReportRender::makeBackgroundColor(BandDesignIntf* band){
    if (band->useAlternateBackgroundColor()){
        return datasources()->variable(QLatin1String("line_") + band->objectName().toLower()).toInt() %2 == 0 ?
                    band->backgroundColor() :
                    band->alternateBackgroundColor();
    }
    return band->backgroundColor();
}

BandDesignIntf* ReportRender::renderBand(BandDesignIntf *patternBand, BandDesignIntf* bandData, ReportRender::DataRenderMode mode, bool isLast)
{
    QCoreApplication::processEvents();
    bool bandIsSliced = false;
    if (patternBand){

        if (patternBand->isHeader())
            m_lastRenderedHeader = patternBand;

        BandDesignIntf* bandClone = 0;

        if (bandData){
           bandClone = bandData;
        } else {
            bandClone = renderData(patternBand);
        }

        if (isLast) bandClone->setBottomSpace(1);

        if (mode == ForcedStartPage){
            savePage();
            startNewPage();
        }

        if (patternBand->isFooter())
            m_lastRenderedFooter = patternBand;

        bandClone->setBackgroundColor(makeBackgroundColor(patternBand));
        patternBand->emitBandRendered(bandClone);
        m_scriptEngineContext->setCurrentBand(bandClone);

        if ( isLast && bandClone->keepFooterTogether() && bandClone->sliceLastRow() ){
            if (m_maxHeightByColumn[m_currentColumn] < (bandClone->height()+m_reportFooterHeight))
                m_maxHeightByColumn[m_currentColumn] -= ((m_maxHeightByColumn[m_currentColumn]-bandClone->height())+(bandClone->height()*calcSlicePercent(bandClone->height())));
         }

        if (!bandClone->isEmpty() || patternBand->printIfEmpty()){
            if (!registerBand(bandClone)){
                if (patternBand && patternBand->isHeader() && patternBand->reprintOnEachPage())
                    m_reprintableBands.removeOne(patternBand);
                if (bandClone->canBeSplitted(m_maxHeightByColumn[m_currentColumn])){
                    bandClone = sliceBand(bandClone, patternBand, isLast);
                    bandIsSliced = true;
                } else {
                    qreal percent = (bandClone->height()-m_maxHeightByColumn[m_currentColumn])/(bandClone->height()/100);
                    if (bandClone->maxScalePercent()>=percent){
                        if (percent<bandClone->maxScalePercent()){
                            percent += 2;
                            bandClone->setScale((100-percent)/100);
                            BandDesignIntf* upperPart = dynamic_cast<BandDesignIntf*>(bandClone->cloneUpperPart(m_maxHeightByColumn[m_currentColumn]));
                            registerBand(upperPart);
                            delete bandClone;
                            bandClone = NULL;
                        }
                    } else {

                        if (mode==StartNewPageAsNeeded){
                            if (bandClone->columnsCount()>1 &&
                                (bandClone->columnsFillDirection()==BandDesignIntf::Vertical ||
                                 bandClone->columnsFillDirection()==BandDesignIntf::VerticalUniform))
                            {
                                startNewColumn();
                                if (patternBand->bandHeader() &&
                                    patternBand->bandHeader()->columnsCount()>1 &&
                                    !m_lostHeadersMoved &&
                                    patternBand->bandNestingLevel() == 0
                                ){
                                    renderBand(patternBand->bandHeader(), 0, mode);
                                }
                            } else {
                                savePage();
                                startNewPage();
                                if (!bandIsSliced){
                                    BandDesignIntf* t = renderData(patternBand, false);
                                    t->copyBandAttributes(bandClone);
                                    patternBand->emitBandReRendered(bandClone, t);
                                    delete bandClone;
                                    bandClone = t;
                                }
                            }
                            if (!registerBand(bandClone)) {
                                BandDesignIntf* upperPart = dynamic_cast<BandDesignIntf*>(bandClone->cloneUpperPart(m_maxHeightByColumn[m_currentColumn]));
                                registerBand(upperPart);
                                patternBand->emitBandReRendered(bandClone, upperPart);
                                delete bandClone;
                                bandClone = NULL;
                            };
                        } else {
                            bandClone->setHeight(m_maxHeightByColumn[m_currentColumn]);
                            registerBand(bandClone);
                        }
                    }
                }
            }
        } else {
            delete bandClone;
            return 0;
        }

        if (patternBand->isFooter()){
            datasources()->clearGroupFunctionValues(patternBand->objectName());
        }

        emit(patternBand->afterRender());
        return bandClone;
    }
    return 0;
}

void ReportRender::renderDataBand(BandDesignIntf *dataBand)
{
    if (dataBand == NULL )
        return;

    IDataSource* bandDatasource = 0;
    m_lastRenderedFooter = 0;

    if (!dataBand->datasourceName().isEmpty())
       bandDatasource = datasources()->dataSource(dataBand->datasourceName());

    BandDesignIntf* header = dataBand->bandHeader();
    BandDesignIntf* footer = dataBand->bandFooter();

    if (header && header->printAlways()) renderDataHeader(header);

    if(bandDatasource && !bandDatasource->eof() && !m_renderCanceled){

        QString varName = QLatin1String("line_")+dataBand->objectName().toLower();
        datasources()->setReportVariable(varName,1);

        if (header && header->reprintOnEachPage())
            m_reprintableBands.append(dataBand->bandHeader());

        if (header && !header->printAlways())
            renderDataHeader(header);

        renderGroupHeader(dataBand, bandDatasource, true);

        bool firstTime = true;


        while(!bandDatasource->eof() && !m_renderCanceled){

            datasources()->updateChildrenData(dataBand->datasourceName());

            BandDesignIntf* rawData = renderData(dataBand);

            if (!rawData->isEmpty() || dataBand->printIfEmpty()){

                if ((firstTime && dataBand->startFromNewPage()) ||
                    (!firstTime && dataBand->startNewPage())) {
                    savePage();
                    startNewPage();
                }

                if (dataBand->tryToKeepTogether()) openDataGroup(dataBand);

                if (dataBand->keepFooterTogether() && !bandDatasource->hasNext())
                    openFooterGroup(dataBand);

                m_lastDataBand = dataBand;

                if (header && !firstTime && header->repeatOnEachRow())
                    renderBand(header, 0, StartNewPageAsNeeded);

                renderBand(dataBand, rawData, StartNewPageAsNeeded, !bandDatasource->hasNext());
                m_newPageStarted = false;
                renderChildBands(dataBand);

            }

            bandDatasource->next();

            datasources()->setReportVariable(varName,datasources()->variable(varName).toInt()+1);

            QList<BandDesignIntf *> bandList;
            QList<BandDesignIntf *> childList;

            bandList = dataBand->childrenByType(BandDesignIntf::GroupHeader);
            while (bandList.size() > 0)
            {
                childList.clear();
                foreach (BandDesignIntf* band, bandList)
                {
                    childList.append(band->childrenByType(BandDesignIntf::GroupHeader));

                    QString groupLineVar = QLatin1String("line_")+band->objectName().toLower();
                    if (datasources()->containsVariable(groupLineVar))
                        datasources()->setReportVariable(groupLineVar,datasources()->variable(groupLineVar).toInt()+1);
                }
                bandList = childList;
            }

            renderGroupHeader(dataBand, bandDatasource, false);
            if (dataBand->tryToKeepTogether()) closeDataGroup(dataBand);
            firstTime = false;
        }

        if (!dataBand->keepFooterTogether())
        	m_reprintableBands.removeOne(header);
        if (header) recalcIfNeeded(header);

        bool didGoBack = !bandDatasource->eof() && bandDatasource->prior();
        renderGroupFooterByHeader(dataBand);
        if (didGoBack){
            bandDatasource->next();
        }

        if (footer && !footer->printAlways()){
            renderBand(footer, 0, StartNewPageAsNeeded);
            if (dataBand->keepFooterTogether())
                m_reprintableBands.removeOne(dataBand);
        }

        datasources()->deleteVariable(varName);

    } else if (bandDatasource==0) {
        renderBand(dataBand, 0, StartNewPageAsNeeded);
    }

    if (footer && footer->printAlways()){
        renderBand(footer, 0, StartNewPageAsNeeded);
        if (dataBand->keepFooterTogether())
            m_reprintableBands.removeOne(dataBand);
    }
}

void ReportRender::renderPageHeader(PageItemDesignIntf *patternPage)
{
    BandDesignIntf* band = patternPage->bandByType(BandDesignIntf::PageHeader);
    if (band){
        if (m_datasources->variable("#PAGE").toInt()!=1 ||
            band->property("printOnFirstPage").toBool()
        )
            renderBand(band, 0);
    }
}

void ReportRender::renderReportHeader(PageItemDesignIntf *patternPage, PageRenderStage stage)
{
    BandDesignIntf* band = patternPage->bandByType(BandDesignIntf::ReportHeader);
    BandDesignIntf* resBand = 0;
    if (band){
        if (band->property("printBeforePageHeader").toBool() && stage == BeforePageHeader )
            resBand = renderBand(band, 0, StartNewPageAsNeeded);
        if (!band->property("printBeforePageHeader").toBool() && stage == AfterPageHeader )
            resBand = renderBand(band, 0, StartNewPageAsNeeded);
        if (resBand && containsGroupFunctions(band)) m_recalcBands.append(resBand);
    }
}

void ReportRender::renderPageFooter(PageItemDesignIntf *patternPage)
{
    BandDesignIntf* band = patternPage->bandByType(BandDesignIntf::PageFooter);
    if (band){
        BandDesignIntf* bandClone = dynamic_cast<BandDesignIntf*>(band->cloneItem(PreviewMode, m_renderPageItem, m_renderPageItem));
        replaceGroupsFunction(bandClone);
        bandClone->updateItemSize(m_datasources);
        bandClone->setItemPos(m_renderPageItem->pageRect().x(),m_renderPageItem->pageRect().bottom()-bandClone->height());
        bandClone->setHeight(m_pageFooterHeight);
        for(int i=0;i<m_maxHeightByColumn.size();++i)
            m_maxHeightByColumn[i]+=m_pageFooterHeight;
        m_renderPageItem->setPageFooter(bandClone);
        registerBand(bandClone);
        datasources()->clearGroupFunctionValues(band->objectName());
    }
}

void ReportRender::renderPageItems(PageItemDesignIntf* patternPage)
{
    QList<BaseDesignIntf*> pageItems;
    foreach (BaseDesignIntf* item, patternPage->childBaseItems()) {
        ItemDesignIntf* id = dynamic_cast<ItemDesignIntf*>(item);
        if (id&&id->itemLocation()==ItemDesignIntf::Page){
            BaseDesignIntf* cloneItem = item->cloneItem(m_renderPageItem->itemMode(),
                                                        m_renderPageItem,
                                                        m_renderPageItem);
            pageItems.append(cloneItem);
        }
    }
    m_renderPageItem->restoreLinks();
    m_renderPageItem->updateSubItemsSize(FirstPass,m_datasources);
    foreach(BaseDesignIntf* item, pageItems){
        if (!item->isWatermark())
            item->setZValue(item->zValue()-100000);
        else
            item->setZValue(item->zValue()+100000);
    }
}

qreal ReportRender::calcPageFooterHeight(PageItemDesignIntf *patternPage)
{
    BandDesignIntf* band = patternPage->bandByType(BandDesignIntf::PageFooter);
    if (band){
        if (m_datasources->variable("#PAGE")!=1)
            return band->height();
        else if (band->property("printOnFirstPage").toBool())
            return band->height();
    }
    return 0;
}

void ReportRender::renderChildHeader(BandDesignIntf *parent, BandPrintMode printMode)
{
    foreach(BandDesignIntf* band,parent->childrenByType(BandDesignIntf::SubDetailHeader)){
        bool printAlways=false;
        if (band->metaObject()->indexOfProperty("printAlways")>0){
            printAlways=band->property("printAlways").toBool();
        }
        if (printAlways == (printMode == PrintAlwaysPrintable))
            renderBand(band, 0, StartNewPageAsNeeded);
    }
}

void ReportRender::renderChildFooter(BandDesignIntf *parent, BandPrintMode printMode)
{
    foreach(BandDesignIntf* band,parent->childrenByType(BandDesignIntf::SubDetailFooter)){
        bool printAlways=false;
        if (band->metaObject()->indexOfProperty("printAlways")>0){
            printAlways=band->property("printAlways").toBool();
        }

        if ( (band != m_lastRenderedFooter) && (printAlways == (printMode == PrintAlwaysPrintable)) )
            renderBand(band, 0, StartNewPageAsNeeded);
    }
}

void ReportRender::renderChildBands(BandDesignIntf *parentBand)
{
    foreach(BandDesignIntf* band,parentBand->childrenByType(BandDesignIntf::SubDetailBand)){
        IDataSource* ds = 0;
        if (!band->datasourceName().isEmpty())
            ds = m_datasources->dataSource(band->datasourceName());
        if (ds) ds->first();
        renderDataBand(band);
        closeFooterGroup(band);
    }
}

BandDesignIntf* ReportRender::findRecalcableBand(BandDesignIntf* patternBand){

    QList<BandDesignIntf*>::iterator it = m_recalcBands.begin();
    for (;it !=m_recalcBands.end() ;++it){
        if ((*it)->patternItem() == patternBand){
            BandDesignIntf* result = (*it);
            m_recalcBands.erase(it);
            return result;
        }
    }
    return 0;
}

void restoreItemsWithGroupFunctions(BaseDesignIntf* container, DataSourceManager* datasources){
    foreach(BaseDesignIntf* bi, container->childBaseItems()){
        ContentItemDesignIntf* pci = dynamic_cast<ContentItemDesignIntf*>(bi->patternItem());
        if (pci && checkContentItem(pci, datasources)){
            ContentItemDesignIntf* ci = dynamic_cast<ContentItemDesignIntf*>(bi);
            ci->setContent(pci->content());
        } else restoreItemsWithGroupFunctions(bi, datasources);
    }
}

void ReportRender::recalcIfNeeded(BandDesignIntf* band){
    BandDesignIntf* recalcBand = findRecalcableBand(band);
    if (recalcBand){
        QString bandName = recalcBand->objectName();
        restoreItemsWithGroupFunctions(recalcBand, m_datasources);
        recalcBand->setObjectName(recalcBand->patternItem()->objectName());
        replaceGroupsFunction(recalcBand);
        recalcBand->updateItemSize(datasources());
        recalcBand->setObjectName(bandName);
        datasources()->clearGroupFunctionValues(recalcBand->patternItem()->objectName());
    }
}

void ReportRender::renderDataHeader(BandDesignIntf *header)
{
    recalcIfNeeded(header);
    BandDesignIntf* renderedHeader = renderBand(header, 0, StartNewPageAsNeeded);
    if (containsGroupFunctions(header))
        m_recalcBands.append(renderedHeader);
}

void ReportRender::renderGroupHeader(BandDesignIntf *parentBand, IDataSource* dataSource, bool firstTime)
{
    foreach(BandDesignIntf* band,parentBand->childrenByType(BandDesignIntf::GroupHeader)){
        IGroupBand* gb = dynamic_cast<IGroupBand*>(band);
        if (gb&&gb->isNeedToClose(datasources())){
            if (band->childBands().count()>0){
                bool didGoBack = !dataSource->eof() && dataSource->prior();
                renderGroupFooterByHeader(band);
                if (didGoBack){
                    dataSource->next();
                }
            }
            closeDataGroup(band);
        }

        if (gb && !gb->isStarted()){
            if (band->reprintOnEachPage()){
                m_reprintableBands.append(band);
            }
            gb->startGroup(m_datasources);
            openDataGroup(band);
            BandDesignIntf* renderedHeader = 0;
            if (!firstTime && gb->startNewPage() && !m_newPageStarted){
                if (gb->resetPageNumber()) resetPageNumber(BandReset);
                if (band->reprintOnEachPage()){
                    savePage();
                    startNewPage();
                } else {
                    renderedHeader = renderBand(band, 0, ForcedStartPage);
                }
            } else {
                renderedHeader = renderBand(band, 0, StartNewPageAsNeeded);
            }
            if (containsGroupFunctions(band))
                m_recalcBands.append(renderedHeader);
        }

        renderGroupHeader(band, dataSource, firstTime);
    }
}

void ReportRender::renderGroupFooterByHeader(BandDesignIntf* groupHeader){
    if (groupHeader->reprintOnEachPage()) m_reprintableBands.removeOne(groupHeader);
    foreach (BandDesignIntf* header, groupHeader->childrenByType(BandDesignIntf::GroupHeader)){
        renderGroupFooterByHeader(header);
    }
    foreach (BandDesignIntf* footer, groupHeader->childrenByType(BandDesignIntf::GroupFooter)){
        renderBand(footer, 0, StartNewPageAsNeeded);
    }
    closeDataGroup(groupHeader);
}

void ReportRender::renderGroupFooter(BandDesignIntf *parentBand)
{
    foreach(BandDesignIntf* band,parentBand->childrenByType(BandDesignIntf::GroupHeader)){
        IGroupBand* gb = dynamic_cast<IGroupBand*>(band);
        if (gb && gb->isStarted()){
            renderGroupFooterByHeader(band);
        }
    }
}

void ReportRender::initGroups()
{
    m_datasources->clearGroupFunction();
    foreach(BandDesignIntf* band, m_patternPageItem->childBands()){
        if (band->isFooter()) extractGroupFunctions(band);
        if (band->isHeader()){
            IGroupBand* gb = dynamic_cast<IGroupBand*>(band);
            if (gb) gb->closeGroup();
            extractGroupFunctions(band);
        }
    }
}

void ReportRender::popPageFooterGroupValues(BandDesignIntf *dataBand)
{
    BandDesignIntf* pageFooter = m_patternPageItem->bandByType(BandDesignIntf::PageFooter);
    if (pageFooter){
        foreach(GroupFunction* gf, datasources()->groupFunctionsByBand(pageFooter->objectName())){
            if ((gf->dataBandName()==dataBand->objectName())){
                // FIXME Probably coincidence Field and Variables
                if ((!m_popupedExpression.contains(dataBand))||(!m_popupedExpression.values(dataBand).contains(gf->data()))){
                    m_popupedExpression.insert(dataBand,gf->data());
                    m_popupedValues.insert(QString("%1").arg((quintptr)dataBand)+'|'+gf->data(), gf->values()[gf->values().count()-1]);
                    gf->values().pop_back();
                }
            }
        }
    }
}

void ReportRender::pushPageFooterGroupValues(BandDesignIntf *dataBand)
{
    BandDesignIntf* pageFooter = m_patternPageItem->bandByType(BandDesignIntf::PageFooter);
    if (pageFooter){
        foreach(GroupFunction* gf, datasources()->groupFunctionsByBand(pageFooter->objectName())){
            if ((gf->dataBandName()==dataBand->objectName())){
                // FIXME Probably coincidence Field and Variables
                if ((m_popupedExpression.contains(dataBand))&&(m_popupedExpression.values(dataBand).contains(gf->data()))){
                    gf->values().push_back(m_popupedValues.value(QString("%1").arg((quintptr)dataBand)+'|'+gf->data()));
                }
            }
        }
    }
}

void ReportRender::closeGroup(BandDesignIntf *band)
{
    QMultiMap< BandDesignIntf*, GroupBandsHolder* >::iterator it;
    it = m_childBands.find(band);

    while (it!=m_childBands.end()&&it.key()==band){
        GroupBandsHolder* bl = it.value();
        if (bl){
            bl->clear();
            delete bl;
        }
        ++it;
    }

    m_childBands.remove(band);
}

void ReportRender::openDataGroup(BandDesignIntf *band)
{
    m_childBands.insert(band,new GroupBandsHolder(band->tryToKeepTogether()));
}

void ReportRender::openFooterGroup(BandDesignIntf *band)
{
    GroupBandsHolder* holder = new GroupBandsHolder(true);
    holder->setIsFooterGroup();
    m_childBands.insert(band,holder);
}

void ReportRender::closeDataGroup(BandDesignIntf *band)
{
    IGroupBand* groupBand = dynamic_cast<IGroupBand*>(band);
    if (groupBand){
        groupBand->closeGroup();
        if (band->reprintOnEachPage()) m_reprintableBands.removeOne(band);

        QList<BandDesignIntf*>::Iterator it = m_reprintableBands.begin();
        while (it != m_reprintableBands.end()){
            if ((*it)->bandIndex()>band->bandIndex())
                it = m_reprintableBands.erase(it);
            else
                ++it;
        }
    }
    recalcIfNeeded(band);
    closeGroup(band);
}

void ReportRender::closeFooterGroup(BandDesignIntf *band){
    closeGroup(band);
}

qreal maxVectorValue(QVector<qreal> vector){
    qreal curValue = 0;
    foreach (qreal value, vector) {
        if (curValue<value) curValue=value;
    }
    return curValue;
}

qreal minVectorValue(QVector<qreal> vector){
    qreal curValue = vector[0];
    foreach (qreal value, vector) {
        if (curValue>value) curValue=value;
    }
    return curValue;
}

void ReportRender::placeBandOnPage(BandDesignIntf* band, int columnIndex){

    qreal bandPos = m_currentStartDataPos[columnIndex];

    m_currentStartDataPos[columnIndex] += band->height();
    m_maxHeightByColumn[columnIndex] -= band->height();
    band->setPos(m_renderPageItem->pageRect().x()+band->width()*columnIndex, bandPos);
    band->setBandIndex(++m_currentIndex);
    band->setColumnIndex(columnIndex);

    m_renderPageItem->registerBand(band);
    m_currentColumn = columnIndex;
}

bool isMultiColumnHeader(BandDesignIntf* band){
    return ( (band->columnsCount() > 1 ) &&
             (band->isHeader() &&
             ((band->bandNestingLevel() == 0) || (band->columnsFillDirection() == BandDesignIntf::Horizontal))));
}

bool ReportRender::registerBand(BandDesignIntf *band, bool registerInChildren)
{

    if (band->columnsCount() == 1 && m_maxHeightByColumn.size() > 1 ){
        if (band->bandType() != BandDesignIntf::PageFooter){
            rearrangeColumnsItems();
            m_currentColumn = 0;
            qreal minValue = minVectorValue(m_maxHeightByColumn);
            m_maxHeightByColumn.clear();
            m_maxHeightByColumn.append(minValue);
            qreal maxValue = maxVectorValue(m_currentStartDataPos);
            m_currentStartDataPos.clear();
            m_currentStartDataPos.append(maxValue);
        }
    }

    if (band->columnsCount() != m_maxHeightByColumn.size()){
        for(int i=1;i<band->columnsCount();++i){
            m_maxHeightByColumn.append(m_maxHeightByColumn[0]);
            m_currentStartDataPos.append(m_currentStartDataPos[0]);
        }
        m_currentColumn = -1;
    }

    if ( (band->columnsCount() > 1) &&
         (!band->isHeader() || (band->bandNestingLevel() > 0 && band->columnsFillDirection() != BandDesignIntf::Horizontal ))){

        if (band->columnsFillDirection() == BandDesignIntf::Horizontal){
            if (m_currentColumn < band->columnsCount()-1)
                m_currentColumn = m_currentColumn+1;
            else
                m_currentColumn = 0;
        } else {
            m_currentColumn = m_currentColumn == -1 ? 0: m_currentColumn;
            if ((m_currentColumn !=0) &&
                (m_maxHeightByColumn[0] == m_maxHeightByColumn[m_currentColumn]) &&
                (m_maxHeightByColumn[0] >= band->height())
            ){
                m_currentColumn = 0;
            }
        }
    }

    m_currentColumn = m_currentColumn == -1 ? 0: m_currentColumn;

    if (  (band->height() <= m_maxHeightByColumn[m_currentColumn]) ||
          m_patternPageItem->endlessHeight() ||
          (isMultiColumnHeader(band) && (band->height() <= m_maxHeightByColumn[0]))
    ){

        if ( (band->bandType() == BandDesignIntf::PageFooter) ){
           for (int i=0; i < m_maxHeightByColumn.size(); ++i)
               m_maxHeightByColumn[i]+=band->height();
        }

        if ( isMultiColumnHeader(band)){

            if (!band->parent()){
                for (int i = 0; i < band->columnsCount(); ++i){
                    m_currentColumn = i;
                    if (i != 0) band = dynamic_cast<BandDesignIntf*>(band->cloneItem(PreviewMode));
                    placeBandOnPage(band, i);
                }
            } else {
                placeBandOnPage(band, band->columnIndex());
            }


        } else {

            if (band->bandType() != BandDesignIntf::PageFooter){
                placeBandOnPage(band, m_currentColumn);
            }

            if (band->columnsCount() > 1){
                m_columnedBandItems.append(band);
            }

        }

        foreach(QList<BandDesignIntf*>* list, m_childBands.values()){
            if (registerInChildren &&
                band->bandType()!=BandDesignIntf::PageHeader &&
                band->bandType()!=BandDesignIntf::PageFooter &&
                band->bandType()!=BandDesignIntf::ReportHeader &&
                band->bandType()!=BandDesignIntf::ReportFooter &&
                !list->contains(band) &&
                !band->reprintOnEachPage()
            )
                list->append(band);
        }

        if (band->isData()) m_renderedDataBandCount++;
        band->setObjectName(band->objectName()+QString::number(++m_currentNameIndex));
        renameChildItems(band);
        if (m_lastDataBand){
#if QT_VERSION < 0x050000
            m_lastDataBand->metaObject()->invokeMethod(m_lastDataBand,"bandRegistred");
#else
            emit m_lastDataBand->bandRegistred();
#endif
        }
        if (band->bandType() != BandDesignIntf::PageFooter)
            m_lastRenderedBand = band;
        return true;
    } else return false;
}

qreal ReportRender::calcSlicePercent(qreal height){
    return (height*3/(m_dataAreaSize))/100;
}

BandDesignIntf* ReportRender::sliceBand(BandDesignIntf *band, BandDesignIntf* patternBand, bool isLast)
{
    while (band->height()>m_maxHeightByColumn[m_currentColumn]) {
        band = saveUppperPartReturnBottom(band,m_maxHeightByColumn[m_currentColumn],patternBand);
        if (!band->isEmpty()) {
            if (band->autoHeight()){
                if (band->isNeedUpdateSize(FirstPass)){
                    band->setHeight(0);
                }
                band->updateItemSize(m_datasources);
            }
            DataBandDesignIntf* data = dynamic_cast<DataBandDesignIntf*>(band);
            if (isLast && data && data->keepFooterTogether() &&
                band->height()<m_maxHeightByColumn[m_currentColumn] && band->sliceLastRow()
            ){
                if (band->height()>(m_maxHeightByColumn[m_currentColumn]-m_reportFooterHeight)){
                    m_maxHeightByColumn[m_currentColumn] -= ((m_maxHeightByColumn[m_currentColumn]-band->height())+(band->height()*calcSlicePercent(band->height())));
                }
            }
            if (registerBand(band)) break;
        } else break;
    }

    if (band->isEmpty()) {
        delete band;
        band = 0;
    }

    return band;

}

void ReportRender::updateTOC(BaseDesignIntf* item, int pageNumber){
    BookmarkContainerDesignIntf* bookmarkContainer = dynamic_cast<BookmarkContainerDesignIntf*>(item);
    if (bookmarkContainer){
        TableOfContents* toc = m_scriptEngineContext->tableOfContents();
        foreach (QString key, bookmarkContainer->bookmarks()){
            toc->setItem(key, bookmarkContainer->getBookMark(key).toString(), pageNumber);
        }
    }
}

void ReportRender::secondRenderPass(ReportPages renderedPages)
{
    if (!m_scriptEngineContext->tableOfContents()->isEmpty()){
        for(int i = 0; i < renderedPages.count(); ++i){
            PageItemDesignIntf::Ptr page = renderedPages.at(i);
            updateTOC(page.data(), m_pagesRanges.findPageNumber(i));
            foreach(BaseDesignIntf* item, page->childBaseItems()){
                updateTOC(item, m_pagesRanges.findPageNumber(i));
            }
        }
    }

    for(int i = 0; i < renderedPages.count(); ++i){
        PageItemDesignIntf::Ptr page = renderedPages.at(i);
        m_datasources->setReportVariable("#PAGE",m_pagesRanges.findPageNumber(i));
        m_datasources->setReportVariable("#PAGE_COUNT",m_pagesRanges.findLastPageNumber(i));
        foreach(BaseDesignIntf* item, page->childBaseItems()){
            if (item->isNeedUpdateSize(SecondPass))
                item->updateItemSize(m_datasources, SecondPass);
        }
    }
}

void ReportRender::createTOCMarker(bool startNewRange)
{
    m_pagesRanges.addTOCMarker(startNewRange);
}

BandDesignIntf *ReportRender::saveUppperPartReturnBottom(BandDesignIntf *band, int height, BandDesignIntf* patternBand)
{
    //int sliceHeight = height;
    BandDesignIntf* upperBandPart = dynamic_cast<BandDesignIntf*>(band->cloneUpperPart(height));
    BandDesignIntf* bottomBandPart = dynamic_cast<BandDesignIntf*>(band->cloneBottomPart(height));
    if (!bottomBandPart->isEmpty()){
        if (patternBand->keepFooterTogether())
            closeFooterGroup(patternBand);
        if (upperBandPart->isEmpty())
            bottomBandPart->copyBookmarks(band);
    }
    if (!upperBandPart->isEmpty()){
        upperBandPart->setBottomSpace(0);
        upperBandPart->updateItemSize(m_datasources, FirstPass, height);
        registerBand(upperBandPart);
        upperBandPart->copyBookmarks(band);
        if (patternBand->isFooter())
            closeFooterGroup(m_lastDataBand);
    } else delete upperBandPart;

    if (band->columnsCount()>1 &&
        (band->columnsFillDirection()==BandDesignIntf::Vertical ||
         band->columnsFillDirection()==BandDesignIntf::VerticalUniform)){
        startNewColumn();
        if (patternBand->bandHeader() &&
            patternBand->bandHeader()->columnsCount()>1 &&
            !m_lostHeadersMoved &&
            patternBand->bandNestingLevel() == 0
        ){
            renderBand(patternBand->bandHeader(), 0, StartNewPageAsNeeded);
        }

    } else {
        savePage();
        startNewPage();
    }

    delete band;
    return bottomBandPart;
}

BandDesignIntf *ReportRender::renderData(BandDesignIntf *patternBand, bool emitBeforeRender)
{
    BandDesignIntf* bandClone = dynamic_cast<BandDesignIntf*>(patternBand->cloneItem(PreviewMode));

    m_scriptEngineContext->baseDesignIntfToScript(patternBand->parent()->objectName(), bandClone);
    m_scriptEngineContext->setCurrentBand(bandClone);
    if (emitBeforeRender)
        emit(patternBand->beforeRender());

    if (patternBand->isFooter()){
        replaceGroupsFunction(bandClone);
    }

    if (patternBand->isHeader()){
        replaceGroupsFunction(bandClone);
    }

    emit(patternBand->preparedForRender());
    bandClone->setBottomSpace(patternBand->height() - patternBand->findMaxBottom());
    bandClone->updateItemSize(m_datasources);

    //m_scriptEngineContext->baseDesignIntfToScript(bandClone);
    emit(patternBand->afterData());

    return bandClone;
}

void ReportRender::startNewColumn(){
    if (m_currentColumn < m_maxHeightByColumn.size()-1){
        m_currentColumn++;
        checkLostHeadersInPrevColumn();
    } else {
        savePage();
        startNewPage();
    }
}

void ReportRender::startNewPage(bool isFirst)
{
    m_renderPageItem = 0;
    m_newPageStarted = true;
    initColumns();
    initRenderPage();
    m_scriptEngineContext->setCurrentPage(m_renderPageItem);
    m_scriptEngineContext->baseDesignIntfToScript(m_renderPageItem->patternName(), m_renderPageItem);
    emit m_patternPageItem->beforeRender();

    m_renderPageItem->setObjectName(QLatin1String("ReportPage")+QString::number(m_pageCount));
    m_maxHeightByColumn[m_currentColumn] = m_renderPageItem->pageRect().height();
    m_currentStartDataPos[m_currentColumn] = m_patternPageItem->topMargin() * Const::mmFACTOR;
    m_currentIndex = 0;

    if (isFirst) {
        renderReportHeader(m_patternPageItem, BeforePageHeader);
        emit m_patternPageItem->beforeFirstPageRendered();
    }

    renderPageHeader(m_patternPageItem);

    m_pageFooterHeight = calcPageFooterHeight(m_patternPageItem)+2;
    m_maxHeightByColumn[m_currentColumn] -= m_pageFooterHeight;
    m_currentIndex = 10;
    m_dataAreaSize = m_maxHeightByColumn[m_currentColumn];
    m_renderedDataBandCount = 0;

    foreach (BandDesignIntf* band, m_reprintableBands) {
        renderBand(band, 0);
    }

    checkLostHeadersOnPrevPage();
    pasteGroups();

}

void ReportRender::resetPageNumber(ResetPageNuberType resetType)
{
    m_pagesRanges.startNewRange();
    if (resetType == PageReset)
        m_datasources->setReportVariable("#PAGE",1);
}

void ReportRender::cutGroups()
{
    m_popupedExpression.clear();
    m_popupedValues.clear();
    //foreach(BandDesignIntf* groupBand,m_childBands.keys()){
    foreach(BandDesignIntf* groupBand, m_childBands.keys()){
        if (m_childBands.value(groupBand)->tryToKeepTogether()){
            foreach(BandDesignIntf* band, *m_childBands.value(groupBand)){
                m_renderPageItem->removeBand(band);
                popPageFooterGroupValues(band);
                band->setParent(0);
                band->setParentItem(0);
            }
        }
    }

}

void ReportRender::checkFooterGroup(BandDesignIntf *groupBand)
{
    if (m_childBands.contains(groupBand)){
        GroupBandsHolder* holder = m_childBands.value(groupBand);
        foreach(BandDesignIntf* band, *holder){
            qreal percent = band->height()*100 / m_dataAreaSize;
            if (m_renderedDataBandCount<=1 || percent>20 ){
                 holder->removeAll(band);
            }
        }
    }
}

void ReportRender::pasteGroups()
{
    BandDesignIntf* groupBand = findEnclosingGroup();
    if (groupBand){
        foreach(BandDesignIntf* band, *m_childBands.value(groupBand)){
            registerBand(band,false);
            if (band->isData()) m_renderedDataBandCount++;
            pushPageFooterGroupValues(band);
        }
        foreach(GroupBandsHolder* holder, m_childBands.values())
            holder->setTryToKeepTogether(false);
    }
    m_popupedExpression.clear();
    m_popupedValues.clear();
}

bool bandLessThen(BandDesignIntf* b1, BandDesignIntf* b2){
    return b1->bandIndex() < b2->bandIndex();
}

void ReportRender::checkLostHeadersOnPrevPage()
{
    QVector<BandDesignIntf*> lostHeaders;

    if (m_renderedPages.isEmpty()) return;
    PageItemDesignIntf::Ptr page = m_renderedPages.last();
    if (page->bands().isEmpty()) return;

    QMutableListIterator<BandDesignIntf*>it(page->bands());

    it.toBack();
    if (it.hasPrevious()){
        if (it.previous()->isFooter()){
                if (it.hasPrevious()) it.previous();
                else return;
        }
    }

    while (it.hasPrevious()){
        if (it.value()->isHeader()){
            if (it.value()->reprintOnEachPage()){
                delete it.value();
            } else { lostHeaders.append(it.value());}
            it.remove();
            it.previous();
        } else break;
    }

    if (lostHeaders.size() > 0){
        m_lostHeadersMoved = true;
        //std::sort(lostHeaders.begin(), lostHeaders.end(), bandLessThen);
        std::sort(lostHeaders.begin(), lostHeaders.end(), bandLessThen);
        foreach(BandDesignIntf* header, lostHeaders){
            registerBand(header);
        }
    } else {
        m_lostHeadersMoved = false;
    }


}

void ReportRender::checkLostHeadersInPrevColumn()
{
    QVector<BandDesignIntf*> lostHeaders;

    QMutableListIterator<BandDesignIntf*>it(m_renderPageItem->bands());

    it.toBack();
    if (it.hasPrevious()){
        if (it.previous()->isFooter()){
                if (it.hasPrevious()) it.previous();
                else return;
        }
    }

    while (it.hasPrevious()){
        if (it.value()->isHeader()){
            if (it.value()->reprintOnEachPage()){
                delete it.value();
            } else { lostHeaders.append(it.value());}
            it.remove();
            it.previous();
        } else break;
    }

    if (lostHeaders.size() > 0){
        m_lostHeadersMoved = true;
//        std::sort(lostHeaders.begin(), lostHeaders.end(), bandLessThen);
        std::sort(lostHeaders.begin(), lostHeaders.end(), bandLessThen);
        foreach(BandDesignIntf* header, lostHeaders){
            registerBand(header);
        }
    } else {
        m_lostHeadersMoved = false;
    }
}

BandDesignIntf* ReportRender::findEnclosingGroup()
{
    BandDesignIntf* result=0;
    int groupIndex = -1;
    if (!m_childBands.isEmpty()){
        foreach(BandDesignIntf* gb, m_childBands.keys()){
            if (m_childBands.value(gb)->tryToKeepTogether()&&
                ((gb->bandIndex()<groupIndex)||(groupIndex==-1))
            ){
                result=gb;
                groupIndex=result->bandIndex();
            }
        }
    }
    return result;
}

void ReportRender::savePage(bool isLast)
{

    m_datasources->setReportVariable("#IS_LAST_PAGEFOOTER",isLast);
    m_datasources->setReportVariable("#IS_FIRST_PAGEFOOTER",m_datasources->variable("#PAGE").toInt()==1);

    renderPageItems(m_patternPageItem);

    if (m_renderPageItem->isEmpty() && m_renderPageItem->notPrintIfEmpty()) return;

    if (m_renderPageItem->isTOC())
        m_pagesRanges.addTOCPage();
    else
        m_pagesRanges.addPage();

    checkFooterGroup(m_lastDataBand);
    cutGroups();
    rearrangeColumnsItems();
    m_columnedBandItems.clear();

    BandDesignIntf* pf = m_patternPageItem->bandByType(BandDesignIntf::PageFooter);
    if (pf && m_datasources->variable("#PAGE").toInt()!=1 && !isLast){
        renderPageFooter(m_patternPageItem);
    } else {
        if (pf && pf->property("printOnFirstPage").toBool() && m_datasources->variable("#PAGE").toInt()==1){
            renderPageFooter(m_patternPageItem);
        } else if(pf && pf->property("printOnLastPage").toBool() && isLast){
            renderPageFooter(m_patternPageItem);
        }
    }

    if (m_pagesRanges.currentRange(m_patternPageItem->isTOC()).firstPage == 0) {
        m_datasources->setReportVariable("#PAGE",1);
    } else {
        m_datasources->setReportVariable("#PAGE",m_datasources->variable("#PAGE").toInt()+1);
    }

    m_renderedPages.append(PageItemDesignIntf::Ptr(m_renderPageItem));
    m_pageCount++;
    emit pageRendered(m_pageCount);

    if (isLast){
        BandDesignIntf* ph = m_renderPageItem->bandByType(BandDesignIntf::PageHeader);
        if (ph && !ph->property("printOnLastPage").toBool()){
            delete ph;
        }
    }

    if (m_renderPageItem->pageFooter()){
        m_renderPageItem->pageFooter()->setBandIndex(++m_currentIndex);
        if (m_renderPageItem->pageFooter()->property("removeGap").toBool()){
            m_renderPageItem->pageFooter()->setPos(m_lastRenderedBand->geometry().bottomLeft());
        }
    }
    m_renderPageItem->placeTearOffBand();

    //m_scriptEngineContext->setCurrentPage(m_renderPageItem);
    emit m_patternPageItem->afterRender();
    if (isLast)
        emit m_patternPageItem->afterLastPageRendered();
    if (isLast && m_patternPageItem->endlessHeight()){
        qreal pageHeight = 0;
        foreach (BandDesignIntf* band, m_renderPageItem->bands()) {
            pageHeight += band->height();
        }
        m_renderPageItem->setHeight(pageHeight + 10 +
           (m_patternPageItem->topMargin() + m_patternPageItem->bottomMargin()) * Const::mmFACTOR);
    }
}

QString ReportRender::toString()
{
    QScopedPointer<ItemsWriterIntf> writer(new XMLWriter());
    foreach(PageItemDesignIntf::Ptr page,m_renderedPages){
        writer->putItem(page.data());
    }
    return writer->saveToString();
}

ReportRender::~ReportRender(){
    m_renderedPages.clear();
}

void ReportRender::cancelRender(){
    m_renderCanceled = true;
}

int PagesRanges::findLastPageNumber(int index)
{
    index++;
    foreach (PagesRange range, m_ranges) {
        if ( range.firstPage <= (index) && range.lastPage>= (index) )
            return (range.lastPage-(range.firstPage))+1;
    }
    return 0;
}

int PagesRanges::findPageNumber(int index)
{
    index++;
    foreach (PagesRange range, m_ranges) {
        if ( range.firstPage <= (index) && range.lastPage >= (index) )
            return (index - range.firstPage)+1;
    }
    return 0;
}

PagesRange&PagesRanges::currentRange(bool isTOC)
{
    Q_ASSERT( (isTOC && m_TOCRangeIndex!=-1) || !isTOC);
    if (isTOC && m_TOCRangeIndex !=-1) return m_ranges[m_TOCRangeIndex];
    return m_ranges.last();
}

void PagesRanges::startNewRange(bool isTOC)
{
    PagesRange range;
    if (!m_ranges.isEmpty()){
        range.firstPage = 0;
        range.lastPage = m_ranges.last().lastPage + 1;
    } else {
        range.firstPage = 0;
        range.lastPage = 0;
    }
    range.isTOC = isTOC;
    m_ranges.append(range);
    if (isTOC) m_TOCRangeIndex = m_ranges.size()-1;
}

void PagesRanges::addTOCMarker(bool addNewRange)
{
    if ( addNewRange || m_ranges.isEmpty()){
        startNewRange(true);
    } else {
        m_TOCRangeIndex =  m_ranges.size()-1;
        m_ranges.last().isTOC = true;
    }
}

void PagesRanges::addPage()
{
    if (m_ranges.isEmpty()) startNewRange();
    if (m_ranges.last().firstPage == 0){
        m_ranges.last().firstPage = m_ranges.last().lastPage == 0 ? 1 : m_ranges.last().lastPage;
        m_ranges.last().lastPage = m_ranges.last().lastPage == 0 ? 1 : m_ranges.last().lastPage;
    } else {
        m_ranges.last().lastPage++;
    }
}

void PagesRanges::shiftRangesNextToTOC(){
    for(int i = m_TOCRangeIndex+1; i < m_ranges.size(); ++i){
        m_ranges[i].firstPage++;
        m_ranges[i].lastPage++;
    }
}

void PagesRanges::addTOCPage()
{
    Q_ASSERT(m_TOCRangeIndex != -1);
    if (m_TOCRangeIndex != -1){
        PagesRange& tocRange = m_ranges[m_TOCRangeIndex];
        if (tocRange.firstPage == 0) {
            tocRange.firstPage = tocRange.lastPage == 0 ? 1 :  tocRange.lastPage;
            tocRange.lastPage = tocRange.lastPage == 0 ? 1 :  tocRange.lastPage;
            if (tocRange.firstPage == 1 && tocRange.lastPage == 1)
                shiftRangesNextToTOC();
        } else {
            tocRange.lastPage++;
            shiftRangesNextToTOC();
        }
    }
}

void PagesRanges::clear()
{
    m_ranges.clear();
}

} // namespace LimeReport
