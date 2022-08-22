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

bool horizontalLessThen(BaseDesignIntf *c1, BaseDesignIntf* c2){
    return c1->pos().x()<c2->pos().x();
}

HorizontalLayout::HorizontalLayout(QObject *owner, QGraphicsItem *parent)
    : AbstractLayout(xmlTag, owner, parent)
{}

HorizontalLayout::~HorizontalLayout()
{}

BaseDesignIntf *HorizontalLayout::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new LimeReport::HorizontalLayout(owner, parent);
}

bool HorizontalLayout::canBeSplitted(int height) const
{
    foreach(QGraphicsItem* qgItem,childItems()){
        BaseDesignIntf* item=dynamic_cast<BaseDesignIntf*>(qgItem);
        if (item)
            if (!item->canBeSplitted(height - item->pos().y())) return false;
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

void HorizontalLayout::sortChildren()
{
    std::sort(layoutsChildren().begin(),layoutsChildren().end(),horizontalLessThen);
}

void HorizontalLayout::updateLayoutSize()
{
    int spaceBorder = (borderLines() != 0) ? borderLineSize() : 0;
    qreal w = spaceBorder*2;
    qreal h = 0;
    int visibleItemCount = 0;
    foreach(BaseDesignIntf* item, layoutsChildren()){
        if (item->isEmpty() && hideEmptyItems()) item->setVisible(false);
        if (item->isVisible()){
            if (h<item->height()) h=item->height();
            w+=item->width();
            visibleItemCount++;
        }
    }
    if (h>0) setHeight(h+spaceBorder*2);    
    if (layoutType() == Layout)
        setWidth(w + layoutSpacingMM() * (visibleItemCount-1));
    else{
        relocateChildren();
        if (!isRelocating()){
            divideSpace();
        }
    }
}

void HorizontalLayout::relocateChildren()
{
    int spaceBorder = (borderLines() != 0) ? borderLineSize() : 0;
    QList<BaseDesignIntf*> newChildren;
    if (layoutsChildren().count() < childItems().size()-1){
        auto oldChildren = layoutsChildren();
        layoutsChildren().clear();
        foreach (BaseDesignIntf* item, childBaseItems()) {
            if (!oldChildren.contains(item)) {
                newChildren.append(item);
            }
            layoutsChildren().append(item);
        }
    }
    std::sort(layoutsChildren().begin(),layoutsChildren().end(),horizontalLessThen);
    qreal curX = spaceBorder;
    setIsRelocating(true);
    foreach (BaseDesignIntf* item, layoutsChildren()) {
        if (item->isVisible() || itemMode() == DesignMode){
            item->setPos(curX,spaceBorder);
            curX += item->width() + layoutSpacingMM();
            item->setHeight(height()-(spaceBorder * 2));
        }
    }
    setIsRelocating(false);

    for (BaseDesignIntf* item : newChildren) {
        connectToLayout(item);
    }
}

void HorizontalLayout::divideSpace(){
    setIsRelocating(true);
    qreal itemsSumSize = 0;
    int visibleItemsCount = 0;
    int spaceBorder = (borderLines() != 0) ? borderLineSize() : 0;

    foreach(BaseDesignIntf* item, layoutsChildren()){
        if (item->isVisible() || itemMode() == DesignMode ){
            itemsSumSize += item->width();
            visibleItemsCount++;
        }
    }

    itemsSumSize += layoutSpacingMM() * (visibleItemsCount-1);

    if (itemMode() == DesignMode && !layoutsChildren().isEmpty()){
        qreal delta = (width() - (itemsSumSize+spaceBorder*2));
        layoutsChildren().last()->setWidth(layoutsChildren().last()->width()+delta);
    } else {
        qreal delta = (width() - (itemsSumSize+spaceBorder*2)) / (visibleItemsCount!=0 ? visibleItemsCount : 1);
        for (int i=0; i<layoutsChildren().size(); ++i){
            if (layoutsChildren()[i]->isVisible() || itemMode() == DesignMode)
                layoutsChildren()[i]->setWidth(layoutsChildren()[i]->width()+delta);
            if ((i+1)<layoutsChildren().size())
                if (layoutsChildren()[i+1]->isVisible() || itemMode() == DesignMode)
                    layoutsChildren()[i+1]->setPos(layoutsChildren()[i+1]->pos().x()+delta*(i+1),layoutsChildren()[i+1]->pos().y());
        }
    }
    setIsRelocating(false);
}

void HorizontalLayout::placeItemInLayout(BaseDesignIntf* item)
{
    if (layoutsChildren().count() > 0)
        item->setPos(layoutsChildren().last()->pos().x() + layoutsChildren().last()->width(), 0);
    else
        item->setPos(0, 0);
}

void HorizontalLayout::insertItemInLayout(BaseDesignIntf* item)
{
    foreach (BaseDesignIntf* child, childBaseItems()) {
        if (child->pos() == item->pos()){
            int index = layoutsChildren().indexOf(child)-1;
            layoutsChildren().insert(index, item);
            child->setPos(item->pos().x()+item->width(), 0);
            break;
        }
    }
}

} // namespace LimeReport
