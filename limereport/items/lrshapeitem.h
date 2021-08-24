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
#ifndef LRSHAPEITEM_H
#define LRSHAPEITEM_H
#include "lritemdesignintf.h"
#include <QtGlobal>

namespace LimeReport{

class ShapeItem: public LimeReport::ItemDesignIntf
{
    Q_OBJECT
    Q_PROPERTY(ShapeType shape READ shapeType WRITE setShapeType)
    Q_PROPERTY(QColor shapeColor READ shapeColor WRITE setShapeColor)
    Q_PROPERTY(QColor shapeBrushColor READ shapeBrushColor WRITE setShapeBrushColor)
    Q_PROPERTY(Qt::BrushStyle shapeBrush READ shapeBrushType WRITE setShapeBrushType)
    Q_PROPERTY(qreal lineWidth READ lineWidth WRITE setLineWidth)
    Q_PROPERTY(Qt::PenStyle penStyle READ penStyle WRITE setPenStyle)
    Q_PROPERTY(int opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius)
public:
    enum ShapeType{HorizontalLine,VerticalLine,Ellipse,Rectangle};
#if QT_VERSION >= 0x050500
    Q_ENUM(ShapeType)
#else
    Q_ENUMS(ShapeType)
#endif
    ShapeItem(QObject *owner, QGraphicsItem *parent);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void    setShapeColor(QColor value);
    QColor  shapeColor() const {return m_shapeColor;}
    void    setShapeBrushColor(QColor value);
    QColor  shapeBrushColor() const {return m_shapeBrushColor;}
    void    setShapeBrushType(Qt::BrushStyle value);
    Qt::BrushStyle shapeBrushType() const {return m_shapeBrushType;}
    void        setShapeType(ShapeType value);
    ShapeType   shapeType() const {return m_shape;}
    void    setLineWidth(qreal value);
    qreal   lineWidth() const {return m_lineWidth;}
    Qt::PenStyle penStyle() const;
    void setPenStyle(const Qt::PenStyle &value);
    int cornerRadius() const;
    void setCornerRadius(int cornerRadius);

protected:
    BaseDesignIntf* createSameTypeItem(QObject *owner, QGraphicsItem *parent);
    bool drawDesignBorders() const {return false;}
private:
    ShapeType m_shape;
    QColor m_shapeColor;
    QColor m_shapeBrushColor;
    Qt::BrushStyle m_shapeBrushType;
    qreal m_lineWidth;
    Qt::PenStyle m_penStyle;
//    int m_opacity;
    int m_cornerRadius;
};

}
#endif // LRSHAPEITEM_H
