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
#include <QFrame>
#include "lrbasedesignintf.h"
#include "lrglobal.h"
#include "lrpagedesignintf.h"
#include "lrreportdesignwidget.h"
#include "qgraphicsitem.h"
#include "lrdesignelementsfactory.h"
#include "lrhorizontallayout.h"
#include "serializators/lrstorageintf.h"
#include "serializators/lrxmlreader.h"

#include <memory>
#include <QMetaObject>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>
#include <QMenu>
#include <QClipboard>

namespace LimeReport
{

BaseDesignIntf::BaseDesignIntf(const QString &storageTypeName, QObject *owner, QGraphicsItem *parent) :
    QObject(owner), QGraphicsItem(parent),
    m_resizeHandleSize(Const::RESIZE_HANDLE_SIZE),
    m_selectionPenSize(Const::SELECTION_PEN_SIZE),
    m_possibleResizeDirectionFlags(ResizeTop | ResizeBottom | ResizeLeft | ResizeRight),
    m_possibleMoveDirectionFlags(All),
    m_resizeDirectionFlags(0),
    m_width(200),
    m_height(50),
    m_fontColor(Qt::black),
    m_mmFactor(Const::mmFACTOR),
    m_fixedPos(false),
    m_borderLineSize(1),
    m_BGMode(OpaqueMode),
    m_opacity(100),
    m_borderLinesFlags(0),
    m_storageTypeName(storageTypeName),
    m_itemMode(DesignMode),
    m_objectState(ObjectCreated),
    m_selectionMarker(0),
    m_joinMarker(0),
    m_backgroundBrushStyle(SolidPattern),
    m_backgroundColor(Qt::white),
    m_margin(4),
    m_itemAlign(DesignedItemAlign),
    m_changingItemAlign(false),
    m_borderColor(Qt::black),
    m_reportSettings(0),
    m_patternName(""),
    m_watermark(false)
{
    setGeometry(QRectF(0, 0, m_width, m_height));
    if (BaseDesignIntf *item = dynamic_cast<BaseDesignIntf *>(parent)) {
        m_font = item->font();
    } else{
        m_font = QFont("Arial",10);
    }
    initFlags();
}

QRectF BaseDesignIntf::boundingRect() const
{
    if (m_boundingRect.isNull()) {
        qreal halfpw = pen().widthF() / 2;
        halfpw += 2;
        m_boundingRect = rect();
        m_boundingRect.adjust(-halfpw, -halfpw, halfpw, halfpw);
    };
    return m_boundingRect;
}

BaseDesignIntf::~BaseDesignIntf(void) {
    delete m_selectionMarker;
    delete m_joinMarker;
}

void BaseDesignIntf::setParentReportItem(const QString &value)
{
    if (page() && !value.isEmpty()) {
        BaseDesignIntf *p = page()->reportItemByName(value);
        if (p) {
            setParentItem(p);
            setParent(p);
        }
    }
}

QString BaseDesignIntf::parentReportItemName() const
{
    BaseDesignIntf *parent = dynamic_cast<BaseDesignIntf *>(parentItem());
    if (parent) return parent->objectName();
    else return "";
}

void BaseDesignIntf::setBackgroundBrushStyle(BrushStyle value)
{
    if ( value != m_backgroundBrushStyle  ){
        BrushStyle oldValue = m_backgroundBrushStyle;
        m_backgroundBrushStyle=value;
        if (!isLoading()) update();
        notify("backgroundBrushStyle", static_cast<int>(oldValue), static_cast<int>(value));
    }
}

void BaseDesignIntf::setBackgroundColor(QColor value)
{
    if (value != m_backgroundColor){
        QColor oldValue = m_backgroundColor;
        m_backgroundColor=value;
        if (!isLoading()) update();
        notify("backgroundColor", oldValue, value);
    }
}

QPen BaseDesignIntf::pen() const
{
    return m_pen;
}

void BaseDesignIntf::setPen(QPen &pen)
{
    m_pen = pen;
    update();
}

QFont BaseDesignIntf::font() const
{
    return m_font;
}

void BaseDesignIntf::setFont(QFont &font)
{
    m_font = font;
    update();
}

qreal BaseDesignIntf::width() const
{
    return rect().width();
}

void BaseDesignIntf::setWidth(qreal width)
{
    setGeometry(QRectF(rect().x(), rect().y(), width, rect().height()));
    if  (!m_changingItemAlign)
        updateItemAlign();
}

QString BaseDesignIntf::setItemWidth(qreal width)
{
    setWidth(width * mmFactor());
    return QString();
}

qreal BaseDesignIntf::height() const
{
    return rect().height();
}

QRectF BaseDesignIntf::geometry() const
{
    return QRectF(pos().x(), pos().y(), width(), height());
}

void BaseDesignIntf::setHeight(qreal height)
{
    setGeometry(QRectF(rect().x(), rect().y(), rect().width(), height));
}

QString BaseDesignIntf::setItemHeight(qreal height)
{
    setHeight(height * mmFactor());
    return QString();
}

qreal BaseDesignIntf::getItemWidth()
{
    return width() / mmFactor();
}

qreal BaseDesignIntf::getItemHeight()
{
    return height() / mmFactor();
}

qreal BaseDesignIntf::getItemPosX()
{
    return x() / mmFactor();
}

qreal BaseDesignIntf::getItemPosY()
{
    return y() / mmFactor();
}

QString BaseDesignIntf::setItemPosX(qreal xValue)
{
    setItemPos(xValue * mmFactor(),y());
    return QString();
}

QString BaseDesignIntf::setItemPosY(qreal yValue)
{
    setItemPos(x(),yValue * mmFactor());
    return QString();
}

QFont BaseDesignIntf::transformToSceneFont(const QFont& value) const
{
    QFont f = value;
    f.setPixelSize(f.pointSize()*Const::fontFACTOR);
    return f;
}

QString BaseDesignIntf::expandDataFields(QString context, ExpandType expandType, DataSourceManager* dataManager)
{
    ScriptEngineManager& sm = ScriptEngineManager::instance();
    if (sm.dataManager() != dataManager) sm.setDataManager(dataManager);
    return sm.expandDataFields(context, expandType, m_varValue, this);
}

QString BaseDesignIntf::expandUserVariables(QString context, RenderPass pass, ExpandType expandType, DataSourceManager* dataManager)
{

    ScriptEngineManager& sm = ScriptEngineManager::instance();
    if (sm.dataManager() != dataManager) sm.setDataManager(dataManager);
    return sm.expandUserVariables(context, pass, expandType, m_varValue);

}

QString BaseDesignIntf::expandScripts(QString context, DataSourceManager* dataManager)
{

    ScriptEngineManager& sm = ScriptEngineManager::instance();
    if (sm.dataManager() != dataManager) sm.setDataManager(dataManager);
    return sm.expandScripts(context,m_varValue,this);

}

void BaseDesignIntf::setupPainter(QPainter *painter) const
{
    if (!painter) {
        return;
    }
    painter->setFont(m_font);
    painter->setPen(m_fontColor);
}

BaseDesignIntf::BGMode BaseDesignIntf::backgroundMode() const
{
    return m_BGMode;
}
void BaseDesignIntf::setBackgroundMode(BGMode bgMode)
{
    m_BGMode = bgMode;
    update(boundingRect());
}

int BaseDesignIntf::opacity() const
{
    return m_opacity;
}

void BaseDesignIntf::setOpacity(int opacity)
{
    if (m_opacity!=opacity){
        if (opacity < 0) {
            m_opacity = 0;
        }
        else if (opacity > 100) {
            m_opacity = 100;
        }
        else {
            m_opacity =  opacity;
        }
        update();
    }
}

void BaseDesignIntf::setSize(QSizeF size)
{
    setWidth(size.width());
    setHeight(size.height());
}

QSizeF BaseDesignIntf::size() const
{
    return QSizeF(width(), height());
}

QSizeF BaseDesignIntf::sizeMM() const
{
    return QSizeF(width() / m_mmFactor, height() / m_mmFactor);
}

qreal BaseDesignIntf::widthMM() const
{
    return width() / m_mmFactor;
}

qreal BaseDesignIntf::heightMM() const
{
    return height() / m_mmFactor;
}

void BaseDesignIntf::setMMFactor(qreal mmFactor)
{
    m_mmFactor = mmFactor;
}

qreal BaseDesignIntf::mmFactor() const
{
    return m_mmFactor;
}

QPointF BaseDesignIntf::posMM() const
{
    return QPointF(pos().x() / m_mmFactor, pos().y() / m_mmFactor);
}

QRectF BaseDesignIntf::rect() const
{
    return m_rect;
}

void BaseDesignIntf::setFixedPos(bool fixedPos)
{
    m_fixedPos = fixedPos;
}

void BaseDesignIntf::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_resizeDirectionFlags = resizeDirectionFlags(event->pos());
        //m_startScenePos = event->scenePos();
        m_startPos = pos();
        m_oldGeometry = geometry();
        QGraphicsItem::mousePressEvent(event);
        //QApplication::processEvents();
        emit(itemSelected(this));
    }
    else QGraphicsItem::mousePressEvent(event);
}

void BaseDesignIntf::paint(QPainter *ppainter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    setupPainter(ppainter);
    drawBorder(ppainter, rect());
    if (isSelected()) {drawSelection(ppainter, rect());}
    drawResizeZone(ppainter);
}

QColor calcColor(QColor color){

    int R = color.red();
    int G = color.green();
    int B = color.blue();

    if (0.222*R + 0.707*G + 0.071*B <= 127)
      return Qt::white;
    else
      return Qt::black;
}

void BaseDesignIntf::prepareRect(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    painter->save();

    QRectF r = rect().adjusted(0, 0, borderLineSize(), borderLineSize());
    QBrush brush(m_backgroundColor,static_cast<Qt::BrushStyle>(m_backgroundBrushStyle));
    brush.setTransform(painter->worldTransform().inverted());

    if (isSelected() && (opacity() == 100) && (m_BGMode!=TransparentMode)) {
        painter->fillRect(r, brush);
    }
    else {
        if (m_BGMode == OpaqueMode) {
            qreal o = (itemMode() & DesignMode) ? 0.5 : qreal(m_opacity) / 100;
            painter->setOpacity(o);
            painter->fillRect(r, brush);
        } else if (itemMode() & DesignMode){
            painter->setOpacity(0.1);
            painter->fillRect(r, QBrush(QPixmap(":/report/images/empty")));
        }
    }
    painter->restore();
}

void BaseDesignIntf::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (page()) {
        if (!page()->isItemInsertMode() && isSelected()) {
            if (m_resizeDirectionFlags != resizeDirectionFlags(event->pos())) {
                m_resizeDirectionFlags = resizeDirectionFlags(event->pos());
                QVector<QRectF *>oldResizeAreas(m_resizeAreas);
                initResizeZones();
                invalidateRects(oldResizeAreas);
                invalidateRects(m_resizeAreas);
                switch (m_resizeDirectionFlags) {
                case ResizeRight:
                case ResizeLeft:
                    setCursor(Qt::SizeHorCursor);
                break;
                case ResizeBottom:
                case ResizeTop:
                    setCursor(Qt::SizeVerCursor);
                break;
                case ResizeRight | ResizeBottom:
                case ResizeLeft  | ResizeTop:
                    setCursor(Qt::SizeFDiagCursor);
                break;
                case ResizeLeft  | ResizeBottom:
                case ResizeRight | ResizeTop:
                    setCursor(Qt::SizeBDiagCursor);
                break;
                default:
                    setCursor(Qt::ArrowCursor);
                break;
                }
            }
        }
    }
}

void BaseDesignIntf::invalidateRects(QVector<QRectF *> rects)
{
    foreach(QRectF * rect, rects)
      scene()->update(mapToScene(*rect).boundingRect());
}

void BaseDesignIntf::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    setCursor(QCursor(Qt::ArrowCursor));
    m_resizeDirectionFlags = 0;
    scene()->update(sceneBoundingRect());
    m_resizeAreas.clear();
}


void BaseDesignIntf::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!isSelected()){
        QGraphicsItem::mouseMoveEvent(event);
        return;
    }

    int hStep = dynamic_cast<PageDesignIntf*>(scene())->horizontalGridStep();
    int vStep = dynamic_cast<PageDesignIntf*>(scene())->verticalGridStep();

    if (m_resizeDirectionFlags & ResizeLeft) {
        if ((event->scenePos().x()) <= (mapToScene(0, 0).x() + (width() - Const::MINIMUM_ITEM_WIDTH)) &&
             (width() + (event->lastScenePos().x() - event->scenePos().x()) > Const::MINIMUM_ITEM_WIDTH)
           ) {
            qreal posRightCorner = mapToScene(0, 0).x() + width();
            qreal posLeftCorner = div(mapToParent(event->pos()).x(), hStep).quot * hStep;
            if (posLeftCorner < 0 )
                posLeftCorner = 0;
            setItemPos(posLeftCorner, y());
            setWidth(div(posRightCorner - mapToScene(0, 0).x(), hStep).quot * hStep);
        }
    }

    if (m_resizeDirectionFlags & ResizeRight) {
        if ((event->scenePos().x() >= (mapToScene(0, 0).x() + Const::MINIMUM_ITEM_WIDTH)) ||
             (event->scenePos().x() >= (mapToScene(0, 0).x() + width()))) {
            setWidth(div(event->scenePos().x() - mapToScene(0, 0).x(), hStep).quot * hStep);
        }
    }

    if (m_resizeDirectionFlags & ResizeTop) {
        if ((event->scenePos().y()) <= (mapToScene(0, 0).y() + (height() - Const::MINIMUM_ITEM_HEIGHT)) &&
             (height() + (event->lastScenePos().y() - event->scenePos().y()) > Const::MINIMUM_ITEM_HEIGHT)
           ) {
            qreal posBottomCorner = mapToScene(0, 0).y() + height();
            qreal posTopCorner = div(mapToParent(event->pos()).y(), vStep).quot * vStep;
            if (posTopCorner < 0 )
                posTopCorner = 0;
            setItemPos(x(), posTopCorner);
            setHeight(div(posBottomCorner - mapToScene(0, 0).y(), vStep).quot * vStep);
        }
    }

    if (m_resizeDirectionFlags & ResizeBottom) {
        if ((event->scenePos().y() > (mapToScene(0, 0).y() + height())) ||
             (event->scenePos().y() > (mapToScene(0, 0).y() + Const::MINIMUM_ITEM_HEIGHT))
           ) {
            setHeight(div(event->scenePos().y() - mapToScene(0, 0).y(), vStep).quot * vStep);
        }
    }

    if ((m_resizeDirectionFlags == Fixed) && !m_fixedPos) {

        QPointF tmpPos = pos();
        QPointF delta;
        switch (possibleMoveDirectionFlags()) {
        case LeftRight: {
            delta = QPoint(div(event->buttonDownScenePos(Qt::LeftButton).x() - event->scenePos().x(), hStep).quot * hStep, 0);
            break;
        }
        case TopBotom: {
            delta = QPoint(0, div(event->buttonDownScenePos(Qt::LeftButton).y() - event->scenePos().y(), vStep).quot * vStep);
            break;
        }
        case All: {
            delta = QPoint(div(event->buttonDownScenePos(Qt::LeftButton).x() - event->scenePos().x(), hStep).quot * hStep,
                           div(event->buttonDownScenePos(Qt::LeftButton).y() - event->scenePos().y(), vStep).quot * vStep);
            break;
        }
        };

        setItemPos(QPointF(div(m_startPos.x(), hStep).quot * hStep, div(m_startPos.y(), vStep).quot * vStep) - delta);

        if (page()){
            if (!isBand() && page()->selectedItems().count()>1)
                moveSelectedItems(tmpPos - pos());
            if (page()->selectedItems().count()==1 && (page()->magneticMovement()))
                page()->itemMoved(this);
        }
    }
}

int BaseDesignIntf::possibleResizeDirectionFlags() const
{
    return m_possibleResizeDirectionFlags;
}

int BaseDesignIntf::resizeHandleSize() const
{
    return m_resizeHandleSize;
}

int BaseDesignIntf::resizeDirectionFlags(QPointF position)
{
    int flags = Fixed;

    if (possibleResizeDirectionFlags()&ResizeTop && m_topRect.contains(position)) {
        flags |= ResizeTop;
    }

    if (possibleResizeDirectionFlags()&ResizeLeft && m_leftRect.contains(position)) {
        flags |= ResizeLeft;
    }

    if (possibleResizeDirectionFlags()&ResizeBottom && m_bottomRect.contains(position)) {
        flags |= ResizeBottom;
    }

    if (possibleResizeDirectionFlags()&ResizeRight && m_rightRect.contains(position)) {
        flags |= ResizeRight;
    }

    return flags;
}

Qt::CursorShape BaseDesignIntf::getPossibleCursor(int cursorFlags)
{

    if ((cursorFlags == Fixed) || (scene()->selectedItems().count() > 1)) return Qt::ArrowCursor;

    if (((cursorFlags & ResizeRight) && (cursorFlags & ResizeTop)) ||
         ((cursorFlags & ResizeLeft) && (cursorFlags & ResizeBottom))) {
        return Qt::SizeBDiagCursor;
    }
    if (((cursorFlags & ResizeLeft) && (cursorFlags & ResizeTop)) ||
         ((cursorFlags & ResizeRight) && (cursorFlags & ResizeBottom))) {
        return Qt::SizeFDiagCursor;
    }
    if ((cursorFlags & ResizeLeft) || (cursorFlags & ResizeRight)) { return Qt::SizeHorCursor; }
    if ((cursorFlags & ResizeBottom) || (cursorFlags & ResizeTop)) { return Qt::SizeVerCursor; }

    return Qt::ArrowCursor;
}

void BaseDesignIntf::setZValueProperty(qreal value)
{
    if (zValue()!=value){
        qreal oldValue = zValue();
        setZValue(value);
        notify("zOrder",oldValue,value);
    }
}

BaseDesignIntf::ItemAlign BaseDesignIntf::itemAlign() const
{
    return m_itemAlign;
}

QPointF BaseDesignIntf::modifyPosForAlignedItem(const QPointF& pos){
    QPointF result = pos;
    BaseDesignIntf* parent = dynamic_cast<BaseDesignIntf*>(parentItem());
    PageItemDesignIntf* parentPage = dynamic_cast<PageItemDesignIntf*>(parentItem());
    if (parent){
        qreal leftBorder = parentPage?parentPage->leftMargin()*mmFactor():0;
        qreal rightBorder = parentPage?parentPage->rightMargin()*mmFactor():0;
        qreal avaibleSpace = parent->width()-(leftBorder+rightBorder);

        switch(m_itemAlign){
        case LeftItemAlign:
            result.setX(leftBorder);
            break;
        case RightItemAlign:
            result.setX(parent->width() - (rightBorder + width()));
            break;
        case CenterItemAlign:
            result.setX((avaibleSpace-width()) / 2 + leftBorder);
            break;
        case ParentWidthItemAlign:
            result.setX(leftBorder);
        case DesignedItemAlign:
           break;
        }
    }
    return result;
}

void BaseDesignIntf::turnOnJoinMarker(bool value)
{
    if (value){
        m_joinMarker = new Marker(this);
        m_joinMarker->setColor(Const::JOIN_COLOR);
        m_joinMarker->setRect(rect());
        m_joinMarker->setVisible(true);
    } else {
        delete m_joinMarker;
        m_joinMarker = 0;
    }
}

void BaseDesignIntf::updateItemAlign(){
    BaseDesignIntf* parent = dynamic_cast<BaseDesignIntf*>(parentItem());
    PageItemDesignIntf* parentPage = dynamic_cast<PageItemDesignIntf*>(parentItem());
    m_changingItemAlign = true;
    if (parent){
        qreal leftBorder = parentPage?parentPage->leftMargin()*mmFactor():0;
        qreal rightBorder = parentPage?parentPage->rightMargin()*mmFactor():0;
        qreal aviableSpace = parent->width()-(leftBorder+rightBorder);

        setPos(modifyPosForAlignedItem(pos()));
        if (m_itemAlign == ParentWidthItemAlign)
            setWidth(aviableSpace);
    }
    m_changingItemAlign = false;
}

void BaseDesignIntf::updatePossibleDirectionFlags(){
    setPossibleResizeDirectionFlags(AllDirections);
    switch(m_itemAlign){
    case LeftItemAlign:
        setPossibleResizeDirectionFlags(AllDirections^ResizeLeft);
        break;
    case RightItemAlign:
        setPossibleResizeDirectionFlags(AllDirections^ResizeRight);
        break;
    case ParentWidthItemAlign:
        setPossibleResizeDirectionFlags(ResizeBottom|ResizeTop);
    case CenterItemAlign:
    case DesignedItemAlign:
       break;
    }
}

void BaseDesignIntf::turnOnSelectionMarker(bool value)
{
    if (value && !m_selectionMarker){
        m_selectionMarker = new SelectionMarker(this);
        m_selectionMarker->setColor(selectionMarkerColor());
        updateSelectionMarker();
        m_selectionMarker->setVisible(true);
    } else {
        delete m_selectionMarker;
        m_selectionMarker = 0;
    }
}

bool BaseDesignIntf::isWatermark() const
{
    return m_watermark;
}

void BaseDesignIntf::setWatermark(bool watermark)
{
    if (m_watermark != watermark){
        m_watermark = watermark;
        notify("watermark",!watermark,watermark);
    }
}

QString BaseDesignIntf::patternName() const
{
    return (m_patternName.isEmpty()) ? objectName() : m_patternName;
}

void BaseDesignIntf::setPatternName(const QString &patternName)
{
    m_patternName = patternName;
}

ReportSettings *BaseDesignIntf::reportSettings() const
{
    return m_reportSettings;
}

void BaseDesignIntf::setReportSettings(ReportSettings *reportSettings)
{
    m_reportSettings = reportSettings;
    foreach(BaseDesignIntf* child, childBaseItems()){
        child->setReportSettings(reportSettings);
    }
}

QColor BaseDesignIntf::borderColor() const
{
    return m_borderColor;
}

void BaseDesignIntf::setBorderColor(const QColor &borderColor)
{
    if (m_borderColor != borderColor){
        QColor oldValue = m_borderColor;
        m_borderColor = borderColor;
        notify("borderColor",oldValue,borderColor);
        update();
    }
}

void BaseDesignIntf::setItemVisible(const bool &value)
{
    if (isVisible()!=value){
        setVisible(value);
        emit itemVisibleHasChanged(this);
    }
}

void BaseDesignIntf::setItemAlign(const ItemAlign &itemAlign)
{
    if (m_itemAlign != itemAlign){
        ItemAlign oldValue = m_itemAlign;
        m_itemAlign = itemAlign;
        notify("itemAlign",oldValue,itemAlign);
        updatePossibleDirectionFlags();
        updateItemAlign();
        emit itemAlignChanged(this, oldValue, itemAlign);
    }
}

QString BaseDesignIntf::itemTypeName() const
{
    return m_itemTypeName;
}

void BaseDesignIntf::setItemTypeName(const QString &itemTypeName)
{
    m_itemTypeName = itemTypeName;
}

void BaseDesignIntf::emitObjectNamePropertyChanged(const QString &oldName, const QString &newName)
{
    emit propertyObjectNameChanged(oldName,newName);
}

int BaseDesignIntf::borderLineSize() const
{
    return m_borderLineSize;
}

void BaseDesignIntf::setBorderLineSize(int value)
{
    int oldValue = m_borderLineSize;
    m_borderLineSize = value;
    update();
    notify("borderLineSize",oldValue,value);
}


void BaseDesignIntf::moveRight()
{
    if (!m_fixedPos && page()) setItemPos(pos().x() + page()->horizontalGridStep(), pos().y());
}

void BaseDesignIntf::moveLeft()
{
    if (!m_fixedPos && page()) setItemPos(pos().x() - page()->horizontalGridStep(), pos().y());
}

void BaseDesignIntf::moveDown()
{
    if (!m_fixedPos && page()) setItemPos(pos().x(), pos().y() + page()->verticalGridStep());
}

void BaseDesignIntf::moveUp()
{
    if (!m_fixedPos && page()) setItemPos(pos().x(), pos().y() - page()->verticalGridStep());
}

void BaseDesignIntf::sizeRight()
{
    if ((m_possibleResizeDirectionFlags & ResizeLeft) ||
         (m_possibleResizeDirectionFlags & ResizeRight)) {
        if (page()) setWidth(width() + page()->horizontalGridStep());
    }
}

void BaseDesignIntf::sizeLeft()
{
    if ((m_possibleResizeDirectionFlags & ResizeLeft) ||
         (m_possibleResizeDirectionFlags & ResizeRight)) {
        if(page()) setWidth(width() - page()->horizontalGridStep());
    }
}

void BaseDesignIntf::sizeUp()
{
    if ((m_possibleResizeDirectionFlags & ResizeTop) ||
         (m_possibleResizeDirectionFlags & ResizeBottom)) {
        if (page()) setHeight(height() - page()->verticalGridStep());
    }
}

void BaseDesignIntf::sizeDown()
{
    if ((m_possibleResizeDirectionFlags & ResizeTop) ||
         (m_possibleResizeDirectionFlags & ResizeBottom)) {
        if (page()) setHeight(height() + page()->verticalGridStep());
    }
}

void BaseDesignIntf::setBorderLinesFlags(BorderLines flags)
{
    if (m_borderLinesFlags!=flags){
        BorderLines oldValue = m_borderLinesFlags;
        m_borderLinesFlags = flags;
        if (!isLoading()) {
            update(rect());
            notify("borders",QVariant(oldValue),QVariant(flags));
        }
    }
}

BaseDesignIntf::BorderLines BaseDesignIntf::borderLines() const
{
    return m_borderLinesFlags;
}

void BaseDesignIntf::drawTopLine(QPainter *painter, QRectF rect) const
{
    painter->setPen(borderPen(TopLine));
    painter->drawLine(rect.x(), rect.y(), rect.width(), rect.y());
}

void BaseDesignIntf::drawBootomLine(QPainter *painter, QRectF rect) const
{
    painter->setPen(borderPen(BottomLine));
    painter->drawLine(rect.x(), rect.height(), rect.width(), rect.height());
}

void BaseDesignIntf::drawRightLine(QPainter *painter, QRectF rect) const
{
    painter->setPen(borderPen(RightLine));
    painter->drawLine(rect.width(), rect.y(), rect.width(), rect.height());
}

void BaseDesignIntf::drawLeftLine(QPainter *painter, QRectF rect) const
{
    painter->setPen(borderPen(LeftLine));
    painter->drawLine(rect.x(), rect.y(), rect.x(), rect.height());
}

void BaseDesignIntf::drawDesignModeBorder(QPainter *painter, QRectF rect) const
{
    drawTopLine(painter, rect);
    drawBootomLine(painter, rect);
    drawLeftLine(painter, rect);
    drawRightLine(painter, rect);
}

void BaseDesignIntf::drawRenderModeBorder(QPainter *painter, QRectF rect) const
{

    if (m_borderLinesFlags & RightLine)  drawRightLine(painter, rect);
    if (m_borderLinesFlags & LeftLine)   drawLeftLine(painter, rect);
    if (m_borderLinesFlags & TopLine )   drawTopLine(painter, rect);
    if (m_borderLinesFlags & BottomLine) drawBootomLine(painter, rect);
}

void BaseDesignIntf::drawBorder(QPainter *painter, QRectF rect) const
{
    painter->save();
    if (itemMode() & DesignMode && drawDesignBorders()) {
        drawDesignModeBorder(painter, rect);
    }
    else drawRenderModeBorder(painter, rect);
    painter->restore();
}

void BaseDesignIntf::setGeometry(QRectF rect)
{
    if (m_rect == rect) return;
    m_oldGeometry = m_rect;
    if (!isLoading())
        prepareGeometryChange();
    m_rect = rect;
    m_topRect = QRectF(0-resizeHandleSize(), 0-resizeHandleSize(), width()+resizeHandleSize()*2, resizeHandleSize()*2);
    m_bottomRect = QRectF(0-resizeHandleSize(), height() - resizeHandleSize(),  width()+resizeHandleSize()*2, resizeHandleSize()*2);
    m_leftRect = QRectF(0-resizeHandleSize(), 0-resizeHandleSize(), resizeHandleSize()*2, height()+resizeHandleSize()*2);
    m_rightRect = QRectF(width() - resizeHandleSize(), 0-resizeHandleSize(), resizeHandleSize()*2, height()+resizeHandleSize()*2);
    m_boundingRect = QRectF();
    updateSelectionMarker();
    if (!isLoading()){
        geometryChangedEvent(geometry(), m_oldGeometry);
        emit geometryChanged(this, geometry(), m_oldGeometry);
    }
}

void BaseDesignIntf::geometryChangedEvent(QRectF newRect, QRectF oldRect)
{
    Q_UNUSED(oldRect);
    Q_UNUSED(newRect);
}

void BaseDesignIntf::beforeDelete()
{

}

void BaseDesignIntf::setGeometryProperty(QRectF rect)
{
    if (rect!=geometry()){
        QRectF oldValue = geometry();
        if ((rect.x() != geometry().x()) || (rect.y() != geometry().y()))
            setPos(rect.x(), rect.y());
        if (rect.width() != geometry().width())
            setWidth(rect.width());
        if (rect.height() != geometry().height())
            setHeight(rect.height());
        if (!isLoading()) {
            notify("geometry",oldValue,rect);
        }
    }
}

PageDesignIntf *BaseDesignIntf::page()
{
    return dynamic_cast<PageDesignIntf*>(scene());
}

void BaseDesignIntf::setPossibleResizeDirectionFlags(int directionsFlags)
{
    m_possibleResizeDirectionFlags = directionsFlags;
}

QPen BaseDesignIntf::borderPen(BorderSide side/*, bool selected*/) const
{
    QPen pen;
    if (m_borderLinesFlags & side) {
        pen.setColor(m_borderColor);
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(m_borderLineSize);
    } else {
        pen.setColor(Qt::darkGray);
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(1);
    }
    return pen;
}

QColor BaseDesignIntf::selectionColor() const
{
    return Const::SELECTION_COLOR;
}

void BaseDesignIntf::initFlags()
{
    if ((itemMode()&DesignMode) || (itemMode()&EditMode)) {
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        setAcceptHoverEvents(true);
    }
    else {
        setFlag(QGraphicsItem::ItemIsSelectable, false);
        setAcceptHoverEvents(false);
    }

}

void BaseDesignIntf::initMode(ItemMode mode)
{
    Q_UNUSED(mode);
}

QVariant BaseDesignIntf::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionHasChanged) {
        updateSelectionMarker();
    }

    if (change == QGraphicsItem::ItemSelectedChange) {
        turnOnSelectionMarker(value.toBool());
        emit itemSelectedHasBeenChanged(this, value.toBool());
    }
    if (change == QGraphicsItem::ItemParentHasChanged) {
        parentChangedEvent(dynamic_cast<BaseDesignIntf*>(value.value<QGraphicsItem*>()));
    }

    return QGraphicsItem::itemChange(change, value);
}

void BaseDesignIntf::childAddedEvent(BaseDesignIntf *child)
{
    Q_UNUSED(child)
}

void BaseDesignIntf::parentChangedEvent(BaseDesignIntf *)
{

}

void BaseDesignIntf::restoreLinks()
{
#ifdef HAVE_QT5
    foreach(QObject * child, children()) {
#else
    foreach(QObject * child, QObject::children()) {
#endif
        BaseDesignIntf *childItem = dynamic_cast<BaseDesignIntf *>(child);
        if (childItem) {childItem->restoreLinks();}
    }
    restoreLinksEvent();
}

QPainterPath BaseDesignIntf::shape() const
{
    QPainterPath path;
    path.addRect(this->boundingRect());
    return path;
}

void BaseDesignIntf::drawSelection(QPainter *painter, QRectF /*rect*/) const
{
    painter->save();
    //    painter->setPen(QPen(Qt::red,m_selectionPenSize));
    //    painter->drawLine(QPointF(m_resizeHandleSize,0),QPointF(0,0));
    //    painter->drawLine(QPointF(0,m_resizeHandleSize),QPointF(0,0));
    //    painter->drawLine(rect.right()-m_resizeHandleSize,0,rect.right(),0);
    //    painter->drawLine(rect.right(),m_resizeHandleSize,rect.right(),0);
    //    painter->drawLine(0,rect.bottom(),0,rect.bottom()-10);
    //    painter->drawLine(0,rect.bottom(),m_resizeHandleSize,rect.bottom());
    //    painter->drawLine(rect.right()-m_resizeHandleSize,rect.bottom(),rect.right(),rect.bottom());
    //    painter->drawLine(rect.right(),rect.bottom()-m_resizeHandleSize,rect.right(),rect.bottom());
    //    painter->setOpacity(Consts::SELECTION_COLOR_OPACITY);
    //    painter->fillRect(rect,selectionColor());
    painter->restore();
}

void BaseDesignIntf::moveSelectedItems(QPointF delta)
{
    QList<QGraphicsItem *> selectedItems;
    selectedItems = scene()->selectedItems();
    BaseDesignIntf *selectedItem;
    foreach(QGraphicsItem * item, selectedItems) {
        if (item != this) {
            selectedItem = dynamic_cast<BaseDesignIntf *>(item);
            if (selectedItem && !selectedItem->isBand()) {
                if (!selectedItem->m_fixedPos)
                    selectedItem->setItemPos(selectedItem->pos() - delta);
            }
        }
    }
}

void BaseDesignIntf::setItemPos(qreal x, qreal y)
{
    setItemPos(QPointF(x, y));
}

void BaseDesignIntf::setItemMode(ItemMode mode)
{
    m_itemMode = mode;
    foreach(QGraphicsItem * item, childItems()) {
        BaseDesignIntf *ri = dynamic_cast<BaseDesignIntf *>(item);
        if (ri) ri->setItemMode(mode);
    }
    initMode(mode);
    initFlags();
}
void BaseDesignIntf::setItemPos(const QPointF &newPos)
{
    QPointF oldPos = pos();
    QPointF finalPos = modifyPosForAlignedItem(newPos);
    QGraphicsItem::setPos(finalPos);
    emit posChanging(this, finalPos, oldPos);
}

void BaseDesignIntf::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QRectF newGeometry = geometry();
    if (newGeometry != m_oldGeometry) {
        geometryChangedEvent(newGeometry, m_oldGeometry);
        updateSelectionMarker();
        emit(posChanged(this, newGeometry.topLeft(), m_oldGeometry.topLeft()));
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void BaseDesignIntf::showEditorDialog(){
    QWidget *editor = defaultEditor();
    if (editor) {
        QDialog* dialog = new QDialog(QApplication::activeWindow());
        dialog->setAttribute(Qt::WA_DeleteOnClose);
#ifdef Q_OS_MAC
        dialog->setWindowModality(Qt::WindowModal);
#else
        dialog->setWindowModality(Qt::ApplicationModal);
#endif
        dialog->setLayout(new QVBoxLayout());
        dialog->resize(editor->size());
        dialog->layout()->setContentsMargins(2,2,2,2);
        dialog->layout()->addWidget(editor);
        connect(editor,SIGNAL(destroyed()),dialog,SLOT(close()));
        dialog->exec();
    }
}

void BaseDesignIntf::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton &&
            ((itemMode()&EditMode)||(itemMode()&DesignMode))
       ) {
        showEditorDialog();
    }
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void BaseDesignIntf::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (!(flags() & QGraphicsItem::ItemIsSelectable)) return;
    PageDesignIntf* page = dynamic_cast<PageDesignIntf*>(scene());
    if (!page->selectedItems().contains(this)){
        page->clearSelection();
        this->setSelected(true);
    }
    QMenu menu;
    QAction* copyAction = menu.addAction(QIcon(":/report/images/copy.png"), tr("Copy"));
    copyAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    QAction* cutAction = menu.addAction(QIcon(":/report//images/cut"), tr("Cut"));
    cutAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
    QAction* pasteAction = menu.addAction(QIcon(":/report/images/paste.png"), tr("Paste"));
    pasteAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    pasteAction->setEnabled(false);
    QClipboard *clipboard = QApplication::clipboard();
    ItemsReaderIntf::Ptr reader = StringXMLreader::create(clipboard->text());
    if (reader->first() && reader->itemType() == "Object"){
        pasteAction->setEnabled(true);
    }
    menu.addSeparator();
    QAction* brinToTopAction = menu.addAction(QIcon(":/report//images/bringToTop"), tr("Bring to top"));
    QAction* sendToBackAction = menu.addAction(QIcon(":/report//images/sendToBack"), tr("Send to back"));
    menu.addSeparator();
    QAction* noBordersAction = menu.addAction(QIcon(":/report//images/noLines"), tr("No borders"));
    QAction* allBordersAction = menu.addAction(QIcon(":/report//images/allLines"), tr("All borders"));
    preparePopUpMenu(menu);
    QAction* a = menu.exec(event->screenPos());
    if (a){
        if (a == cutAction)
        {
            page->cut();
            return;
        }
        if (a == copyAction)
            page->copy();
        if (a == pasteAction)
            page->paste();
        if (a == brinToTopAction)
            page->bringToFront();
        if (a == sendToBackAction)
            page->sendToBack();
        if (a == noBordersAction)
            page->setBorders(BaseDesignIntf::NoLine);
        if (a == allBordersAction)
            page->setBorders(BaseDesignIntf::AllLines);
        processPopUpAction(a);
    }
}

int BaseDesignIntf::possibleMoveDirectionFlags() const
{
    return m_possibleMoveDirectionFlags;
}

void BaseDesignIntf::setPossibleMoveFlags(int directionsFlags)
{
    m_possibleMoveDirectionFlags = directionsFlags;
}

void BaseDesignIntf::setMarginSize(int value)
{
    if (m_margin!=value){
        int oldValue = m_margin;
        m_margin=value;
        if (!isLoading()){
            update(rect());
            notify("margin", oldValue, value);
        }
    }
}

void BaseDesignIntf::updateSelectionMarker()
{
    if (m_selectionMarker && (itemMode() & DesignMode || itemMode() & EditMode)) {
        if ((!m_selectionMarker->scene()) && scene()) scene()->addItem(m_selectionMarker);
        if (parentItem()) {
            m_selectionMarker->setRect(rect());
            m_selectionMarker->setPos(0,0);
        }
    }
}

void BaseDesignIntf::drawResizeZone(QPainter* /*painter*/)
{

//    if (m_resizeAreas.count() > 0) {
//        painter->save();
//        painter->setPen(QPen(Const::RESIZE_ZONE_COLOR));
//        (isSelected()) ? painter->setOpacity(Const::SELECTED_RESIZE_ZONE_OPACITY) : painter->setOpacity(Const::RESIZE_ZONE_OPACITY);
//        painter->setBrush(QBrush(Qt::green, Qt::SolidPattern));
//        foreach(QRectF * resizeArea, m_resizeAreas) painter->drawRect(*resizeArea);
//        painter->restore();
//    }

}

void BaseDesignIntf::initResizeZones()
{
    m_resizeAreas.clear();
    if (m_resizeDirectionFlags & ResizeBottom) m_resizeAreas.append(&m_bottomRect);
    if (m_resizeDirectionFlags & ResizeTop) m_resizeAreas.append(&m_topRect);
    if (m_resizeDirectionFlags & ResizeLeft) m_resizeAreas.append(&m_leftRect);
    if (m_resizeDirectionFlags & ResizeRight) m_resizeAreas.append(&m_rightRect);
}

void BaseDesignIntf::invalidateRect(const QRectF &rect)
{
    if (scene())
        scene()->update(mapToScene(rect).boundingRect());
}

ReportEnginePrivate *BaseDesignIntf::reportEditor()
{
    PageDesignIntf *page = dynamic_cast<PageDesignIntf *>(scene());
    if (page) return page->reportEditor();
    else return 0;
}

void BaseDesignIntf::updateItemSize(DataSourceManager *dataManager, RenderPass pass, int maxHeight)
{
    Q_UNUSED(maxHeight);
    Q_UNUSED(dataManager);
    m_currentPass = pass;
    updateItemAlign();
}

bool BaseDesignIntf::isNeedUpdateSize(RenderPass /*pass*/) const
{return false;}

void BaseDesignIntf::drawPinArea(QPainter *painter) const
{
    painter->drawRect(QRect(0, 0, 16, 16));
}

QObject *BaseDesignIntf::createElement(const QString& /*collectionName*/, const QString &elementType)
{
    BaseDesignIntf* obj = 0;
    try{
        if (LimeReport::DesignElementsFactory::instance().objectCreator(elementType)){
            obj = LimeReport::DesignElementsFactory::instance().objectCreator(elementType)(this, this);
            connect(obj,SIGNAL(propertyChanged(QString,QVariant,QVariant)),page(),SLOT(slotItemPropertyChanged(QString,QVariant,QVariant)));
        }
    } catch (ReportError error){
        qDebug()<<error.what();
    }
    return obj;
}

int BaseDesignIntf::elementsCount(const QString& /*collectionName*/)
{
    return QObject::children().count();
}

QObject *BaseDesignIntf::elementAt(const QString& /*collectionName*/, int index)
{
    return QObject::children().at(index);
}

void BaseDesignIntf::collectionLoadFinished(const QString &collectionName)
{
    if (collectionName.compare("children", Qt::CaseInsensitive) == 0) {
        if (page()) {
#ifdef HAVE_QT5
            foreach(QObject * obj, children()) {
#else
            foreach(QObject * obj, QObject::children()) {
#endif
                BaseDesignIntf *item = dynamic_cast<BaseDesignIntf *>(obj);
                if (item && page()) {
                    page()->registerItem(item);
                }
            }
        }
    }
    emit loadCollectionFinished(collectionName);
}

BaseDesignIntf *BaseDesignIntf::cloneItem(ItemMode mode, QObject *owner, QGraphicsItem *parent)
{
    BaseDesignIntf *clone = cloneItemWOChild(mode, owner, parent);
    clone->setPatternName(this->objectName());
#ifdef HAVE_QT5
    foreach(QObject * child, children()) {
#else
    foreach(QObject * child, QObject::children()) {
#endif
        BaseDesignIntf *childItem = dynamic_cast<BaseDesignIntf *>(child);
        if (childItem) {clone->childAddedEvent(childItem->cloneItem(mode, clone, clone));}
    }
    return clone;
}

BaseDesignIntf *BaseDesignIntf::cloneItemWOChild(ItemMode mode, QObject *owner, QGraphicsItem *parent)
{
    BaseDesignIntf *clone = createSameTypeItem(owner, parent);
    clone->setObjectName(this->objectName());
    clone->setItemMode(mode);
    clone->objectLoadStarted();
    clone->setReportSettings(this->reportSettings());
    for (int i = 0; i < clone->metaObject()->propertyCount(); i++) {
        if (clone->metaObject()->property(i).isWritable())
            clone->setProperty(clone->metaObject()->property(i).name(), property(clone->metaObject()->property(i).name()));
    }
    clone->objectLoadFinished();
    return clone;
}

void BaseDesignIntf::initFromItem(BaseDesignIntf *source)
{
    objectLoadStarted();
    for (int i = 0; i < metaObject()->propertyCount(); i++) {
        if (strcmp(metaObject()->property(i).name(),"objectName")!=0)
            if (source->property(metaObject()->property(i).name()).isValid()) {
                if (metaObject()->property(i).isWritable())
                    setProperty(metaObject()->property(i).name(), source->property(metaObject()->property(i).name()));
            }
    }
    objectLoadFinished();
}

bool BaseDesignIntf::canBeSplitted(int height) const
{Q_UNUSED(height); return false;}

bool BaseDesignIntf::isEmpty() const
{return false;}

BaseDesignIntf *BaseDesignIntf::cloneUpperPart(int height, QObject *owner, QGraphicsItem *parent)
{Q_UNUSED(height); Q_UNUSED(owner); Q_UNUSED(parent); return 0;}

BaseDesignIntf *BaseDesignIntf::cloneBottomPart(int height, QObject *owner, QGraphicsItem *parent)
{Q_UNUSED(height); Q_UNUSED(owner); Q_UNUSED(parent); return 0;}

BaseDesignIntf *BaseDesignIntf::cloneEmpty(int height, QObject *owner, QGraphicsItem *parent)
{Q_UNUSED(height); Q_UNUSED(owner); Q_UNUSED(parent); return 0;}


void BaseDesignIntf::objectLoadStarted()
{
    m_objectState = ObjectLoading;
}

void BaseDesignIntf::objectLoadFinished()
{
    m_objectState = ObjectLoaded;
    emit objectLoaded(this);
}

void BaseDesignIntf::parentObjectLoadFinished()
{}

QList<BaseDesignIntf *> BaseDesignIntf::childBaseItems()
{
    QList<BaseDesignIntf *> resList;
    foreach(QGraphicsItem * item, childItems()) {
        BaseDesignIntf *baseItem = dynamic_cast<BaseDesignIntf *>(item);
        if (baseItem) resList << baseItem;
    }
    return resList;
}

BaseDesignIntf *BaseDesignIntf::childByName(const QString &name)
{
    foreach(BaseDesignIntf* item, childBaseItems()){
        if (item->objectName().compare(name,Qt::CaseInsensitive)==0){
            return item;
        } else {
            BaseDesignIntf* child = item->childByName(name);
            if (child) return child;
        }
    }
    return 0;
}

QWidget *BaseDesignIntf::defaultEditor()
{
    return 0;
}

void BaseDesignIntf::notify(const QString &propertyName, const QVariant& oldValue, const QVariant& newValue)
{
    if (!isLoading())
        emit propertyChanged(propertyName, oldValue, newValue);
}

void BaseDesignIntf::notify(const QVector<QString>& propertyNames)
{
    if (!isLoading())
      emit propertyesChanged(propertyNames);
}

SelectionMarker::SelectionMarker(QGraphicsItem *parent)//, QGraphicsScene *scene)
    : Marker(parent)
{
    setAcceptHoverEvents(true);
}

void SelectionMarker::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    BaseDesignIntf* baseItem = dynamic_cast<BaseDesignIntf*>(parentItem());
    if(baseItem) baseItem->hoverMoveEvent(event);
}

void SelectionMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    parentItem()->setSelected(true);
    BaseDesignIntf* baseItem = dynamic_cast<BaseDesignIntf*>(parentItem());
    if(baseItem) baseItem->mousePressEvent(event);
    QGraphicsItem::mousePressEvent(event);
}

void SelectionMarker::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    BaseDesignIntf* baseItem = dynamic_cast<BaseDesignIntf*>(parentItem());
    if(baseItem) baseItem->mouseReleaseEvent(event);
}

void SelectionMarker::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    BaseDesignIntf* baseItem = dynamic_cast<BaseDesignIntf*>(parentItem());
    if(baseItem) baseItem->mouseDoubleClickEvent(event);
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void SelectionMarker::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    BaseDesignIntf* baseItem = dynamic_cast<BaseDesignIntf*>(parentItem());
    if(baseItem) baseItem->mouseMoveEvent(event);
}

QRectF Marker::boundingRect() const
{
    return m_rect.adjusted(-15,-15,15,15);
}

void Marker::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPen pen;
    const int markerSize = 5;
    pen.setColor(color());
    pen.setWidth(2);
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);
    painter->setOpacity(Const::SELECTION_COLOR_OPACITY);
    painter->drawRect(rect());
    painter->setBrush(color());
    painter->setPen(Qt::transparent);
    painter->setOpacity(1);
    painter->drawRect(QRectF(-markerSize,-markerSize,markerSize*2,markerSize*2));
    painter->drawRect(QRectF(rect().right()-markerSize,rect().bottom()-markerSize,markerSize*2,markerSize*2));
    painter->drawRect(QRectF(rect().right()-markerSize,rect().top()-markerSize,markerSize*2,markerSize*2));
    painter->drawRect(QRectF(rect().left()-markerSize,rect().bottom()-markerSize,markerSize*2,markerSize*2));
    painter->drawRect(QRectF(rect().left()-markerSize,
                                rect().bottom()-rect().height()/2-markerSize,markerSize*2,markerSize*2));
    painter->drawRect(QRectF(rect().right()-markerSize,
                                rect().bottom()-rect().height()/2-markerSize,markerSize*2,markerSize*2));
    painter->drawRect(QRectF(rect().left()+rect().width()/2-markerSize,
                                rect().top()-markerSize,markerSize*2,markerSize*2));
    painter->drawRect(QRectF(rect().left()+rect().width()/2-markerSize,
                                rect().bottom()-markerSize,markerSize*2,markerSize*2));
}

QRectF Marker::rect() const
{
    return m_rect;
}

QColor Marker::color() const
{
    return m_color;
}

BaseDesignIntf *Marker::object() const
{
    return m_object;
}

} //namespace LimeReport







