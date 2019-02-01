#ifndef LRVERTICALLAYOUT_H
#define LRVERTICALLAYOUT_H

#include "lritemdesignintf.h"
#include "lrlayoutmarker.h"
#include "lrabstractlayout.h"

namespace LimeReport{

class VerticalLayout : public AbstractLayout
{
    Q_OBJECT
public:
    friend class BaseDesignIntf;
    VerticalLayout(QObject *owner = 0, QGraphicsItem *parent = 0);
    ~VerticalLayout();
    // BaseDesignIntf interface
    BaseDesignIntf*createSameTypeItem(QObject* owner, QGraphicsItem* parent);
    bool isSplittable() const { return true;}
protected:
    void updateLayoutSize();
    void relocateChildren();
    bool canBeSplitted(int height) const;
    BaseDesignIntf* cloneUpperPart(int height, QObject* owner=0, QGraphicsItem* parent=0);
    BaseDesignIntf* cloneBottomPart(int height, QObject *owner=0, QGraphicsItem *parent=0);
private:
    void sortChildren();
    void divideSpace();
    void placeItemInLayout(BaseDesignIntf* item);
    void insertItemInLayout(BaseDesignIntf* item);
};

} // namespace LimeReport
#endif // LRVERTICALLAYOUT_H
