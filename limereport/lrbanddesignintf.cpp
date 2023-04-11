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
#include "lrbanddesignintf.h"
#include "lritemdesignintf.h"
#include "lrglobal.h"
#include <algorithm>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>

namespace LimeReport {

BandMarker::BandMarker(BandDesignIntf *band, QGraphicsItem* parent)
    :QGraphicsItem(parent),m_rect(0,0,30,30),m_band(band)
{
    setAcceptHoverEvents(true);
}

QRectF BandMarker::boundingRect() const
{
    return m_rect;
}

void BandMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem* /**option*/, QWidget* /*widget*/)
{
    painter->save();
    painter->setOpacity(Const::BAND_MARKER_OPACITY);
    painter->fillRect(boundingRect(),m_color);
    painter->setOpacity(1);
    painter->setPen(QPen(QBrush(Qt::lightGray),2));
    painter->fillRect(QRectF(
                boundingRect().bottomLeft().x(),
                boundingRect().bottomLeft().y()-4,
                boundingRect().width(),4), Qt::lightGray
    );
    painter->setRenderHint(QPainter::Antialiasing);
    qreal size = (boundingRect().width()<boundingRect().height()) ? boundingRect().width() : boundingRect().height();
    QRectF r = QRectF(0,0,size,size);
    painter->setBrush(Qt::white);
    painter->setPen(Qt::white);
    painter->drawEllipse(r.adjusted(5,5,-5,-5));
    if (m_band->isSelected()){
        painter->setBrush(LimeReport::Const::SELECTION_COLOR);
        painter->drawEllipse(r.adjusted(7,7,-7,-7));
    }

    painter->restore();
}

void BandMarker::setHeight(qreal height)
{
    if (m_rect.height()!=height){
        prepareGeometryChange();
        m_rect.setHeight(height);
    }
}

void BandMarker::setWidth(qreal width)
{
    if (m_rect.width()!=width){
        prepareGeometryChange();
        m_rect.setWidth(width);
    }
}

void BandMarker::setColor(QColor color)
{
    if (m_color!=color){
        m_color = color;
        update(boundingRect());
    }
}

void BandMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button()==Qt::LeftButton) {
        if (!(event->modifiers() & Qt::ControlModifier))
            m_band->scene()->clearSelection();
        m_band->setSelected(true);
        m_oldBandPos = m_band->pos();
        update(0,0,boundingRect().width(),boundingRect().width());
    }
}

void BandMarker::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    m_band->contextMenuEvent(event);
}

void BandMarker::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    if (QRectF(0, height()-10, width(), 10).contains(event->pos())){
       setCursor(Qt::SizeVerCursor);
    } else {
        unsetCursor();
    }
}

void BandMarker::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    qreal delta = event->pos().y() - event->lastPos().y();
    if (hasCursor()){
        m_band->setHeight(m_band->height() + delta);
    } else {
        if (!m_band->isFixedPos())
            m_band->setItemPos(QPointF(m_band->pos().x(),m_band->pos().y()+delta));
    }
}

void BandMarker::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event)
    m_band->posChanged(m_band, m_band->pos(), m_oldBandPos);
}

BandDesignIntf::BandDesignIntf(BandsType bandType, const QString &xmlTypeName, QObject* owner, QGraphicsItem *parent) :
    ItemsContainerDesignInft(xmlTypeName, owner,parent),
    m_bandType(bandType),
    m_bandIndex(static_cast<int>(bandType)),
    m_dataSourceName(""),
    m_autoHeight(true),
    m_keepBottomSpace(false),
    m_keepTopSpace(true),
    m_parentBand(0),
    m_parentBandName(""),
    m_bandMarker(0),
    m_tryToKeepTogether(false),
    m_splitable(false),
    m_keepFooterTogether(false),
    m_maxScalePercent(0),
    m_sliceLastRow(false),
    m_printIfEmpty(false),
    m_columnsCount(1),
    m_columnIndex(0),
    m_columnsFillDirection(Horizontal),
    m_reprintOnEachPage(false),
    m_startNewPage(false),
    m_startFromNewPage(false),
    m_printAlways(false),
    m_repeatOnEachRow(false),
    m_useAlternateBackgroundColor(false),
    m_bottomSpace(0),
    m_shiftItems(0)
{
    setPossibleResizeDirectionFlags(ResizeBottom);
    setPossibleMoveFlags(TopBotom);

    if (parent) {
        BaseDesignIntf* parentItem = dynamic_cast<BaseDesignIntf*>(parent);
        if (parentItem) setWidth(parentItem->width());
    }

    setBackgroundMode(BaseDesignIntf::TransparentMode);
    setFillTransparentInDesignMode(false);
    setHeight(100);
    setFixedPos(true);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    m_bandMarker = new BandMarker(this);
    m_bandMarker->setColor(Qt::magenta);
    m_bandMarker->setHeight(height());
    m_bandMarker->setPos(pos().x()-m_bandMarker->width() - (itemMode() == ItemModes::PrintMode?boundingRect().width() : 0),pos().y());
    if (scene()) scene()->addItem(m_bandMarker);

    m_bandNameLabel = new BandNameLabel(this);
    m_bandNameLabel->setVisible(false);
    if (scene()) scene()->addItem(m_bandNameLabel);
    m_alternateBackgroundColor = backgroundColor();
    connect(this, SIGNAL(propertyObjectNameChanged(QString, QString)),
            this, SLOT(slotPropertyObjectNameChanged(const QString&,const QString&)));
}

BandDesignIntf::~BandDesignIntf()
{
    delete m_bandMarker;
    delete m_bandNameLabel;
}

int extractItemIndex(const BaseDesignIntf* item){
    QString objectName = extractClassName(item->metaObject()->className());
    QString value = item->objectName().right(item->objectName().size() - objectName.size());
    return value.toInt();
}

QString BandDesignIntf::translateBandName(const BaseDesignIntf* item) const{
    QString defaultBandName = extractClassName(item->metaObject()->className()).toLatin1()+QString::number(extractItemIndex(item));
    if (item->objectName().compare(defaultBandName) == 0){
        return tr(extractClassName(item->metaObject()->className()).toLatin1())+QString::number(extractItemIndex(item));
    } else {
        return item->objectName();
    }
}

void BandDesignIntf::setBackgroundModeProperty(BaseDesignIntf::BGMode value)
{
    if (value!=backgroundMode()){
        BaseDesignIntf::BGMode oldValue = backgroundMode();
        setBackgroundMode(value);
        notify("backgroundMode",oldValue,value);
    }
}

void BandDesignIntf::setBackgroundOpacity(int value)
{
    if (opacity()!=value){
        int oldValue = opacity();
        setOpacity(value);
        notify("backgroundOpacity",oldValue,value);
    }
}

bool BandDesignIntf::isNeedUpdateSize(RenderPass pass) const{
    foreach(BaseDesignIntf* item, childBaseItems()){
        if (item->isNeedUpdateSize(pass)) return true;
    }
    return false;
}

void BandDesignIntf::copyBandAttributes(BandDesignIntf *source)
{
    this->copyBookmarks(source);
    this->setBackgroundColor(source->backgroundColor());
}

void BandDesignIntf::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    prepareRect(painter, option, widget);

    if (itemMode() & DesignMode){
        painter->save();
        QString bandText = bandTitle();
        QFont font("Arial", 7 * Const::fontFACTOR, -1, true);
        QFontMetrics fontMetrics(font);

        QVector<QRectF> bandNameRects;
        bandNameRects.push_back(QRectF(8,8,fontMetrics.boundingRect(" "+bandText+" ").width(),fontMetrics.height()));
        //bandNameRects.push_back(QRectF(width()-fontMetrics.width(" "+bandText+" "),2,fontMetrics.width(" "+bandText+" "),fontMetrics.height()));
        //bandNameRects.push_back(QRectF(2,height()-fontMetrics.height(),fontMetrics.width(" "+bandText+" "),fontMetrics.height()));
        //bandNameRects.push_back(QRectF(width()-fontMetrics.width(" "+bandText+" "),height()-fontMetrics.height(),fontMetrics.width(" "+bandText+" "),fontMetrics.height()));
        //if (bandNameRects[0].intersects(bandNameRects[2])) bandNameRects.remove(2,2);
        //if (isSelected()) ppainter->setPen(QColor(167,244,167));
       // else ppainter->setPen(QColor(220,220,220));

        painter->setFont(font);
        for (int i=0;i<bandNameRects.count();i++){
            QRectF labelRect = bandNameRects[i].adjusted(-2,-2,2,2);
            if ((labelRect.height())<height() && (childBaseItems().isEmpty()) && !isSelected()){
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setBrush(bandColor());
                painter->setOpacity(Const::BAND_NAME_AREA_OPACITY);
                painter->drawRoundedRect(labelRect,8,8);
                painter->setOpacity(Const::BAND_NAME_TEXT_OPACITY);
                painter->setPen(Qt::black);
                painter->drawText(bandNameRects[i],Qt::AlignHCenter,bandText);
            }
        }
        painter->restore();
    }
    BaseDesignIntf::paint(painter,option,widget);
}

QRectF BandDesignIntf::boundingRect() const
{
    return ItemsContainerDesignInft::boundingRect().adjusted(0,-4,0,4);
}

void BandDesignIntf::translateBandsName()
{
    tr("DataBand");
    tr("DataHeaderBand");
    tr("DataFooterBand");
    tr("ReportHeader");
    tr("ReportFooter");
    tr("PageHeader");
    tr("PageFooter");
    tr("SubDetailBand");
    tr("SubDetailHeaderBand");
    tr("SubDetailFooterBand");
    tr("GroupBandHeader");
    tr("GroupBandFooter");
    tr("TearOffBand");
}

BandDesignIntf::BandsType  BandDesignIntf::bandType() const
{
    return m_bandType;
}

QString  BandDesignIntf::bandTitle() const
{
    QString result = translateBandName(this);
    if (parentBand()) result +=tr(" connected to ") + translateBandName(parentBand());
    return result;
}

QIcon  BandDesignIntf::bandIcon() const
{
    return QIcon();
}

int BandDesignIntf::bandIndex() const
{
    return m_bandIndex;
}

void BandDesignIntf::setBandIndex(int value)
{
    m_bandIndex=value;
}

void BandDesignIntf::changeBandIndex(int value, bool firstTime)
{
    int indexOffset;

    if (firstTime && bandHeader())
        value += 1;

    indexOffset = value - m_bandIndex;

    foreach(BandDesignIntf* band, childBands()){
        int newIndex = band->bandIndex()+indexOffset;
        band->changeBandIndex(newIndex);
    }
    setBandIndex(value);
}

bool BandDesignIntf::isUnique() const
{
    return true;
}

void BandDesignIntf::setItemMode(BaseDesignIntf::ItemMode mode)
{
    ItemsContainerDesignInft::setItemMode(mode);
    updateBandMarkerGeometry();
}

QString BandDesignIntf::datasourceName(){
    return m_dataSourceName;
}

void BandDesignIntf::setDataSourceName(const QString &datasource){
    m_dataSourceName=datasource;
}

void BandDesignIntf::setKeepBottomSpace(bool value){
    if (m_keepBottomSpace!=value){
        m_keepBottomSpace=value;
        if (!isLoading())
            notify("keepBottomSpace",!value,value);
    }
}

void BandDesignIntf::addChildBand(BandDesignIntf *band)
{
    m_childBands.append(band);
    connect(band,SIGNAL(destroyed(QObject*)),this,SLOT(childBandDeleted(QObject*)));
}

void BandDesignIntf::removeChildBand(BandDesignIntf *band)
{
    m_childBands.removeAt(m_childBands.indexOf(band));
}

void BandDesignIntf::setParentBand(BandDesignIntf *band)
{
    m_parentBand=band;
    if (band){
        if (parentBandName().compare(band->objectName(),Qt::CaseInsensitive)!=0)
          setParentBandName(band->objectName());
        band->addChildBand(this);
    }
}

void BandDesignIntf::setParentBandName(const QString &parentBandName)
{
    m_parentBandName=parentBandName;
    if (itemMode()&DesignMode && !m_parentBandName.isEmpty()){
        if ((parentBand() == 0 )||(parentBand()->objectName()!= parentBandName))
            setParentBand(findParentBand());
    }
}

QString BandDesignIntf::parentBandName(){
    if (!m_parentBand) return m_parentBandName;
    else return m_parentBand->objectName();
}

bool BandDesignIntf::isConnectedToBand(BandDesignIntf::BandsType bandType) const
{
    foreach(BandDesignIntf* band,childBands()) if (band->bandType()==bandType) return true;
    return false;
}

int BandDesignIntf::maxChildIndex(BandDesignIntf::BandsType bandType) const{
    int curIndex = bandIndex();
    foreach(BandDesignIntf* childBand, childBands()){
        if ( (childBand->bandIndex() > bandIndex()) && (childBand->bandType() < bandType) ){
            curIndex = std::max(curIndex,childBand->maxChildIndex());
        }
    }
    return curIndex;
}

int BandDesignIntf::maxChildIndex(QSet<BandDesignIntf::BandsType> ignoredBands) const{
    int curIndex = bandIndex();
    foreach(BandDesignIntf* childBand, childBands()){
        if (!ignoredBands.contains(childBand->bandType())){
            curIndex = std::max(curIndex,childBand->maxChildIndex(ignoredBands));
        }
    }
    return curIndex;
}

int BandDesignIntf::rootIndex(BandDesignIntf* parentBand)
{
    return rootBand(parentBand)->bandIndex();
}

BandDesignIntf *BandDesignIntf::rootBand(BandDesignIntf* parentBand)
{
    BandDesignIntf* currentBand = this;
    while (currentBand->parentBand() && currentBand->parentBand() != parentBand)
        currentBand = currentBand->parentBand();
    return currentBand;
}

int BandDesignIntf::minChildIndex(BandDesignIntf::BandsType bandType){
    int curIndex = bandIndex();
    foreach(BandDesignIntf* childBand, childBands()){
        if (curIndex > childBand->bandIndex() && (childBand->bandType() > bandType)){
            curIndex = childBand->bandIndex();
        }
    }
    return curIndex;
}

int BandDesignIntf::minChildIndex(QSet<BandDesignIntf::BandsType> ignoredBands)
{
    int curIndex = bandIndex();
    foreach(BandDesignIntf* childBand, childBands()){
        if (!ignoredBands.contains(childBand->bandType()) && childBand->bandIndex() < bandIndex()){
            curIndex = std::min(curIndex, childBand->minChildIndex(ignoredBands));
        }
    }
    return curIndex;
}



QList<BandDesignIntf *> BandDesignIntf::childrenByType(BandDesignIntf::BandsType type)
{
    QList<BandDesignIntf*> resList;
    foreach(BandDesignIntf* item,childBands()){
        if (item->bandType()==type) resList<<item;
    }
    std::sort(resList.begin(),resList.end(),bandIndexLessThen);
    return resList;
}

bool BandDesignIntf::canBeSplitted(int height) const
{
    if (isSplittable()){
        foreach(QGraphicsItem* qgItem,childItems()){
            BaseDesignIntf* item=dynamic_cast<BaseDesignIntf*>(qgItem);
            if (item)
                if ((item->minHeight()>height) && (item->minHeight()>(this->height()-height))) return false;
        }
    }
    return isSplittable();
}

bool BandDesignIntf::isEmpty() const
{
    if (!isVisible()) return true;
    foreach(QGraphicsItem* qgItem,childItems()){
        BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(qgItem);
        if ((item)&&(!item->isEmpty())) return false;
    }
    return true;
}

bool BandDesignIntf::isNeedRender() const
{
    return true;
}

void BandDesignIntf::setTryToKeepTogether(bool value)
{
    if (m_tryToKeepTogether != value){
        m_tryToKeepTogether = value;
        if (!isLoading())
            notify("keepSubdetailTogether", !value, value);
    }
}

bool BandDesignIntf::tryToKeepTogether()
{
    return m_tryToKeepTogether;
}

void BandDesignIntf::checkEmptyTable(){
    bool isEmpty = true;
    bool allItemsIsText = true;
    foreach (QGraphicsItem* qgItem, childItems()) {
        BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(qgItem);
        if (item && !item->isEmpty()) isEmpty = false;
        if (!item) allItemsIsText = false;
    }
    if (isEmpty && allItemsIsText){
        foreach (QGraphicsItem* qgItem, childItems()) {
            ContentItemDesignIntf* item = dynamic_cast<ContentItemDesignIntf*>(qgItem);
            item->setHeight(0);
        }
    }
}

void BandDesignIntf::setColumnsCount(int value)
{
    if (m_columnsCount!=value && value>0){
        qreal oldValue = m_columnsCount;
        qreal fullWidth = m_columnsCount * width();
        m_columnsCount = value;
        if (!isLoading()){
            setWidth(fullWidth/m_columnsCount);
            notify("columnsCount",oldValue,value);
        }
    }
}

void BandDesignIntf::setColumnsFillDirection(BandDesignIntf::BandColumnsLayoutType value)
{
    if (m_columnsFillDirection!=value){
        qreal oldValue = m_columnsFillDirection;
        m_columnsFillDirection = value;
        if (!isLoading())
            notify("columnsFillDirection",oldValue,value);
    }

}

void BandDesignIntf::moveItemsDown(qreal startPos, qreal offset){
   foreach (QGraphicsItem* item, childItems()){
       if (item->pos().y()>=startPos)
           item->setPos(item->x(),item->y()+offset);
   }
}

void BandDesignIntf::preparePopUpMenu(QMenu &menu)
{

    QList<QString> disabledActions;
    disabledActions << tr("Bring to top") <<
                       tr("Send to back") <<
                       tr("Cut") <<
                       tr("Copy");

    foreach (QAction* action, menu.actions()) {
          action->setEnabled(!disabledActions.contains(action->text()));
    }

    menu.addSeparator();
    QAction* currAction = menu.addAction(tr("Auto height"));
    currAction->setCheckable(true);
    currAction->setChecked(autoHeight());

    currAction = menu.addAction(tr("Splittable"));
    currAction->setEnabled(metaObject()->indexOfProperty("splittable") != -1);
    currAction->setCheckable(true);
    currAction->setChecked(isSplittable());

    currAction = menu.addAction(tr("Keep top space"));
    currAction->setCheckable(true);
    currAction->setChecked(keepTopSpace());

    currAction = menu.addAction(tr("Keep bottom space"));
    currAction->setCheckable(true);
    currAction->setChecked(keepBottomSpace());

    currAction = menu.addAction(tr("Print if empty"));
    currAction->setCheckable(true);
    currAction->setChecked(printIfEmpty());

}

void BandDesignIntf::processPopUpAction(QAction *action)
{
    if (action->text().compare(tr("Auto height")) == 0){
        setProperty("autoHeight", action->isChecked());
    }

    if (action->text().compare(tr("Splittable")) == 0){
        setProperty("splittable", action->isChecked());
    }

    if (action->text().compare(tr("Keep top space")) == 0){
        setProperty("keepTopSpace", action->isChecked());
    }

    if (action->text().compare(tr("Keep bottom space")) == 0){
        setProperty("keepBottomSpace", action->isChecked());
    }
    if (action->text().compare(tr("Keep top space")) == 0){
        setProperty("keepTopSpace",action->isChecked());
    }

    if (action->text().compare(tr("Print if empty")) == 0){
        setProperty("printIfEmpty", action->isChecked());
    }
    ItemsContainerDesignInft::processPopUpAction(action);
}

BaseDesignIntf* BandDesignIntf::cloneUpperPart(int height, QObject *owner, QGraphicsItem *parent)
{
    int maxBottom = 0;
    BandDesignIntf* upperPart = dynamic_cast<BandDesignIntf*>(createSameTypeItem(owner,parent));
    BaseDesignIntf* upperItem = 0;

    upperPart->initFromItem(this);

    foreach(QGraphicsItem* qgItem,childItems()){
        BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(qgItem);
        if (item){
            if (item->geometry().bottom()<=height){
               upperItem = item->cloneItem(item->itemMode(),upperPart,upperPart);
               if (maxBottom<upperItem->geometry().bottom()) maxBottom = upperItem->geometry().bottom();
            }
            else if ((item->geometry().top()<height) && (item->geometry().bottom()>height)){
                int sliceHeight = height-item->geometry().top();
                if (!item->isSplittable()){
                    if (sliceHeight>(this->height()-sliceHeight)){
                        upperItem = item->cloneItem(item->itemMode(),upperPart,upperPart);
                        upperItem->setHeight(height);
                    } else {
                        item->cloneEmpty(sliceHeight,upperPart,upperPart); //for table
                        //qgItem->setPos(item->pos().x(),item->pos().y()+((height+1)-item->geometry().top()));
                        moveItemsDown(item->pos().y(),(height+1)-item->geometry().top());
                    }
                } else if (item->canBeSplitted(sliceHeight)){
                    upperItem = item->cloneUpperPart(sliceHeight,upperPart,upperPart);
                    if (maxBottom<upperItem->geometry().bottom()) maxBottom = upperItem->geometry().bottom();
                    m_slicedItems.insert(upperItem->objectName(),upperItem);
                } else {
                    item->cloneEmpty(sliceHeight,upperPart,upperPart); //for table
                    moveItemsDown(item->pos().y(),(height+1)-item->geometry().top());
                    //qgItem->setPos(item->pos().x(),item->pos().y()+((height+1)-item->geometry().top()));
                }
            }
        }
    }
    upperPart->setHeight(height);
    return upperPart;
}

bool itemLessThen(QGraphicsItem* i1, QGraphicsItem* i2){
    return i1->pos().y()<i2->pos().y();
}

BaseDesignIntf *BandDesignIntf::cloneBottomPart(int height, QObject *owner, QGraphicsItem *parent)
{
    BandDesignIntf* bottomPart = dynamic_cast<BandDesignIntf*>(createSameTypeItem(owner,parent));
    bottomPart->initFromItem(this);

    QList<QGraphicsItem*> bandItems;
    bandItems = childItems();
    std::sort(bandItems.begin(),bandItems.end(), itemLessThen);

    foreach(QGraphicsItem* qgItem, bandItems){
        BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(qgItem);

        if (item){
            if (item->geometry().top()>height){
                BaseDesignIntf* tmpItem = item->cloneItem(item->itemMode(),bottomPart,bottomPart);
                tmpItem->setPos(tmpItem->pos().x(), (tmpItem->pos().y()-height)+borderLineSize());
            }
            else if ((item->geometry().top()<=height) && (item->geometry().bottom()>height)){
                int sliceHeight = height-item->geometry().top();
                if (item->isSplittable() && item->canBeSplitted(sliceHeight)) {
                    BaseDesignIntf* tmpItem=item->cloneBottomPart(sliceHeight,bottomPart,bottomPart);
                    tmpItem->setPos(tmpItem->pos().x(),borderLineSize());
                    BaseDesignIntf* slicedItem = m_slicedItems.value(tmpItem->objectName());
                    if (slicedItem){
                        qreal sizeOffset = (slicedItem->height()+tmpItem->height()) - item->height();
                        qreal bottomOffset = (height - slicedItem->pos().y())-m_slicedItems.value(tmpItem->objectName())->height();
                        moveItemsDown(item->pos().y()+item->height(), sizeOffset + bottomOffset);
                    }
                } else {
                    BaseDesignIntf* tmpItem = item->cloneItem(item->itemMode(),bottomPart,bottomPart);
                    tmpItem->setPos(tmpItem->pos().x(),borderLineSize());
                }
            }
        }
    }

    return bottomPart;
}

void BandDesignIntf::parentObjectLoadFinished()
{
    if (!parentBandName().isEmpty())
        setParentBand(findParentBand());
}

void BandDesignIntf::objectLoadFinished()
{
    m_bandMarker->setHeight(height());
    BaseDesignIntf::objectLoadFinished();
}

void BandDesignIntf::emitBandRendered(BandDesignIntf* band)
{
    emit bandRendered(band);
}

void BandDesignIntf::emitBandReRendered(BandDesignIntf *oldBand, BandDesignIntf *newBand)
{
    emit bandReRendered(oldBand, newBand);
}

void BandDesignIntf::setSplittable(bool value){
    if (m_splitable!=value){
        bool oldValue = m_splitable;
        m_splitable = value;
        if (!isLoading())
            notify("splittable",oldValue,value);
    }
}

bool bandIndexLessThen(const BandDesignIntf* b1, const BandDesignIntf* b2)
{
    return b1->bandIndex()<b2->bandIndex();
}

void BandDesignIntf::trimToMaxHeight(int maxHeight)
{
    foreach(BaseDesignIntf* item,childBaseItems()){
        if (item->height()>maxHeight) item->setHeight(maxHeight);
    }
}

void BandDesignIntf::setBandTypeText(const QString &value){
    m_bandTypeText=value;
    m_bandNameLabel->updateLabel(bandTitle());
}

QSet<BandDesignIntf::BandsType> BandDesignIntf::groupBands()
{
    QSet<BandDesignIntf::BandsType> result;
    result<<GroupHeader<<GroupFooter;
    return result;
}

QSet<BandDesignIntf::BandsType> BandDesignIntf::subdetailBands()
{
    QSet<BandDesignIntf::BandsType> result;
    result<<SubDetailBand<<SubDetailHeader<<SubDetailFooter;
    return result;
}

BandDesignIntf* BandDesignIntf::findParentBand()
{
    if (parent()&&(!dynamic_cast<BaseDesignIntf*>(parent())->isLoading())){
        foreach(QObject* item, parent()->children()){
            BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(item);
            if(band&&(band->objectName().compare(parentBandName(),Qt::CaseInsensitive)==0))
                return band;
        }
    }
    return 0;
}

void BandDesignIntf::updateBandMarkerGeometry()
{
    if (parentItem() && m_bandMarker){
        m_bandMarker->setPos(pos().x()-m_bandMarker->width() - (itemMode() == ItemModes::PrintMode?boundingRect().width() : 0),pos().y());
        m_bandMarker->setHeight(rect().height());
    }
}

void BandDesignIntf::geometryChangedEvent(QRectF, QRectF )
{
    if (((itemMode()&DesignMode) || (itemMode()&EditMode))&&parentItem()){
        updateBandMarkerGeometry();
    }
    foreach (BaseDesignIntf* item, childBaseItems()) {
        if (item->itemAlign()!=DesignedItemAlign){
            item->updateItemAlign();
        }
    }
}

QVariant BandDesignIntf::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if ((change==ItemPositionChange)&&((itemMode()&DesignMode)||(itemMode()&EditMode))){
        if (m_bandMarker){
            m_bandMarker->setPos((value.toPointF().x()-m_bandMarker->boundingRect().width() - (itemMode() == ItemModes::PrintMode?boundingRect().width() : 0)),
                                 value.toPointF().y());
        }
    }
    if (change==ItemSelectedChange){
        if (m_bandMarker){
            m_bandMarker->update(0,0,
                                 m_bandMarker->boundingRect().width(),
                                 m_bandMarker->boundingRect().width());
            m_bandNameLabel->updateLabel(bandTitle());
            m_bandNameLabel->setVisible(value.toBool());

        }
    }
    if (change==ItemChildAddedChange || change==ItemChildRemovedChange){
        update(rect());
    }
    return BaseDesignIntf::itemChange(change,value);
}

void BandDesignIntf::initMode(ItemMode mode)
{
    BaseDesignIntf::initMode(mode);
    if ((mode==PreviewMode)||(mode==PrintMode)){
        m_bandMarker->setVisible(false);
    } else {
        if (!m_bandMarker->scene() && this->scene()){
            this->scene()->addItem(m_bandMarker);
            m_bandMarker->setParentItem(this->parentItem());
            m_bandMarker->setHeight(this->height());
        }
        m_bandMarker->setVisible(true);
    }
}

QColor BandDesignIntf::bandColor() const
{
    return Qt::darkBlue;
}

void BandDesignIntf::setMarkerColor(QColor color)
{
    if (m_bandMarker) m_bandMarker->setColor(color);
}

void BandDesignIntf::childBandDeleted(QObject *band)
{
    m_childBands.removeAt(m_childBands.indexOf(reinterpret_cast<BandDesignIntf*>(band)));
}

bool BandDesignIntf::useAlternateBackgroundColor() const
{
    return m_useAlternateBackgroundColor;
}

void BandDesignIntf::setUseAlternateBackgroundColor(bool useAlternateBackgroundColor)
{
    if (m_useAlternateBackgroundColor != useAlternateBackgroundColor){
        QColor oldValue = m_useAlternateBackgroundColor;
        m_useAlternateBackgroundColor=useAlternateBackgroundColor;
        if (!isLoading())
            notify("useAlternateBackgroundColor",oldValue,useAlternateBackgroundColor);
    }
}

QColor BandDesignIntf::alternateBackgroundColor() const
{
    if (metaObject()->indexOfProperty("alternateBackgroundColor")!=-1)
        return m_alternateBackgroundColor;
    else
        return backgroundColor();
}

void BandDesignIntf::setAlternateBackgroundColor(const QColor &alternateBackgroundColor)
{
    if (m_alternateBackgroundColor != alternateBackgroundColor){
        QColor oldValue = m_alternateBackgroundColor;
        m_alternateBackgroundColor=alternateBackgroundColor;
        if (!isLoading())
            notify("alternateBackgroundColor",oldValue,alternateBackgroundColor);
    }
}

qreal BandDesignIntf::bottomSpace()
{
    return m_bottomSpace;
}

void BandDesignIntf::slotPropertyObjectNameChanged(const QString &, const QString& newName)
{
    update();
    if (m_bandNameLabel)
        m_bandNameLabel->updateLabel(newName);
}

int BandDesignIntf::shiftItems() const
{
    return m_shiftItems;
}

void BandDesignIntf::setShiftItems(int shiftItems)
{
    m_shiftItems = shiftItems;
}

bool BandDesignIntf::keepTopSpace() const
{
    return m_keepTopSpace;
}

void BandDesignIntf::setKeepTopSpace(bool value)
{
    if (m_keepTopSpace != value){
        m_keepTopSpace = value;
        if (!isLoading())
            notify("keepTopSpace",!value,value);
    }
}

void BandDesignIntf::setBottomSpace(qreal bottomSpace)
{
    m_bottomSpace = bottomSpace;
}

bool BandDesignIntf::repeatOnEachRow() const
{
    return m_repeatOnEachRow;
}

void BandDesignIntf::setRepeatOnEachRow(bool repeatOnEachRow)
{
    if (m_repeatOnEachRow != repeatOnEachRow){
        m_repeatOnEachRow = repeatOnEachRow;
        if (!isLoading())
            notify("repeatOnEachRow", !m_repeatOnEachRow, m_repeatOnEachRow);
    }
}

bool BandDesignIntf::printAlways() const
{
    return m_printAlways;
}

void BandDesignIntf::setPrintAlways(bool printAlways)
{
    if (m_printAlways != printAlways){
        m_printAlways = printAlways;
        if (!isLoading())
            notify("printAlways", !m_printAlways, m_printAlways);
    }
}

bool BandDesignIntf::startFromNewPage() const
{
    return m_startFromNewPage;
}

void BandDesignIntf::setStartFromNewPage(bool startWithNewPage)
{
    if (m_startFromNewPage != startWithNewPage){
        m_startFromNewPage = startWithNewPage;
        if (!isLoading())
            notify("startFromNewPage", !startWithNewPage, startWithNewPage);
    }
}

bool BandDesignIntf::startNewPage() const
{
    return m_startNewPage;
}

void BandDesignIntf::setStartNewPage(bool startNewPage)
{
    if (m_startNewPage != startNewPage){
        m_startNewPage = startNewPage;
        if (!isLoading())
            notify("startNewPage", !startNewPage, startNewPage);
    }
}

void BandDesignIntf::setAutoHeight(bool value){
    if (m_autoHeight != value){
        m_autoHeight=value;
        if (!isLoading())
            notify("autoHeight",!value,value);
    }
}

bool BandDesignIntf::reprintOnEachPage() const
{
    return m_reprintOnEachPage;
}

void BandDesignIntf::setReprintOnEachPage(bool reprintOnEachPage)
{
    if (m_reprintOnEachPage != reprintOnEachPage){
        m_reprintOnEachPage = reprintOnEachPage;
        if (!isLoading())
            notify("reprintOnEachPage", !m_reprintOnEachPage, m_reprintOnEachPage);
    }
}

int BandDesignIntf::columnIndex() const
{
    return m_columnIndex;
}

void BandDesignIntf::setColumnIndex(int columnIndex)
{
    m_columnIndex = columnIndex;
}

bool BandDesignIntf::printIfEmpty() const
{
    return m_printIfEmpty;
}

void BandDesignIntf::setPrintIfEmpty(bool printIfEmpty)
{
    if (m_printIfEmpty != printIfEmpty){
        m_printIfEmpty = printIfEmpty;
        if (!isLoading())
            notify("printIfEmpty", !m_printIfEmpty, m_printIfEmpty);
    }

}

BandDesignIntf *BandDesignIntf::bandHeader()
{
    foreach (BandDesignIntf* band, childBands()) {
        if (band->isHeader() && !band->isGroupHeader()){
            if (band->columnsCount() > 1) band->setColumnsFillDirection(this->columnsFillDirection());
            return band;
        }
    }
    return 0;
}

BandDesignIntf *BandDesignIntf::bandFooter()
{
    foreach (BandDesignIntf* band, childBands()) {
        if (band->isFooter()) return band;
    }
    return 0;
}

bool BandDesignIntf::sliceLastRow() const
{
    return m_sliceLastRow;
}

void BandDesignIntf::setSliceLastRow(bool sliceLastRow)
{
    if (m_sliceLastRow != sliceLastRow){
        m_sliceLastRow = sliceLastRow;
        if (!isLoading())
            notify("sliceLastRow", !sliceLastRow, sliceLastRow);
    }
}

int BandDesignIntf::maxScalePercent() const
{
    return m_maxScalePercent;
}

void BandDesignIntf::setMaxScalePercent(int maxScalePercent)
{
    m_maxScalePercent = maxScalePercent;
}

bool BandDesignIntf::keepFooterTogether() const
{
    return m_keepFooterTogether;
}

void BandDesignIntf::setKeepFooterTogether(bool value)
{
    if (m_keepFooterTogether!=value){
        bool oldValue = m_keepFooterTogether;
        m_keepFooterTogether = value;
        if (!isLoading())
            notify("keepFooterTogether",oldValue,value);
    }
}

void BandDesignIntf::updateItemSize(DataSourceManager* dataManager, RenderPass pass, int maxHeight)
{
    qreal spaceBorder = 0;
    if (keepBottomSpace()) spaceBorder = bottomSpace();
    spaceBorder = spaceBorder > 0 ? spaceBorder : 0;
    if (borderLines() != 0){
        spaceBorder += borderLineSize() + 2;
    }

    restoreLinks();
    snapshotItemsLayout();

    arrangeSubItems(pass, dataManager);
    if (autoHeight()){
        if (!keepTopSpace()) {
            qreal minTop = findMinTop() + m_shiftItems;
            foreach (BaseDesignIntf* item, childBaseItems()) {
                item->setY(item->y() - minTop);
            }
        }
        setHeight(findMaxBottom() + spaceBorder);
    }
    if ((maxHeight > 0) && (height() > maxHeight)){
        trimToMaxHeight(maxHeight);
        setHeight(maxHeight);
    }
    BaseDesignIntf::updateItemSize(dataManager, pass, maxHeight);
}

void BandDesignIntf::updateBandNameLabel()
{
    if (m_bandNameLabel) m_bandNameLabel->updateLabel(bandTitle());
}

void BandDesignIntf::initFromItem(BaseDesignIntf *source)
{
    ItemsContainerDesignInft::initFromItem(source);
    BandDesignIntf* source_band = dynamic_cast<BandDesignIntf*>(source);
    if (source_band){
        this->setBottomSpace(source_band->bottomSpace());
    }
}

QColor BandDesignIntf::selectionColor() const
{
    return Qt::yellow;
}

DataBandDesignIntf::DataBandDesignIntf(BandDesignIntf::BandsType bandType, QString xmlTypeName, QObject *owner, QGraphicsItem *parent)
    :BandDesignIntf(bandType,xmlTypeName,owner,parent)
{
}

BandNameLabel::BandNameLabel(BandDesignIntf *band, QGraphicsItem *parent)
    :QGraphicsItem(parent),m_rect(5,5,30,30),m_band(band)
{
    setAcceptHoverEvents(true);
}

void BandNameLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setFont(QFont("Arial",7*Const::fontFACTOR,-1,true));
    painter->setOpacity(1);
    QPen pen(Const::BAND_NAME_BORDER_COLOR);
    //pen.setWidth(2);
    painter->setBrush(Qt::yellow);
    painter->setPen(pen);
    painter->drawRoundedRect(m_rect,8,8);
    painter->setOpacity(0.8);
    painter->setPen(Qt::black);
    painter->drawText(m_rect,Qt::AlignCenter,m_band->bandTitle());
    painter->restore();
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

QRectF BandNameLabel::boundingRect() const
{
    return m_rect;
}

void BandNameLabel::updateLabel(const QString& bandName)
{
    QFont font("Arial",7*Const::fontFACTOR,-1,true);
    QFontMetrics fontMetrics(font);
    prepareGeometryChange();
    m_rect = QRectF(
                m_band->pos().x()+10,
                m_band->pos().y()-(fontMetrics.height()+10),
                fontMetrics.boundingRect(bandName).width()+20,fontMetrics.height()+10
                );
    update();
}

void BandNameLabel::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setVisible(false);
    Q_UNUSED(event)
}

}



