#ifndef LRITEMSCONTAINERDESIGNITF_H
#define LRITEMSCONTAINERDESIGNITF_H

#include "lrbasedesignintf.h"

namespace LimeReport{

class Segment{
public:
    Segment(qreal segmentStart,qreal segmentEnd):m_begin(segmentStart),m_end(segmentEnd){}
    bool intersect(Segment value);
    qreal intersectValue(Segment value);
private:
    qreal m_begin;
    qreal m_end;
};

class VSegment : public Segment{
public:
    VSegment(QRectF rect):Segment(rect.top(),rect.bottom()){}
};

struct HSegment :public Segment{
public:
    HSegment(QRectF rect):Segment(rect.left(),rect.right()){}
};

struct ItemSortContainer {
    QRectF m_rect;
    BaseDesignIntf * m_item;
    ItemSortContainer(BaseDesignIntf *item){
        m_item=item;
        m_rect=item->geometry();
    }
};

typedef QSharedPointer< ItemSortContainer > PItemSortContainer;
bool itemSortContainerLessThen(const PItemSortContainer c1, const PItemSortContainer c2);

class ItemsContainerDesignInft : public BaseDesignIntf{
public:
  ItemsContainerDesignInft(const QString& xmlTypeName, QObject* owner = 0, QGraphicsItem* parent=0):
      BaseDesignIntf(xmlTypeName, owner, parent){}
protected:
  void  snapshotItemsLayout();
  void  arrangeSubItems(RenderPass pass, DataSourceManager *dataManager, ArrangeType type = AsNeeded);
  qreal findMaxBottom() const;
  qreal findMaxHeight() const;
private:
  QVector<PItemSortContainer> m_containerItems;

};

} // namespace LimeReport

#endif // ITEMSCONTAINERDESIGNITF_H
