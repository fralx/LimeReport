#include "lrabstractlayout.h"

namespace LimeReport {

AbstractLayout::AbstractLayout(QString xmlTag, QObject* owner, QGraphicsItem* parent)
    : LayoutDesignIntf(xmlTag, owner, parent), m_isRelocating(false), m_layoutType(Layout),
    m_hideEmptyItems(false), m_layoutSpacing(0)
{
    setPossibleResizeDirectionFlags(AllDirections);
    m_layoutMarker = new LayoutMarker(this);
    m_layoutMarker->setParentItem(this);
    m_layoutMarker->setColor(Qt::red);
    m_layoutMarker->setHeight(height());
    m_layoutMarker->setZValue(1);
}

AbstractLayout::~AbstractLayout()
{
    if (m_layoutMarker) {
        delete m_layoutMarker; m_layoutMarker=0;
    }
}

QList<BaseDesignIntf*>& AbstractLayout::layoutsChildren()
{
    return m_children;
}

bool AbstractLayout::isRelocating() const
{
    return m_isRelocating;
}

void AbstractLayout::setIsRelocating(bool isRelocating)
{
    m_isRelocating = isRelocating;
}

AbstractLayout::LayoutType AbstractLayout::layoutType() const
{
    return m_layoutType;
}

void AbstractLayout::setLayoutType(const LayoutType& layoutType)
{
    m_layoutType = layoutType;
}

void AbstractLayout::addChild(BaseDesignIntf* item, bool updateSize)
{
    placeItemInLayout(item);

    m_children.append(item);
    item->setParentItem(this);
    item->setParent(this);
    item->setFixedPos(true);
    item->setPossibleResizeDirectionFlags(ResizeRight | ResizeBottom);

    connectToLayout(item);

    if (updateSize){
        relocateChildren();
        updateLayoutSize();
    }
}

void AbstractLayout::removeChild(BaseDesignIntf *item)
{
    if (!item) {
        return;
    }
    m_children.removeAll(item);
    disconnectFromLayout(item);
}

void AbstractLayout::restoreChild(BaseDesignIntf* item)
{
    if (m_children.contains(item)) return;
    m_isRelocating=true;

    insertItemInLayout(item);

    connectToLayout(item);

    item->setFixedPos(true);
    item->setPossibleResizeDirectionFlags(ResizeRight | ResizeBottom);
    item->setParent(this);
    item->setParentItem(this);

    updateLayoutSize();
    m_isRelocating=false;
}

bool AbstractLayout::isEmpty() const
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

void AbstractLayout::paintChild(BaseDesignIntf *child, QPointF parentPos, QPainter *painter)
{
    if (!child->childBaseItems().isEmpty()){
        foreach (BaseDesignIntf* item, child->childBaseItems()) {
            paintChild(item, child->pos(),painter);
        }
    }
    painter->drawRect(
        QRectF(parentPos.x()+child->pos().x(), parentPos.y()+child->pos().y(),
               child->rect().bottomRight().rx(),
               child->rect().bottomRight().ry()
        )
    );
}

void AbstractLayout::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (isSelected()){
        painter->save();
        painter->setPen(Qt::red);
        foreach( BaseDesignIntf* item, m_children){
            paintChild(item, QPointF(0,0), painter);
        }
        painter->restore();
    }
    LayoutDesignIntf::paint(painter, option, widget);
}

int AbstractLayout::childrenCount()
{
    return m_children.size();
}

void AbstractLayout::beforeDelete()
{
#ifdef HAVE_QT5
    foreach (QObject *item, children()) {
#else
    foreach (QObject *item, QObject::children()) {
#endif
        BaseDesignIntf *bi = dynamic_cast<BaseDesignIntf*>(item);
        if (bi) {
            bi->disconnect(this);
            bi->setParentItem(parentItem());
            bi->setParent(parent());
            bi->setVisible(true);
            bi->setPos(mapToParent(bi->pos()));
            bi->setFixedPos(false);
            bi->setPossibleResizeDirectionFlags(AllDirections);
        }
    }
    m_children.clear();
}

void AbstractLayout::childAddedEvent(BaseDesignIntf* child)
{
    addChild(child,false);
}

void AbstractLayout::geometryChangedEvent(QRectF newRect, QRectF)
{
    layoutMarker()->setHeight(newRect.height());
    relocateChildren();
    if (!isRelocating()){
        divideSpace();
    }
}

void AbstractLayout::initMode(BaseDesignIntf::ItemMode mode)
{
    BaseDesignIntf::initMode(mode);
    if ((mode==PreviewMode)||(mode==PrintMode)){
        layoutMarker()->setVisible(false);
    } else {
        layoutMarker()->setVisible(true);
    }
}

void AbstractLayout::setBorderLinesFlags(BaseDesignIntf::BorderLines flags)
{
    BaseDesignIntf::setBorderLinesFlags(flags);
    if (flags!=0)
        relocateChildren();
}

void AbstractLayout::collectionLoadFinished(const QString& collectionName)
{
    ItemDesignIntf::collectionLoadFinished(collectionName);
    if (collectionName.compare("children",Qt::CaseInsensitive)==0){
#ifdef HAVE_QT5
        foreach(QObject* obj, children()){
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

void AbstractLayout::objectLoadFinished()
{
    layoutMarker()->setHeight(height());
    LayoutDesignIntf::objectLoadFinished();
}

bool AbstractLayout::isNeedUpdateSize(RenderPass pass) const
{
    Q_UNUSED(pass)
    return true;
}

QVariant AbstractLayout::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged){
        setIsRelocating(true);
        foreach(BaseDesignIntf* item, layoutsChildren()){
            item->setVisible(!value.toBool());
        }
        setIsRelocating(false);
    }
    return LayoutDesignIntf::itemChange(change, value);
}

void AbstractLayout::updateItemSize(DataSourceManager* dataManager, RenderPass pass, int maxHeight)
{
    setIsRelocating(true);
    ItemDesignIntf::updateItemSize(dataManager, pass, maxHeight);
    foreach(QGraphicsItem *child, childItems()){
        BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(child);
        if (item && item->isNeedUpdateSize(pass))
            item->updateItemSize(dataManager, pass, maxHeight);
    }
    updateLayoutSize();
    relocateChildren();
    setIsRelocating(false);
    BaseDesignIntf::updateItemSize(dataManager, pass, maxHeight);
}

void AbstractLayout::rebuildChildrenIfNeeded(){
    if (layoutsChildren().count() < childItems().size()-1){
        layoutsChildren().clear();
        foreach (BaseDesignIntf* childItem, childBaseItems()) {
            layoutsChildren().append(childItem);
        }
        sortChildren();
    }
}

void AbstractLayout::connectToLayout(BaseDesignIntf *item)
{
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
        this, SLOT(slotOnChildVisibleHasChanged(BaseDesignIntf*))
        );
    connect(
        item, SIGNAL(itemSelectedHasBeenChanged(BaseDesignIntf*,bool)),
        this, SLOT(slotOnChildSelectionHasChanged(BaseDesignIntf*,bool))
        );
    connect(
        item, SIGNAL(itemAlignChanged(BaseDesignIntf*, const ItemAlign&, const ItemAlign&)),
        this, SLOT(slotOnChildItemAlignChanged(BaseDesignIntf*,const ItemAlign&,const ItemAlign&))
        );
}

void AbstractLayout::disconnectFromLayout(BaseDesignIntf *item)
{
    disconnect(
        item, SIGNAL(destroyed(QObject*)),
        this, SLOT(slotOnChildDestroy(QObject*))
        );
    disconnect(
        item,SIGNAL(geometryChanged(QObject*,QRectF,QRectF)),
        this,SLOT(slotOnChildGeometryChanged(QObject*,QRectF,QRectF))
        );
    disconnect(
        item, SIGNAL(itemVisibleHasChanged(BaseDesignIntf*)),
        this, SLOT(slotOnChildVisibleHasChanged(BaseDesignIntf*))
        );
    disconnect(
        item, SIGNAL(itemSelectedHasBeenChanged(BaseDesignIntf*,bool)),
        this, SLOT(slotOnChildSelectionHasChanged(BaseDesignIntf*,bool))
        );
    disconnect(
        item, SIGNAL(itemAlignChanged(BaseDesignIntf*, const ItemAlign&, const ItemAlign&)),
        this, SLOT(slotOnChildItemAlignChanged(BaseDesignIntf*,const ItemAlign&,const ItemAlign&))
        );
}

BaseDesignIntf *AbstractLayout::findNext(BaseDesignIntf *item)
{
    rebuildChildrenIfNeeded();
    for (int i=0; i<layoutsChildren().count();++i){
        if (layoutsChildren()[i]==item && layoutsChildren().size()>i+1){ return layoutsChildren()[i+1];}
    }
    return 0;
}

BaseDesignIntf *AbstractLayout::findPrior(BaseDesignIntf *item)
{
    rebuildChildrenIfNeeded();
    for (int i=0; i<layoutsChildren().count();++i){
        if (layoutsChildren()[i]==item && i!=0){ return layoutsChildren()[i-1];}
    }
    return 0;
}

void AbstractLayout::slotOnChildDestroy(QObject* child)
{
    m_children.removeAll(static_cast<BaseDesignIntf*>(child));
    if (m_children.count() < 2 && !static_cast<LayoutDesignIntf*>(child)){
        beforeDelete();
    } else {
        relocateChildren();
        updateLayoutSize();
    }
}

void AbstractLayout::slotOnChildGeometryChanged(QObject* item, QRectF newGeometry, QRectF oldGeometry)
{
    if (!m_isRelocating && !isLoading()){
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

void AbstractLayout::slotOnChildItemAlignChanged(BaseDesignIntf* item, const ItemAlign&, const ItemAlign&)
{
    item->setPossibleResizeDirectionFlags(ResizeBottom | ResizeRight);
}

void AbstractLayout::slotOnChildVisibleHasChanged(BaseDesignIntf*)
{
    relocateChildren();
    if (m_layoutType == Table && !m_isRelocating){
        divideSpace();
    }
}

void AbstractLayout::slotOnChildSelectionHasChanged(BaseDesignIntf* item, bool value)
{
    item->setZValue(value ? item->zValue()+1 : item->zValue()-1);
}

int AbstractLayout::layoutSpacing() const
{
    return m_layoutSpacing;
}

void AbstractLayout::setLayoutSpacing(int layoutSpacing)
{
     if (m_layoutSpacing != layoutSpacing){
        int oldValue = m_layoutSpacing;
        m_layoutSpacing = layoutSpacing;
        if (!isLoading()){
            int delta  = (m_layoutSpacing - oldValue)  * (m_children.count()-1);
            notify("layoutSpacing", oldValue, m_layoutSpacing);
            setWidth(width() + delta);
        }
        relocateChildren();
    }
}

bool AbstractLayout::hideEmptyItems() const
{
    return m_hideEmptyItems;
}

void AbstractLayout::setHideEmptyItems(bool hideEmptyItems)
{
    m_hideEmptyItems = hideEmptyItems;

    if (m_hideEmptyItems != hideEmptyItems){
        m_hideEmptyItems = hideEmptyItems;
        notify("hideEmptyItems", !m_hideEmptyItems, m_hideEmptyItems);
    }
}

QObject* AbstractLayout::at(int index)
{
    rebuildChildrenIfNeeded();
    if (layoutsChildren().size() > index) return layoutsChildren()[index];
    return 0;
}

LayoutMarker* AbstractLayout::layoutMarker() const
{
    return m_layoutMarker;
}

} // namespace LimeReport
