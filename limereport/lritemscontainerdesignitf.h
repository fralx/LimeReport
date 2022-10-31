#ifndef LRITEMSCONTAINERDESIGNITF_H
#define LRITEMSCONTAINERDESIGNITF_H

#include "lrbasedesignintf.h"

namespace LimeReport{

class LIMEREPORT_EXPORT Segment{
public:
    Segment(qreal segmentStart,qreal segmentEnd):m_begin(segmentStart),m_end(segmentEnd){}
    bool intersect(Segment value);
    qreal intersectValue(Segment value);
private:
    qreal m_begin;
    qreal m_end;
};

class LIMEREPORT_EXPORT VSegment : public Segment{
public:
    VSegment(QRectF rect):Segment(rect.top(),rect.bottom()){}
};

struct LIMEREPORT_EXPORT HSegment :public Segment{
public:
    HSegment(QRectF rect):Segment(rect.left(),rect.right()){}
};

struct LIMEREPORT_EXPORT ItemSortContainer {
    QRectF m_rect;
    BaseDesignIntf * m_item;
    ItemSortContainer(BaseDesignIntf *item){
        m_item=item;
        m_rect=item->geometry();
    }
};

typedef QSharedPointer< ItemSortContainer > PItemSortContainer;
bool LIMEREPORT_EXPORT itemSortContainerLessThen(const PItemSortContainer c1, const PItemSortContainer c2);

class LIMEREPORT_EXPORT ItemsContainerDesignInft : public BookmarkContainerDesignIntf{
    Q_OBJECT
public:
  ItemsContainerDesignInft(const QString& xmlTypeName, QObject* owner = 0, QGraphicsItem* parent=0):
      BookmarkContainerDesignIntf(xmlTypeName, owner, parent){}
  enum SnapshotType{Full, IgnoreBands};
protected:
  void  snapshotItemsLayout(SnapshotType type = Full);
  void  arrangeSubItems(RenderPass pass, DataSourceManager *dataManager, ArrangeType type = AsNeeded);
  qreal findMaxBottom() const;
  qreal findMaxHeight() const;
  qreal findMinTop() const;
private:
  QVector<PItemSortContainer> m_containerItems;

};

} // namespace LimeReport

#endif // ITEMSCONTAINERDESIGNITF_H
