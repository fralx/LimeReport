#include "lritemscontainerdesignitf.h"
#include "lritemdesignintf.h"
#include "lrbanddesignintf.h"

namespace LimeReport {

bool Segment::intersect(Segment value)
{
    return (value.m_begin <= m_end) && (value.m_end >= m_begin);
}

qreal Segment::intersectValue(Segment value)
{
    if (!intersect(value)) return 0;
    if ((value.m_end >= m_begin) && (value.m_end <= m_end)){
        return value.m_end-m_begin;
    }
    if ((value.m_begin>=m_begin)&&(value.m_end>=m_end)){
        return m_end-value.m_begin;
    }
    if ((value.m_begin>=m_begin)&&(value.m_end<=m_end)){
        return value.m_end-value.m_begin;
    }
    if ((value.m_begin<m_begin)&&(value.m_end>m_end)){
        return m_end-m_begin;
    }
    return 0;
}

bool itemSortContainerLessThen(const PItemSortContainer c1, const PItemSortContainer c2)
{
    VSegment vS1(c1->m_rect),vS2(c2->m_rect);
    HSegment hS1(c1->m_rect),hS2(c2->m_rect);
    if (vS1.intersectValue(vS2)>hS1.intersectValue(hS2))
        return c1->m_rect.x()<c2->m_rect.x();
    else return c1->m_rect.y()<c2->m_rect.y();
}

void ItemsContainerDesignInft::snapshotItemsLayout(SnapshotType type)
{
    m_containerItems.clear();
    foreach(BaseDesignIntf *childItem, childBaseItems()){
        if (type == IgnoreBands){
            if (!dynamic_cast<BandDesignIntf*>(childItem))
                m_containerItems.append(PItemSortContainer(new ItemSortContainer(childItem)));
        } else
            m_containerItems.append(PItemSortContainer(new ItemSortContainer(childItem)));
    }
    std::sort(m_containerItems.begin(),m_containerItems.end(),itemSortContainerLessThen);
}

void ItemsContainerDesignInft::arrangeSubItems(RenderPass pass, DataSourceManager *dataManager, ArrangeType type)
{
    bool needArrage=(type==Force);

    foreach (PItemSortContainer item, m_containerItems) {
        if (item->m_item->isNeedUpdateSize(pass)){
            item->m_item->updateItemSize(dataManager, pass);
            needArrage=true;
        }
    }

    if (needArrage){
        for (int i=0;i<m_containerItems.count();i++){
            for (int j=i;j<m_containerItems.count();j++){
                if ((i!=j) && (m_containerItems[i]->m_item->collidesWithItem(m_containerItems[j]->m_item))){
                    HSegment hS1(m_containerItems[j]->m_rect),hS2(m_containerItems[i]->m_rect);
                    VSegment vS1(m_containerItems[j]->m_rect),vS2(m_containerItems[i]->m_rect);
                    if (m_containerItems[i]->m_rect.bottom()<m_containerItems[i]->m_item->geometry().bottom()){
                       if (hS1.intersectValue(hS2)>vS1.intersectValue(vS2))
                           m_containerItems[j]->m_item->setY(m_containerItems[i]->m_item->y()+m_containerItems[i]->m_item->height()
                                                      +m_containerItems[j]->m_rect.top()-m_containerItems[i]->m_rect.bottom());

                    }
                    if (m_containerItems[i]->m_rect.right()<m_containerItems[i]->m_item->geometry().right()){
                       if (vS1.intersectValue(vS2)>hS1.intersectValue(hS2))
                       m_containerItems[j]->m_item->setX(m_containerItems[i]->m_item->geometry().right()+
                                                  (m_containerItems[j]->m_rect.x()-m_containerItems[i]->m_rect.right()));
                    }
                }
            }
        }
    }

    if (needArrage||pass==FirstPass){
        int maxBottom = findMaxBottom();
        foreach(BaseDesignIntf* item,childBaseItems()){
            ItemDesignIntf* childItem=dynamic_cast<ItemDesignIntf*>(item);
            if (childItem){
                if (childItem->stretchToMaxHeight())
                    childItem->setHeight(maxBottom-childItem->geometry().top());
            }
        }
    }
}

qreal ItemsContainerDesignInft::findMaxBottom() const
{
    qreal maxBottom = 0;
    foreach(QGraphicsItem* item,childItems()){
        BaseDesignIntf* subItem = dynamic_cast<BaseDesignIntf *>(item);
        if(subItem)
           if ( subItem->isVisible() && (subItem->geometry().bottom()>maxBottom) )
               maxBottom = subItem->geometry().bottom();
    }
    return maxBottom;
}

qreal ItemsContainerDesignInft::findMinTop() const{
    qreal minTop = height();
    foreach(QGraphicsItem* item,childItems()){
        BaseDesignIntf* subItem = dynamic_cast<BaseDesignIntf *>(item);
        if(subItem)
           if ( subItem->isVisible() && (subItem->geometry().top()<minTop) )
               minTop = subItem->geometry().top();
    }
    return minTop > 0 ? minTop : 0;
}

qreal ItemsContainerDesignInft::findMaxHeight() const
{
    qreal maxHeight=0;
    foreach(QGraphicsItem* item,childItems()){
        BaseDesignIntf* subItem = dynamic_cast<BaseDesignIntf *>(item);
        if(subItem)
           if (subItem->geometry().height()>maxHeight) maxHeight=subItem->geometry().height();
    }
    return maxHeight;
}

} // namespace LimeReport
