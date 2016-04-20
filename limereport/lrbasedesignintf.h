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
#ifndef LRBASEDESIGNINTF_H
#define LRBASEDESIGNINTF_H
#include <QObject>
#include <QFrame>
#include <QGraphicsItem>
#include <QtGui>
#include <QtXml>
#include "lrcollection.h"
#include "lrglobal.h"
#include "serializators/lrstorageintf.h"

Q_DECLARE_METATYPE(QList<QObject*>*)

namespace LimeReport {

enum ItemModes{ DesignMode=1, PreviewMode=2, PrintMode=4, EditMode=8, LayoutEditMode=16 };

class ReportEnginePrivate;
class PageDesignIntf;
class  BaseDesignIntf;

class Marker : public QGraphicsItem{
public:
    Marker(QGraphicsItem* parent=0):QGraphicsItem(parent){}
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    void setRect(QRectF rect){prepareGeometryChange();m_rect=rect;}
    void setColor(QColor color){m_color=color;}
    QRectF rect() const;
    QColor color() const;
    BaseDesignIntf *object() const;
private:
    QRectF m_rect;
    QColor m_color;
    BaseDesignIntf* m_object;
};

class SelectionMarker : public Marker{
public:
    SelectionMarker(QGraphicsItem* parent=0);
protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
};

class DataSourceManager;

class  BaseDesignIntf :
        public QObject, public QGraphicsItem, public ICollectionContainer, public ObjectLoadingStateIntf {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_ENUMS(BGMode)
    Q_ENUMS(Qt::BrushStyle)
    Q_ENUMS(BrushMode)
    Q_ENUMS(ItemAlign)
    Q_FLAGS(BorderLines)
    Q_PROPERTY(QRectF geometry READ geometry WRITE setGeometryProperty NOTIFY geometryChanged)
    Q_PROPERTY(ACollectionProperty children READ fakeCollectionReader DESIGNABLE false)
    Q_PROPERTY(qreal zOrder READ zValue WRITE setZValueProperty DESIGNABLE false)
    Q_PROPERTY(BorderLines borders READ borderLines WRITE setBorderLinesFlags)
    Q_PROPERTY(QString parentName READ parentReportItemName WRITE setParentReportItem DESIGNABLE false)
    Q_PROPERTY(int borderLineSize READ borderLineSize WRITE setBorderLineSize)
    Q_PROPERTY(bool isVisible READ isVisible WRITE setItemVisible DESIGNABLE false)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)

public:
    enum BGMode { TransparentMode,OpaqueMode};
    enum BrushMode{Solid,None};
    enum ResizeFlags { Fixed = 0,
                       ResizeLeft = 1,
                       ResizeRight = 2,
                       ResizeTop = 4,
                       ResizeBottom = 8,
                       AllDirections = 15
                     };
    enum MoveFlags  { LeftRight=1,
                      TopBotom=2,
                      All=3
                    };
    enum BorderSide {  TopLine = 1,
                       BottomLine = 2,
                       LeftLine = 4,
                       RightLine = 8
                    };
    enum ObjectState {ObjectLoading, ObjectLoaded, ObjectCreated};
    enum ItemAlign {LeftItemAlign,RightItemAlign,CenterItemAlign,ParentWidthItemAlign,DesignedItemAlign};
    Q_DECLARE_FLAGS(BorderLines, BorderSide)
    Q_DECLARE_FLAGS(ItemMode,ItemModes)
    friend class SelectionMarker;
public:
    BaseDesignIntf(const QString& storageTypeName, QObject* owner = 0, QGraphicsItem* parent = 0);
    virtual ~BaseDesignIntf();

    void setParentReportItem(const QString& value);
    QString parentReportItemName();

    BrushMode backgroundBrushMode(){return m_backgroundBrush;}
    void setBackgroundBrushMode(BrushMode value);
    QColor backgroundColor(){return m_backgroundBrushcolor;}
    void setBackgroundColor(QColor value);

    QPen    pen() const;
    void    setPen(QPen& pen);
    QFont   font() const;
    void    setFont(QFont& font);
    QColor  fontColor(){return m_fontColor;}
    void    setFontColor(QColor value){m_fontColor=value;}

    virtual BGMode  backgroundMode() const;
    virtual void    setBackgroundMode(BGMode bgMode);
    virtual qreal   width() const;
    virtual qreal   widthMM() const;
    virtual void    setWidth(qreal width);
    virtual qreal   height() const;
    virtual qreal   heightMM() const;
    virtual void    setHeight(qreal height);
    virtual QPointF posMM() const;
    virtual int     opacity() const;
    virtual void    setOpacity(int opacity);
    virtual void    setSize(QSizeF size);
    virtual QSizeF  size() const;
    virtual QSizeF  sizeMM() const;

    void    paint(QPainter* ppainter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    void    prepareRect(QPainter* ppainter, const QStyleOptionGraphicsItem*, QWidget*);
    virtual QPainterPath shape() const;

    void setFixedPos(bool fixedPos);
    int resizeHandleSize() const;

    void    setMMFactor(qreal mmFactor);
    qreal   mmFactor() const;
    virtual QRectF  geometry() const;
    void    setGeometry(QRectF rect);

    QRectF rect()const;
    void    setupPainter(QPainter* painter) const;
    virtual QRectF boundingRect() const;

    virtual void moveRight();
    virtual void moveLeft();
    virtual void moveDown();
    virtual void moveUp();

    virtual void sizeRight();
    virtual void sizeLeft();
    virtual void sizeUp();
    virtual void sizeDown();

    void setItemPos(const QPointF &newPos);
    void setItemPos(qreal x, qreal y);

    void setItemMode(LimeReport::BaseDesignIntf::ItemMode mode);
    ItemMode itemMode() const {return m_itemMode;}

    virtual void setBorderLinesFlags(LimeReport::BaseDesignIntf::BorderLines flags);
    void setGeometryProperty(QRectF rect);
    PageDesignIntf* page();

    BorderLines borderLines() const;
    QString storageTypeName() const {return m_storageTypeName;}
    ReportEnginePrivate *reportEditor();

    virtual void updateItemSize(DataSourceManager* dataManager, RenderPass pass=FirstPass, int maxHeight=0);
    virtual bool isNeedUpdateSize(RenderPass) const;
    virtual BaseDesignIntf* cloneItem(LimeReport::BaseDesignIntf::ItemMode mode, QObject* owner=0, QGraphicsItem* parent=0);
    virtual BaseDesignIntf* cloneItemWOChild(LimeReport::BaseDesignIntf::ItemMode mode, QObject* owner=0, QGraphicsItem* parent=0);
    virtual BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0) = 0;
    void    initFromItem(BaseDesignIntf* source);

    virtual bool canBeSplitted(int height) const;
    virtual qreal minHeight() const {return 0;}
    virtual bool isSplittable() const {return false;}
    virtual bool isEmpty() const;
    virtual BaseDesignIntf* cloneUpperPart(int height, QObject* owner=0, QGraphicsItem* parent=0);
    virtual BaseDesignIntf* cloneBottomPart(int height, QObject* owner=0, QGraphicsItem* parent=0);
    virtual BaseDesignIntf* cloneEmpty(int height, QObject* owner=0, QGraphicsItem* parent=0);

    bool isLoaded(){return m_objectState==ObjectLoaded;}
    bool isLoading(){return m_objectState==ObjectLoading;}
    void objectLoadStarted();
    void objectLoadFinished();
    virtual void parentObjectLoadFinished();
    virtual void beforeDelete();

    QList<BaseDesignIntf*> childBaseItems();
    BaseDesignIntf* childByName(const QString& name);

    virtual QWidget *defaultEditor();
    void notify(const QString &propertyName, const QVariant &oldValue, const QVariant &newValue);
    void notify(const QVector<QString> &propertyNames);

    int posibleResizeDirectionFlags() const;
    void setPosibleResizeDirectionFlags(int directionsFlags);
    int posibleMoveDirectionFlags() const;
    void setPosibleMoveFlags(int directionsFlags);

    int marginSize() const {return m_margin;}
    void setMarginSize(int value);

    QString itemTypeName() const;
    void setItemTypeName(const QString &itemTypeName);
    void emitObjectNamePropertyChanged(const QString& oldName, const QString& newName);
    int borderLineSize() const;
    void setBorderLineSize(int value);
    void showEditorDialog();
    ItemAlign itemAlign() const;
    virtual void setItemAlign(const ItemAlign &itemAlign);
    void updateItemAlign();
    QPointF modifyPosForAlignedItem(const QPointF &pos);
    void turnOnJoinMarker(bool value);
    virtual bool isBand(){return false;}
    QColor borderColor() const;
    void setBorderColor(const QColor &borderColor);
    void setItemVisible(const bool& value);
protected:

    //ICollectionContainer
    QObject* createElement(const QString&, const QString& elementType);
    int elementsCount(const QString&);
    QObject* elementAt(const QString&, int index);
    void collectionLoadFinished(const QString& collectionName);
    //ICollectionContainer

    void  mousePressEvent(QGraphicsSceneMouseEvent* event);
    void  hoverMoveEvent(QGraphicsSceneHoverEvent* event);
    void  hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    //void virtual   hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void  mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void  mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void  mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    virtual void geometryChangedEvent(QRectF newRect, QRectF oldRect);
    virtual QPen borderPen(BorderSide side) const;
    virtual QColor selectionColor() const;
    virtual void initFlags();
    virtual void initMode(LimeReport::BaseDesignIntf::ItemMode mode);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void childAddedEvent(BaseDesignIntf* child);
    virtual void parentChangedEvent(BaseDesignIntf*);

    void drawTopLine(QPainter *painter, QRectF rect) const;
    void drawBootomLine(QPainter *painter, QRectF rect) const;
    void drawRightLine(QPainter *painter, QRectF rect) const;
    void drawLeftLine(QPainter *painter, QRectF rect) const;

    void drawBorder(QPainter* painter, QRectF rect) const;
    void drawDesignModeBorder(QPainter* painter, QRectF rect) const;
    void drawRenderModeBorder(QPainter *painter, QRectF rect) const;
    void drawResizeZone(QPainter*);
    void drawSelection(QPainter* painter, QRectF) const;
    void drawPinArea(QPainter* painter) const;

    void initResizeZones();
    void invalidateRect(const QRectF &rect);
    void invalidateRects(QVector<QRectF*> rects);
    QFont transformToSceneFont(const QFont &value) const;

    RenderPass currentRenderPass(){return m_currentPass;}

    virtual bool drawDesignBorders() const {return true;}
    virtual QColor selectionMarkerColor(){ return Const::SELECTION_COLOR;}
private:
    void updateSelectionMarker();
    int resizeDirectionFlags(QPointF position);
    void moveSelectedItems(QPointF delta);
    Qt::CursorShape getPosibleCursor(int cursorFlags);
    void setZValueProperty(qreal value);
    void updatePosibleDirectionFlags();
    void turnOnSelectionMarker(bool value);
private:
    QPointF m_startPos;
    //QPointF m_startScenePos;
    int     m_resizeHandleSize;
    int     m_selectionPenSize;
    int     m_posibleResizeDirectionFlags;
    int     m_posibleMoveDirectionFlags;
    int     m_resizeDirectionFlags;
    qreal   m_width, m_height;
    QPen    m_pen;
    QFont   m_font;
    QColor  m_fontColor;
    qreal   m_mmFactor;
    bool    m_fixedPos;
    int     m_borderLineSize;

    QRectF  m_rect;
    mutable QRectF  m_boundingRect;

    QRectF  m_oldGeometry;
    BGMode  m_BGMode;
    int     m_opacity;
    BorderLines m_borderLinesFlags;

    QRectF m_bottomRect;
    QRectF m_topRect;
    QRectF m_leftRect;
    QRectF m_rightRect;

    QVector<QRectF*> m_resizeAreas;
    QFrame* m_hintFrame;
    QString m_storageTypeName;
    ItemMode m_itemMode;

    ObjectState m_objectState;
    SelectionMarker* m_selectionMarker;
    Marker* m_joinMarker;

    BrushMode m_backgroundBrush;
    QColor  m_backgroundBrushcolor;
    RenderPass m_currentPass;
    int     m_margin;
    QString m_itemTypeName;
    ItemAlign m_itemAlign;
    bool    m_changingItemAlign;
    QColor  m_borderColor;
signals:
    void geometryChanged(QObject* object, QRectF newGeometry, QRectF oldGeometry);
    void posChanged(QObject* object, QPointF newPos, QPointF oldPos);
    void itemSelected(LimeReport::BaseDesignIntf *item);
    void loadCollectionFinished(const QString& collectionName);
    void objectLoaded(QObject* object);
    void objectChanged(QObject* object);
    void propertyChanged(const QString& propertName, const QVariant& oldValue,const QVariant& newValue);
    void propertyObjectNameChanged(const QString& oldValue, const QString& newValue);
    void propertyesChanged(QVector<QString> propertyNames);
    void itemAlignChanged(BaseDesignIntf* item, const ItemAlign& oldValue, const ItemAlign& newValue);
    void itemVisibleHasChanged(BaseDesignIntf* item);
};

} //namespace LimeReport

#endif // LRBASEDESIGNINTF_H
