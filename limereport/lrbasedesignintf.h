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
#ifndef LRBASEDESIGNINTF_H
#define LRBASEDESIGNINTF_H
#include <QObject>
#include <QFrame>
#include <QGraphicsItem>
#include <QtGui>
#include <QtXml>
#include <QMenu>
#include "lrcollection.h"
#include "lrglobal.h"
#include "serializators/lrstorageintf.h"

Q_DECLARE_METATYPE(QList<QObject*>*)

namespace LimeReport {

enum ItemModes{ DesignMode=1, PreviewMode=2, PrintMode=4, EditMode=8, LayoutEditMode=16 };

class ReportEnginePrivate;
class PageDesignIntf;
class  BaseDesignIntf;

class LIMEREPORT_EXPORT Marker : public QGraphicsItem{
public:
    Marker(QGraphicsItem* parent = 0, BaseDesignIntf* owner = 0): QGraphicsItem(parent), m_owner(owner){}
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    void setRect(QRectF rect){prepareGeometryChange();m_rect=rect;}
    void setColor(QColor color){m_color=color;}
    QRectF rect() const {return m_rect;}
    virtual QColor color() const;
    BaseDesignIntf* owner() const {return m_owner;}
private:
    QRectF m_rect;
    QColor m_color;
    BaseDesignIntf* m_owner;
};

class LIMEREPORT_EXPORT SelectionMarker : public Marker{
public:
    SelectionMarker(QGraphicsItem* parent=0, BaseDesignIntf* owner = 0);
    QColor color() const;
protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
};

class DataSourceManager;
class ReportRender;

class LIMEREPORT_EXPORT BaseDesignIntf :
        public QObject, public QGraphicsItem, public ICollectionContainer, public ObjectLoadingStateIntf {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_FLAGS(BorderLines)
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometryProperty NOTIFY geometryChanged)
    Q_PROPERTY(ACollectionProperty children READ fakeCollectionReader DESIGNABLE false)
    Q_PROPERTY(qreal zOrder READ zValue WRITE setZValueProperty DESIGNABLE false)
    Q_PROPERTY(BorderLines borders READ borderLines WRITE setBorderLinesFlags)
    Q_PROPERTY(QString parentName READ parentReportItemName WRITE setParentReportItem DESIGNABLE false)
    Q_PROPERTY(qreal borderLineSize READ borderLineSize WRITE setBorderLineSize)
    Q_PROPERTY(bool isVisible READ isVisible WRITE setItemVisible DESIGNABLE false)
    Q_PROPERTY(bool shadow READ hasShadow WRITE setShadow)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
    Q_PROPERTY(bool geometryLocked READ isGeometryLocked WRITE setGeometryLocked)
    Q_PROPERTY(BorderStyle borderStyle READ borderStyle WRITE setBorderStyle)

    friend class ReportRender;
public:
    enum BGMode { TransparentMode, OpaqueMode};
    enum BorderStyle { NoStyle = Qt::NoPen,
                       Solid = Qt::SolidLine,
                       Dashed = Qt::DashLine,
                       Dot = Qt::DotLine,

                       DashDot = Qt::DashDotLine,
                       DashDotDot = Qt::DashDotDotLine,
                       Doubled = 7
                     };


    enum BrushStyle{ NoBrush,
                     SolidPattern,
                     Dense1Pattern,
                     Dense2Pattern,
                     Dense3Pattern,
                     Dense4Pattern,
                     Dense5Pattern,
                     Dense6Pattern,
                     Dense7Pattern,
                     HorPattern,
                     VerPattern,
                     CrossPattern,
                     BDiagPattern,
                     FDiagPattern };


    enum ResizeFlags { Fixed = 0,
                       ResizeLeft = 1,
                       ResizeRight = 2,
                       ResizeTop = 4,
                       ResizeBottom = 8,
                       AllDirections = 15
                     };

    enum MoveFlags  { None = 0,
                      LeftRight=1,
                      TopBotom=2,
                      All=3
                    };

    enum BorderSide {
                        NoLine = 0,
                        TopLine = 1,
                        BottomLine = 2,
                        LeftLine = 4,
                        RightLine = 8,
                        AllLines = 15
                    };

    enum ObjectState {ObjectLoading, ObjectLoaded, ObjectCreated};

    enum ItemAlign {LeftItemAlign,RightItemAlign,CenterItemAlign,ParentWidthItemAlign,DesignedItemAlign};

    enum UnitType {Millimeters, Inches};
#if QT_VERSION >= 0x050500
    Q_ENUM(BGMode)
    Q_ENUM(BrushStyle)
    Q_ENUM(BorderStyle)
    Q_ENUM(ResizeFlags)
    Q_ENUM(MoveFlags)
    Q_ENUM(BorderSide)
    Q_ENUM(ObjectState)
    Q_ENUM(ItemAlign)
    Q_ENUM(UnitType)

#else
    Q_ENUMS(BGMode)
    Q_ENUMS(BrushStyle)
    Q_ENUM(BorderStyle)
    Q_ENUMS(ResizeFlags)
    Q_ENUMS(MoveFlags)
    Q_ENUMS(BorderSide)
    Q_ENUMS(ObjectState)
    Q_ENUMS(ItemAlign)
    Q_ENUMS(UnitType)

#endif
//    enum ExpandType {EscapeSymbols, NoEscapeSymbols, ReplaceHTMLSymbols};
    Q_DECLARE_FLAGS(BorderLines, BorderSide)
    Q_DECLARE_FLAGS(ItemMode,ItemModes)
    friend class SelectionMarker;
public:
    BaseDesignIntf(const QString& storageTypeName, QObject* owner = 0, QGraphicsItem* parent = 0);
    virtual ~BaseDesignIntf();

    void    setParentReportItem(const QString& value);
    QString parentReportItemName() const;

    BrushStyle  backgroundBrushStyle() const {return m_backgroundBrushStyle;}
    BorderStyle borderStyle() const {return m_borderStyle;}
    void        setBackgroundBrushStyle(BrushStyle value);
    QColor      backgroundColor() const {return m_backgroundColor;}
    void        setBackgroundColor(QColor value);

    QPen    pen() const;
    void    setPen(QPen& pen);
    QFont   font() const;
    void    setFont(QFont& font);
    QColor  fontColor() const {return m_fontColor;}
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
    void    prepareRect(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);
    virtual QPainterPath shape() const;

    void setFixedPos(bool fixedPos);
    bool isFixedPos(){return  m_fixedPos;}
    int resizeHandleSize() const;

    qreal    unitFactor() const;
    void     setUnitType(UnitType unitType);
    UnitType unitType();
    virtual QRect  geometry() const;
    void    setGeometry(QRectF rect);

    QRectF  rect() const;
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

    virtual void setItemMode(LimeReport::BaseDesignIntf::ItemMode mode);
    ItemMode itemMode() const {return m_itemMode;}

    virtual void setBorderLinesFlags(LimeReport::BaseDesignIntf::BorderLines flags);
    void setGeometryProperty(QRect rect);
    PageDesignIntf* page();

    BorderLines borderLines() const;

    QString storageTypeName() const {return m_storageTypeName;}
    ReportEnginePrivate *reportEditor();

    virtual void updateItemSize(DataSourceManager* dataManager, RenderPass pass=FirstPass, int maxHeight=0);
    virtual bool isNeedUpdateSize(RenderPass) const;
    virtual BaseDesignIntf* cloneItem(LimeReport::BaseDesignIntf::ItemMode mode, QObject* owner=0, QGraphicsItem* parent=0);
    virtual BaseDesignIntf* cloneItemWOChild(LimeReport::BaseDesignIntf::ItemMode mode, QObject* owner=0, QGraphicsItem* parent=0);
    virtual BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0) = 0;
    virtual void initFromItem(BaseDesignIntf* source);

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

    QList<BaseDesignIntf*> childBaseItems() const;
    QList<BaseDesignIntf*> allChildBaseItems();
    BaseDesignIntf* childByName(const QString& name);

    virtual QWidget *defaultEditor();
    void notify(const QString &propertyName, const QVariant &oldValue, const QVariant &newValue);
    void notify(const QVector<QString> &propertyNames);

    int possibleResizeDirectionFlags() const;
    void setPossibleResizeDirectionFlags(int directionsFlags);
    int possibleMoveDirectionFlags() const;
    void setPossibleMoveFlags(int directionsFlags);

    int marginSize() const {return m_margin;}
    void setMarginSize(int value);

    QString itemTypeName() const;
    void setItemTypeName(const QString &itemTypeName);

    qreal borderLineSize() const;
    void setBorderStyle(BorderStyle b);
    void setBorderLineSize(qreal value);
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
    virtual bool canContainChildren() const { return false;}
    virtual bool canAcceptPaste() const{ return false;}
    ReportSettings* reportSettings() const;
    void setReportSettings(ReportSettings *reportSettings);
    void setZValueProperty(qreal value);
    QString patternName() const;
    void setPatternName(const QString &patternName);
    BaseDesignIntf* patternItem() const;
    void setPatternItem(BaseDesignIntf* patternItem);
    virtual QMap<QString, QString> getStringForTranslation();
    bool fillInSecondPass() const;
    void setFillInSecondPass(bool fillInSecondPass);
    bool isWatermark() const;
    virtual void setWatermark(bool watermark);
    void updateSelectionMarker();
    void turnOnSelectionMarker(bool value);
    bool fillTransparentInDesignMode() const;
    void setFillTransparentInDesignMode(bool fillTransparentInDesignMode);
    void emitPosChanged(QPointF oldPos, QPointF newPos);
    void emitObjectNamePropertyChanged(const QString& oldName, const QString& newName);
    bool isGeometryLocked() const;
    void setGeometryLocked(bool itemLocked);
    bool isChangingPos() const;
    void setIsChangingPos(bool isChangingPos);
    bool isShapeItem() const;
    bool hasShadow();
    void setShadow(bool sh);
    Q_INVOKABLE QString setItemWidth(qreal width);
    Q_INVOKABLE QString setItemHeight(qreal height);
    Q_INVOKABLE qreal getItemWidth();
    Q_INVOKABLE qreal getItemHeight();
    Q_INVOKABLE qreal getItemPosX();
    Q_INVOKABLE qreal getItemPosY();
    Q_INVOKABLE qreal getAbsolutePosX();
    Q_INVOKABLE qreal getAbsolutePosY();
    Q_INVOKABLE QString setItemPosX(qreal xValue);
    Q_INVOKABLE QString setItemPosY(qreal yValue);

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
    void  hoverEnterEvent(QGraphicsSceneHoverEvent* );
    void  mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void  mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void  mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    void  contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    virtual void geometryChangedEvent(QRectF newRect, QRectF oldRect);
    virtual QPen borderPen(BorderSide side) const;
    virtual QColor selectionColor() const;
    virtual void initFlags();
    virtual void initMode(LimeReport::BaseDesignIntf::ItemMode mode);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void childAddedEvent(BaseDesignIntf* child);
    virtual void parentChangedEvent(BaseDesignIntf*);
    void restoreLinks();
    virtual void restoreLinksEvent(){}

    void drawTopLine(QPainter *painter, QRectF rect) const;
    void drawBootomLine(QPainter *painter, QRectF rect) const;
    void drawRightLine(QPainter *painter, QRectF rect) const;
    void drawLeftLine(QPainter *painter, QRectF rect) const;


    void drawBorder(QPainter* painter, QRectF rect) const;
    void drawShadow(QPainter* painter, QRectF rect, qreal shadowSize) const;
    void drawDesignModeBorder(QPainter* painter, QRectF rect) const;
    void drawRenderModeBorder(QPainter *painter, QRectF rect) const;
    void drawResizeZone(QPainter*);
    void drawMarker(QPainter* painter, QColor color) const;
    void drawPinArea(QPainter* painter) const;

    void initResizeZones();
    void invalidateRect(const QRectF &rect);
    void invalidateRects(QVector<QRectF*> rects);
    QFont transformToSceneFont(const QFont &value) const;

    RenderPass currentRenderPass(){return m_currentPass;}

    virtual bool drawDesignBorders() const {return true;}
    virtual QColor selectionMarkerColor(){ return Const::SELECTION_COLOR;}

    QString expandUserVariables(QString context, RenderPass pass, ExpandType expandType, DataSourceManager *dataManager);
    QString expandDataFields(QString context, ExpandType expandType, DataSourceManager *dataManager);
    QString expandScripts(QString context, DataSourceManager *dataManager);

    QVariant m_varValue;

    virtual void preparePopUpMenu(QMenu& menu){Q_UNUSED(menu)}
    virtual void processPopUpAction(QAction* action);

    void addChildItems(QList<BaseDesignIntf*>* list);
    qreal calcAbsolutePosY(qreal currentOffset, BaseDesignIntf* item);
    qreal calcAbsolutePosX(qreal currentOffset, BaseDesignIntf* item);

    QWidget* findRootWidget(QWidget* widget);
    void showDialog(QWidget *widget);

private:
    int resizeDirectionFlags(QPointF position);
    void moveSelectedItems(QPointF delta);
    Qt::CursorShape getPossibleCursor(int cursorFlags);
    void updatePossibleDirectionFlags();

private slots:
    void onChangeGeometryTimeOut();

private:
    QPointF m_startPos;
    int     m_resizeHandleSize;
    int     m_selectionPenSize;
    int     m_possibleResizeDirectionFlags;
    int     m_possibleMoveDirectionFlags;
    int     m_savedPossibleResizeDirectionFlags;
    int     m_savedPossibleMoveDirectionFlags;
    int     m_savedFixedPos;
    int     m_resizeDirectionFlags;
    qreal   m_width;
    qreal   m_height;
    QPen    m_pen;
    QFont   m_font;
    QColor  m_fontColor;
    bool    m_fixedPos;
    qreal     m_borderLineSize;


    QRectF  m_rect;
    mutable QRectF  m_boundingRect;

    QRectF  m_oldGeometry;
    BGMode  m_BGMode;
    int     m_opacity;
    BorderLines m_borderLinesFlags;
    BorderStyle m_borderStyle;

    QRectF m_bottomRect;
    QRectF m_topRect;
    QRectF m_leftRect;
    QRectF m_rightRect;

    QVector<QRectF*> m_resizeAreas;
    QString m_storageTypeName;
    ItemMode m_itemMode;

    ObjectState m_objectState;

    BrushStyle  m_backgroundBrushStyle;
    QColor      m_backgroundColor;

    RenderPass m_currentPass;
    int     m_margin;
    QString m_itemTypeName;
    ItemAlign m_itemAlign;
    bool    m_changingItemAlign;
    QColor  m_borderColor;
    ReportSettings* m_reportSettings;
    QString m_patternName;
    BaseDesignIntf* m_patternItem;
    bool    m_fillInSecondPass;
    bool    m_watermark;
    bool    m_hovered;
    bool    m_joinMarkerOn;
    SelectionMarker* m_selectionMarker;
    Marker*          m_joinMarker;
    bool     m_fillTransparentInDesignMode;
    QRect    m_itemGeometry;
    UnitType m_unitType;
    bool     m_itemGeometryLocked;
    bool     m_isChangingPos;
    bool     m_isMoveable;
    bool    m_shadow;

signals:
    void geometryChanged(QObject* object, QRectF newGeometry, QRectF oldGeometry);
    void posChanging(QObject* object, QPointF newPos, QPointF oldPos);
    void posChanged(QObject* object, QPointF newPos, QPointF oldPos);
    void itemSelected(LimeReport::BaseDesignIntf *item);
    void itemSelectedHasBeenChanged(BaseDesignIntf *item, bool value);
    void loadCollectionFinished(const QString& collectionName);
    void objectLoaded(QObject* object);
    void objectChanged(QObject* object);
    void propertyChanged(const QString& propertName, const QVariant& oldValue,const QVariant& newValue);
    void propertyObjectNameChanged(const QString& oldValue, const QString& newValue);
    void propertyesChanged(QVector<QString> propertyNames);
    void itemAlignChanged(BaseDesignIntf* item, const ItemAlign& oldValue, const ItemAlign& newValue);
    void itemVisibleHasChanged(BaseDesignIntf* item);
    void beforeRender();
    void afterData();
    void afterRender();
};

class LIMEREPORT_EXPORT BookmarkContainerDesignIntf: public BaseDesignIntf{
    Q_OBJECT
public:
    BookmarkContainerDesignIntf(const QString& storageTypeName, QObject* owner = 0, QGraphicsItem* parent = 0)
        :BaseDesignIntf(storageTypeName, owner, parent){}
    void addBookmark(const QString& key, const QVariant& value){ m_bookmarks.insert(key, value);}
    QList<QString> bookmarks(){ return m_bookmarks.keys();}
    QVariant getBookMark(const QString& key);
    void copyBookmarks(BookmarkContainerDesignIntf* source);
private:
    QHash<QString,QVariant>  m_bookmarks;
};

} //namespace LimeReport

#endif // LRBASEDESIGNINTF_H
