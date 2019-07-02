#include "lrlayoutmarker.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

namespace LimeReport{

LayoutMarker::LayoutMarker(BaseDesignIntf* layout, QGraphicsItem *parent)
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
