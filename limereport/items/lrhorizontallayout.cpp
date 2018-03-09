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
#include "lrhorizontallayout.h"
#include "lrdesignelementsfactory.h"

#include <QDebug>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include "lrbasedesignintf.h"

const QString xmlTag = "HLayout";

namespace {

LimeReport::BaseDesignIntf *createHLayout(QObject *owner, LimeReport::BaseDesignIntf  *parent)
{
    return new LimeReport::HorizontalLayout(owner, parent);
}
bool VARIABLE_IS_NOT_USED registred = LimeReport::DesignElementsFactory::instance().registerCreator(
                     xmlTag,
                     LimeReport::ItemAttribs(QObject::tr("HLayout"), LimeReport::Const::bandTAG),
                     createHLayout
                 );
}


namespace LimeReport {

bool lessThen(BaseDesignIntf *c1, BaseDesignIntf* c2){
    return c1->pos().x()<c2->pos().x();
}


HorizontalLayout::HorizontalLayout(QObject *owner, QGraphicsItem *parent)
    : LayoutDesignIntf(xmlTag, owner, parent),m_isRelocating(false),m_layoutType(Layout)
{
    setPossibleResizeDirectionFlags(AllDirections);
    m_layoutMarker = new LayoutMarker(this);
    m_layoutMarker->setParentItem(this);
    m_layoutMarker->setColor(Qt::red);
    m_layoutMarker->setHeight(height());
    m_layoutMarker->setZValue(1);
}

HorizontalLayout::~HorizontalLayout()
{
    if (m_layoutMarker) {
        delete m_layoutMarker; m_layoutMarker=0;
    }
}

BaseDesignIntf *HorizontalLayout::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new LimeReport::HorizontalLayout(owner, parent);
}

void HorizontalLayout::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    LayoutDesignIntf::hoverEnterEvent(event);
//    if ((itemMode() & LayoutEditMode) || isSelected()){
//        setChildVisibility(false);
//    }
}

void HorizontalLayout::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    LayoutDesignIntf::hoverLeaveEvent(event);
//    setChildVisibility(true);
}

void HorizontalLayout::geometryChangedEvent(QRectF newRect, QRectF )
{
    m_layoutMarker->setHeight(newRect.height());
    relocateChildren();
    if (/*m_layoutType == Table && */!m_isRelocating){
        divideSpace();
    }
}

void HorizontalLayout::setChildVisibility(bool value){
    foreach(QGraphicsItem* child,childItems()){
        BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(child);
        if(item)
            item->setVisible(value);
    }
}

int HorizontalLayout::childrenCount()
{
    return m_children.size();
}

void HorizontalLayout::initMode(BaseDesignIntf::ItemMode mode)
{
    BaseDesignIntf::initMode(mode);
    if ((mode==PreviewMode)||(mode==PrintMode)){
        m_layoutMarker->setVisible(false);
    } else {
        m_layoutMarker->setVisible(true);
    }
}

bool HorizontalLayout::canBeSplitted(int height) const
{
    foreach(QGraphicsItem* qgItem,childItems()){
        BaseDesignIntf* item=dynamic_cast<BaseDesignIntf*>(qgItem);
        if (item)
            if (!item->canBeSplitted(height-item->pos().y())) return false;
    }
    return true;
}

BaseDesignIntf *HorizontalLayout::cloneUpperPart(int height, QObject *owner, QGraphicsItem *parent)
{
    HorizontalLayout* upperPart = dynamic_cast<HorizontalLayout*>(createSameTypeItem(owner,parent));
    upperPart->initFromItem(this);
    qreal maxHeight = 0;
    foreach(BaseDesignIntf* item,childBaseItems()){

        if ((item->geometry().top()<height) && (item->geometry().bottom()>height)){
            int sliceHeight = height-item->geometry().top();
            if (item->canBeSplitted(sliceHeight)){
                BaseDesignIntf* slicedPart = item->cloneUpperPart(sliceHeight,upperPart,upperPart);
                if (maxHeight<slicedPart->height()) maxHeight = slicedPart->height();
            } else {
                item->cloneEmpty(sliceHeight,upperPart,upperPart);
                item->setPos(item->pos().x(),item->pos().y()+((height+1)-item->geometry().top()));
            }
        }
    }

    foreach(BaseDesignIntf* item, upperPart->childBaseItems()){
        item->setHeight((maxHeight<height)?maxHeight:height);
    }
    upperPart->setHeight(height);

    return upperPart;
}

BaseDesignIntf *HorizontalLayout::cloneBottomPart(int height, QObject *owner, QGraphicsItem *parent)
{
    qreal maxHeight = 0;
    HorizontalLayout* bottomPart = dynamic_cast<HorizontalLayout*>(createSameTypeItem(owner,parent));
    bottomPart->initFromItem(this);
    foreach(BaseDesignIntf* item,childBaseItems()){
        if ((item->geometry().top()<height) && (item->geometry().bottom()>height)){
            BaseDesignIntf* tmpItem=item->cloneBottomPart(height,bottomPart,bottomPart);
            tmpItem->setPos(tmpItem->pos().x(),0);
            if (maxHeight<tmpItem->height())
                maxHeight = tmpItem->height();
        }
    }

    if (!bottomPart->isEmpty()){
        foreach (BaseDesignIntf* item, bottomPart->childBaseItems()) {
            item->setHeight(maxHeight);
        }
        bottomPart->setHeight(maxHeight);
    }
    return bottomPart;
}

void HorizontalLayout::setItemAlign(const BaseDesignIntf::ItemAlign &itemAlign)
{
    if (itemAlign == ParentWidthItemAlign)
        setLayoutType(Table);
    BaseDesignIntf::setItemAlign(itemAlign);
}

void HorizontalLayout::setBorderLinesFlags(BaseDesignIntf::BorderLines flags)
{
    BaseDesignIntf::setBorderLinesFlags(flags);
    if (flags!=0)
        relocateChildren();
}

QVariant HorizontalLayout::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged){
        m_isRelocating = true;
        foreach(BaseDesignIntf* item, m_children){
            item->setVisible(!value.toBool());
        }
        m_isRelocating = false;
    }
    return LayoutDesignIntf::itemChange(change, value);
}

void HorizontalLayout::paint(QPainter* ppainter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (isSelected()){
        foreach( BaseDesignIntf* item, m_children){
            ppainter->save();
            ppainter->setPen(Qt::red);
            ppainter->drawRect(
                QRectF(item->pos().x(),item->pos().y(),
                       item->rect().bottomRight().rx(),
                       item->rect().bottomRight().ry()
                )
            );
            ppainter->restore();
        }
    }
    LayoutDesignIntf::paint(ppainter, option, widget);
}

void HorizontalLayout::restoreChild(BaseDesignIntf* item){
    if (m_children.contains(item)) return;

    m_isRelocating=true;
    foreach (BaseDesignIntf* child, childBaseItems()) {
        if (child->pos()==item->pos()){
            int index = m_children.indexOf(child)-1;
            m_children.insert(index,item);
            child->setPos(item->pos().x()+item->width(),0);
            break;
        }
    }

    connect(item,SIGNAL(destroyed(QObject*)),this,SLOT(slotOnChildDestroy(QObject*)));
    connect(item,SIGNAL(geometryChanged(QObject*,QRectF,QRectF)),
            this,SLOT(slotOnChildGeometryChanged(QObject*,QRectF,QRectF)));
    connect(item, SIGNAL(itemAlignChanged(BaseDesignIntf*,ItemAlign,ItemAlign)),
            this, SLOT(slotOnChildItemAlignChanged(BaseDesignIntf*,ItemAlign,ItemAlign)));

    item->setFixedPos(true);
    item->setPossibleResizeDirectionFlags(ResizeRight | ResizeBottom);
    item->setParent(this);
    item->setParentItem(this);

    updateLayoutSize();
    m_isRelocating=false;
}

bool HorizontalLayout::isEmpty() const
{
    bool isEmpty = true;
    bool allItemsIsText = true;
    foreach (QGraphicsItem* qgItem, childItems()) {
        ContentItemDesignIntf* item = dynamic_cast<ContentItemDesignIntf*>(qgItem);
        if (item && !item->content().isEmpty()) isEmpty = false;
        if (!item && dynamic_cast<BaseDesignIntf*>(qgItem))
            allItemsIsText = false;
    }
    return (isEmpty && allItemsIsText);
}

void HorizontalLayout::addChild(BaseDesignIntf *item, bool updateSize)
{
    if (m_children.count() > 0)
        item->setPos(m_children.last()->pos().x() + m_children.last()->width(), 0);
    else
        item->setPos(0, 0);

    m_children.append(item);
    item->setParentItem(this);
    item->setParent(this);
    item->setFixedPos(true);
    item->setPossibleResizeDirectionFlags(ResizeRight | ResizeBottom);

    connect(
        item, SIGNAL(destroyed(QObject*)),
        this, SLOT(slotOnChildDestroy(QObject*))
    );
    connect(
        item,SIGNAL(geometryChanged(QObject*,QRectF,QRectF)),
        this,SLOT(slotOnChildGeometryChanged(QObject*,QRectF,QRectF))
    );
    connect(
        item, SIGNAL(itemVisibleHasChanged(BaseDesignIntf*)),
        this,SLOT(slotOnChildVisibleHasChanged(BaseDesignIntf*))
    );
    connect(
        item, SIGNAL(itemSelectedHasBeenChanged(BaseDesignIntf*,bool)),
        this, SLOT(slotOnChildSelectionHasChanged(BaseDesignIntf*,bool))
    );

    if (updateSize){
        relocateChildren();
        updateLayoutSize();
    }
}

void HorizontalLayout::collectionLoadFinished(const QString &collectionName)
{
    ItemDesignIntf::collectionLoadFinished(collectionName);
    if (collectionName.compare("children",Qt::CaseInsensitive)==0){
#ifdef HAVE_QT5
        foreach(QObject* obj,children()){
#else
        foreach(QObject* obj,QObject::children()){
#endif
            BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(obj);
            if (item) {
                addChild(item,false);
            }
        }
    }
}

void HorizontalLayout::objectLoadFinished()
{
    m_layoutMarker->setHeight(height());
    LayoutDesignIntf::objectLoadFinished();
}

void HorizontalLayout::updateLayoutSize()
{
    int spaceBorder = (borderLines() != 0) ? borderLineSize() : 0;
    int w = spaceBorder*2;
    qreal h = 0;
    foreach(BaseDesignIntf* item, m_children){
        if (item->isVisible()){
            if (h<item->height()) h=item->height();
            w+=item->width();
        }
    }
    if (h>0) setHeight(h+spaceBorder*2);
    setWidth(w);
}

void HorizontalLayout::relocateChildren()
{
    int spaceBorder = (borderLines() != 0) ? borderLineSize() : 0;
    if (m_children.count()<childItems().size()-1){
        m_children.clear();
        foreach (BaseDesignIntf* item, childBaseItems()) {
            m_children.append(item);
        }
    }
    qSort(m_children.begin(),m_children.end(),lessThen);
    qreal curX = spaceBorder;
    m_isRelocating = true;
    foreach (BaseDesignIntf* item, m_children) {
        if (item->isVisible() || itemMode() == DesignMode){
            item->setPos(curX,spaceBorder);
            curX+=item->width();
            item->setHeight(height()-(spaceBorder * 2));
        }
    }
    m_isRelocating = false;
}

void HorizontalLayout::beforeDelete()
{
    m_children.clear();
#ifdef HAVE_QT5
    foreach (QObject *item, children()) {
#else
    foreach (QObject *item, QObject::children()) {
#endif
        BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf*>(item);
        if (bdItem) {
            bdItem->setParentItem(parentItem());
            bdItem->setParent(parent());
            bdItem->setVisible(true);
            bdItem->setPos(mapToParent(bdItem->pos()));
            bdItem->setFixedPos(false);
            bdItem->setPossibleResizeDirectionFlags(AllDirections);
        }
    }
}

void HorizontalLayout::updateItemSize(DataSourceManager* dataManager, RenderPass pass, int maxHeight)
{
    m_isRelocating=true;
    ItemDesignIntf::updateItemSize(dataManager, pass, maxHeight);
    foreach(QGraphicsItem *child, childItems()){
        BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(child);
        if (item) item->updateItemSize(dataManager, pass, maxHeight);
    }
    updateLayoutSize();
    relocateChildren();
    m_isRelocating=false;
    BaseDesignIntf::updateItemSize(dataManager, pass, maxHeight);
}

bool HorizontalLayout::isNeedUpdateSize(RenderPass pass) const
{
    foreach (QGraphicsItem *child, childItems()) {
        BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(child);
        if (item && item->isNeedUpdateSize(pass))
            return true;
    }
    return false;
}

void HorizontalLayout::childAddedEvent(BaseDesignIntf *child)
{
    addChild(child,false);
}

void HorizontalLayout::slotOnChildDestroy(QObject* child)
{
    m_children.removeAll(static_cast<BaseDesignIntf*>(child));
    if (m_children.count()<2){
        beforeDelete();
//        deleteLater();
    } else {
        relocateChildren();
        updateLayoutSize();
    }
}

BaseDesignIntf* HorizontalLayout::findNext(BaseDesignIntf* item){
    if (m_children.count()<childItems().size()-1){
        m_children.clear();
        foreach (BaseDesignIntf* childItem, childBaseItems()) {
            m_children.append(childItem);
        }
    }
    qSort(m_children.begin(),m_children.end(),lessThen);
    for (int i=0; i<m_children.count();++i){
        if (m_children[i]==item && m_children.size()>i+1){ return m_children[i+1];}
    }
    return 0;
}

BaseDesignIntf* HorizontalLayout::findPrior(BaseDesignIntf* item){
    if (m_children.count()<childItems().size()-1){
        m_children.clear();
        foreach (BaseDesignIntf* childItem, childBaseItems()) {
            m_children.append(childItem);
        }
    }
    qSort(m_children.begin(),m_children.end(),lessThen);
    for (int i=0; i<m_children.count();++i){
        if (m_children[i]==item && i!=0){ return m_children[i-1];}
    }
    return 0;
}

void HorizontalLayout::divideSpace(){
    m_isRelocating = true;
    qreal itemsSumSize = 0;
    int visibleItemsCount = 0;
    int spaceBorder = (borderLines() != 0) ? borderLineSize() : 0;

    foreach(BaseDesignIntf* item, m_children){
        if (item->isVisible() || itemMode() == DesignMode ){
            itemsSumSize += item->width();
            visibleItemsCount++;
        }
    }
    qreal delta = (width() - (itemsSumSize+spaceBorder*2)) / (visibleItemsCount!=0 ? visibleItemsCount : 1);

    for (int i=0; i<m_children.size(); ++i){
        if (m_children[i]->isVisible() || itemMode() == DesignMode)
            m_children[i]->setWidth(m_children[i]->width()+delta);
        if ((i+1)<m_children.size())
            if (m_children[i+1]->isVisible() || itemMode() == DesignMode)
                m_children[i+1]->setPos(m_children[i+1]->pos().x()+delta*(i+1),m_children[i+1]->pos().y());
    }
    m_isRelocating = false;
}
void HorizontalLayout::slotOnChildGeometryChanged(QObject *item, QRectF newGeometry, QRectF oldGeometry)
{
    if (!m_isRelocating){
        //setHeight(newGeometry.height());
        if (m_layoutType == Layout){
            relocateChildren();
            updateLayoutSize();
        } else {
            m_isRelocating = true;
            qreal delta = newGeometry.width()-oldGeometry.width();
            BaseDesignIntf* resizingItem = findNext(dynamic_cast<BaseDesignIntf*>(item));
            if (resizingItem) {
                resizingItem->setWidth(resizingItem->width()-delta);
                resizingItem->setPos(resizingItem->pos().x()+delta,resizingItem->pos().y());
            }
            updateLayoutSize();
            m_isRelocating = false;
        }
    }
}

void HorizontalLayout::slotOnChildItemAlignChanged(BaseDesignIntf *item, const BaseDesignIntf::ItemAlign &, const BaseDesignIntf::ItemAlign&)
{
    item->setPossibleResizeDirectionFlags(ResizeBottom | ResizeRight);
}

void HorizontalLayout::slotOnChildVisibleHasChanged(BaseDesignIntf *)
{
    relocateChildren();
    if (m_layoutType == Table && !m_isRelocating){
        divideSpace();
    }
}

void HorizontalLayout::slotOnChildSelectionHasChanged(BaseDesignIntf* item, bool value)
{
    item->setZValue(value ? item->zValue()+1 : item->zValue()-1);
}

HorizontalLayout::LayoutType HorizontalLayout::layoutType() const
{
    return m_layoutType;
}

void HorizontalLayout::setLayoutType(const LayoutType &layoutType)
{
    if (m_layoutType != layoutType){
        LayoutType oldValue = m_layoutType;
        m_layoutType = layoutType;
        notify("layoutType",oldValue,layoutType);
    }

}

LayoutMarker::LayoutMarker(HorizontalLayout *layout, QGraphicsItem *parent)
    :QGraphicsItem(parent), m_rect(0,0,30,30), m_color(Qt::red), m_layout(layout){
    setFlag(QGraphicsItem::ItemIsMovable);
}

void LayoutMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();
    painter->setOpacity(Const::LAYOUT_MARKER_OPACITY);
    painter->fillRect(boundingRect(),m_color);

    painter->setRenderHint(QPainter::Antialiasing);
    qreal size = (boundingRect().width()<boundingRect().height()) ? boundingRect().width() : boundingRect().height();

    if (m_layout->isSelected()){
        painter->setOpacity(1);
        QRectF r = QRectF(0,0,size,size);
        painter->setBrush(Qt::white);
        painter->setPen(Qt::white);
        painter->drawEllipse(r.adjusted(5,5,-5,-5));
        painter->setBrush(m_color);
        painter->drawEllipse(r.adjusted(7,7,-7,-7));
    }
    painter->restore();
}

void LayoutMarker::setHeight(qreal height)
{
    if (m_rect.height()!=height){
        prepareGeometryChange();
        m_rect.setHeight(height);
    }
}

void LayoutMarker::setWidth(qreal width)
{
    if (m_rect.width()!=width){
        prepareGeometryChange();
        m_rect.setWidth(width);
    }
}

void LayoutMarker::setColor(QColor color)
{
    if (m_color!=color){
        m_color = color;
        update(boundingRect());
    }
}

void LayoutMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button()==Qt::LeftButton) {
        if (!(event->modifiers() & Qt::ControlModifier))
            m_layout->scene()->clearSelection();
        m_layout->setSelected(true);
        //m_layout->setChildVisibility(false);
        update(0,0,boundingRect().width(),boundingRect().width());
    }
}

} // namespace LimeReport
