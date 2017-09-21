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
#ifndef LRHORIZONTALLAYOUT_H
#define LRHORIZONTALLAYOUT_H
#include "lritemdesignintf.h"

namespace LimeReport
{

class HorizontalLayout;

class LayoutMarker : public QGraphicsItem{
public:
    explicit LayoutMarker(HorizontalLayout* layout, QGraphicsItem *parent=0);
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
    HorizontalLayout* m_layout;
};

class HorizontalLayout : public LayoutDesignIntf
{
    Q_OBJECT
    Q_ENUMS(LayoutType)
    Q_PROPERTY(LayoutType layoutType READ layoutType WRITE setLayoutType)
public:
    friend class LayoutMarker;
    enum LayoutType{Layout,Table};
    HorizontalLayout(QObject *owner = 0, QGraphicsItem *parent = 0);
    ~HorizontalLayout();
    BaseDesignIntf *createSameTypeItem(QObject *owner = 0, QGraphicsItem *parent = 0);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void geometryChangedEvent(QRectF newRect, QRectF);
    void addChild(BaseDesignIntf *item,bool updateSize=true);
    friend class BaseDesignIntf;
    void restoreChild(BaseDesignIntf *item);
    bool isEmpty() const;
    LayoutType layoutType() const;
    void setLayoutType(const LayoutType &layoutType);
    bool isSplittable() const { return true;}
protected:
    void collectionLoadFinished(const QString &collectionName);
    void objectLoadFinished();
    void updateLayoutSize();
    void relocateChildren();
    BaseDesignIntf *findNext(BaseDesignIntf *item);
    BaseDesignIntf *findPrior(BaseDesignIntf *item);
    void beforeDelete();
    void updateItemSize(DataSourceManager *dataManager, RenderPass pass, int maxHeight);
    bool isNeedUpdateSize(RenderPass pass) const;
    void childAddedEvent(BaseDesignIntf *child);
    void setChildVisibility(bool value);
    int  childrenCount();
    void initMode(ItemMode mode);

    bool canBeSplitted(int height) const;
    BaseDesignIntf* cloneUpperPart(int height, QObject* owner=0, QGraphicsItem* parent=0);
    BaseDesignIntf* cloneBottomPart(int height, QObject *owner=0, QGraphicsItem *parent=0);

    void setItemAlign(const ItemAlign &itemAlign);
    void setBorderLinesFlags(BorderLines flags);

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void    paint(QPainter* ppainter, const QStyleOptionGraphicsItem* option, QWidget* widget);
private slots:
    void slotOnChildDestroy(QObject *child);
    void slotOnChildGeometryChanged(QObject*item, QRectF newGeometry, QRectF oldGeometry);
    void slotOnChildItemAlignChanged(BaseDesignIntf* item, const ItemAlign&, const ItemAlign&);
    void slotOnChildVisibleHasChanged(BaseDesignIntf*);
    void slotOnChildSelectionHasChanged(BaseDesignIntf* item, bool value);
    //void slotOnPosChanged(QObject*, QPointF newPos, QPointF );
private:
    void divideSpace();
private:
    QList<BaseDesignIntf *> m_children;
    bool m_isRelocating;
    LayoutMarker* m_layoutMarker;
    LayoutType m_layoutType;
};

} //namespace LimeReport
#endif // LRHORIZONTALLAYOUT_H
