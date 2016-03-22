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
#include <stdexcept>
#include <QMessageBox>
#include <QApplication>

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
        int avg = m_columnedBandItems.size()/m_columnedBandItems[0]->columnsCount();
        for (int i=1;i<m_columnedBandItems[0]->columnsCount();++i){
           if (columnItemsCount(i)<avg){
               int getCount = avg * (m_columnedBandItems[0]->columnsCount()-i) - columnItemsCount(i);
               for (int j=0;j<getCount;++j){
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

ReportRender::ReportRender(QObject *parent)
    :QObject(parent), m_renderPageItem(0), m_pageCount(0), m_currentColumn(0)
{
    initColumns();
}

void ReportRender::setDatasources(DataSourceManager *value)
{
    m_datasources=value;
}

void ReportRender::initDatasources(){
    try{
        datasources()->setAllDatasourcesToFirst();
    } catch(ReportError &exception){
        //TODO posible should thow exeption
        QMessageBox::critical(0,tr("Error"),exception.what());
        return;
    }
}

void ReportRender::renderPage(PageDesignIntf* patternPage)
{
    m_patternPageItem=patternPage->pageItem();
    m_pageCount=1;
    m_renderCanceled = false;
    BandDesignIntf* reportFooter = m_patternPageItem->bandByType(BandDesignIntf::ReportFooter);
    m_reportFooterHeight = 0;
    if (reportFooter)
        m_reportFooterHeight = reportFooter->height();

    initVariables();
    initGroupFunctions();

    clearPageMap();
    resetPageNumber();
    startNewPage();

    renderBand(m_patternPageItem->bandByType(BandDesignIntf::ReportHeader),StartNewPageAsNeeded);

    BandDesignIntf* lastRenderedBand = 0;
    for (int i=0;i<m_patternPageItem->dataBandCount() && !m_renderCanceled;i++){
        lastRenderedBand = m_patternPageItem->dataBandAt(i);
        initDatasources();
        renderDataBand(lastRenderedBand);
        if (i<m_patternPageItem->dataBandCount()-1) closeFooterGroup(lastRenderedBand);
    }

    if (reportFooter)
        renderBand(reportFooter,StartNewPageAsNeeded);
    if (lastRenderedBand && lastRenderedBand->keepFooterTogether())
        closeFooterGroup(lastRenderedBand);

    savePage();
    if (!m_renderCanceled)
        secondRenderPass();
}

int ReportRender::pageCount()
{
    return m_renderedPages.count();
}

PageItemDesignIntf::Ptr ReportRender::pageAt(int index)
{
    if ((index>m_renderedPages.count()-1)||(index<0)) throw ReportError("page index out of range");
    else return m_renderedPages.at(index);
}

QString ReportRender::renderPageToString(PageDesignIntf *patternPage)
{
    renderPage(patternPage);
    return toString();
}

ReportPages ReportRender::renderPageToPages(PageDesignIntf *patternPage)
{
    renderPage(patternPage);
    return m_renderedPages;
}

void ReportRender::initRenderPage()
{
    if (!m_renderPageItem) {
        m_renderPageItem = new PageItemDesignIntf(m_patternPageItem->pageSize(), m_patternPageItem->pageRect());
        m_renderPageItem->initFromItem(m_patternPageItem);
        m_renderPageItem->setItemMode(PreviewMode);
    }
}

void ReportRender::initVariables()
{
    m_datasources->setReportVariable("#PAGE",1);
    m_datasources->setReportVariable("#PAGE_COUNT",0);
}

void ReportRender::clearPageMap()
{
    m_renderedPages.clear();
}

void ReportRender::extractGroupsFunction(BandDesignIntf *band)
{
    foreach(BaseDesignIntf* item,band->childBaseItems()){
        ContentItemDesignIntf* contentItem = dynamic_cast<ContentItemDesignIntf*>(item);
        if (contentItem&&(contentItem->content().contains(QRegExp("\\$S\\s*\\{.*\\}")))){
            foreach(QString functionName, m_datasources->groupFunctionNames()){
                QRegExp rx(QString(Const::GROUP_FUNCTION_RX).arg(functionName));
                QRegExp rxName(QString(Const::GROUP_FUNCTION_NAME_RX).arg(functionName));
                if (rx.indexIn(contentItem->content())>=0){
                    BandDesignIntf* dataBand = m_patternPageItem->bandByName(rx.cap(Const::DATASOURCE_INDEX));
                    if (dataBand){
                        GroupFunction* gf = datasources()->addGroupFunction(functionName,rx.cap(Const::VALUE_INDEX),band->objectName(),dataBand->objectName());
                        if (gf){
                            connect(dataBand,SIGNAL(bandRendered(BandDesignIntf*)),gf,SLOT(slotBandRendered(BandDesignIntf*)));
                        }
                    } else {
                        GroupFunction* gf = datasources()->addGroupFunction(functionName,rx.cap(Const::VALUE_INDEX),band->objectName(),rx.cap(Const::DATASOURCE_INDEX));
                        gf->setInvalid(tr("Databand \"%1\" not found").arg(rx.cap(Const::DATASOURCE_INDEX)));
                    }
                } else if (rxName.indexIn(contentItem->content())>=0){
                    GroupFunction* gf = datasources()->addGroupFunction(functionName,rxName.cap(1),band->objectName(),"");
                    gf->setInvalid(tr("Wrong using function %1").arg(functionName));
                }
            }
        }
    }
}

void ReportRender::replaceGroupsFunction(BandDesignIntf *band)
{
    foreach(BaseDesignIntf* item,band->childBaseItems()){
        ContentItemDesignIntf* contentItem = dynamic_cast<ContentItemDesignIntf*>(item);
        if (contentItem){
            QString content = contentItem->content();
            foreach(QString functionName, m_datasources->groupFunctionNames()){
                QRegExp rx(QString(Const::GROUP_FUNCTION_RX).arg(functionName));
                if (rx.indexIn(content)>=0){
                    content.replace(rx,QString("%1(%2,%3)").arg(functionName).arg('"'+rx.cap(4)+'"').arg('"'+band->objectName()+'"'));
                    contentItem->setContent(content);
                }
            }
        }
    }
}

void ReportRender::renderBand(BandDesignIntf *patternBand, ReportRender::DataRenderMode mode, bool isLast)
{
    QApplication::processEvents();
    if (patternBand){

        if (mode == ForcedStartPage){
            savePage();
            startNewPage();
        }

        BandDesignIntf* bandClone=renderData(patternBand);
        patternBand->emitBandRendered(bandClone);

        if ( isLast && bandClone->keepFooterTogether() && bandClone->sliceLastRow() ){
            if (m_maxHeightByColumn[m_currentColumn] < (bandClone->height()+m_reportFooterHeight))
                m_maxHeightByColumn[m_currentColumn] -= ((m_maxHeightByColumn[m_currentColumn]-bandClone->height())+(bandClone->height()*calcSlicePercent(bandClone->height())));
         }

        if (!bandClone->isEmpty() || patternBand->printIfEmpty()){
            if (!registerBand(bandClone)){
                if (bandClone->canBeSplitted(m_maxHeightByColumn[m_currentColumn])){
                    bandClone = sliceBand(bandClone,patternBand,isLast);
                } else {
                    qreal percent = (bandClone->height()-m_maxHeightByColumn[m_currentColumn])/(bandClone->height()/100);
                    if (bandClone->maxScalePercent()>=percent){
                        if (percent<bandClone->maxScalePercent()){
                            percent += 2;
                            bandClone->setScale((100-percent)/100);
                            bandClone->setHeight(m_maxHeightByColumn[m_currentColumn]);
                            registerBand(bandClone);
                        }
                    } else {

                        if (mode==StartNewPageAsNeeded){
                            if (bandClone->columnsCount()>1 &&
                                (bandClone->columnsFillDirection()==BandDesignIntf::Vertical ||
                                 bandClone->columnsFillDirection()==BandDesignIntf::VerticalUniform))
                            {
                                startNewColumn();
                            } else {
                                savePage();
                                startNewPage();
                            }
                            if (!registerBand(bandClone)) {
                                bandClone->setHeight(m_maxHeightByColumn[m_currentColumn]);
                                registerBand(bandClone);
                            };
                        } else {
                            bandClone->setHeight(m_maxHeightByColumn[m_currentColumn]);
                            registerBand(bandClone);
                        }
                    }
                }
            }
        } else {delete bandClone;}
        if (patternBand->isFooter())
            datasources()->clearGroupFunctionValues(patternBand->objectName());
    }
}

void ReportRender::renderDataBand(BandDesignIntf *dataBand)
{
    IDataSource* bandDatasource = 0;
    if (dataBand && !dataBand->datasourceName().isEmpty())
       bandDatasource = datasources()->dataSource(dataBand->datasourceName());

    if(bandDatasource && !bandDatasource->eof() && !m_renderCanceled){

        QString varName = QLatin1String("line_")+dataBand->objectName().toLower();
        datasources()->setReportVariable(varName,1);

        renderBand(dataBand->bandHeader());

        if (dataBand->bandHeader() && dataBand->bandHeader()->reprintOnEachPage())
            m_reprintableBands.append(dataBand->bandHeader());

        renderChildHeader(dataBand,PrintNotAlwaysPrintable);
        renderGroupHeader(dataBand, bandDatasource, true);

        bool firstTime = true;

        while(!bandDatasource->eof() && !m_renderCanceled){

            if (!firstTime && dataBand->startNewPage()) {
                savePage();
                startNewPage();
            }

            if (dataBand->tryToKeepTogether()) openDataGroup(dataBand);

            if (dataBand->keepFooterTogether() && !bandDatasource->hasNext())
                openFooterGroup(dataBand);

            datasources()->updateChildrenData(dataBand->datasourceName());
            m_lastDataBand = dataBand;

            renderBand(dataBand,StartNewPageAsNeeded,!bandDatasource->hasNext());
            renderChildBands(dataBand);

            bandDatasource->next();

            datasources()->setReportVariable(varName,datasources()->variable(varName).toInt()+1);
            foreach (BandDesignIntf* band, dataBand->childrenByType(BandDesignIntf::GroupHeader)){
                QString groupLineVar = QLatin1String("line_")+band->objectName().toLower();
                if (datasources()->containsVariable(groupLineVar))
                    datasources()->setReportVariable(groupLineVar,datasources()->variable(groupLineVar).toInt()+1);
            }

            renderGroupHeader(dataBand, bandDatasource, false);
            if (dataBand->tryToKeepTogether()) closeDataGroup(dataBand);
            firstTime = false;
        }

        m_reprintableBands.removeOne(dataBand->bandHeader());
        renderBand(dataBand->bandFooter(),StartNewPageAsNeeded);
        renderGroupFooter(dataBand);
        datasources()->deleteVariable(varName);
    } else if (bandDatasource==0) {
        renderBand(dataBand,StartNewPageAsNeeded);
    }
}

void ReportRender::renderPageHeader(PageItemDesignIntf *patternPage)
{
    BandDesignIntf* band = patternPage->bandByType(BandDesignIntf::PageHeader);
    if (band) renderBand(band);
}

void ReportRender::renderPageFooter(PageItemDesignIntf *patternPage)
{
    BandDesignIntf* band = patternPage->bandByType(BandDesignIntf::PageFooter);
    if (band){
        BandDesignIntf* bandClone = dynamic_cast<BandDesignIntf*>(band->cloneItem(PreviewMode, m_renderPageItem,m_renderPageItem));
        replaceGroupsFunction(bandClone);
        bandClone->updateItemSize(m_datasources);
        bandClone->setItemPos(m_patternPageItem->pageRect().x(),m_patternPageItem->pageRect().bottom()-bandClone->height());
        bandClone->setHeight(m_pageFooterHeight);
        for(int i=0;i<m_maxHeightByColumn.size();++i)
            m_maxHeightByColumn[i]+=m_pageFooterHeight;
        registerBand(bandClone);
        datasources()->clearGroupFunctionValues(band->objectName());
    }
}

void ReportRender::renderPageItems(PageItemDesignIntf* patternPage)
{
    foreach (BaseDesignIntf* item, patternPage->childBaseItems()) {
        ItemDesignIntf* id = dynamic_cast<ItemDesignIntf*>(item);
        if (id&&id->itemLocation()==ItemDesignIntf::Page){
            BaseDesignIntf* cloneItem = item->cloneItem(m_renderPageItem->itemMode(),
                                                        m_renderPageItem,
                                                        m_renderPageItem);
            cloneItem->updateItemSize(m_datasources);
        }
    }
}

qreal ReportRender::calcPageFooterHeight(PageItemDesignIntf *patternPage)
{
    BandDesignIntf* band = patternPage->bandByType(BandDesignIntf::PageFooter);
    if (band){
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
        if (printAlways == (printMode==PrintAlwaysPrintable) )  renderBand(band,StartNewPageAsNeeded);
    }
}

void ReportRender::renderChildFooter(BandDesignIntf *parent, BandPrintMode printMode)
{
    foreach(BandDesignIntf* band,parent->childrenByType(BandDesignIntf::SubDetailFooter)){
        bool printAlways=false;
        if (band->metaObject()->indexOfProperty("printAlways")>0){
            printAlways=band->property("printAlways").toBool();
        }
        if (printAlways == (printMode==PrintAlwaysPrintable)) renderBand(band,StartNewPageAsNeeded);
    }
}

void ReportRender::renderChildBands(BandDesignIntf *parentBand)
{
    foreach(BandDesignIntf* band,parentBand->childrenByType(BandDesignIntf::SubDetailBand)){
        IDataSource* ds = m_datasources->dataSource(band->datasourceName());
        if (ds) ds->first();
        renderChildHeader(band,PrintAlwaysPrintable);
        renderDataBand(band);
        renderChildFooter(band,PrintAlwaysPrintable);
        closeFooterGroup(band);
    }
}

void ReportRender::renderGroupHeader(BandDesignIntf *parentBand, IDataSource* dataSource, bool firstTime)
{
    foreach(BandDesignIntf* band,parentBand->childrenByType(BandDesignIntf::GroupHeader)){
        IGroupBand* gb = dynamic_cast<IGroupBand*>(band);
        if (gb&&gb->isNeedToClose(m_datasources)){
            if (band->childBands().count()>0){
                dataSource->prior();
                foreach (BandDesignIntf* subBand, parentBand->childrenByType(BandDesignIntf::GroupHeader)) {
                    if ( (subBand->bandIndex() > band->bandIndex()) &&
                         (subBand->childBands().count()>0)
                    ){
                        renderBand(subBand->childBands().at(0));
                        closeDataGroup(subBand);
                    }
                }

                renderBand(band->childBands().at(0),StartNewPageAsNeeded);
                dataSource->next();
            }
            closeDataGroup(band);
        }
        if (!gb->isStarted()){
            if (band->reprintOnEachPage())
                m_reprintableBands.append(band);
            gb->startGroup(m_datasources);
            openDataGroup(band);
            if (!firstTime && gb->startNewPage()){
                if (gb->resetPageNumber()) resetPageNumber();
                renderBand(band,ForcedStartPage);
            } else {
                renderBand(band,StartNewPageAsNeeded);
            }
        }
    }
}

void ReportRender::renderGroupFooter(BandDesignIntf *parentBand)
{
    foreach(BandDesignIntf* band,parentBand->childrenByType(BandDesignIntf::GroupHeader)){
        IGroupBand* gb = dynamic_cast<IGroupBand*>(band);
        if (gb->isStarted()){
            if (band->reprintOnEachPage()) m_reprintableBands.removeOne(band);
            if (band->childBands().count()>0){
                renderBand(band->childBands().at(0),StartNewPageAsNeeded);
            }
            closeDataGroup(band);
        }
    }
}

void ReportRender::initGroupFunctions()
{
    m_datasources->clearGroupFunction();
    foreach(BandDesignIntf* band, m_patternPageItem->childBands()){
        if (band->isFooter()) extractGroupsFunction(band);
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
                    m_popupedValues.insert(QString::number((long)dataBand,16)+'|'+gf->data(), gf->values()[gf->values().count()-1]);
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
                    gf->values().push_back(m_popupedValues.value(QString::number((long)dataBand,16)+'|'+gf->data()));
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
        it++;
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
    }
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

bool ReportRender::registerBand(BandDesignIntf *band, bool registerInChildren)
{
    if (band->columnsCount()==1 && m_maxHeightByColumn.size()>1){
        if (band->bandType()!=BandDesignIntf::PageFooter){
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

    if ( (band->columnsCount()>1) && !band->isHeader()){

        if (m_maxHeightByColumn.size()!=band->columnsCount()){
            for(int i=1;i<band->columnsCount();++i){
                m_maxHeightByColumn.append(m_maxHeightByColumn[0]);
                m_currentStartDataPos.append(m_currentStartDataPos[0]);
            }
            m_currentColumn = 0;
        } else {
            if (band->columnsFillDirection()==BandDesignIntf::Horizontal){
                if (m_currentColumn<band->columnsCount()-1)
                    m_currentColumn = m_currentColumn+1;
                else
                    m_currentColumn = 0;
            }
        }

    }

    if (band->height()<=m_maxHeightByColumn[m_currentColumn]){

        if (band->bandType()==BandDesignIntf::PageFooter){
           for (int i=0;i<m_maxHeightByColumn.size();++i)
               m_maxHeightByColumn[i]+=band->height();
        } else {
            m_maxHeightByColumn[m_currentColumn]-=band->height();
        }

        if (band->isHeader() && band->columnsCount()>1){

            qreal bandPos = m_currentStartDataPos[m_currentColumn];
            m_currentStartDataPos[m_currentColumn]+=band->height();
            for (int i=0;i<band->columnsCount();++i){
                if (i!=0) band = dynamic_cast<BandDesignIntf*>(band->cloneItem(PreviewMode));
                band->setPos(m_renderPageItem->pageRect().x()+band->width()*i,bandPos);
                band->setBandIndex(++m_currentIndex);
                band->setColumnIndex(i);
                m_renderPageItem->registerBand(band);
            }

        } else {

            if (band->bandType()!=BandDesignIntf::PageFooter){
                band->setPos(m_renderPageItem->pageRect().x()+band->width()*m_currentColumn,
                             m_currentStartDataPos[m_currentColumn]);
                m_currentStartDataPos[m_currentColumn]+=band->height();
                band->setBandIndex(++m_currentIndex);
                band->setColumnIndex(m_currentColumn);
            }

            if (band->columnsCount()>1){
                m_columnedBandItems.append(band);
            }

            m_renderPageItem->registerBand(band);
        }

        foreach(QList<BandDesignIntf*>* list,m_childBands.values()){
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
            band->setHeight(0);
            band->updateItemSize(m_datasources);
            DataBandDesignIntf* data = dynamic_cast<DataBandDesignIntf*>(band);
            if (isLast && data && data->keepFooterTogether() &&
                band->height()<m_maxHeightByColumn[m_currentColumn] && band->sliceLastRow()
            ){
                if (band->height()>(m_maxHeightByColumn[m_currentColumn]-m_reportFooterHeight)){
                    m_maxHeightByColumn[m_currentColumn] -= ((m_maxHeightByColumn[m_currentColumn]-band->height())+(band->height()*calcSlicePercent(band->height())));
                }
            }
            if (registerBand(band)) break;
        }
    }

    if (band->isEmpty()) {
        delete band;
        band = 0;

    }

    return band;

}

void ReportRender::secondRenderPass()
{
    for(int i=0; i<m_renderedPages.count(); ++i){
        PageItemDesignIntf::Ptr page = m_renderedPages.at(i);
        m_datasources->setReportVariable("#PAGE_COUNT",findLastPageNumber(i));
        foreach(BandDesignIntf* band, page->childBands()){
            band->updateItemSize(m_datasources, SecondPass);
        }
    }
}

BandDesignIntf *ReportRender::saveUppperPartReturnBottom(BandDesignIntf *band, int height, BandDesignIntf* patternBand)
{
    int sliceHeight = height;
    BandDesignIntf* upperBandPart = dynamic_cast<BandDesignIntf*>(band->cloneUpperPart(sliceHeight));
    BandDesignIntf* bottomBandPart = dynamic_cast<BandDesignIntf*>(band->cloneBottomPart(sliceHeight));
    if (!bottomBandPart->isEmpty()){
        //bottomBandPart->updateItemSize(FirstPass,height);
        if (patternBand->keepFooterTogether())
            closeFooterGroup(patternBand);
    }
    if (!upperBandPart->isEmpty()){
        upperBandPart->updateItemSize(m_datasources, FirstPass, height);
        registerBand(upperBandPart);
    } else delete upperBandPart;

    if (band->columnsCount()>1 &&
        (band->columnsFillDirection()==BandDesignIntf::Vertical ||
         band->columnsFillDirection()==BandDesignIntf::VerticalUniform)){
       startNewColumn();
    } else {
        savePage();
        startNewPage();
    }
//    if (!bottomBandPart->isEmpty() && patternBand->keepFooterTogether())
//        openFooterGroup(patternBand);
    delete band;
    return bottomBandPart;
}

BandDesignIntf *ReportRender::renderData(BandDesignIntf *patternBand)
{
    BandDesignIntf* bandClone = dynamic_cast<BandDesignIntf*>(patternBand->cloneItem(PreviewMode));
    if (patternBand->isFooter()){
        replaceGroupsFunction(bandClone);
    }
    bandClone->updateItemSize(m_datasources);
    return bandClone;
}

void ReportRender::startNewColumn(){
    if (m_currentColumn < m_maxHeightByColumn.size()-1){
        m_currentColumn++;
    } else {
        savePage();
        startNewPage();
    }
}

void ReportRender::startNewPage()
{
    m_renderPageItem=0;
    m_currentColumn=0;

    initColumns();
    initRenderPage();

    m_renderPageItem->setObjectName(QLatin1String("ReportPage")+QString::number(m_pageCount));
    m_maxHeightByColumn[m_currentColumn]=m_renderPageItem->pageRect().height();
    m_currentStartDataPos[m_currentColumn]=m_patternPageItem->topMargin()*Const::mmFACTOR;
    m_currentIndex=0;

    renderPageHeader(m_patternPageItem);
    m_pageFooterHeight = calcPageFooterHeight(m_patternPageItem);
    m_maxHeightByColumn[m_currentColumn] -= m_pageFooterHeight;
    m_currentIndex=10;
    m_dataAreaSize = m_maxHeightByColumn[m_currentColumn];
    m_renderedDataBandCount = 0;

    foreach (BandDesignIntf* band, m_reprintableBands) {
        renderBand(band);
    }
    checkLostHeadersOnPrevPage();

    pasteGroups();
    renderPageItems(m_patternPageItem);
}

void ReportRender::resetPageNumber()
{
    PagesRange range;
    if (!m_ranges.isEmpty()){
        m_ranges.last().lastPage = m_pageCount;
        range.firstPage = m_pageCount+1;
    } else {
        range.firstPage = m_pageCount;
    }
    range.lastPage = 0;
    m_ranges.append(range);
}

int ReportRender::findLastPageNumber(int currentPage)
{
    foreach (PagesRange range, m_ranges) {
        if ( range.firstPage<= (currentPage+1) && range.lastPage>= (currentPage+1) )
            return range.lastPage-(range.firstPage-1);
    }
    return 0;
}

void ReportRender::cutGroups()
{
    m_popupedExpression.clear();
    m_popupedValues.clear();
    foreach(BandDesignIntf* groupBand,m_childBands.keys()){
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

void ReportRender::checkLostHeadersOnPrevPage()
{
    if (m_renderedPages.isEmpty()) return;
    PageItemDesignIntf::Ptr page = m_renderedPages.last();
    if (page->bands().isEmpty()) return;

    QMutableListIterator<BandDesignIntf*>it(page->bands());

    it.toBack();
    if (it.hasPrevious())
        if (it.previous()->isFooter())
                it.previous();

    while (it.hasPrevious()){
        if (it.value()->isHeader()){
            if (it.value()->reprintOnEachPage()){
                delete it.value();
            } else { registerBand(it.value());}
            it.remove();
            it.previous();
        } else break;
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

void ReportRender::savePage()
{
    checkFooterGroup(m_lastDataBand);
    cutGroups();
    rearrangeColumnsItems();
    m_columnedBandItems.clear();
    renderPageFooter(m_patternPageItem);

    if (m_ranges.last().lastPage==0 && m_ranges.count()>1) {
        m_datasources->setReportVariable("#PAGE",1);
    } else {
        m_datasources->setReportVariable("#PAGE",m_datasources->variable("#PAGE").toInt()+1);
    }

    m_ranges.last().lastPage = m_pageCount;
    m_pageCount++;

    BandDesignIntf* pageFooter = m_renderPageItem->bandByType(BandDesignIntf::PageFooter);
    if (pageFooter) pageFooter->setBandIndex(++m_currentIndex);
    m_renderedPages.append(PageItemDesignIntf::Ptr(m_renderPageItem));
    emit pageRendered(m_pageCount);
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

}
