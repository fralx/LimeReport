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
#include "lrpageitemdesignintf.h"
#include "lrbanddesignintf.h"
#include <QGraphicsScene>
#include <QPrinter>

namespace LimeReport {

bool bandSortBandLessThenByIndex(const BandDesignIntf *c1, const BandDesignIntf *c2){
    if (c1->bandIndex()!=c2->bandIndex()){
        return c1->bandIndex()<c2->bandIndex();
    } else {
        return c1->bandType()<c2->bandType();
    }
}

PageItemDesignIntf::PageItemDesignIntf(QObject *owner, QGraphicsItem *parent) :
    BaseDesignIntf("PageItem",owner,parent),
    m_topMargin(0), m_bottomMargin(0), m_leftMargin(0), m_rightMargin(0),
    m_pageOrientaion(Portrait), m_pageSize(A4), m_sizeChainging(false)
{
    setFixedPos(true);
    setPosibleResizeDirectionFlags(Fixed);
    initPageSize(m_pageSize);
}

PageItemDesignIntf::PageItemDesignIntf(const PageSize pageSize, const QRectF &rect, QObject *owner, QGraphicsItem *parent) :
    BaseDesignIntf("PageItem",owner,parent),
    m_topMargin(0), m_bottomMargin(0), m_leftMargin(0), m_rightMargin(0),
    m_pageOrientaion(Portrait), m_pageSize(pageSize), m_sizeChainging(false)
{
    setFixedPos(true);
    setPosibleResizeDirectionFlags(Fixed);
    initPageSize(rect.size());
}

PageItemDesignIntf::~PageItemDesignIntf()
{
    foreach(BandDesignIntf* band,m_bands) band->disconnect(this);
    m_bands.clear();
}

void PageItemDesignIntf::paint(QPainter *ppainter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    if (itemMode() & DesignMode){
        ppainter->save();
        ppainter->setOpacity(0.8);
        ppainter->fillRect(boundingRect(),pageBorderColor());
        ppainter->setOpacity(1);
        ppainter->fillRect(pageRect(),Qt::white);
        paintGrid(ppainter);
        ppainter->setPen(gridColor());
        ppainter->drawRect(boundingRect());
        ppainter->restore();
    }

    if (itemMode() & PreviewMode) {
        ppainter->save();
        ppainter->fillRect(rect(),Qt::white);
        QPen pen;
        pen.setColor(Qt::gray);
        pen.setWidth(2);
        pen.setStyle(Qt::DotLine);
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
//    if (scene())
//      scene()->setSceneRect(newRect);
    Q_UNUSED(newRect)
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
    //return QColor(180,220,150);
    return QColor(100,150,50);
}

QColor PageItemDesignIntf::gridColor() const
{
    //return QColor(240,240,240);
    return QColor(170,200,150);
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
    }    return 0;
}

bool PageItemDesignIntf::isBandExists(BandDesignIntf::BandsType bandType)
{
    foreach(BandDesignIntf* band,childBands()){
        if (band->bandType()==bandType) return true;
    }
    return false;
}

bool PageItemDesignIntf::isBandExists(const QString &bandType)
{
    foreach(BandDesignIntf* band, m_bands){
        if (band->bandTitle()==bandType) return true;
    }
    return false;
}

BandDesignIntf* PageItemDesignIntf::bandByIndex(int index)
{
    foreach(BandDesignIntf* band,m_bands){
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
    QSet<BandDesignIntf::BandsType> subDetailsBands;
    subDetailsBands << BandDesignIntf::SubDetailBand << BandDesignIntf::SubDetailHeader << BandDesignIntf::SubDetailFooter;

    int bandIndex=-1;
    qSort(m_bands.begin(),m_bands.end(),bandSortBandLessThenByIndex);
    foreach(BandDesignIntf* band,m_bands){
        if ((band->bandType()==BandDesignIntf::GroupHeader)&&(band->bandType()>bandType)) break;
        if ((band->bandType()>bandType)&&(band->parentBand()==0)) break;
        if (bandIndex<band->bandIndex()) bandIndex=band->bandIndex()+1;
    }

    if (bandIndex==-1) {
        bandIndex = (int)(bandType);
        increaseBandIndex = true;
    }

    if (parentBand) {
        switch (bandType) {
        case BandDesignIntf::SubDetailBand:
            bandIndex = parentBand->bandIndex() + 1;
            increaseBandIndex = true;
            break;
        case BandDesignIntf::SubDetailHeader:
            bandIndex = parentBand->bandIndex();
            increaseBandIndex = true;
            break;
        case BandDesignIntf::SubDetailFooter:
            bandIndex = parentBand->maxChildIndex(subDetailsBands) + 1;
            increaseBandIndex = true;
            break;
        case BandDesignIntf::GroupHeader:
            bandIndex = parentBand->groupHeaderIndex();
            increaseBandIndex = true;
            break;
        case BandDesignIntf::GroupFooter:
            bandIndex = parentBand->parentBand()->groupFooterIndex(parentBand->bandIndex()) + 1;
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
    foreach(BandDesignIntf* band,m_bands){
            if (band->bandIndex()>=startIndex){
                band->setBandIndex(band->bandIndex()+1);
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
        if (band->bandIndex()>childBands().count()-1)
            m_bands.append(band);
        else
            m_bands.insert(band->bandIndex(),band);

        band->setParent(this);
        band->setParentItem(this);
        band->setWidth(pageRect().width());
        connect(band, SIGNAL(destroyed(QObject*)),this,SLOT(bandDeleted(QObject*)));
        connect(band, SIGNAL(geometryChanged(QObject*,QRectF,QRectF)),this,SLOT(bandGeometryChanged(QObject*,QRectF,QRectF)));
    }
}

void PageItemDesignIntf::relocateBands()
{
    if (isLoading()) return;

    int bandSpace = (itemMode() & DesignMode)?4:0;

    qSort(m_bands.begin(),m_bands.end(),bandSortBandLessThenByIndex);

    if (m_bands.count()>0) m_bands[0]->setPos(pageRect().x(),pageRect().y());
    if(m_bands.count()>1){
        m_bands[0]->setBandIndex(0);
        for(int i=0;i<(m_bands.count()-1);i++){
            if ((m_bands[i+1]->bandType()!=BandDesignIntf::PageFooter) || (itemMode() & DesignMode))
                m_bands[i+1]->setPos(pageRect().x(),m_bands[i]->pos().y()+m_bands[i]->height()+bandSpace);
            m_bands[i+1]->setBandIndex(i+1);
        }
    }
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
    qSort(m_bands.begin(),m_bands.end(),bandSortBandLessThenByIndex);
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
        int oldValue = m_topMargin;
        m_topMargin=value;
        updateMarginRect();
        notify("topMargin",oldValue,value);
    }
}

void PageItemDesignIntf::setBottomMargin(int value)
{
    if (m_bottomMargin!=value){
        int oldValue = m_bottomMargin;
        m_bottomMargin=value;
        updateMarginRect();
        notify("bottomMargin",oldValue,value);
    }
}

void PageItemDesignIntf::setLeftMargin(int value)
{
    if (m_leftMargin!=value){
        int oldValue = m_leftMargin;
        m_leftMargin=value;
        updateMarginRect();
        notify("leftMargin",oldValue,value);
    }
}

void PageItemDesignIntf::setRightMargin(int value)
{
    if (m_rightMargin!=value){
        int oldValue = m_rightMargin;
        m_rightMargin=value;
        updateMarginRect();
        notify("rightMargin",oldValue,value);
    }
}

void PageItemDesignIntf::setPageOrientation(PageItemDesignIntf::Orientation value)
{
    if (!m_sizeChainging && m_pageOrientaion!=value){
        m_sizeChainging = true;
        m_pageOrientaion = value;
        if (!isLoading()){
            qreal tmpWidth = width();
            setWidth(height());
            setHeight(tmpWidth);
        }
        m_sizeChainging =  false;
    }
}

QSizeF PageItemDesignIntf::getRectByPageSize(const PageSize& size)
{
    if (size != Custom) {
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOrientation((QPrinter::Orientation)pageOrientation());
        printer.setPageSize((QPrinter::PageSize)size);
        return QSizeF(printer.paperRect(QPrinter::Millimeter).width() * 10,
                      printer.paperSize(QPrinter::Millimeter).height() * 10);
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

void PageItemDesignIntf::bandGeometryChanged(QObject *, QRectF /*newGeometry*/, QRectF /*oldGeometry*/)
{
    relocateBands();
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
    m_pageRect.adjust(m_leftMargin*mmFactor(),m_topMargin*mmFactor(),
                         -m_rightMargin*mmFactor(),-m_bottomMargin*mmFactor());
    foreach(BandDesignIntf* band,m_bands){
        band->setWidth(pageRect().width());
        relocateBands();
    }
    update();
}

void PageItemDesignIntf::paintGrid(QPainter *ppainter)
{
    ppainter->save();
    ppainter->setPen(QPen(gridColor()));
    ppainter->setOpacity(0.5);
    for (int i=0;i<=(pageRect().height()-50)/100;i++){
        ppainter->drawLine(pageRect().x(),(i*100)+pageRect().y()+50,pageRect().right(),i*100+pageRect().y()+50);
    };
    for (int i=0;i<=((pageRect().width()-50)/100);i++){
        ppainter->drawLine(i*100+pageRect().x()+50,pageRect().y(),i*100+pageRect().x()+50,pageRect().bottom());
    };

    ppainter->setPen(QPen(gridColor()));
    ppainter->setOpacity(1);
    for (int i=0;i<=(pageRect().width()/100);i++){
        ppainter->drawLine(i*100+pageRect().x(),pageRect().y(),i*100+pageRect().x(),pageRect().bottom());
    };
    for (int i=0;i<=pageRect().height()/100;i++){
        ppainter->drawLine(pageRect().x(),i*100+pageRect().y(),pageRect().right(),i*100+pageRect().y());
    };
    ppainter->drawRect(pageRect());
    ppainter->restore();
}

PageItemDesignIntf::Ptr PageItemDesignIntf::create(QObject *owner)
{
    return PageItemDesignIntf::Ptr(new PageItemDesignIntf(owner));
}

}
