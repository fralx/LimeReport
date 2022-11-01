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
#include "lrpageitemdesignintf.h"
#include "lrbanddesignintf.h"
#include "lrpagedesignintf.h"

#include <QGraphicsScene>
#include <QPrinter>
#include <QMenu>
#include <lrpageeditor.h>
namespace LimeReport {

bool bandSortBandLessThenByIndex(const BandDesignIntf *c1, const BandDesignIntf *c2){
    if (c1->bandIndex()!=c2->bandIndex()){
        return c1->bandIndex()<c2->bandIndex();
    } else {
        return c1->bandType()<c2->bandType();
    }
}

PageItemDesignIntf::PageItemDesignIntf(QObject *owner, QGraphicsItem *parent) :
    ItemsContainerDesignInft("PageItem",owner,parent),
    m_topMargin(0), m_bottomMargin(0), m_leftMargin(0), m_rightMargin(0),
    m_pageOrientaion(Portrait), m_pageSize(A4), m_sizeChainging(false),
    m_fullPage(false), m_oldPrintMode(false), m_resetPageNumber(false),
    m_isExtendedInDesignMode(false), m_extendedHeight(1000), m_isTOC(false),
    m_setPageSizeToPrinter(false), m_endlessHeight(false), m_printable(true),
    m_pageFooter(0), m_printBehavior(Split), m_dropPrinterMargins(false),
    m_notPrintIfEmpty(false), m_mixWithPriorPage(false)
{
    setFixedPos(true);
    setPossibleResizeDirectionFlags(Fixed);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    initPageSize(m_pageSize);
}

PageItemDesignIntf::PageItemDesignIntf(const PageSize pageSize, const QRectF &rect, QObject *owner, QGraphicsItem *parent) :
    ItemsContainerDesignInft("PageItem",owner,parent),
    m_topMargin(0), m_bottomMargin(0), m_leftMargin(0), m_rightMargin(0),
    m_pageOrientaion(Portrait), m_pageSize(pageSize), m_sizeChainging(false),
    m_fullPage(false), m_oldPrintMode(false), m_resetPageNumber(false),
    m_isExtendedInDesignMode(false), m_extendedHeight(1000), m_isTOC(false),
    m_setPageSizeToPrinter(false), m_endlessHeight(false), m_printable(true),
    m_pageFooter(0), m_printBehavior(Split), m_dropPrinterMargins(false),
    m_notPrintIfEmpty(false), m_mixWithPriorPage(false)
{
    setFixedPos(true);
    setPossibleResizeDirectionFlags(Fixed);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    initPageSize(rect.size());
}

PageItemDesignIntf::~PageItemDesignIntf()
{
    foreach(BandDesignIntf* band, m_bands)
        band->disconnect(this);
    m_bands.clear();
}

void PageItemDesignIntf::paint(QPainter *ppainter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    if (itemMode() & DesignMode){
        QRectF rect = pageRect();
        if (isExtendedInDesignMode()) rect.adjust(0,0,0,m_extendedHeight);
        ppainter->save();
        ppainter->setOpacity(0.8);
        ppainter->fillRect(boundingRect(), pageBorderColor());
        ppainter->setOpacity(1);
        ppainter->fillRect(rect, Qt::white);
        paintGrid(ppainter, rect);
        ppainter->setPen(gridColor());
        ppainter->drawRect(boundingRect());
        drawShadow(ppainter, boundingRect(), 10);
        ppainter->restore();
    }

    if (itemMode() & PreviewMode) {
        ppainter->save();
        ppainter->fillRect(rect(), Qt::white);
        QPen pen;
        pen.setColor(Qt::gray);
        pen.setWidth(1);
        pen.setStyle(Qt::SolidLine);
        ppainter->setPen(pen);
        QRectF tmpRect = rect();
        tmpRect.adjust(-4,-4,4,4);
        ppainter->drawRect(tmpRect);
        ppainter->restore();
        BaseDesignIntf::paint(ppainter,option,widget);
    }

}

BaseDesignIntf *PageItemDesignIntf::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new PageItemDesignIntf(owner,parent);
}

void PageItemDesignIntf::geometryChangedEvent(QRectF newRect, QRectF)
{
    Q_UNUSED(newRect)
    if (itemMode() == DesignMode || !endlessHeight())
        updateMarginRect();
    PageSize oldSize = m_pageSize;
    if (!m_sizeChainging && !isLoading())
        m_pageSize = Custom;
    notify("pageSize",oldSize,m_pageSize);
}

QColor PageItemDesignIntf::selectionColor() const
{
    return Qt::gray;
}

QColor PageItemDesignIntf::pageBorderColor() const
{
    return QColor(100,150,50);
}

QColor PageItemDesignIntf::gridColor() const
{
    return QColor(170,200,150);
}

QRectF PageItemDesignIntf::boundingRect() const
{
    if (!isExtendedInDesignMode())
        return BaseDesignIntf::boundingRect();
    else {
        QRectF result = BaseDesignIntf::boundingRect();
        return result.adjusted(0,0,0,m_extendedHeight);
    }
}

void PageItemDesignIntf::setItemMode(BaseDesignIntf::ItemMode mode)
{
    ItemsContainerDesignInft::setItemMode(mode);
    relocateBands();
}

void PageItemDesignIntf::clear()
{
    foreach(QGraphicsItem* item, childItems()){
        delete item;
    }
    childItems().clear();
    m_bands.clear();
}

BandDesignIntf *PageItemDesignIntf::bandByType(BandDesignIntf::BandsType bandType) const
{
    QList<BandDesignIntf*>::const_iterator it = childBands().constBegin();
    for(;it!=childBands().constEnd();++it){
        if ( (*it)->bandType()==bandType) return (*it);
    }
    return 0;
}

bool PageItemDesignIntf::isBandExists(BandDesignIntf::BandsType bandType)
{
    foreach(BandDesignIntf* band, childBands()){
        if (band->bandType()==bandType) return true;
    }
    return false;
}

bool PageItemDesignIntf::isBandExists(const QString &bandType)
{
    foreach(BandDesignIntf* band, childBands()){
        if (band->bandTitle()==bandType) return true;
    }
    return false;
}

BandDesignIntf* PageItemDesignIntf::bandByIndex(int index)
{
    foreach(BandDesignIntf* band, childBands()){
        if (band->bandIndex()==index) return band;
    }
    return 0;
}

BandDesignIntf *PageItemDesignIntf::bandByName(QString bandObjectName)
{
    foreach(BandDesignIntf* band, childBands()){
        if (band->objectName().compare(bandObjectName,Qt::CaseInsensitive)==0){
            return band;
        }
    }
    return 0;
}

int PageItemDesignIntf::calcBandIndex(BandDesignIntf::BandsType bandType, BandDesignIntf *parentBand, bool& increaseBandIndex)
{

    increaseBandIndex = false;

    QSet<BandDesignIntf::BandsType> groupFooterIgnoredBands;
    groupFooterIgnoredBands << BandDesignIntf::DataFooter << BandDesignIntf::GroupHeader;

    int bandIndex = -1;
    std::sort(m_bands.begin(), m_bands.end(), bandSortBandLessThenByIndex);
    if (bandType != BandDesignIntf::Data){
        foreach(BandDesignIntf* band,m_bands){
            if ((band->bandType() == BandDesignIntf::GroupHeader) && ( band->bandType() > bandType)) break;
            if ((band->bandType() <= bandType)){
                if (bandIndex <= band->bandIndex()) {
                    bandIndex=band->maxChildIndex(bandType) + 1;
                }
            } else { increaseBandIndex = true; break;}
        }
    } else {
        int maxChildIndex = 0;
        foreach(BandDesignIntf* band, m_bands){
            maxChildIndex = std::max(maxChildIndex, band->maxChildIndex());
        }
        bandIndex = std::max(bandIndex, maxChildIndex + 1);
    }

    if (bandIndex == -1) {
        bandIndex = static_cast<int>(bandType);
        increaseBandIndex = true;
    }

    if (parentBand) {

        BandDesignIntf* dataBand = parentBand;
        while (dataBand->parentBand() && dataBand->bandType()==BandDesignIntf::GroupHeader){
            dataBand = dataBand->parentBand();
        }

        switch (bandType) {
        case BandDesignIntf::SubDetailBand:
            bandIndex = parentBand->maxChildIndex(bandType) + 1;
            increaseBandIndex = true;
            break;
        case BandDesignIntf::SubDetailHeader:
            bandIndex = parentBand->bandIndex();
            increaseBandIndex = true;
            break;
        case BandDesignIntf::SubDetailFooter:
            bandIndex = parentBand->maxChildIndex() + 1;
            increaseBandIndex = true;
            break;
        case BandDesignIntf::GroupHeader:
            if (parentBand->bandType()==BandDesignIntf::GroupHeader)
                bandIndex = parentBand->bandIndex() + 1;
            else
                bandIndex = parentBand->minChildIndex(BandDesignIntf::GroupHeader);
            increaseBandIndex = true;
            break;
        case BandDesignIntf::GroupFooter:
            bandIndex = std::max(parentBand->maxChildIndex(),
                                 dataBand->maxChildIndex(groupFooterIgnoredBands))  + 1;
            increaseBandIndex = true;
            break;
        case BandDesignIntf::DataHeader:
            bandIndex = parentBand->minChildIndex(BandDesignIntf::DataHeader);
            increaseBandIndex = true;
            break;
        case BandDesignIntf::DataFooter:
            bandIndex = parentBand->maxChildIndex() + 1;
            increaseBandIndex = true;
            break;
        default :
            ;
        }

    }
    return bandIndex;
}

void PageItemDesignIntf::increaseBandIndex(int startIndex)
{
    if (bandByIndex(startIndex)){
    foreach(BandDesignIntf* band, m_bands){
            if (band->bandIndex() >= startIndex){
                band->setBandIndex(band->bandIndex() + 1);
            }
        }
    }
}

bool PageItemDesignIntf::isBandRegistred(BandDesignIntf *band)
{
    foreach(BandDesignIntf* cBand,childBands()){
        if (band==cBand) return true;
    }
    return false;
}

void PageItemDesignIntf::registerBand(BandDesignIntf *band)
{
    if (!isBandRegistred(band)){
        if (band->bandIndex() > childBands().count() - 1)
            m_bands.append(band);
        else
            m_bands.insert(band->bandIndex(), band);
        band->setParent(this);
        band->setParentItem(this);
        band->setWidth(pageRect().width() / band->columnsCount());
        connect(band, SIGNAL(destroyed(QObject*)),this,SLOT(bandDeleted(QObject*)));
        connect(band, SIGNAL(posChanged(QObject*, QPointF, QPointF)),
                this, SLOT(bandPositionChanged(QObject*, QPointF, QPointF)));
        connect(band, SIGNAL(geometryChanged(QObject*, QRectF, QRectF)),
                this, SLOT(bandGeometryChanged(QObject*, QRectF, QRectF)));

    }
}

void PageItemDesignIntf::initColumnsPos(QVector<qreal> &posByColumns, qreal pos, int columnCount){
    posByColumns.clear();
    for(int i=0;i<columnCount;++i){
        posByColumns.append(pos);
    }
}

bool PageItemDesignIntf::notPrintIfEmpty() const
{
    return m_notPrintIfEmpty;
}

void PageItemDesignIntf::setNotPrintIfEmpty(bool notPrintIfEmpty)
{
    m_notPrintIfEmpty = notPrintIfEmpty;
}

bool PageItemDesignIntf::dropPrinterMargins() const
{
    return m_dropPrinterMargins;
}

void PageItemDesignIntf::setDropPrinterMargins(bool dropPrinterMargins)
{
    m_dropPrinterMargins = dropPrinterMargins;
}

bool PageItemDesignIntf::isEmpty() const
{
    return childBaseItems().isEmpty();
}

void PageItemDesignIntf::setPrintBehavior(const PrintBehavior &printBehavior)
{
    m_printBehavior = printBehavior;
}

PageItemDesignIntf::PrintBehavior PageItemDesignIntf::printBehavior() const
{
    return m_printBehavior;
}

QString PageItemDesignIntf::printerName() const
{
    return m_printerName;
}

void PageItemDesignIntf::setPrinterName(const QString& printerName)
{
    m_printerName = printerName;
}

bool PageItemDesignIntf::isPrintable() const
{
    return m_printable;
}

void PageItemDesignIntf::setPrintable(bool printable)
{
    m_printable = printable;
}

bool PageItemDesignIntf::endlessHeight() const
{
    return m_endlessHeight;
}

void PageItemDesignIntf::setEndlessHeight(bool endlessPage)
{
    m_endlessHeight = endlessPage;
}

bool PageItemDesignIntf::getSetPageSizeToPrinter() const
{
    return m_setPageSizeToPrinter;
}

void PageItemDesignIntf::setSetPageSizeToPrinter(bool setPageSizeToPrinter)
{
    if (m_setPageSizeToPrinter != setPageSizeToPrinter){
        m_setPageSizeToPrinter = setPageSizeToPrinter;
        notify("setPageSizeToPrinter", !setPageSizeToPrinter, setPageSizeToPrinter);
    }
}

bool PageItemDesignIntf::isTOC() const
{
    return m_isTOC;
}

void PageItemDesignIntf::setIsTOC(bool isTOC)
{
    if (m_isTOC != isTOC){
        m_isTOC = isTOC;
        notify("pageIsTOC", !isTOC, isTOC);
    }
}

int PageItemDesignIntf::extendedHeight() const
{
    return m_extendedHeight;
}

void PageItemDesignIntf::setExtendedHeight(int extendedHeight)
{
    m_extendedHeight = extendedHeight;
    PageDesignIntf* page = dynamic_cast<PageDesignIntf*>(scene());
    if (page) page->updatePageRect();
    update();
}

bool PageItemDesignIntf::isExtendedInDesignMode() const
{
    return m_isExtendedInDesignMode;
}

void PageItemDesignIntf::setExtendedInDesignMode(bool pageIsExtended)
{
    m_isExtendedInDesignMode = pageIsExtended;
    PageDesignIntf* page = dynamic_cast<PageDesignIntf*>(scene());
    if (page) page->updatePageRect();
    update();
}

BandDesignIntf *PageItemDesignIntf::pageFooter() const
{
    return m_pageFooter;
}

void PageItemDesignIntf::setPageFooter(BandDesignIntf *pageFooter)
{
    m_pageFooter = pageFooter;
}

void PageItemDesignIntf::placeTearOffBand()
{
    BandDesignIntf* tearOffBand = bandByType(BandDesignIntf::TearOffBand);
    if (tearOffBand){
        BandDesignIntf* pf = pageFooter();
        if (pf){
            qreal bottomSpace = pageRect().bottom() - (tearOffBand->height() + pf->height() + bottomMargin());
            tearOffBand->setItemPos(pageRect().x(),
                                    bottomSpace);
        } else {
            qreal bottomSpace = pageRect().bottom() - (tearOffBand->height() + bottomMargin());
            tearOffBand->setItemPos(pageRect().x(), bottomSpace);
        }
    }
}

bool PageItemDesignIntf::resetPageNumber() const
{
    return m_resetPageNumber;
}

void PageItemDesignIntf::setResetPageNumber(bool resetPageNumber)
{
    if (m_resetPageNumber!=resetPageNumber){
        m_resetPageNumber = resetPageNumber;
        notify("resetPageNumber",!m_resetPageNumber,m_resetPageNumber);
    }
}

void PageItemDesignIntf::updateSubItemsSize(RenderPass pass, DataSourceManager *dataManager)
{
    if (!endlessHeight()){
        snapshotItemsLayout(IgnoreBands);
        arrangeSubItems(pass, dataManager);
    }
}

bool PageItemDesignIntf::oldPrintMode() const
{
    return m_oldPrintMode;
}

void PageItemDesignIntf::setOldPrintMode(bool oldPrintMode)
{
    m_oldPrintMode = oldPrintMode;
}

bool PageItemDesignIntf::fullPage() const
{
    return m_fullPage;
}

void PageItemDesignIntf::setFullPage(bool fullPage)
{
    if (m_fullPage != fullPage){
        m_fullPage = fullPage;
        if (!isLoading()){
            updateMarginRect();
            notify("fullPage", !m_fullPage, m_fullPage);
        }
    }
}

void PageItemDesignIntf::relocateBands()
{
    if (isLoading()) return;

    int bandSpace = 0;

    QVector<qreal> posByColumn;

    std::sort(m_bands.begin(), m_bands.end(), bandSortBandLessThenByIndex);

    int bandIndex = 0;
    if (!(itemMode() & DesignMode)){
        while ( (bandIndex < m_bands.count()) &&
                ((m_bands[bandIndex]->bandType() == BandDesignIntf::TearOffBand) ||
                (m_bands[bandIndex]->bandType() == BandDesignIntf::PageFooter) ||
                 m_bands[bandIndex]->bandType() == BandDesignIntf::ReportFooter )
        ){
            bandIndex++;
        }
    }

    if ( (m_bands.count()>0) && (bandIndex<m_bands.count()) ) {
        initColumnsPos(posByColumn,pageRect().y(),m_bands[bandIndex]->columnsCount());
        m_bands[bandIndex]->setPos(pageRect().x(),pageRect().y());
        posByColumn[0]+=m_bands[bandIndex]->height()+bandSpace;
    }

    if(m_bands.count()>1){
        for(int i=0;i<(m_bands.count()-1);i++){
            if (((m_bands[i+1]->bandType()!=BandDesignIntf::PageFooter) &&
                (m_bands[i+1]->bandType()!=BandDesignIntf::TearOffBand)) || (itemMode() & DesignMode)){
                if (m_bands[i+1]->columnsCount()>1 &&
                    m_bands[i]->columnsCount() != m_bands[i+1]->columnsCount())
                {
                    qreal curPos = posByColumn[0];
                    initColumnsPos(posByColumn,
                                   curPos,
                                   m_bands[i+1]->columnsCount());
                }
                if (m_bands[i+1]->columnIndex()==0){
                    if ( ((m_bands[i]->borderLines() & BandDesignIntf::BottomLine) &&
                         (m_bands[i+1]->borderLines() & BandDesignIntf::TopLine)) ||
                         (!(m_bands[i]->borderLines() & BandDesignIntf::BottomLine) &&
                          !(m_bands[i+1]->borderLines() & BandDesignIntf::TopLine))  ){
                            m_bands[i+1]->setPos(pageRect().x(),posByColumn[0]);
                            posByColumn[0] += m_bands[i+1]->height()+bandSpace;
                    } else {
                        m_bands[i+1]->setPos(pageRect().x(),posByColumn[0]);
                        posByColumn[0] += m_bands[i+1]->height()+bandSpace;
                    }
                } else {
                    m_bands[i+1]->setPos(m_bands[i+1]->pos().x(),posByColumn[m_bands[i+1]->columnIndex()]);
                    posByColumn[m_bands[i+1]->columnIndex()] += m_bands[i+1]->height()+bandSpace;
                }
            }
        }
        foreach(BandDesignIntf* band, m_bands){
            if (band->isSelected()) band->updateBandNameLabel();
        }
    }

    if (!(itemMode() & DesignMode))
        placeTearOffBand();
}

void PageItemDesignIntf::removeBand(BandDesignIntf *band)
{
    if (!m_bands.isEmpty()){
        m_bands.removeOne(band);
    }
}

int PageItemDesignIntf::dataBandCount()
{
    int count=0;
    foreach(BandDesignIntf* band,m_bands){
        if (band->bandType()==BandDesignIntf::Data) count++;
    }
    return count;
}

BandDesignIntf *PageItemDesignIntf::dataBandAt(int index)
{
    int count=0;
    std::sort(m_bands.begin(),m_bands.end(),bandSortBandLessThenByIndex);
    foreach(BandDesignIntf* band,m_bands){
        if (band->bandType()==BandDesignIntf::Data){
            if(count==index) return band;
            count++;
        }
    }
    return 0;
}

void PageItemDesignIntf::setTopMargin(int value)
{
    if (m_topMargin!=value){
        m_sizeChainging = true;
        int oldValue = m_topMargin;
        m_topMargin=value;
        if (!isLoading()){
            updateMarginRect();
            notify("topMargin",oldValue,value);;
        }
        m_sizeChainging =  false;
    }
}

void PageItemDesignIntf::setBottomMargin(int value)
{
    if (m_bottomMargin!=value){
        m_sizeChainging = true;
        int oldValue = m_bottomMargin;
        m_bottomMargin=value;
        if (!isLoading()){
            updateMarginRect();
            notify("bottomMargin",oldValue,value);
        }
        m_sizeChainging = false;
    }
}

void PageItemDesignIntf::setLeftMargin(int value)
{
    if (m_leftMargin!=value){
        m_sizeChainging = true;
        int oldValue = m_leftMargin;
        m_leftMargin=value;
        if (!isLoading()){
            updateMarginRect();
            notify("leftMargin",oldValue,value);
        }
        m_sizeChainging = false;
    }
}

void PageItemDesignIntf::setRightMargin(int value)
{
    if (m_rightMargin!=value){
        m_sizeChainging = true;
        int oldValue = m_rightMargin;
        m_rightMargin=value;
        if (!isLoading()){
            updateMarginRect();
            notify("rightMargin",oldValue,value);
        }
        m_sizeChainging = false;
    }
}

void PageItemDesignIntf::setPageOrientation(PageItemDesignIntf::Orientation value)
{
    if (!m_sizeChainging && m_pageOrientaion!=value){
        m_sizeChainging = true;
        PageItemDesignIntf::Orientation oldValue = m_pageOrientaion;
        m_pageOrientaion = value;
        if (!isLoading()){
            qreal tmpWidth = width();
            setWidth(height());
            setHeight(tmpWidth);
            updateMarginRect();
            notify("pageOrientation",oldValue,value);
        }
        m_sizeChainging =  false;
    }
}

QSizeF PageItemDesignIntf::getRectByPageSize(const PageSize& size)
{
    if (size != Custom) {
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
        printer.setOrientation((QPrinter::Orientation)pageOrientation());
        printer.setPaperSize((QPrinter::PageSize)size);
        return QSizeF(printer.paperSize(QPrinter::Millimeter).width() * 10,
                      printer.paperSize(QPrinter::Millimeter).height() * 10);

#else
        QPageSize pageSize = QPageSize((QPageSize::PageSizeId)size);
        qreal width = pageSize.size(QPageSize::Millimeter).width() * 10;
        qreal height = pageSize.size(QPageSize::Millimeter).height() * 10;
        return QSizeF(pageOrientation() == Portrait ? width : height,
                      pageOrientation() == Portrait ? height : width);

//        printer.setPageOrientation((QPageLayout::Orientation)pageOrientation());
//        printer.setPageSize(QPageSize((QPageSize::PageSizeId)size));
//        return QSizeF(printer.pageLayout().pageSize().size(QPageSize::Millimeter).width() * 10,
//                      printer.pageLayout().pageSize().size(QPageSize::Millimeter).height() * 10);
#endif
    }

    else {
        return QSizeF(width(),height());
    }
}

void PageItemDesignIntf::initPageSize(const QSizeF& size)
{
    m_sizeChainging = true;
    setWidth(size.width());
    setHeight(size.height());
    m_sizeChainging=false;
}

void PageItemDesignIntf::preparePopUpMenu(QMenu &menu)
{


    foreach (QAction* action, menu.actions()) {
        if (action->text().compare(tr("Paste")) != 0)
            action->setVisible(false);
    }
    menu.addSeparator();
    menu.addAction(tr("Edit"));


    menu.addSeparator();

    QAction* action = menu.addAction(tr("Page is TOC"));
    action->setCheckable(true);
    action->setChecked(isTOC());

    action = menu.addAction(tr("Reset page number"));
    action->setCheckable(true);
    action->setChecked(resetPageNumber());

    action = menu.addAction(tr("Full page"));
    action->setCheckable(true);
    action->setChecked(fullPage());

    action = menu.addAction(tr("Set page size to printer"));
    action->setCheckable(true);
    action->setChecked(getSetPageSizeToPrinter());

    action = menu.addAction(tr("Mix with prior page"));
    action->setCheckable(true);
    action->setChecked(mixWithPriorPage());

//    action = menu.addAction(tr("Transparent"));
//    action->setCheckable(true);
//    action->setChecked(backgroundMode() == TransparentMode);

}

void PageItemDesignIntf::processPopUpAction(QAction *action)
{
    if (action->text().compare(tr("Page is TOC")) == 0){
        page()->setPropertyToSelectedItems("pageIsTOC",action->isChecked());
    }
    if (action->text().compare(tr("Reset page number")) == 0){
        page()->setPropertyToSelectedItems("resetPageNumber",action->isChecked());
    }
    if (action->text().compare(tr("Full page")) == 0){
        page()->setPropertyToSelectedItems("fullPage",action->isChecked());
    }
    if (action->text().compare(tr("Set page size to printer")) == 0){
        page()->setPropertyToSelectedItems("setPageSizeToPrinter",action->isChecked());
    }

    if (action->text().compare(tr("Mix with prior page")) == 0){
        page()->setPropertyToSelectedItems("mixWithPriorPage",action->isChecked());
    }
    if(action->text() == tr("Edit"))
    {
        PageEditor pageEdit(NULL,this);
        pageEdit.exec();
    }

}
void PageItemDesignIntf::initPageSize(const PageItemDesignIntf::PageSize &size)
{
    m_sizeChainging = true;
    if (m_pageSize != Custom){
        QSizeF pageSize = getRectByPageSize(size);
        setWidth(pageSize.width());
        setHeight(pageSize.height());
    }
    m_sizeChainging=false;
}

void PageItemDesignIntf::setPageSize(const PageSize &size)
{
    if(m_pageSize!=size){
        m_pageSize=size;
        initPageSize(m_pageSize);
    }
}

void PageItemDesignIntf::bandDeleted(QObject *band)
{
    removeBand(reinterpret_cast<BandDesignIntf*>(band));
    relocateBands();
}

void PageItemDesignIntf::swapBands(BandDesignIntf* band, BandDesignIntf* bandToSwap){

    int firstIndex = std::min(band->minChildIndex(), bandToSwap->minChildIndex());
    int secondIndex = std::max(band->minChildIndex(), bandToSwap->minChildIndex());
    int moveIndex = std::min(band->maxChildIndex(), bandToSwap->maxChildIndex());

    QList<BandDesignIntf*> bandToMove;
    foreach(BandDesignIntf* curBand, m_bands){
        if ( curBand->bandIndex() > moveIndex && curBand->bandIndex() < secondIndex &&
            curBand->bandType() == band->bandType() &&
            curBand != band && curBand != bandToSwap
        )
            bandToMove.append(curBand);
    }

    BandDesignIntf* firstMoveBand = (bandToSwap->bandIndex() > band->bandIndex()) ? bandToSwap: band;

    firstMoveBand->changeBandIndex(firstIndex, true);
    moveIndex = firstIndex;
    std::sort(bandToMove.begin(), bandToMove.end(), bandIndexLessThen);

    foreach(BandDesignIntf* curBand, bandToMove){
       curBand->changeBandIndex(moveIndex,true);
       moveIndex = curBand->maxChildIndex() + 1;
    }

    if (firstMoveBand == band){
        bandToSwap->changeBandIndex(moveIndex,true);
    } else {
        band->changeBandIndex(moveIndex, true);
    }
    relocateBands();

}

QList<BandDesignIntf*> PageItemDesignIntf::createBandGroup(int beginIndex, int endIndex)
{
    QList<BandDesignIntf*> result;
    foreach(BandDesignIntf* curBand, m_bands){
        if ( curBand->bandIndex() >= beginIndex && curBand->bandIndex() <= endIndex)
            result.append(curBand);
    }
    std::sort(result.begin(), result.end(), bandIndexLessThen);
    return result;
}

void PageItemDesignIntf::moveBandFromTo(int from, int to)
{

    BandDesignIntf* fromBand = 0;
    BandDesignIntf* toBand = 0;

    foreach(BandDesignIntf* band, bands()){
        if (band->bandIndex() == from){
            fromBand = band->rootBand(band->parentBand());
        }
        if (band->bandIndex() == to){
            toBand = band->rootBand(band->parentBand());
        }
        if (fromBand && toBand) break;
    }

    if (!fromBand || !toBand) return;

    int beginIndex = 0;
    int endIndex = 0;
    if (from < to){
        beginIndex = fromBand->maxChildIndex()+1;
        endIndex = toBand->maxChildIndex();
    } else {
        beginIndex = toBand->minChildIndex();
        endIndex = fromBand->minChildIndex()-1;
    }

    QList<BandDesignIntf*> firstGroup = createBandGroup(fromBand->minChildIndex(), fromBand->maxChildIndex());
    QList<BandDesignIntf*> secondGroup = createBandGroup(beginIndex, endIndex);

    if (from < to){
        int currentIndex = fromBand->minChildIndex();
        foreach(BandDesignIntf* band, secondGroup){
            band->setBandIndex(currentIndex);
            currentIndex++;
        }
        foreach(BandDesignIntf* band, firstGroup){
            band->setBandIndex(currentIndex);
            currentIndex++;
        }
    } else {
        int currentIndex = toBand->minChildIndex();
        foreach(BandDesignIntf* band, firstGroup){
            band->setBandIndex(currentIndex);
            currentIndex++;
        }
        foreach(BandDesignIntf* band, secondGroup){
            band->setBandIndex(currentIndex);
            currentIndex++;
        }
    }

    relocateBands();

}

void PageItemDesignIntf::bandPositionChanged(QObject* object, QPointF newPos, QPointF oldPos)
{
    BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(object);
    if (band && !band->isChangingPos() && (itemMode() == DesignMode)){
        int curIndex = band->bandIndex();
        BandDesignIntf* bandToSwap = 0;
        foreach(BandDesignIntf* curBand, bands()){
            if (newPos.y() > oldPos.y()) {
                if (curBand->bandType() == band->bandType()
                        && curIndex < curBand->bandIndex()
                        && (curBand->pos().y() + (curBand->height()/2)) < newPos.y()
                        && curBand->parentBand() == band->parentBand())
                {
                    curIndex = curBand->bandIndex();
                    bandToSwap =  curBand;
                }
            } else {
                if (curBand->bandType() == band->bandType()
                        && curIndex>curBand->bandIndex()
                        && (curBand->pos().y() + (curBand->height()/2)) > newPos.y()
                        && curBand->parentBand() == band->parentBand())
                {
                    curIndex = curBand->bandIndex();
                    bandToSwap =  curBand;
                }
            }
        }
        if (curIndex != band->bandIndex() && itemMode() == DesignMode){
            if (page())
                page()->saveCommand(BandMoveFromToCommand::create(page(), band->bandIndex(), bandToSwap->bandIndex()), true);
        }
    }
    if (band && !band->isChangingPos())
        relocateBands();
}

void PageItemDesignIntf::bandGeometryChanged(QObject* object, QRectF newGeometry, QRectF oldGeometry)
{
    bandPositionChanged(object, newGeometry.topLeft(), oldGeometry.topLeft());
}

void PageItemDesignIntf::setUnitTypeProperty(BaseDesignIntf::UnitType value)
{
    if (unitType() != value){
        UnitType oldValue = unitType();
        setUnitType(value);
        if (!isLoading()){
            update();
            notify("units", oldValue, value);
        }
    }
}

void PageItemDesignIntf::collectionLoadFinished(const QString &collectionName)
{
    if (collectionName.compare("children",Qt::CaseInsensitive)==0){
        m_bands.clear();
#ifdef HAVE_QT5
        foreach(QObject* obj,children()){
#else
        foreach(QObject* obj,QObject::children()){
#endif
            BandDesignIntf* item = dynamic_cast<BandDesignIntf*>(obj);
            if (item) {
                registerBand(item);
            }
        }
    }
    BaseDesignIntf::collectionLoadFinished(collectionName);
}

void PageItemDesignIntf::updateMarginRect()
{
    m_pageRect = rect();
    m_pageRect.adjust( leftMargin() * Const::mmFACTOR,
                       topMargin() * Const::mmFACTOR,
                      -rightMargin() * Const::mmFACTOR,
                       -bottomMargin() * Const::mmFACTOR
    );

    foreach(BandDesignIntf* band,m_bands){
        band->setWidth(pageRect().width()/band->columnsCount());
        relocateBands();
    }
    foreach (BaseDesignIntf* item, childBaseItems()) {
        if (item->itemAlign()!=DesignedItemAlign){
            item->updateItemAlign();
        }
    }
    update();
}

void PageItemDesignIntf::paintGrid(QPainter *ppainter, QRectF rect)
{
    ppainter->save();
    ppainter->setPen(QPen(gridColor()));
    ppainter->setOpacity(0.5);
    for (int i = 0; i <= (rect.height() - 5 * unitFactor()) / (10 * unitFactor()); ++i){
        if (i * 10 * unitFactor() + 5 * unitFactor() >= topMargin() * Const::mmFACTOR)
            ppainter->drawLine(rect.x(), (i * 10 * unitFactor()) + ( (rect.y() + 5 * unitFactor()) - (topMargin() * Const::mmFACTOR)),
                           rect.right(), i * 10 * unitFactor() +( (rect.y() + 5 * unitFactor()) - (topMargin() * Const::mmFACTOR)));
    };
    for (int i=0; i<=((rect.width() - 5 * unitFactor()) / (10 * unitFactor())); ++i){
        if (i * 10 * unitFactor() + 5 * unitFactor() >= leftMargin() * Const::mmFACTOR)
            ppainter->drawLine(i * 10 * unitFactor() + ((rect.x() + 5 * unitFactor()) - (leftMargin() * Const::mmFACTOR)), rect.y(),
                           i * 10 * unitFactor() + ((rect.x() + 5 * unitFactor()) - (leftMargin() * Const::mmFACTOR)), rect.bottom());
    };
    ppainter->setPen(QPen(gridColor()));
    ppainter->setOpacity(1);
    for (int i = 0; i <= (rect.width() / (10 * unitFactor())); ++i){
        if (i * 10 * unitFactor() >= leftMargin() * Const::mmFACTOR)
            ppainter->drawLine(i * 10 * unitFactor() + (rect.x() - (leftMargin() * Const::mmFACTOR)), rect.y(),
                           i * 10 * unitFactor() + (rect.x() - (leftMargin() * Const::mmFACTOR)), rect.bottom());
    };
    for (int i = 0; i <= rect.height() / (10 * unitFactor()); ++i){
        if (i * 10 * unitFactor() >= topMargin() * Const::mmFACTOR)
            ppainter->drawLine(rect.x(), i * 10 * unitFactor() + (rect.y() - (topMargin() * Const::mmFACTOR)),
                           rect.right(), i * 10 * unitFactor() + (rect.y() - (topMargin() * Const::mmFACTOR)));
    };
    ppainter->drawRect(rect);
    ppainter->restore();
}

QList<BandDesignIntf *>& PageItemDesignIntf::bands()
{
    return m_bands;
}

void PageItemDesignIntf::setGridStep(int value)
{
    if (page()) {
        page()->setHorizontalGridStep(value);
        page()->setVerticalGridStep(value);
    }
}

int PageItemDesignIntf::gridStep()
{
    if (page()) return page()->horizontalGridStep();
    else return Const::DEFAULT_GRID_STEP;
}

void PageItemDesignIntf::objectLoadFinished()
{
    BaseDesignIntf::objectLoadFinished();
    updateMarginRect();
}

PageItemDesignIntf::Ptr PageItemDesignIntf::create(QObject *owner)
{
    return PageItemDesignIntf::Ptr(new PageItemDesignIntf(owner));
}

bool PageItemDesignIntf::mixWithPriorPage() const
{
    return m_mixWithPriorPage;
}

void PageItemDesignIntf::setMixWithPriorPage(bool value)
{
    if (m_mixWithPriorPage != value){
        m_mixWithPriorPage = value;
        if (!isLoading()){
            update();
            notify("mixWithPriorPage", !value, value);
        }
    }

}

}
