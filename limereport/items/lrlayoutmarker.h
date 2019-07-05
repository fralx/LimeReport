#ifndef LRLAYOUTMARKER_H
#define LRLAYOUTMARKER_H

#include <QGraphicsItem>
#include "lrbanddesignintf.h"

namespace LimeReport{

class LayoutMarker : public QGraphicsItem{
public:
    explicit LayoutMarker(BaseDesignIntf* layout, QGraphicsItem *parent=0);
    virtual QRectF boundingRect() const{return m_rect;}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    void setHeight(qreal height);
    void setWidth(qreal width);
    void setColor(QColor color);
    qreal width(){return m_rect.width();}
    qreal height(){return m_rect.height();}
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
private:
    QRectF m_rect;
    QColor m_color;
    BaseDesignIntf* m_layout;
};

} // namespace LimeReport
#endif // LRLAYOUTMARKER_H
