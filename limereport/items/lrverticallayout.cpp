#include "lrverticallayout.h"

#include "lrbasedesignintf.h"
#include "lrdesignelementsfactory.h"

const QString xmlTag = "VLayout";

namespace {

LimeReport::BaseDesignIntf *createVLayout(QObject *owner, LimeReport::BaseDesignIntf  *parent)
{
    return new LimeReport::VerticalLayout(owner, parent);
}
bool VARIABLE_IS_NOT_USED registred = LimeReport::DesignElementsFactory::instance().registerCreator(
                     xmlTag,
                     LimeReport::ItemAttribs(QObject::tr("VLayout"), LimeReport::Const::bandTAG),
                     createVLayout
                 );
}

namespace LimeReport{

bool verticalLessThen(BaseDesignIntf *c1, BaseDesignIntf* c2){
    return c1->pos().y()<c2->pos().y();
}

VerticalLayout::VerticalLayout(QObject* owner, QGraphicsItem* parent)
    : AbstractLayout(xmlTag, owner, parent)
{}

VerticalLayout::~VerticalLayout()
{}

BaseDesignIntf* VerticalLayout::createSameTypeItem(QObject* owner, QGraphicsItem* parent)
{
    return new LimeReport::VerticalLayout(owner, parent);
}

void VerticalLayout::updateLayoutSize()
{
    int spaceBorder = (borderLines() != 0) ? borderLineSize() : 0;
    int h = spaceBorder*2;
    qreal w = 0;
    int visibleItemCount = 0;
    foreach(BaseDesignIntf* item, layoutsChildren()){
        if (item->isEmpty() && hideEmptyItems()) item->setVisible(false);
        if (item->isVisible()){
            if (w < item->width()) w = item->width();
            h+=item->height();
            visibleItemCount++;
        }
    }
    if (w>0) setWidth(w+spaceBorder*2);
    setHeight(h + layoutSpacingMM() *(visibleItemCount-1));
}

void VerticalLayout::relocateChildren()
{
    int spaceBorder = (borderLines() != 0) ? borderLineSize() : 0;
    QList<BaseDesignIntf*> newChildren;
    if (layoutsChildren().count() < childItems().size() - 1){
        auto oldChildren = layoutsChildren();
        layoutsChildren().clear();
        foreach (BaseDesignIntf* item, childBaseItems()) {
            if (!oldChildren.contains(item)) {
                newChildren.append(item);
            }
            layoutsChildren().append(item);
        }
    }
    std::sort(layoutsChildren().begin(),layoutsChildren().end(), verticalLessThen);
    qreal curY = spaceBorder;
    setIsRelocating(true);
    foreach (BaseDesignIntf* item, layoutsChildren()) {
        if (item->isVisible() || itemMode() == DesignMode){
            item->setPos(spaceBorder, curY);
            curY+=item->height() + layoutSpacingMM();
            item->setWidth(width() - (spaceBorder * 2));
        }
    }
    setIsRelocating(false);

    for (BaseDesignIntf* item : newChildren) {
        connectToLayout(item);
    }
}

bool VerticalLayout::canBeSplitted(int height) const
{
    if (childItems().isEmpty()) return false;
    BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(childItems().at(0));
    if (item){
        if (item->height() > height )
            return item->canBeSplitted(height);
        else return true;
    }
    return false;
}

BaseDesignIntf* VerticalLayout::cloneUpperPart(int height, QObject* owner, QGraphicsItem* parent)
{
    VerticalLayout* upperPart = dynamic_cast<VerticalLayout*>(createSameTypeItem(owner,parent));
    upperPart->initFromItem(this);
    foreach(BaseDesignIntf* item, childBaseItems()){
        if ((item->geometry().bottom() <= height) ){
            item->cloneItem(item->itemMode(),upperPart,upperPart);
        } else {
            if ((item->geometry().top() < height) && ( item->geometry().bottom() > height)){
                int sliceHeight = height - item->geometry().top();
                if (item->isSplittable() && item->canBeSplitted(sliceHeight)){
                    item->cloneUpperPart(sliceHeight,upperPart,upperPart);
                }
            }
        }
    }

    upperPart->setHeight(height);

    return upperPart;
}

BaseDesignIntf* VerticalLayout::cloneBottomPart(int height, QObject* owner, QGraphicsItem* parent)
{
    VerticalLayout* bottomPart = dynamic_cast<VerticalLayout*>(createSameTypeItem(owner,parent));
    bottomPart->initFromItem(this);

    foreach(BaseDesignIntf* item,childBaseItems()){
        if (item->geometry().bottom() > height){
            int sliceHeight = height - item->geometry().top();
            if ((item->geometry().top() < height) && (item->canBeSplitted(sliceHeight))){
                BaseDesignIntf* tmpItem = item->cloneBottomPart(sliceHeight, bottomPart, bottomPart);
                tmpItem->setHeight(sliceHeight);
                bottomPart->addChild(tmpItem);
            } else {
                bottomPart->addChild(item->cloneItem(item->itemMode(), bottomPart, bottomPart));
            }
        }
    }

    if (!bottomPart->isEmpty()){
        int currentHeight = 0;
        foreach (BaseDesignIntf* item, bottomPart->childBaseItems()) {
            currentHeight+=item->height();
        }
        bottomPart->setHeight(currentHeight);
    }
    return bottomPart;
}

void VerticalLayout::sortChildren()
{
    std::sort(layoutsChildren().begin(),layoutsChildren().end(),verticalLessThen);
}

void VerticalLayout::divideSpace()
{
    setIsRelocating(true);
    qreal itemsSumSize = 0;
    int visibleItemsCount = 0;
    int spaceBorder = (borderLines() != 0) ? borderLineSize() : 0;

    foreach(BaseDesignIntf* item, layoutsChildren()){
        if (item->isVisible() || itemMode() == DesignMode ){
            itemsSumSize += item->height();
            visibleItemsCount++;
        }
    }

    itemsSumSize += layoutSpacingMM() * (visibleItemsCount - 1);
    qreal delta = (height() - (itemsSumSize+spaceBorder*2)) / (visibleItemsCount!=0 ? visibleItemsCount : 1);

    for (int i=0; i<layoutsChildren().size(); ++i){
        if (layoutsChildren()[i]->isVisible() || itemMode() == DesignMode)
            layoutsChildren()[i]->setHeight(layoutsChildren()[i]->height()+delta);
        if ((i+1)<layoutsChildren().size())
            if (layoutsChildren()[i+1]->isVisible() || itemMode() == DesignMode)
                layoutsChildren()[i+1]->setPos(layoutsChildren()[i+1]->pos().x(), layoutsChildren()[i+1]->pos().y()+delta*(i+1));
    }
    setIsRelocating(false);
}

void VerticalLayout::placeItemInLayout(BaseDesignIntf* item)
{
    if (layoutsChildren().count() > 0)
        item->setPos(0, layoutsChildren().last()->pos().y() + layoutsChildren().last()->height());
    else
        item->setPos(0, 0);
}

void VerticalLayout::insertItemInLayout(BaseDesignIntf* item)
{
    foreach (BaseDesignIntf* child, childBaseItems()) {
        if (child->pos() == item->pos()){
            int index = layoutsChildren().indexOf(child)-1;
            layoutsChildren().insert(index, item);
            child->setPos(0, item->pos().y()+item->height());
            break;
        }
    }
}

} // namespace LimeReport
