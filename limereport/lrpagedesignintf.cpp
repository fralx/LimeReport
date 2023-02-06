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
#include "lrpagedesignintf.h"
#include "lrbasedesignintf.h"
#include "lrtextitem.h"
#include "lrhorizontallayout.h"
#include "lrverticallayout.h"
//#include "lrbarcodeitem.h"
#include "lrbanddesignintf.h"
#include "lrbandsmanager.h"
#include "lrreportengine_p.h"

#include "serializators/lrstorageintf.h"
#include "serializators/lrxmlwriter.h"
#include "serializators/lrxmlreader.h"
#include "lrdesignelementsfactory.h"

#include "lrpageheader.h"
#include "lrpagefooter.h"

#include "lrglobal.h"

#include <QPrinter>
#include <QDebug>
#include <QGraphicsItem>
#include <QString>
#include <QDrag>
#include <QMimeData>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QMessageBox>


namespace LimeReport
{

bool bandSortBandLessThen(const BandDesignIntf *c1, const BandDesignIntf *c2)
{
    return c1->geometry().top() < c2->geometry().top();
}

PageDesignIntf::PageDesignIntf(QObject *parent):
    QGraphicsScene(parent),
    m_pageItem(0),
    m_insertMode(false),
    m_itemInsertRect(0),
    m_itemMode(DesignMode),
    m_cutterBorder(0),
//    m_infoPosRect(0),
    m_currentCommand(-1),
    m_changeSizeMode(false),
    m_changePosMode(false),
    m_changePosOrSizeMode(false),
    m_executingCommand(false),
    m_hasChanges(false),
    m_isLoading(false),
    m_executingGroupCommand(false),
    m_settings(0),
    m_selectionRect(0),
    m_verticalGridStep(Const::DEFAULT_GRID_STEP),
    m_horizontalGridStep(Const::DEFAULT_GRID_STEP),
    m_updating(false),
    m_currentObjectIndex(1),
    m_multiSelectStarted(false),
    m_movedItem(0),
    m_joinItem(0),
    m_magneticMovement(false),
    m_reportSettings(0),
    m_currentPage(0)

{
    m_reportEditor = dynamic_cast<ReportEnginePrivate *>(parent);
    updatePageRect();
    connect(this, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
    setBackgroundBrush(QBrush(Qt::white));

}

PageDesignIntf::~PageDesignIntf()
{
    if (!m_pageItem.isNull()) {
        removeItem(m_pageItem.data());
        m_pageItem.clear();
    }
    foreach (PageItemDesignIntf::Ptr pageItem, m_reportPages) {
       removeItem(pageItem.data());
    }
    m_commandsList.clear();
}

void PageDesignIntf::updatePageRect()
{
    if (m_pageItem.isNull()) {
        m_pageItem =  PageItemDesignIntf::create(this);
        addItem(m_pageItem.data());
        m_pageItem->setTopMargin(5);
        m_pageItem->setBottomMargin(5);
        m_pageItem->setLeftMargin(5);
        m_pageItem->setRightMargin(5);
        m_pageItem->setObjectName("ReportPage1");
        connect(m_pageItem.data(), SIGNAL(itemSelected(LimeReport::BaseDesignIntf *)), this, SIGNAL(itemSelected(LimeReport::BaseDesignIntf *)));
        connect(m_pageItem.data(), SIGNAL(geometryChanged(QObject *, QRectF, QRectF)), this, SLOT(slotPageGeometryChanged(QObject *, QRectF, QRectF)));
        connect(m_pageItem.data(), SIGNAL(objectLoaded(QObject *)), this, SLOT(slotPageItemLoaded(QObject *)));
    }
    this->setSceneRect(-Const::SCENE_MARGIN, -Const::SCENE_MARGIN,
                       pageItem()->geometry().width() + Const::SCENE_MARGIN*2,
                       pageItem()->boundingRect().height() + Const::SCENE_MARGIN*2);
    emit sceneRectChanged(sceneRect());
}

void PageDesignIntf::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::NoModifier ||
         (event->key() != Qt::Key_Left &&
          event->key() != Qt::Key_Right &&
          event->key() != Qt::Key_Up &&
          event->key() != Qt::Key_Down &&
          !m_changePosMode && !m_changeSizeMode )
       ) {
        QGraphicsScene::keyPressEvent(event);
        return;
    }

    if ((event->modifiers()& Qt::ControlModifier) && (!m_changeSizeMode)) {
        if (!m_changePosMode) {
            saveSelectedItemsPos();
            m_changePosMode = true;
        }
    }

    if ((event->modifiers()& Qt::ShiftModifier) && (!m_changePosMode)) {
        if (!m_changeSizeMode) {
            saveSelectedItemsGeometry();
            m_changeSizeMode = true;
        }
    }

    if ((event->modifiers()& Qt::ControlModifier) && m_changePosMode && (!(event->modifiers()& Qt::ShiftModifier))) {
        foreach(QGraphicsItem * item, selectedItems()) {
            if (dynamic_cast<BaseDesignIntf *>(item)) {
                switch (event->key()) {
                case Qt::Key_Right:
                    dynamic_cast<BaseDesignIntf *>(item)->moveRight();
                    break;
                case Qt::Key_Left:
                    dynamic_cast<BaseDesignIntf *>(item)->moveLeft();
                    break;
                case Qt::Key_Up:
                    dynamic_cast<BaseDesignIntf *>(item)->moveUp();
                    break;
                case Qt::Key_Down:
                    dynamic_cast<BaseDesignIntf *>(item)->moveDown();
                    break;
                }
            }
        }
    }

    if ((event->modifiers()& Qt::ShiftModifier) && m_changeSizeMode && (!(event->modifiers()& Qt::ControlModifier))) {
        foreach(QGraphicsItem * item, selectedItems()) {
            if (dynamic_cast<BaseDesignIntf *>(item)) {
                switch (event->key()) {
                case Qt::Key_Right:
                    dynamic_cast<BaseDesignIntf *>(item)->sizeRight();
                    break;
                case Qt::Key_Left:
                    dynamic_cast<BaseDesignIntf *>(item)->sizeLeft();
                    break;
                case Qt::Key_Up:
                    dynamic_cast<BaseDesignIntf *>(item)->sizeUp();
                    break;
                case Qt::Key_Down:
                    dynamic_cast<BaseDesignIntf *>(item)->sizeDown();
                    break;
                }
            }
        }
    }
}

void PageDesignIntf::keyReleaseEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Control) && m_changePosMode) {
        checkSizeOrPosChanges();
    }

    if ((event->key() == Qt::Key_Shift) && m_changeSizeMode ) {
        checkSizeOrPosChanges();
    }

    QGraphicsScene::keyReleaseEvent(event);
}

void PageDesignIntf::startInsertMode(const QString &ItemType)
{
    if (m_insertMode) emit itemInsertCanceled(m_insertItemType);

    emit insertModeStarted();
    m_insertMode = true;
    m_insertItemType = ItemType;
    m_itemInsertRect = this->addRect(0, 0, 200, 50);
    m_itemInsertRect->setVisible(false);
    PageItemDesignIntf* page = pageItem() ? pageItem() : getCurrentPage();
    if (page)
        m_itemInsertRect->setParentItem(page);
}

void PageDesignIntf::startEditMode()
{
    if (m_insertMode) emit itemInsertCanceled(m_insertItemType);

    finalizeInsertMode();
    m_insertMode = false;
}

PageItemDesignIntf *PageDesignIntf::pageItem()
{
    return m_currentPage ? m_currentPage : m_pageItem.data();
}

void PageDesignIntf::setPageItem(PageItemDesignIntf::Ptr pageItem)
{
    if (pageItem.isNull()) return;
    if (!m_pageItem.isNull()) {
        removeItem(m_pageItem.data());
        m_pageItem->setParent(0);
    }
    m_pageItem = pageItem;
    m_pageItem->setItemMode(itemMode());
    setSceneRect(pageItem->rect().adjusted(-10 * Const::mmFACTOR,
                                           -10 * Const::mmFACTOR,
                                           10 * Const::mmFACTOR,
                                           10 * Const::mmFACTOR));
    addItem(m_pageItem.data());
    registerItem(m_pageItem.data());
}

void PageDesignIntf::setPageItems(QList<PageItemDesignIntf::Ptr> pages)
{
    m_currentPage = 0;
    if (!m_pageItem.isNull()) {
        if (m_pageItem->scene() == this)
            removeItem(m_pageItem.data());
        m_pageItem.clear();
    }
    int curHeight = 0;
    int curWidth = 0;
    m_reportPages = pages;
    foreach (PageItemDesignIntf::Ptr pageItem, pages) {
        pageItem->setItemMode(itemMode());
        addItem(pageItem.data());
        registerItem(pageItem.data());
        pageItem->setPos(0,curHeight);
        curHeight+=pageItem->height()+20;
        if (curWidth<pageItem->width()) curWidth=pageItem->width();
    }
    setSceneRect(QRectF( 0, 0, curWidth,curHeight).adjusted( -10 * Const::mmFACTOR,
                                                             -10 * Const::mmFACTOR,
                                                             10 * Const::mmFACTOR,
                                                             10 * Const::mmFACTOR));
    if (m_reportPages.count()>0)
        m_currentPage = m_reportPages.at(0).data();

}

void PageDesignIntf::removePageItem(PageItemDesignIntf::Ptr pageItem)
{
    if (m_pageItem == pageItem){
        removeItem(m_pageItem.data());
        m_pageItem.clear();
    }
}

void PageDesignIntf::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_insertMode) {
        finalizeInsertMode();
        CommandIf::Ptr command = InsertItemCommand::create(this, m_insertItemType, event->scenePos(), QSize(200, 50));
        saveCommand(command);
        emit itemInserted(this, event->scenePos(), m_insertItemType);
    }
    if (event->buttons() & Qt::LeftButton && event->modifiers()==Qt::ShiftModifier){
        m_multiSelectStarted = true;
    } else {
        QGraphicsScene::mousePressEvent(event);
    }
}


void PageDesignIntf::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    PageItemDesignIntf* page = pageItem() ? pageItem() : getCurrentPage();

    if (event->buttons() & Qt::LeftButton) {
        if (!m_changePosOrSizeMode) {
            saveSelectedItemsPos();
            saveSelectedItemsGeometry();
            m_changePosOrSizeMode = true;
        }
//        qreal posY = div(page->mapFromScene(event->scenePos()).y(), verticalGridStep()).quot * verticalGridStep();
//        qreal posX = div(page->mapFromScene(event->scenePos()).x(), verticalGridStep()).quot * horizontalGridStep();

//        if(!m_infoPosRect)
//        {
//            m_infoPosRect = new QGraphicsTextItem();
//            m_infoPosRect->setDefaultTextColor(QColor(100,150,50));

//            QFont font("Arial");
//            font.setPointSize(16);
//            font.setBold(true);
//            m_infoPosRect->setFont(font);
//            addItem(m_infoPosRect);
//        }

//        m_infoPosRect->setPlainText("(x: "+QString::number(posX/100)+", y: "+QString::number(posY/100)+") cm");
//        m_infoPosRect->setPos(posX,posY+30);

    }

    if (event->buttons() & Qt::LeftButton && m_multiSelectStarted){
        if (!m_selectionRect){
            m_selectionRect = new QGraphicsRectItem();
            QBrush brush(QColor(140,190,30,50));
            m_selectionRect->setBrush(brush);
            m_selectionRect->setPen(Qt::DashLine);
            addItem(m_selectionRect);
        }

        QRectF selectionRect;
        selectionRect.setX(qMin(event->buttonDownScenePos(Qt::LeftButton).x(),event->scenePos().x()));
        selectionRect.setY(qMin(event->buttonDownScenePos(Qt::LeftButton).y(),event->scenePos().y()));
        selectionRect.setRight(qMax(event->buttonDownScenePos(Qt::LeftButton).x(),event->scenePos().x()));
        selectionRect.setBottom(qMax(event->buttonDownScenePos(Qt::LeftButton).y(),event->scenePos().y()));
        m_selectionRect->setRect(selectionRect);
    }

    if ((m_insertMode) && (page && page->rect().contains(page->mapFromScene(event->scenePos())))) {
        if (!m_itemInsertRect->isVisible()) m_itemInsertRect->setVisible(true);
        qreal posY = div(page->mapFromScene(event->scenePos()).y(), verticalGridStep()).quot * verticalGridStep();
        qreal posX = div(page->mapFromScene(event->scenePos()).x(), verticalGridStep()).quot * horizontalGridStep();
        m_itemInsertRect->setPos(posX,posY);
        if (magneticMovement()){
            rectMoved(
                QRectF(m_itemInsertRect->pos().x(),
                       m_itemInsertRect->pos().y(),
                       m_itemInsertRect->boundingRect().width(),
                       m_itemInsertRect->boundingRect().height()
                )
            );
        }
    } else {
        if (m_insertMode) m_itemInsertRect->setVisible(false);
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void PageDesignIntf::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if ( (event->button() == Qt::LeftButton)) {
        if (m_joinItem && selectedItems().count()==1){
            BaseDesignIntf* selectedItem = dynamic_cast<BaseDesignIntf*>(selectedItems().at(0));
            if (m_magneticMovement){
                if (m_joinType == Width){

                    QPointF tmpPos;
                    if (selectedItem->pos().y()>m_joinItem->pos().y())
                        tmpPos = QPointF(m_joinItem->x(),m_joinItem->pos().y()+m_joinItem->height());
                    else
                        tmpPos = QPointF(m_joinItem->x(),m_joinItem->pos().y()-selectedItem->height());

                    selectedItem->setPos(tmpPos);
                    selectedItem->setWidth(m_joinItem->width());

                } else {

                    QPointF tmpPos;
                    if (selectedItem->pos().x()>m_joinItem->pos().x())
                        tmpPos = QPointF(m_joinItem->x()+m_joinItem->width(),m_joinItem->pos().y());
                    else
                        tmpPos = QPointF(m_joinItem->x()-selectedItem->width(),m_joinItem->pos().y());

                    selectedItem->setPos(tmpPos);
                    selectedItem->setHeight(m_joinItem->height());
                }
            }
            m_joinItem->turnOnJoinMarker(false);
            m_joinItem = 0;
        }
        checkSizeOrPosChanges();
    }
    if (m_selectionRect) {
        setSelectionRect(m_selectionRect->rect());
        delete m_selectionRect;
        m_selectionRect = 0;
        m_multiSelectStarted = false;
    }
//    if(m_infoPosRect)
//    {
//        delete m_infoPosRect;
//        m_infoPosRect = 0;
//    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void PageDesignIntf::setSelectionRect(QRectF selectionRect){
    clearSelection();
    foreach(QGraphicsItem* item, items()){
        if ( selectionRect.intersects(item->mapRectToScene(item->boundingRect())))
            if (dynamic_cast<ItemDesignIntf*>(item))
                item->setSelected(true);
    }
}

BaseDesignIntf *PageDesignIntf::addBand(const QString &bandType)
{
    return internalAddBand(bandType);
}

BaseDesignIntf *PageDesignIntf::addBand(BandDesignIntf::BandsType bandType)
{
    return internalAddBand(bandType);
}

template <typename T>
BaseDesignIntf *PageDesignIntf::internalAddBand(T bandType)
{

    if (m_insertMode) {
        m_insertMode = false;
        emit itemInsertCanceled(m_insertItemType);
    }

    QSet<BandDesignIntf::BandsType> needParentBands;
    needParentBands << BandDesignIntf::SubDetailBand
                    << BandDesignIntf::SubDetailHeader
                    << BandDesignIntf::SubDetailFooter
                    << BandDesignIntf::GroupHeader
                    << BandDesignIntf::GroupFooter
                    << BandDesignIntf::DataHeader
                    << BandDesignIntf::DataFooter;

    BandsManager bandsManager;
    BandDesignIntf *band = bandsManager.createBand(bandType, pageItem(), pageItem());

    if (band->isUnique()) {
        if (pageItem()->isBandExists(bandType)) {
            delete band;
            return 0;
        }
    }

    band->setObjectName(genObjectName(*band));
    band->setItemTypeName("Band");

    BandDesignIntf* pb = 0;
    if (selectedItems().count() > 0) {
        pb = dynamic_cast<BandDesignIntf *>(selectedItems().at(0));
    }

    bool increaseBandIndex = false;
    int bandIndex = pageItem()->calcBandIndex(band->bandType(), pb, increaseBandIndex);
    band->setBandIndex(bandIndex);
    if (needParentBands.contains(band->bandType())){
        band->setParentBand(pb);
    }
    if (increaseBandIndex) pageItem()->increaseBandIndex(bandIndex);

    registerItem(band);
    foreach(QGraphicsItem * item, selectedItems()) item->setSelected(false);
    band->setSelected(true);
    CommandIf::Ptr command = InsertBandCommand::create(this, band->objectName());
    saveCommand(command, false);
    return band;
}

void PageDesignIntf::bandGeometryChanged(QObject* /*object*/, QRectF newGeometry, QRectF oldGeometry)
{
    Q_UNUSED(newGeometry);
    Q_UNUSED(oldGeometry);
    pageItem()->relocateBands();
}

QPointF PageDesignIntf::placePosOnGrid(QPointF point){
    return QPointF(div(point.x(), horizontalGridStep()).quot * horizontalGridStep(),
                   div(point.y(), verticalGridStep()).quot * verticalGridStep());
}

QSizeF PageDesignIntf::placeSizeOnGrid(QSizeF size){
    return QSizeF(div(size.width(), horizontalGridStep()).quot * horizontalGridStep(),
                   div(size.height(), verticalGridStep()).quot * verticalGridStep());
}

BaseDesignIntf *PageDesignIntf::addReportItem(const QString &itemType, QPointF pos, QSizeF size)
{
    BandDesignIntf *band = bandAt(pos);
    if (band) {
        BaseDesignIntf *reportItem = addReportItem(itemType, band, band);
        reportItem->setPos(placePosOnGrid(band->mapFromScene(pos)));
        reportItem->setSize(placeSizeOnGrid(size));
        reportItem->setUnitType(pageItem()->unitType());
        return reportItem;
    } else {
        PageItemDesignIntf* page = pageItem() ? pageItem() : m_currentPage;
        if (page){
            BaseDesignIntf *reportItem = addReportItem(itemType, page, page);
            reportItem->setPos(placePosOnGrid(page->mapFromScene(pos)));
            reportItem->setSize(placeSizeOnGrid(size));
            reportItem->setUnitType(pageItem()->unitType());
            ItemDesignIntf* ii = dynamic_cast<ItemDesignIntf*>(reportItem);
            if (ii)
                ii->setItemLocation(ItemDesignIntf::Page);
            return reportItem;
        }
    }

    return 0;
}

BaseDesignIntf *PageDesignIntf::addReportItem(const QString &itemType, QObject *owner, LimeReport::BaseDesignIntf  *parent)
{
    BaseDesignIntf *item = LimeReport::DesignElementsFactory::instance().objectCreator(itemType)((owner) ? owner : pageItem(), (parent) ? parent : pageItem());
    item->setObjectName(genObjectName(*item));
    item->setItemTypeName(itemType);
    item->setUnitType(pageItem()->unitType());
    registerItem(item);
    return item;
}

BaseDesignIntf *PageDesignIntf::createReportItem(const QString &itemType, QObject* owner, BaseDesignIntf* parent)
{
    return LimeReport::DesignElementsFactory::instance().objectCreator(itemType)((owner) ? owner : pageItem(), (parent) ? parent : pageItem());
}

CommandIf::Ptr createBandDeleteCommand(PageDesignIntf* page, BandDesignIntf* band){

    if (band->hasChildren()){
        CommandIf::Ptr command = CommandGroup::create();
        command->addCommand(DeleteItemCommand::create(page,band),false);
        foreach(BandDesignIntf* curband, band->childBands()){
            command->addCommand(createBandDeleteCommand(page,curband),false);
        }
        return command;
    } else {
        CommandIf::Ptr command = DeleteItemCommand::create(page,band);
        return command;
    }
}

CommandIf::Ptr PageDesignIntf::removeReportItemCommand(BaseDesignIntf *item){
    BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(item);
    if (band){
        CommandIf::Ptr command = createBandDeleteCommand(this,band);
        return command;
    } else {
        LayoutDesignIntf* parentLayout = dynamic_cast<LayoutDesignIntf*>(item->parent());
        LayoutDesignIntf* layout = dynamic_cast<LayoutDesignIntf*>(item);
        // When removing layout child all his children will be assigned to parent
        if (!layout && parentLayout && (parentLayout->childrenCount() == 2)) {
            CommandGroup::Ptr commandGroup = CommandGroup::create();
            commandGroup->addCommand(DeleteLayoutCommand::create(this, parentLayout),false);
            commandGroup->addCommand(DeleteItemCommand::create(this,item),false);
            return commandGroup;
        } else {
            return layout ? DeleteLayoutCommand::create(this, layout) : DeleteItemCommand::create(this, item) ;
        }
    }
}

void PageDesignIntf::removeReportItem(BaseDesignIntf *item, bool createComand)
{

    if (!createComand){
        removeItem(item);
        BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(item);
        if (band){
            emit bandRemoved(this,band);
        } else {
            emit itemRemoved(this,item);
        }
        delete item;
    } else {

        BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(item);
        if (band){
            CommandIf::Ptr command = createBandDeleteCommand(this,band);
            saveCommand(command);
        } else {
            LayoutDesignIntf* layout = dynamic_cast<LayoutDesignIntf*>(item->parent());
            if (layout && (layout->childrenCount()==2)){
                CommandGroup::Ptr commandGroup = CommandGroup::create();
                commandGroup->addCommand(DeleteLayoutCommand::create(this, layout),false);
                commandGroup->addCommand(DeleteItemCommand::create(this,item),false);
                saveCommand(commandGroup);
            } else {
                CommandIf::Ptr command = (dynamic_cast<LayoutDesignIntf*>(item))?
                            DeleteLayoutCommand::create(this, dynamic_cast<LayoutDesignIntf*>(item)) :
                            DeleteItemCommand::create(this, item) ;
                saveCommand(command);
            }
        }
    }

}

bool PageDesignIntf::saveCommand(CommandIf::Ptr command, bool runCommand)
{
    if (m_executingCommand||m_isLoading) return false;
    if (runCommand) {
        m_executingCommand = true;
        if (!command->doIt()) {
            m_executingCommand = false;
            return false;
        }
        m_executingCommand = false;
    }

    if (m_currentCommand < (m_commandsList.count() - 1))
        m_commandsList.remove(m_currentCommand + 1, m_commandsList.size() - (m_currentCommand + 1));

    m_commandsList.push_back(command);
    m_currentCommand = m_commandsList.count() - 1;
    m_hasChanges = true;
    emit commandHistoryChanged();
    return true;
}

bool PageDesignIntf::isCanRedo()
{
    return m_currentCommand < m_commandsList.count() - 1;
}

bool PageDesignIntf::isCanUndo()
{
    return m_currentCommand >= 0;
}

bool PageDesignIntf::isHasChanges()
{
    return (m_commandsList.count() > 0) && m_hasChanges;
}

bool PageDesignIntf::isItemInsertMode()
{
    return m_insertMode;
}

void PageDesignIntf::bandPosChanged(QObject * /*object*/, QPointF /*newPos*/, QPointF /*oldPos*/)
{
    //relocateBands(dynamic_cast<BandDesignIntf*>(object));
}

QString PageDesignIntf::genObjectName(const QObject &object)
{
    //int index = 1;
    QString className(object.metaObject()->className());
    className = className.right(className.length() - (className.lastIndexOf("::") + 2));

    QString tmpName = QString("%1%2").arg(className).arg(m_currentObjectIndex);

    QList<QGraphicsItem*> itemsList = items();
    while (isExistsObjectName(tmpName,itemsList)) {
        ++m_currentObjectIndex;
        tmpName = QString("%1%2").arg(className).arg(m_currentObjectIndex);
    }

    return tmpName;
}

bool PageDesignIntf::isExistsObjectName(const QString &objectName, QList<QGraphicsItem*>& itemsList) const
{
    QObject *item = 0;
    //QList<QGraphicsItem*> itemList = items();
    for (int i = 0; i < itemsList.count(); i++) {
        item = dynamic_cast<QObject *>(itemsList[i]);
        if (item)
            if (item->objectName() == objectName) return true;
    }

    return false;
}

//QRectF PageDesignIntf::getRectByPageSize(PageDesignIntf::PageSize pageSize)
//{
//    if (m_pageSize != PageSize::Custom) {
//        QPrinter printer;
//        printer.setOutputFormat(QPrinter::PdfFormat);
//        printer.setOrientation((QPrinter::Orientation)getOrientation());
//        printer.setPageSize((QPrinter::PageSize)pageSize);
//        return QRectF(0, 0, printer.paperRect(QPrinter::Millimeter).width() * 10,
//                      printer.paperSize(QPrinter::Millimeter).height() * 10);
//    }

//    else {
//        return QRectF(0, 0, m_pageSizeValue.width() * 10,
//                      m_pageSizeValue.height() * 10);
//    }
//}

bool PageDesignIntf::isLoading()
{
    return m_isLoading;
}

void PageDesignIntf::objectLoadStarted()
{
    m_isLoading=true;
}

void PageDesignIntf::objectLoadFinished()
{
    m_isLoading=false;
}

void PageDesignIntf::removeBand(BandDesignIntf *band)
{
    removeItem(band);
    pageItem()->removeBand(band);
}

ReportEnginePrivate *PageDesignIntf::reportEditor()
{
    return m_reportEditor;
}

void PageDesignIntf::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (!event->mimeData()->text().isEmpty()){
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->setDropAction(Qt::IgnoreAction);
        event->ignore();
    }
}

void PageDesignIntf::dragMoveEvent(QGraphicsSceneDragDropEvent* /**event*/)
{
//    event->setDropAction(Qt::CopyAction);
//    event->accept();
}

void PageDesignIntf::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    if (event->mimeData()->hasText() &&
            ((event->mimeData()->text().indexOf("field:")==0) ||
             (event->mimeData()->text().indexOf("variable:")==0))
    ){
        bool isVar = event->mimeData()->text().indexOf("variable:")==0;
        BaseDesignIntf* item = addReportItem("TextItem",event->scenePos(),QSize(250, 50));
        TextItem* ti = dynamic_cast<TextItem*>(item);
        QString data = event->mimeData()->text().remove(0,event->mimeData()->text().indexOf(":")+1);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
        if (isVar) data = data.remove(QRegExp("  \\[.*\\]"));
#else
        if (isVar) data = data.remove(QRegularExpression("  \\[.*\\]", QRegularExpression::DotMatchesEverythingOption));
#endif
        ti->setContent(data);
        if (!isVar){
            BandDesignIntf* parentBand = dynamic_cast<BandDesignIntf*>(ti->parentItem());
            if (parentBand && parentBand->datasourceName().isEmpty()){
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
                QRegExp dataSource("(?:\\$D\\{\\s*(.*)\\..*\\})");
                if (dataSource.indexIn(data) != -1){
                    parentBand->setProperty("datasource",dataSource.cap(1));
                }
#else
                QRegularExpression dataSource("(?:\\$D\\{\\s*(.*)\\..*\\})", QRegularExpression::DotMatchesEverythingOption);
                QRegularExpressionMatch match = dataSource.match(data);
                if(match.hasMatch()){
                    parentBand->setProperty("datasource", match.captured(1));
                }
#endif
            }
        }
    }
}

void PageDesignIntf::dragLeaveEvent(QGraphicsSceneDragDropEvent *)
{
    //removeItem(m_itemInsertRect);
    //delete m_itemInsertRect;
    //m_itemInsertRect = 0;
}

QStringList PageDesignIntf::possibleParentItems()
{
    QStringList itemsList;
    foreach(QGraphicsItem * item, items()) {
        BandDesignIntf *band = dynamic_cast<BandDesignIntf *>(item);

        if (band) {
            itemsList.append(band->objectName());
        }
    }
    return itemsList;
}

void PageDesignIntf::slotPageGeometryChanged(QObject *, QRectF /*newGeometry*/, QRectF)
{
    if (!m_isLoading){
        pageItem()->relocateBands();
        updatePageRect();
        emit geometryChanged(sceneRect());
    }
}

void PageDesignIntf::slotItemPropertyChanged(QString propertyName, const QVariant &oldValue, const QVariant& newValue)
{
    if (!m_isLoading && m_animationList.isEmpty()){
        saveChangeProppertyCommand(sender()->objectName(),propertyName,oldValue,newValue);
        emit itemPropertyChanged(sender()->objectName(),propertyName,oldValue,newValue);
    }
}

void PageDesignIntf::slotItemPropertyObjectNameChanged(const QString &oldName, const QString &newName)
{
    if (oldName.compare(newName)!=0 && !m_executingCommand){
        CommandIf::Ptr command = PropertyObjectNameChangedCommand::create(this, oldName, newName);
        saveCommand(command, false);
        emit itemPropertyObjectNameChanged(oldName, newName);
    }
}

void PageDesignIntf::bandDeleted(QObject *band)
{
    pageItem()->removeBand(reinterpret_cast<BandDesignIntf *>(band));
    delete band;
    pageItem()->relocateBands();
}

void PageDesignIntf::slotPageItemLoaded(QObject *)
{
    setItemMode(m_itemMode);
}

void PageDesignIntf::slotSelectionChanged()
{
    if (selectedItems().count() == 1) {
        m_firstSelectedItem = dynamic_cast<BaseDesignIntf *>(selectedItems().at(0));
    }
}

void PageDesignIntf::slotAnimationStoped(QObject *animation)
{
    m_animationList.removeOne(animation);
}

void PageDesignIntf::finalizeInsertMode()
{
    if (m_insertMode) {
        m_insertMode = false;
        if (m_itemInsertRect) {
            removeItem(m_itemInsertRect);
            delete m_itemInsertRect;
            m_itemInsertRect = 0;
        }
    }
}

void PageDesignIntf::saveSelectedItemsPos()
{
    m_positionStamp.clear();
    foreach(QGraphicsItem * item, selectedItems()) {
        BaseDesignIntf *reportItem = dynamic_cast<BaseDesignIntf *>(item);

        if (reportItem) {
            ReportItemPos rp;
            rp.objectName = reportItem->objectName();
            rp.pos = reportItem->pos();
            m_positionStamp.push_back(rp);
        }
    }
}

void PageDesignIntf::saveSelectedItemsGeometry()
{
    m_geometryStamp.clear();
    foreach(QGraphicsItem * item, selectedItems()) {
        BaseDesignIntf *reportItem = dynamic_cast<BaseDesignIntf *>(item);

        if (reportItem) {
            ReportItemSize rs;
            rs.objectName = reportItem->objectName();
            rs.size = reportItem->size();
            m_geometryStamp.append(rs);
        }
    }
}

void PageDesignIntf::checkSizeOrPosChanges()
{
    CommandIf::Ptr posCommand;
    if ((selectedItems().count() > 0) && (m_positionStamp.count() > 0)) {
        bool bandFound = false;
        foreach(QGraphicsItem* item, selectedItems()){
            BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(item);
            if (band){
                bandFound = true;
                break;
            }
        }
        if (!bandFound && (m_positionStamp[0].pos != selectedItems().at(0)->pos())) {
            posCommand = createChangePosCommand();
        }
        m_positionStamp.clear();
    }

    CommandIf::Ptr sizeCommand;
    if ((selectedItems().count() > 0) && (m_geometryStamp.count() > 0)) {
        BaseDesignIntf *reportItem = dynamic_cast<BaseDesignIntf *>(selectedItems()[0]);
        if (reportItem && (m_geometryStamp[0].size != reportItem->size())) {
            sizeCommand = createChangeSizeCommand();
        }
        m_geometryStamp.clear();
    }

    if (sizeCommand && posCommand){
        CommandGroup::Ptr cm = CommandGroup::create();
        cm->addCommand(sizeCommand, false);
        cm->addCommand(posCommand, false);
        saveCommand(cm);
    } else {
        if (sizeCommand) saveCommand(sizeCommand);
        if (posCommand) saveCommand(posCommand);
    }

    m_changeSizeMode = false;
    m_changePosMode = false;
    m_changePosOrSizeMode = false;

}

CommandIf::Ptr PageDesignIntf::createChangePosCommand()
{
    QVector<ReportItemPos> newPoses;
    foreach(ReportItemPos itemPos, m_positionStamp) {
        BaseDesignIntf *reportItem = reportItemByName(itemPos.objectName);
        if (reportItem) {
            ReportItemPos newPos;
            newPos.objectName = reportItem->objectName();
            newPos.pos = reportItem->pos();
            newPoses.append(newPos);
        }
    }
    return PosChangedCommand::create(this, m_positionStamp, newPoses);
}

CommandIf::Ptr PageDesignIntf::createChangeSizeCommand()
{
    QVector<ReportItemSize> newSizes;

    foreach(ReportItemSize itemPos, m_geometryStamp) {
        BaseDesignIntf *reportItem = reportItemByName(itemPos.objectName);

        if (reportItem) {
            ReportItemSize newSize;
            newSize.objectName = reportItem->objectName();
            newSize.size = reportItem->size();
            newSizes.append(newSize);
        }
    }
    return SizeChangedCommand::create(this, m_geometryStamp, newSizes);
}

void PageDesignIntf::reactivatePageItem(PageItemDesignIntf::Ptr pageItem)
{
    pageItem->setItemMode(itemMode());
    if (pageItem.data()->scene()!=this)
        addItem(pageItem.data());
}

void PageDesignIntf::animateItem(BaseDesignIntf *item)
{
    if (item && (item->metaObject()->indexOfProperty("backgroundColor")>-1)){

        foreach (QObject* obj, m_animationList) {
            QPropertyAnimation* animation = dynamic_cast<QPropertyAnimation*>(obj);
            if (animation->targetObject() == item) return;
        }

        QPropertyAnimation* ani1 = new QPropertyAnimation(item,"backgroundColor");
        m_animationList.append(ani1);

        QColor startColor = QColor(Qt::red);
        QColor endColor = item->backgroundColor();

        ani1->setDuration(500);
        ani1->setEasingCurve(QEasingCurve::Linear);
        ani1->setStartValue(startColor);
        ani1->setEndValue(endColor);
        ani1->start(QAbstractAnimation::DeleteWhenStopped);


        connect(ani1,SIGNAL(destroyed(QObject*)), this, SLOT(slotAnimationStoped(QObject*)));
    }
}

void PageDesignIntf::registerItem(BaseDesignIntf *item)
{
    item->setItemMode(itemMode());
    BandDesignIntf *band = dynamic_cast<BandDesignIntf *>(item);
    if (band){
        registerBand(band);
        connect(band, SIGNAL(propertyObjectNameChanged(QString,QString)), this, SLOT(slotItemPropertyObjectNameChanged(QString,QString)));
        emit bandAdded(this,band);
    } else {
        connect(item, SIGNAL(propertyChanged(QString,QVariant,QVariant)), this, SLOT(slotItemPropertyChanged(QString,QVariant,QVariant)));
        connect(item, SIGNAL(propertyObjectNameChanged(QString,QString)), this, SLOT(slotItemPropertyObjectNameChanged(QString,QString)));
        emit itemAdded(this,item);
    }
}

void PageDesignIntf::emitRegisterdItem(BaseDesignIntf *item){
    emit itemAdded(this,item);
}

void PageDesignIntf::emitItemRemoved(BaseDesignIntf *item)
{
    BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(item);
    if (band){
        emit bandRemoved(this,band);
    } else {
        emit itemRemoved(this,item);
    }
}

DataSourceManager *PageDesignIntf::datasourceManager()
{
    if (m_reportEditor) return m_reportEditor->dataManager();
    return 0;
}

void PageDesignIntf::registerBand(BandDesignIntf *band)
{
    if (pageItem()&&!pageItem()->isBandRegistred(band)) {
        pageItem()->registerBand(band);
        if (itemMode() == DesignMode) pageItem()->relocateBands();
    }
}

void PageDesignIntf::slotUpdateItemSize()
{
    foreach(QGraphicsItem * item, items()) {
        BandDesignIntf *reportBand = dynamic_cast<BandDesignIntf *>(item);

        if (reportBand) reportBand->updateItemSize(0);
    }
}

void PageDesignIntf::saveChangeProppertyCommand(const QString &objectName, const QString &propertyName, const QVariant &oldPropertyValue, const QVariant &newPropertyValue)
{
    if (!m_executingCommand) {
        CommandIf::Ptr command;
        if (propertyName.compare("ItemAlign",Qt::CaseInsensitive)==0){
            command = PropertyItemAlignChangedCommand::create(this, objectName,
                                                              BaseDesignIntf::ItemAlign(oldPropertyValue.toInt()),
                                                              BaseDesignIntf::ItemAlign(newPropertyValue.toInt())
            );
        } else {
            command = PropertyChangedCommand::create(this, objectName, propertyName, oldPropertyValue, newPropertyValue);
        }
        saveCommand(command, false);
    }
}

void PageDesignIntf::changeSelectedGroupProperty(const QString &name, const QVariant &value)
{
    if ((selectedItems().count() > 0) && m_firstSelectedItem) {
        CommandGroup::Ptr cm = CommandGroup::create();
        m_executingCommand = true;
        foreach(QGraphicsItem * item, selectedItems()) {
            BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(item);
            if (bdItem) {
                QVariant oldValue = bdItem->property(name.toLatin1());
                if (oldValue.isValid()){
                    bdItem->setProperty(name.toLatin1(),value);
                    CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), name, oldValue, value);
                    cm->addCommand(command, false);
                }
            }
        }
        m_executingCommand = false;
        saveCommand(cm, false);
    }
}

PageItemDesignIntf* PageDesignIntf::getCurrentPage() const
{
    return m_currentPage;
}

void PageDesignIntf::setCurrentPage(PageItemDesignIntf* currentPage)
{
    if (m_currentPage != currentPage ){
        if (m_currentPage) m_currentPage->setItemMode(PreviewMode);
        m_currentPage = currentPage;
        if (m_itemMode == DesignMode){
            m_currentPage->setItemMode(DesignMode);
        }
    }
}

ReportSettings *PageDesignIntf::getReportSettings() const
{
    return m_reportSettings;
}

void PageDesignIntf::setReportSettings(ReportSettings *reportSettings)
{
    m_reportSettings = reportSettings;
    m_pageItem->setReportSettings(m_reportSettings);
}

void PageDesignIntf::setPropertyToSelectedItems(const char* name, const QVariant& value)
{
    foreach(QGraphicsItem* gi, selectedItems()){
        BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(gi);
        if(item && item->metaObject()->indexOfProperty(name) != -1 ) item->setProperty(name,value);
    }
}

bool PageDesignIntf::magneticMovement() const
{
    return m_magneticMovement;
}

void PageDesignIntf::setMagneticMovement(bool magneticMovement)
{
    m_magneticMovement = magneticMovement;
}

int PageDesignIntf::horizontalGridStep() const
{
    return m_horizontalGridStep;
}

void PageDesignIntf::setHorizontalGridStep(int horizontalGridStep)
{
    m_horizontalGridStep = horizontalGridStep;
}

void PageDesignIntf::endUpdate()
{
    m_updating = false;
    emit pageUpdateFinished(this);
}


void PageDesignIntf::activateItemToJoin(QRectF itemRect, QList<ItemProjections>& items){
    QRectF r1(itemRect.x(), itemRect.y()-50, itemRect.width(), itemRect.height()+100);
    QRectF r2(itemRect.x()-50, itemRect.y(), itemRect.width()+100, itemRect.height());
    qreal maxSquare = 0;

    if (m_joinItem) {
        m_joinItem->turnOnJoinMarker(false);
        m_joinItem = 0;
    }

    foreach(ItemProjections p, items){
        qreal tmpSquare = qMax(p.square(r1)/itemRect.width(),p.square(r2)/itemRect.height());
        if (tmpSquare>maxSquare) {
            maxSquare = tmpSquare;
            m_joinItem = p.item();
            if (p.square(r1)/itemRect.width() > p.square(r2) / itemRect.height())
                m_joinType = Width;
            else
                m_joinType = Height;
        }
    }

    if (m_joinItem) m_joinItem->turnOnJoinMarker(true);
}

void PageDesignIntf::selectAllChildren(BaseDesignIntf *item)
{
    if (item)
        foreach(BaseDesignIntf* child, item->childBaseItems()){
            child->setSelected(true);
        }
}

void PageDesignIntf::rectMoved(QRectF itemRect, BaseDesignIntf* container){
    if (!container){
        container = bandAt(QPointF(itemRect.topLeft()));
        if (!container) container = this->pageItem();
    }

    if (container){
        m_projections.clear();
        foreach(BaseDesignIntf* bi, container->childBaseItems()){
            m_projections.append(ItemProjections(bi));
        }
    }

    activateItemToJoin(itemRect, m_projections);

}

void PageDesignIntf::itemMoved(BaseDesignIntf *item)
{
    if (m_movedItem!=item){
        m_movedItem = item;
        BaseDesignIntf* curItem = dynamic_cast<BaseDesignIntf*>(item->parentItem()); ;
        while (curItem){
            m_movedItemContainer = dynamic_cast<BandDesignIntf*>(curItem);
            if (!m_movedItemContainer)
                m_movedItemContainer = dynamic_cast<PageItemDesignIntf*>(curItem);
            if (m_movedItemContainer) break;
            else curItem = dynamic_cast<BaseDesignIntf*>(curItem->parentItem());
        }
        if (m_movedItemContainer){
            m_projections.clear();
            foreach(BaseDesignIntf* bi, m_movedItemContainer->childBaseItems()){
                if (bi != item)
                    m_projections.append(ItemProjections(bi));
            }
        }
    }

    activateItemToJoin(item->geometry(), m_projections);
//    QRectF r1(item->pos().x(),item->pos().y()-50,item->width(),item->height()+100);
//    QRectF r2(item->pos().x()-50,item->pos().y(),item->width()+100,item->height());
//    qreal maxSquare = 0;

//    if (m_joinItem) {
//        m_joinItem->turnOnJoinMarker(false);
//        m_joinItem = 0;
//    }

//    foreach(ItemProjections p, m_projections){
//        qreal tmpSquare = qMax(p.square(r1)/item->width(),p.square(r2)/item->height());
//        if (tmpSquare>maxSquare) {
//            maxSquare = tmpSquare;
//            m_joinItem = p.item();
//            if (p.square(r1)/item->width()>p.square(r2)/item->height())
//                m_joinType = Width;
//            else
//                m_joinType = Height;
//        }
//    }

//    if (m_joinItem) m_joinItem->turnOnJoinMarker(true);

}

int PageDesignIntf::verticalGridStep() const
{
    return m_verticalGridStep;
}

void PageDesignIntf::setVerticalGridStep(int verticalGridStep)
{
    m_verticalGridStep = verticalGridStep;
}

Qt::AlignmentFlag transformFlags(bool horizontalAlign, Qt::AlignmentFlag value, Qt::AlignmentFlag flag){
    int tmpValue = value;
    if (horizontalAlign){
        tmpValue &= ~(Qt::AlignHCenter  | Qt::AlignLeft | Qt::AlignRight | Qt::AlignJustify);
        tmpValue |= flag;
    } else {
        tmpValue &= ~(Qt::AlignVCenter | Qt::AlignTop | Qt::AlignBottom);
        tmpValue |= flag;
    }
    return Qt::AlignmentFlag(tmpValue);
}

void PageDesignIntf::changeSelectedGrpoupTextAlignPropperty(const bool& horizontalAlign, Qt::AlignmentFlag flag)
{
    if (selectedItems().count() > 0) {
        CommandGroup::Ptr cm = CommandGroup::create();
        m_executingCommand = true;
        foreach(QGraphicsItem * item, selectedItems()) {
            BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(item);
            if (bdItem) {
                QVariant oldValue = bdItem->property("alignment");
                if (oldValue.isValid()){
                    QVariant value = transformFlags(horizontalAlign, Qt::AlignmentFlag(oldValue.toInt()), flag);
                    bdItem->setProperty("alignment",value);
                    CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "alignment", oldValue, value);
                    cm->addCommand(command, false);
                }
            }
        }
        m_executingCommand = false;
        saveCommand(cm, false);
    }
}

void PageDesignIntf::undo()
{
    if (m_currentCommand >= 0) {
        m_executingCommand = true;
        m_commandsList.at(m_currentCommand)->undoIt();
        m_currentCommand--;
        m_hasChanges = true;
        m_executingCommand = false;
    }
}

void PageDesignIntf::redo()
{
    if (m_currentCommand < m_commandsList.count() - 1) {
        m_executingCommand = true;
        m_currentCommand++;
        m_commandsList.at(m_currentCommand)->doIt();
        m_hasChanges = true;
        m_executingCommand = false;
    }
}

void PageDesignIntf::copy()
{
    if (!selectedItems().isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        ItemsWriterIntf *writer = new XMLWriter;
        bool shouldWrite = false;
        foreach(QGraphicsItem * item, selectedItems()) {
            ItemDesignIntf *reportItem = dynamic_cast<ItemDesignIntf *>(item);

            if (reportItem) {
                writer->putItem(reportItem);
                shouldWrite = true;
            }
        }

        if (shouldWrite) {
            clipboard->setText(writer->saveToString());
        }

        delete writer;
    }
}

BaseDesignIntf* PageDesignIntf::findDestObject(BaseDesignIntf* item){
    if (item && item->canAcceptPaste()) return item;
    BaseDesignIntf * curItem = item;
    while (curItem && !curItem->canAcceptPaste()){
        curItem = dynamic_cast<BaseDesignIntf*>(curItem->parentItem());
    }
    return curItem;
}

void PageDesignIntf::paste()
{
    QClipboard *clipboard = QApplication::clipboard();
    ItemsReaderIntf::Ptr reader = StringXMLreader::create(clipboard->text());
    if (reader->first() && reader->itemType() == "Object"){
        BaseDesignIntf* destItem = 0;
        if (!selectedItems().isEmpty())
            destItem = findDestObject(dynamic_cast<BaseDesignIntf*>(selectedItems().at(0)));
        else
            destItem = this->pageItem();
        if (destItem){
            CommandIf::Ptr command = PasteCommand::create(this, clipboard->text(), destItem);
            saveCommand(command);
        }
    }
}

void PageDesignIntf::deleteSelected()
{
    if (selectedItems().count()==1){
        if (!dynamic_cast<PageItemDesignIntf*>(selectedItems().at(0)))
          saveCommand(removeReportItemCommand(dynamic_cast<BaseDesignIntf*>(selectedItems().at(0))));
    } else {

        QList<QGraphicsItem*> itemsToDelete = selectedItems();

        CommandGroup::Ptr commandGroup = CommandGroup::create();

        QList<BandDesignIntf*> bands;
        QList<QGraphicsItem*>::iterator it;

        for(it = itemsToDelete.begin(); it != itemsToDelete.end();){
            BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(*it);
            if (band) {
                bands.append(band);
                it = itemsToDelete.erase(it);
            } else {
                ++it;
            }
        }

        foreach (BandDesignIntf* band, bands){
            foreach (QGraphicsItem* bandItem, band->childItems()) {
                itemsToDelete.removeOne(bandItem);
            }
        }

        if (!itemsToDelete.isEmpty()){
            foreach(QGraphicsItem* item, itemsToDelete){
                if (!dynamic_cast<PageItemDesignIntf*>(item))
                    commandGroup->addCommand(removeReportItemCommand(dynamic_cast<BaseDesignIntf*>(item)),false);
            }
        }

        if (!bands.isEmpty()){
            foreach (BandDesignIntf* band, bands) {
                commandGroup->addCommand(removeReportItemCommand(band),false);
            }
        }

        saveCommand(commandGroup);
    }

}

void PageDesignIntf::cut()
{
    CommandIf::Ptr command = CutCommand::create(this);
    saveCommand(command);
}

void PageDesignIntf::setToSaved()
{
    m_hasChanges = false;
}

void PageDesignIntf::bringToFront()
{
    foreach(QGraphicsItem * item, selectedItems()) {
        qreal zOrder = 0;
        foreach(QGraphicsItem * colItem, collidingItems(item)) {
            if (zOrder <= colItem->zValue())
                zOrder = colItem->zValue() + 0.1;
        }
        BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(item);

        if (bdItem){
            saveChangeProppertyCommand(bdItem->objectName(), "zOrder", bdItem->zValue(), zOrder);
            BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(item);
            PageItemDesignIntf* page = dynamic_cast<PageItemDesignIntf*>(item);
            if (!band && !page)
                bdItem->setZValueProperty(zOrder);
        } else {
            item->setZValue(zOrder);
        }
    }
}

void PageDesignIntf::sendToBack()
{
    foreach(QGraphicsItem * item, selectedItems()) {
        qreal zOrder = 0;
        foreach(QGraphicsItem * colItem, collidingItems(item)) {
            if (zOrder >= colItem->zValue())
                zOrder = colItem->zValue() - 0.1;
        }
        BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(item);
        if (bdItem){
            saveChangeProppertyCommand(bdItem->objectName(), "zOrder", bdItem->zValue(), zOrder);
            BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(item);
            PageItemDesignIntf* page = dynamic_cast<PageItemDesignIntf*>(item);
            if (!band && !page)
                bdItem->setZValueProperty(zOrder);
        } else {
            item->setZValue(zOrder);
        }
    }
}

bool PageDesignIntf::selectionContainsBand(){
    foreach(QGraphicsItem * item,selectedItems()){
        BandDesignIntf *band = dynamic_cast<BandDesignIntf *>(item);
        if (band) return true;
    }
    return false;
}

void PageDesignIntf::alignToLeft()
{
    if ((selectedItems().count() > 0) && m_firstSelectedItem) {
        CommandGroup::Ptr cm = CommandGroup::create();
        bool moveInBand = selectionContainsBand();
        foreach(QGraphicsItem * item, selectedItems()) {
            BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(item);
            if (bdItem && !bdItem->isGeometryLocked()) {
                QRectF oldGeometry = bdItem->geometry();
                bdItem->setPos(QPointF(moveInBand ? 0 : m_firstSelectedItem->pos().x(), item->pos().y()));
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, bdItem->geometry());
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, false);
    }
}

void PageDesignIntf::alignToRigth()
{
    if ((selectedItems().count() > 0) && m_firstSelectedItem) {
        CommandGroup::Ptr cm = CommandGroup::create();
        bool moveInBand = selectionContainsBand();
        foreach(QGraphicsItem * item, selectedItems()) {
            BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(item);
            if (bdItem && !bdItem->isGeometryLocked() && !bdItem->isBand()) {
                QRectF oldGeometry = bdItem->geometry();
                if (moveInBand && dynamic_cast<BandDesignIntf*>(bdItem->parent()))
                {
                    bdItem->setPos(QPointF(dynamic_cast<BandDesignIntf*>(bdItem->parent())->geometry().width() - bdItem->width(),
                                          bdItem->pos().y()));
                } else {
                    qreal x = m_firstSelectedItem->geometry().right() - bdItem->width();
                    bdItem->setPos(QPointF(x+1, bdItem->pos().y()));
                }
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, bdItem->geometry());
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, false);
    }
}

void PageDesignIntf::alignToVCenter()
{
    if ((selectedItems().count() > 0) && m_firstSelectedItem) {
        CommandGroup::Ptr cm = CommandGroup::create();
        bool moveInBand = selectionContainsBand();
        foreach(QGraphicsItem * item, selectedItems()) {
            BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(item);
            if (bdItem && !bdItem->isGeometryLocked() && !bdItem->isBand()) {
                QRectF oldGeometry = bdItem->geometry();
                if (moveInBand && dynamic_cast<BandDesignIntf*>(bdItem->parent())){
                    bdItem->setPos(QPointF((dynamic_cast<BandDesignIntf*>(bdItem->parent())->geometry().width() / 2) - bdItem->width() / 2,
                                          bdItem->pos().y()));
                } else {
                    qreal x = (m_firstSelectedItem->geometry().right() - m_firstSelectedItem->width() / 2) - bdItem->width() / 2;
                    bdItem->setPos(QPointF(x+1, bdItem->pos().y()));
                }
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, bdItem->geometry());
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, false);
    }
}

void PageDesignIntf::alignToTop()
{
    if ((selectedItems().count() > 0) && m_firstSelectedItem) {
        CommandGroup::Ptr cm = CommandGroup::create();
        bool moveInBand = selectionContainsBand();
        foreach(QGraphicsItem * item, selectedItems()) {
            BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(item);
            if (bdItem && !bdItem->isGeometryLocked() && !bdItem->isBand()) {
                QRectF oldGeometry = bdItem->geometry();
                if (moveInBand){
                    bdItem->setPos(QPointF(0, m_firstSelectedItem->pos().y()));
                } else {
                    bdItem->setPos(QPointF(bdItem->pos().x(), m_firstSelectedItem->pos().y()));
                }
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, bdItem->geometry());
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, false);
    }
}

void PageDesignIntf::alignToBottom()
{
    if ((selectedItems().count() > 0) && m_firstSelectedItem) {
        CommandGroup::Ptr cm = CommandGroup::create();
        bool moveInBand = selectionContainsBand();
        foreach(QGraphicsItem * item, selectedItems()) {
            BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(item);
            if (bdItem && !bdItem->isGeometryLocked() && !bdItem->isBand()) {
                QRectF oldGeometry = bdItem->geometry();
                if (moveInBand && dynamic_cast<BandDesignIntf*>(bdItem->parent())){
                   bdItem->setPos(QPointF(bdItem->pos().x(), dynamic_cast<BandDesignIntf*>(bdItem->parent())->height() - bdItem->height()));
                } else {
                    qreal y = m_firstSelectedItem->geometry().bottom() - bdItem->height();
                    bdItem->setPos(QPointF(bdItem->pos().x(), y+1));
                }
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, bdItem->geometry());
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, false);
    }
}

void PageDesignIntf::alignToHCenter()
{
    if ((selectedItems().count() > 0) && m_firstSelectedItem) {
        CommandGroup::Ptr cm = CommandGroup::create();
        bool moveInBand = selectionContainsBand();
        foreach(QGraphicsItem * item, selectedItems()) {
            BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(item);
            if (bdItem && !bdItem->isGeometryLocked() && !bdItem->isBand()) {
                QRectF oldGeometry = bdItem->geometry();
                if (moveInBand && dynamic_cast<BandDesignIntf*>(bdItem->parent())){
                    bdItem->setPos(QPointF(bdItem->pos().x(), (dynamic_cast<BandDesignIntf*>(bdItem->parent())->height() / 2) - bdItem->height() / 2));
                } else {
                    qreal y = (m_firstSelectedItem->geometry().bottom() - m_firstSelectedItem->height() / 2) - bdItem->height() / 2;
                    bdItem->setPos(QPointF(bdItem->pos().x(), y+1));
                }
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, bdItem->geometry());
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, false);
    }
}

void PageDesignIntf::sameWidth()
{
    if ((selectedItems().count() > 0) && m_firstSelectedItem) {
        CommandGroup::Ptr cm = CommandGroup::create();
        foreach(QGraphicsItem * item, selectedItems()) {
            BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(item);
            if (bdItem && !bdItem->isGeometryLocked()) {
                QRectF oldGeometry = bdItem->geometry();
                bdItem->setWidth(m_firstSelectedItem->width());
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, bdItem->geometry());
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, false);
    }
}

void PageDesignIntf::sameHeight()
{
    if ((selectedItems().count() > 0) && m_firstSelectedItem) {
        CommandGroup::Ptr cm = CommandGroup::create();
        foreach(QGraphicsItem * item, selectedItems()) {
            BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(item);
            if (bdItem && !bdItem->isGeometryLocked()) {
                QRectF oldGeometry = bdItem->geometry();
                bdItem->setHeight(m_firstSelectedItem->height());
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, bdItem->geometry());
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, false);
    }
}

void PageDesignIntf::addHLayout()
{

    if (selectedItems().isEmpty()) return;

    QList<QGraphicsItem *> si = selectedItems();
    QList<QGraphicsItem *>::iterator it = si.begin();

    int itemsCount = 0;
    for (; it != si.end();) {
        if (dynamic_cast<ItemDesignIntf *>(*it)){
            itemsCount++;
            break;
        }
        ++it;
    };

    if (itemsCount == 0) return;

    for (; it != si.end();) {
        if (!dynamic_cast<ItemDesignIntf *>(*it)) {
            (*it)->setSelected(false);
            it = si.erase(it);
        }
        else ++it;
    }

    if (!si.isEmpty()){
        it = si.begin();
        QGraphicsItem* elementsParent = (*it)->parentItem();
        for (; it != si.end();++it) {
            if ((*it)->parentItem()!=elementsParent){
                QMessageBox::information(0,QObject::tr("Attention!"),QObject::tr("Selected elements have different parent containers"));
                return;
            }
        }
        CommandIf::Ptr cm = InsertHLayoutCommand::create(this);
        saveCommand(cm,true);
    }


}

void PageDesignIntf::addVLayout()
{
    if (selectedItems().isEmpty()) return;

    QList<QGraphicsItem *> si = selectedItems();
    QList<QGraphicsItem *>::iterator it = si.begin();

    int itemsCount = 0;
    for (; it != si.end();) {
        if (dynamic_cast<ItemDesignIntf *>(*it)){
            itemsCount++;
            break;
        }
        ++it;
    };

    if (itemsCount == 0) return;

    for (; it != si.end();) {
        if (!dynamic_cast<ItemDesignIntf *>(*it)) {
            (*it)->setSelected(false);
            it = si.erase(it);
        }
        else ++it;
    }

    if (!si.isEmpty()){
        it = si.begin();
        QGraphicsItem* elementsParent = (*it)->parentItem();
        for (; it != si.end();++it) {
            if ((*it)->parentItem()!=elementsParent){
                QMessageBox::information(0,QObject::tr("Attention!"),QObject::tr("Selected elements have different parent containers"));
                return;
            }
        }
        CommandIf::Ptr cm = InsertVLayoutCommand::create(this);
        saveCommand(cm,true);
    }
}

bool hLayoutLessThen(QGraphicsItem *c1, QGraphicsItem *c2)
{
    return c1->pos().x() < c2->pos().x();
}

HorizontalLayout* PageDesignIntf::internalAddHLayout()
{
    if (m_firstSelectedItem && (selectedItems().count() > 1)) {

        QList<QGraphicsItem *> si = selectedItems();
        QList<QGraphicsItem *>::iterator it = si.begin();
        std::sort(si.begin(), si.end(), hLayoutLessThen);
        it = si.begin();

        if (si.count() > 1) {

            it = si.begin();
            ItemDesignIntf *firstElement = dynamic_cast<ItemDesignIntf *>(*it);

            HorizontalLayout *layout = new HorizontalLayout(firstElement->parent(), firstElement->parentItem());
            layout->setItemLocation(firstElement->itemLocation());
            layout->setPos(firstElement->pos());
            layout->setWidth(0);
            layout->setHeight(firstElement->height());

            for (; it != si.end(); ++it) {
                BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(*it);
                layout->addChild(bdItem);
            }

            foreach(QGraphicsItem * item, selectedItems()) {
                item->setSelected(false);
            }

            layout->setObjectName(genObjectName(*layout));
            layout->setItemTypeName("HorizontalLayout");
            layout->setSelected(true);
            registerItem(layout);
            return layout;
        }
    }
    return 0;
}

bool vLayoutLessThen(QGraphicsItem *c1, QGraphicsItem *c2)
{
    return c1->pos().y() < c2->pos().y();
}

VerticalLayout* PageDesignIntf::internalAddVLayout()
{
    if (m_firstSelectedItem && (selectedItems().count() > 1)) {

        QList<QGraphicsItem *> si = selectedItems();
        QList<QGraphicsItem *>::iterator it = si.begin();
        std::sort(si.begin(), si.end(), vLayoutLessThen);
        it = si.begin();

        if (si.count() > 1) {

            it = si.begin();
            ItemDesignIntf *firstElement = dynamic_cast<ItemDesignIntf *>(*it);

            VerticalLayout *layout = new VerticalLayout(firstElement->parent(), firstElement->parentItem());
            layout->setItemLocation(firstElement->itemLocation());
            layout->setPos(firstElement->pos());
            layout->setWidth(firstElement->width());
            layout->setHeight(0);

            for (; it != si.end(); ++it) {
                BaseDesignIntf *bdItem = dynamic_cast<BaseDesignIntf *>(*it);
                layout->addChild(bdItem);
            }

            foreach(QGraphicsItem * item, selectedItems()) {
                item->setSelected(false);
            }

            layout->setObjectName(genObjectName(*layout));
            layout->setItemTypeName("VerticalLayout");
            layout->setSelected(true);
            registerItem(layout);
            return layout;
        }
    }
    return 0;
}

void PageDesignIntf::setFont(const QFont& font)
{
    changeSelectedGroupProperty("font",font);
}

void PageDesignIntf::setTextAlign(const Qt::Alignment& alignment)
{
    changeSelectedGroupProperty("alignment",QVariant(alignment));
}

void PageDesignIntf::setBorders(const BaseDesignIntf::BorderLines& border)
{
    changeSelectedGroupProperty("borders", (int)border);
}

void PageDesignIntf::setBordersExt(
        const BaseDesignIntf::BorderLines& border,
        const double borderWidth,
        const LimeReport::BaseDesignIntf::BorderStyle style,
        const QString color

)
{
    changeSelectedGroupProperty("borders", (int)border);
    changeSelectedGroupProperty("borderLineSize", borderWidth);
    changeSelectedGroupProperty("borderStyle", style);
    changeSelectedGroupProperty("borderColor", color);
}

void PageDesignIntf::lockSelectedItems()
{
    foreach(QGraphicsItem* graphicItem, selectedItems()){
        BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(graphicItem);
        if (item) item->setProperty("geometryLocked", true);
    }
}

void PageDesignIntf::unlockSelectedItems()
{
    foreach(QGraphicsItem* graphicItem, selectedItems()){
        BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(graphicItem);
        if (item) item->setProperty("geometryLocked", false);
    }
}


void PageDesignIntf::selectOneLevelItems()
{
    foreach(QGraphicsItem* graphicItem, selectedItems()){
        BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(graphicItem->parentItem());
        if (item)
            selectAllChildren(item);
        else
            selectAllChildren(dynamic_cast<BaseDesignIntf*>(graphicItem));
    }
}

void PageDesignIntf::removeAllItems()
{
    pageItem()->clear();
    m_commandsList.clear();
}

void PageDesignIntf::setItemMode(BaseDesignIntf::ItemMode mode)
{
    if (m_itemMode != mode){
        m_itemMode = mode;
        if (m_currentPage) {
            m_currentPage->setItemMode(mode);
        } else {
            foreach(QGraphicsItem * item, items()) {
                BaseDesignIntf *reportItem = dynamic_cast<BaseDesignIntf *>(item);
                if (reportItem) {
                    reportItem->setItemMode(itemMode());
                }
            }
        }
        foreach(PageItemDesignIntf::Ptr page, m_reportPages){
            page->setItemMode(mode);
        }
    }
}

BaseDesignIntf* PageDesignIntf::reportItemByName(const QString &name)
{

    foreach(QGraphicsItem * item, items()) {
        BaseDesignIntf *bd = dynamic_cast<BaseDesignIntf *>(item);
        if (bd && (bd->objectName().compare(name, Qt::CaseInsensitive) == 0)) return bd;
    }

    return 0;
}

QList<BaseDesignIntf*> PageDesignIntf::reportItemsByName(const QString &name){
    QList<BaseDesignIntf*> result;
    foreach(QGraphicsItem * item, items()) {
        BaseDesignIntf *bd = dynamic_cast<BaseDesignIntf *>(item);
        if (bd && (bd->objectName().compare(name, Qt::CaseInsensitive) == 0)) result.append(bd);
    }
    return result;
}

BandDesignIntf *PageDesignIntf::bandAt(QPointF pos)
{
    BandDesignIntf *band=0;
    foreach(QGraphicsItem * item, items(pos)) {
        band = dynamic_cast<BandDesignIntf *>(item);
        if (band) break;
    }
    return band;
}

void CommandIf::addCommand(Ptr command, bool execute)
{
    Q_UNUSED(command)
    Q_UNUSED(execute)
}

CommandIf::Ptr InsertItemCommand::create(PageDesignIntf *page, const QString &itemType, QPointF pos, QSizeF size)
{
    InsertItemCommand *command = new InsertItemCommand();
    command->setPage(page);
    command->setType(itemType);
    command->setPos(pos);
    command->setSize(size);
    return CommandIf::Ptr(command);
}

bool InsertItemCommand::doIt()
{
    BaseDesignIntf *item = page()->addReportItem(m_itemType, m_pos, m_size);
    if (item) m_itemName = item->objectName();
    return item != 0;
}

void InsertItemCommand::undoIt()
{
    BaseDesignIntf *item = page()->reportItemByName(m_itemName);
    if (item){
        page()->removeReportItem(item,false);
    }
//    page()->removeItem(item);
//    delete item;
}

CommandIf::Ptr DeleteItemCommand::create(PageDesignIntf *page, BaseDesignIntf *item)
{
    DeleteItemCommand *command = new DeleteItemCommand();
    //QScopedPointer<ItemsWriterIntf> writer(new XMLWriter());
    //writer->putItem(item);
    command->setPage(page);
    command->setItem(item);
    LayoutDesignIntf* layout = dynamic_cast<LayoutDesignIntf*>(item->parent());
    if (layout)
        command->m_layoutName = layout->objectName();
    //command->m_itemXML = writer->saveToString();
    return CommandIf::Ptr(command);
}

bool DeleteItemCommand::doIt()
{
    BaseDesignIntf *item = page()->reportItemByName(m_itemName);
    if (item) {
        item->beforeDelete();
        page()->removeItem(item);
        page()->emitItemRemoved(item);
        delete item;
        return true;
    }
    return false;
}

void DeleteItemCommand::undoIt()
{
    BaseDesignIntf *item = page()->createReportItem(m_itemType);
    ItemsReaderIntf::Ptr reader = StringXMLreader::create(m_itemXML);
    if (reader->first()) reader->readItem(item);
    BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(item);
    if (band){
        page()->pageItem()->increaseBandIndex(band->bandIndex());
    }
    page()->registerItem(item);

    if (!m_layoutName.isEmpty()) {
        LayoutDesignIntf* layout = dynamic_cast<LayoutDesignIntf*>(page()->reportItemByName(m_layoutName));
        if (layout){
            layout->restoreChild(item);
        }
        page()->emitRegisterdItem(item);
    }
}

void DeleteItemCommand::setItem(BaseDesignIntf *value)
{
    m_itemName = value->objectName();
    m_itemType = value->storageTypeName();
    QScopedPointer<ItemsWriterIntf> writer(new XMLWriter());
    writer->putItem(value);
    m_itemXML = writer->saveToString();
}

CommandIf::Ptr DeleteLayoutCommand::create(PageDesignIntf *page, LayoutDesignIntf *item)
{
    DeleteLayoutCommand* command = new DeleteLayoutCommand();
    command->setPage(page);
    command->setItem(item);
    foreach (BaseDesignIntf* childItem, item->childBaseItems()){
        command->m_childItems.append(childItem->objectName());
    }
    LayoutDesignIntf* layout = dynamic_cast<LayoutDesignIntf*>(item->parent());
    if (layout)
        command->m_layoutName = layout->objectName();
    return CommandIf::Ptr(command);
}

bool DeleteLayoutCommand::doIt()
{
    BaseDesignIntf *item = page()->reportItemByName(m_itemName);
    if (item) {
        item->beforeDelete();
        QScopedPointer<ItemsWriterIntf> writer(new XMLWriter());
        writer->putItem(item);
        m_itemXML = writer->saveToString();
        page()->removeItem(item);
        page()->emitItemRemoved(item);
        delete item;
        return true;
    }
    return false;
}

void DeleteLayoutCommand::undoIt()
{
    BaseDesignIntf *item = page()->addReportItem(m_itemType);
    ItemsReaderIntf::Ptr reader = StringXMLreader::create(m_itemXML);
    if (reader->first()) reader->readItem(item);
    if (!m_layoutName.isEmpty()) {
        LayoutDesignIntf* layout = dynamic_cast<LayoutDesignIntf*>(page()->reportItemByName(m_layoutName));
        if (layout){
            layout->restoreChild(item);
        }
        page()->emitRegisterdItem(item);
    }
    foreach(QString ci, m_childItems){
        BaseDesignIntf* ri = page()->reportItemByName(ci);
        if (ri){
            LayoutDesignIntf* parentLayout = dynamic_cast<LayoutDesignIntf*>(ri->parent());
            if (parentLayout) {
                parentLayout->removeChild(ri);
            }
            dynamic_cast<LayoutDesignIntf*>(item)->addChild(ri);
        }
        page()->emitRegisterdItem(item);
    }
}

void DeleteLayoutCommand::setItem(BaseDesignIntf *item)
{
    m_itemName = item->objectName();
    m_itemType = item->storageTypeName();
}

CommandIf::Ptr PasteCommand::create(PageDesignIntf *page, const QString &itemsXML, BaseDesignIntf *parent)
{
    PasteCommand *command = new PasteCommand();
    command->setPage(page);
    command->setItemsXML(itemsXML);
    command->setParent(parent);
    return CommandIf::Ptr(command);
}

bool PasteCommand::doIt()
{
    m_itemNames.clear();

    ItemsReaderIntf::Ptr reader = StringXMLreader::create(m_itemsXML);

    if (reader->first() && reader->itemType() == "Object") {
        page()->beginUpdate();
        insertItem(reader);

        while (reader->next()) {
            insertItem(reader);
        }
        page()->endUpdate();
    }
    else return false;

    page()->selectedItems().clear();
    foreach(QString name, m_itemNames) {
        page()->reportItemByName(name)->setSelected(true);
    }
    return m_itemNames.count() > 0;
}

void PasteCommand::undoIt()
{
    foreach(QString name, m_itemNames) {
        BaseDesignIntf *item = page()->reportItemByName(name);
        page()->emitItemRemoved(item);
        page()->removeItem(item);
        delete item;
    }
}

void PasteCommand::setItemsXML(const QString &itemsXML)
{
    m_itemsXML = itemsXML;
}

bool PasteCommand::insertItem(ItemsReaderIntf::Ptr reader)
{
    BaseDesignIntf* parentItem = page()->reportItemByName(m_parentItemName);
    if (parentItem){
        BaseDesignIntf *item = page()->addReportItem(reader->itemClassName(), parentItem, parentItem);
        if (item) {
            QString objectName = item->objectName();
            reader->readItem(item);
            item->setParent(parentItem);
            item->setParentItem(parentItem);
            if (page()->reportItemsByName(item->objectName()).size()>1){
                item->setObjectName(objectName);
            }
            foreach (BaseDesignIntf* child, item->childBaseItems()){
                changeName(page(), child);
            };
            m_itemNames.push_back(item->objectName());
        }
        return true;
    }
    return false;
}

void PasteCommand::changeName(PageDesignIntf *page, BaseDesignIntf* item)
{
    item->setObjectName(page->genObjectName(*item));
    foreach(BaseDesignIntf* child, item->childBaseItems()){
        changeName(page, child);
    };
}

CommandIf::Ptr CutCommand::create(PageDesignIntf *page)
{
    CutCommand *command = new CutCommand();
    command->setPage(page);
    ItemsWriterIntf *writer = new XMLWriter();
    foreach(QGraphicsItem * item, page->selectedItems()) {
        if (!dynamic_cast<PageItemDesignIntf*>(item)){
            ItemDesignIntf *reportItem = dynamic_cast<ItemDesignIntf *>(item);

            if (reportItem) {
                command->m_itemNames.push_back(reportItem->objectName());
                writer->putItem(reportItem);
            }
        }
    }
    command->setXML(writer->saveToString());

    if (command->m_itemNames.count() > 0) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(writer->saveToString());
    }

    return CommandIf::Ptr(command);
}

bool CutCommand::doIt()
{
    foreach(QString name, m_itemNames) {
        BaseDesignIntf *item = page()->reportItemByName(name);
        page()->removeItem(item);
        page()->emitItemRemoved(item);
        delete item;
    }
    return m_itemNames.count() > 0;
}

void CutCommand::undoIt()
{
    ItemsReaderIntf::Ptr reader = StringXMLreader::create(m_itemsXML);

    if (reader->first()) {
        BaseDesignIntf *item = page()->addReportItem(reader->itemClassName());

        if (item) reader->readItem(item);

        while (reader->next()) {
            item = page()->addReportItem(reader->itemClassName());

            if (item) reader->readItem(item);
        }
    }
}

CommandIf::Ptr PosChangedCommand::create(PageDesignIntf *page, QVector<ReportItemPos> &oldPos, QVector<ReportItemPos> &newPos)
{
    PosChangedCommand *command = new PosChangedCommand();
    command->setPage(page);
    command->m_newPos = newPos;
    command->m_oldPos = oldPos;
    return CommandIf::Ptr(command);
}

bool PosChangedCommand::doIt()
{
    for (int i = 0; i < m_newPos.count(); i++) {
        BaseDesignIntf *reportItem = page()->reportItemByName(m_newPos[i].objectName);

        if (reportItem && (reportItem->pos() != m_newPos[i].pos)){
            QPointF oldValue = reportItem->pos();
            reportItem->setPos(m_newPos[i].pos);
            reportItem->emitPosChanged(oldValue, reportItem->pos());
        }
    }

    return true;
}

void PosChangedCommand::undoIt()
{
    for (int i = 0; i < m_oldPos.count(); i++) {
        BaseDesignIntf *reportItem = page()->reportItemByName(m_oldPos[i].objectName);

        if (reportItem && (reportItem->pos() != m_oldPos[i].pos)){
            QPointF oldValue = reportItem->pos();
            reportItem->setPos(m_oldPos[i].pos);
            reportItem->emitPosChanged(oldValue, reportItem->pos());
        }
    }
}

CommandIf::Ptr SizeChangedCommand::create(PageDesignIntf *page, QVector<ReportItemSize> &oldSize, QVector<ReportItemSize> &newSize)
{
    SizeChangedCommand *command = new SizeChangedCommand();
    command->setPage(page);
    command->m_newSize = newSize;
    command->m_oldSize = oldSize;
    return CommandIf::Ptr(command);
}

bool SizeChangedCommand::doIt()
{
    for (int i = 0; i < m_newSize.count(); i++) {
        BaseDesignIntf *reportItem = page()->reportItemByName(m_newSize[i].objectName);

        if (reportItem && (reportItem->size() != m_newSize[i].size)) reportItem->setSize(m_newSize[i].size);
    }

    return true;
}

void SizeChangedCommand::undoIt()
{
    for (int i = 0; i < m_oldSize.count(); i++) {
        BaseDesignIntf *reportItem = page()->reportItemByName(m_oldSize[i].objectName);

        if (reportItem && (reportItem->size() != m_oldSize[i].size)) reportItem->setSize(m_oldSize[i].size);
    }
}

CommandIf::Ptr PropertyChangedCommand::create(PageDesignIntf *page, const QString &objectName, const QString &propertyName,
        const QVariant &oldValue, const QVariant &newValue)
{
    PropertyChangedCommand *command = new PropertyChangedCommand();
    command->setPage(page);
    command->m_objectName = objectName;
    command->m_propertyName = propertyName;
    command->m_oldValue = oldValue;
    command->m_newValue = newValue;
    return CommandIf::Ptr(command);
}

bool PropertyChangedCommand::doIt()
{
    BaseDesignIntf *reportItem = page()->reportItemByName(m_objectName);

    if (reportItem && (reportItem->property(m_propertyName.toLatin1()) != m_newValue)) {
        reportItem->setProperty(m_propertyName.toLatin1(), m_newValue);
    }

    return true;
}

void PropertyChangedCommand::undoIt()
{
    BaseDesignIntf *reportItem = page()->reportItemByName(m_objectName);

    if (reportItem && (reportItem->property(m_propertyName.toLatin1()) != m_oldValue)) {
        reportItem->setProperty(m_propertyName.toLatin1(), m_oldValue);
    }
}

CommandIf::Ptr InsertBandCommand::create(PageDesignIntf *page, const QString &bandName)
{
    InsertBandCommand *command = new InsertBandCommand();
    command->setPage(page);
    BandDesignIntf *band = dynamic_cast<BandDesignIntf *>(page->reportItemByName(bandName));
    command->m_bandType = band->bandType();
    command->m_bandName = band->objectName();
    if (band->parentBand())
        command->m_parentBandName =  band->parentBandName();
    return CommandIf::Ptr(command);
}

bool InsertBandCommand::doIt()
{
    if (!m_parentBandName.isEmpty() && page()->reportItemByName(m_parentBandName))
        page()->reportItemByName(m_parentBandName)->setSelected(true);
    BaseDesignIntf *item = page()->addBand(m_bandType);

    if (item) {
        m_bandName = item->objectName();
        return true;
    }

    return false;
}

void InsertBandCommand::undoIt()
{
    BaseDesignIntf *item = page()->reportItemByName(m_bandName);

    if (item) {
        page()->removeReportItem(item,false);
    }
}

CommandIf::Ptr CommandGroup::create()
{
    return CommandIf::Ptr(new CommandGroup);
}

bool CommandGroup::doIt()
{
    foreach(CommandIf::Ptr command, m_commands) {
        if (!command->doIt())
            return false;
    }
    return true;
}

void CommandGroup::undoIt()
{
    foreach(CommandIf::Ptr command, m_commands) {
        command->undoIt();
    }
}

void CommandGroup::addCommand(CommandIf::Ptr command, bool execute)
{
    if (execute){
        if (command->doIt())
            m_commands.append(command);
    } else
        m_commands.append(command);
}

PrintRange::PrintRange(QAbstractPrintDialog::PrintRange rangeType, int fromPage, int toPage)
    :m_rangeType(rangeType), m_fromPage(fromPage), m_toPage(toPage)
{}

CommandIf::Ptr InsertHLayoutCommand::create(PageDesignIntf *page)
{
    InsertHLayoutCommand *command = new InsertHLayoutCommand();
    command->setPage(page);

    QList<QGraphicsItem *> si = page->selectedItems();
    QList<QGraphicsItem *>::iterator it = si.begin();

    BaseDesignIntf* parentItem = dynamic_cast<BaseDesignIntf*>((*it)->parentItem());
    command->m_oldParentName = (parentItem)?(parentItem->objectName()):"";

    for(it = si.begin();it!=si.end();++it){
        BaseDesignIntf* bi = dynamic_cast<BaseDesignIntf*>(*it);
        if (bi)
            command->m_elements.insert(bi->objectName(),bi->pos());
    }

    return CommandIf::Ptr(command);
}

bool InsertHLayoutCommand::doIt()
{
    foreach (QString itemName, m_elements.keys()) {
        BaseDesignIntf* bi = page()->reportItemByName(itemName);
        if (bi)
          bi->setSelected(true);
    }
    LayoutDesignIntf* layout = page()->internalAddHLayout();
    if (layout)
        m_layoutName = layout->objectName();
    return layout != 0;
}

void InsertHLayoutCommand::undoIt()
{
    HorizontalLayout* layout = dynamic_cast<HorizontalLayout*>(page()->reportItemByName(m_layoutName));
    if (layout){
        foreach(QGraphicsItem* item, layout->childBaseItems()){
            BaseDesignIntf* bi = dynamic_cast<BaseDesignIntf*>(item);
            BaseDesignIntf* parent = page()->reportItemByName(m_oldParentName);
            if (bi && parent){
                bi->setParentItem(parent);
                bi->setParent(parent);
                bi->setPos(m_elements.value(bi->objectName()));
                bi->setFixedPos(false);
                bi->setPossibleResizeDirectionFlags(BaseDesignIntf::AllDirections);
                bi->setVisible(true);
            }
        }
        page()->removeReportItem(layout,false);
    }
}

CommandIf::Ptr PropertyObjectNameChangedCommand::create(PageDesignIntf *page, const QString &oldValue, const QString &newValue)
{
    PropertyObjectNameChangedCommand *command = new PropertyObjectNameChangedCommand();
    command->setPage(page);
    command->m_oldName = oldValue;
    command->m_newName = newValue;
    return CommandIf::Ptr(command);
}

bool PropertyObjectNameChangedCommand::doIt()
{
    BaseDesignIntf *reportItem = page()->reportItemByName(m_oldName);

    if (reportItem ) {
        reportItem->setObjectName(m_newName);
        reportItem->emitObjectNamePropertyChanged(m_oldName,m_newName);
        return true;
    }
    return false;
}

void PropertyObjectNameChangedCommand::undoIt()
{
    BaseDesignIntf *reportItem = page()->reportItemByName(m_newName);

    if (reportItem ) {
        reportItem->setObjectName(m_oldName);
        reportItem->emitObjectNamePropertyChanged(m_newName,m_oldName);
    }

}

CommandIf::Ptr PropertyItemAlignChangedCommand::create(PageDesignIntf *page, const QString &objectName,
                                                       BaseDesignIntf::ItemAlign oldValue, BaseDesignIntf::ItemAlign newValue)
{
    PropertyItemAlignChangedCommand *command = new PropertyItemAlignChangedCommand();
    command->setPage(page);
    command->m_objectName = objectName;
    command->m_propertyName = "itemAlign";
    command->m_oldValue = oldValue;
    command->m_newValue = newValue;

    BaseDesignIntf *reportItem = page->reportItemByName(objectName);
    if (oldValue == BaseDesignIntf::DesignedItemAlign){
        command->m_savedPos = reportItem->pos();
    }

    return CommandIf::Ptr(command);
}

bool PropertyItemAlignChangedCommand::doIt()
{
    BaseDesignIntf *reportItem = page()->reportItemByName(m_objectName);

    if (reportItem && (reportItem->property(m_propertyName.toLatin1()) != m_newValue)) {
        reportItem->setProperty(m_propertyName.toLatin1(), m_newValue);
    }

    return true;
}

void PropertyItemAlignChangedCommand::undoIt()
{
    BaseDesignIntf *reportItem = page()->reportItemByName(m_objectName);

    if (reportItem && (reportItem->property(m_propertyName.toLatin1()) != m_oldValue)) {
        reportItem->setProperty(m_propertyName.toLatin1(), m_oldValue);
    }
    if (reportItem && (m_oldValue == BaseDesignIntf::DesignedItemAlign)){
        reportItem->setPos(m_savedPos);
    }
}

bool Projection::intersect(Projection projection)
{
    if (
       (projection.start()>=start() && projection.start()<=end()) ||
       (projection.end()>=start() && projection.end()<=end()) ||
       (projection.start()<=start() && projection.end()>=end())
    ) return true;
    return false;
}

qreal Projection::start() const
{
    return m_start;
}

qreal Projection::end() const
{
    return m_end;
}

bool ItemProjections::intersect(QRectF rect)
{
    Projection xProjection(rect.x(),rect.x()+rect.width());
    Projection yProjection(rect.y(),rect.y()+rect.height());
    if (m_xProjection.intersect(xProjection) && m_yProjection.intersect(yProjection))
        return true;
    return false;
}

bool ItemProjections::intersect(BaseDesignIntf *item)
{
    return intersect(QRectF(item->pos().x(), item->pos().y(), item->width(), item->height()));
}

qreal lineLength(qreal start, qreal end, Projection p){
    qreal result = 0;
    if (start>=p.start() && end<=p.end())
        result = end - start;
    if (start>=p.start() && start<=p.end())
        result = p.end() - start;
    else if (end>=p.start() && end<=p.end())
        result = end-p.start();
    else if (start<=p.start() && end>=p.end())
        result = p.end() - p.start();
    return result;
}

qreal ItemProjections::square(QRectF rect)
{
    qreal a = lineLength(rect.left(),rect.right(),m_xProjection);
    qreal b = lineLength(rect.top(),rect.bottom(),m_yProjection);
    return a*b;
}

qreal ItemProjections::square(BaseDesignIntf *item)
{
    return square(QRectF(item->pos().x(),item->pos().y(),item->width(),item->height()));
}

CommandIf::Ptr InsertVLayoutCommand::create(PageDesignIntf* page)
{
    InsertVLayoutCommand *command = new InsertVLayoutCommand();
    command->setPage(page);

    QList<QGraphicsItem *> si = page->selectedItems();
    QList<QGraphicsItem *>::iterator it = si.begin();

    BaseDesignIntf* parentItem = dynamic_cast<BaseDesignIntf*>((*it)->parentItem());
    command->m_oldParentName = (parentItem)?(parentItem->objectName()):"";

    for(it = si.begin();it!=si.end();++it){
        BaseDesignIntf* bi = dynamic_cast<BaseDesignIntf*>(*it);
        if (bi)
            command->m_elements.insert(bi->objectName(),bi->pos());
    }

    return CommandIf::Ptr(command);
}

bool InsertVLayoutCommand::doIt()
{
    foreach (QString itemName, m_elements.keys()) {
        BaseDesignIntf* bi = page()->reportItemByName(itemName);
        if (bi)
          bi->setSelected(true);
    }
    LayoutDesignIntf* layout = page()->internalAddVLayout();
    if (layout)
        m_layoutName = layout->objectName();
    return layout != 0;
}

void InsertVLayoutCommand::undoIt()
{
    VerticalLayout* layout = dynamic_cast<VerticalLayout*>(page()->reportItemByName(m_layoutName));
    if (layout){
        foreach(QGraphicsItem* item, layout->childBaseItems()){
            BaseDesignIntf* bi = dynamic_cast<BaseDesignIntf*>(item);
            BaseDesignIntf* parent = page()->reportItemByName(m_oldParentName);
            if (bi && parent){
                bi->setParentItem(parent);
                bi->setParent(parent);
                bi->setPos(m_elements.value(bi->objectName()));
                bi->setFixedPos(false);
                bi->setPossibleResizeDirectionFlags(BaseDesignIntf::AllDirections);
                bi->setVisible(true);
            }
        }
        page()->removeReportItem(layout,false);
    }
}

CommandIf::Ptr BandSwapCommand::create(PageDesignIntf *page, const QString &bandName, const QString &bandToSwapName)
{
    BandSwapCommand *command = new BandSwapCommand();
    command->setPage(page);
    command->bandName = bandName;
    command->bandToSwapName = bandToSwapName;
    return CommandIf::Ptr(command);
}

bool BandSwapCommand::doIt()
{
    BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(page()->reportItemByName(bandName));
    BandDesignIntf* bandToSwap = dynamic_cast<BandDesignIntf*>(page()->reportItemByName(bandToSwapName));
    if (page() && band && bandToSwap){
        page()->pageItem()->swapBands(band, bandToSwap);
        return true;
    }
    return false;
}

void BandSwapCommand::undoIt()
{
    BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(page()->reportItemByName(bandName));
    BandDesignIntf* bandToSwap = dynamic_cast<BandDesignIntf*>(page()->reportItemByName(bandToSwapName));
    if (page() && band && bandToSwap)
        page()->pageItem()->swapBands(bandToSwap, band);
}

CommandIf::Ptr BandMoveFromToCommand::create(PageDesignIntf* page, int from, int to)
{
    BandMoveFromToCommand* command = new BandMoveFromToCommand();
    command->setPage(page);
    command->from = from;
    command->to = to;
    return CommandIf::Ptr(command);
}

bool BandMoveFromToCommand::doIt()
{
    if (page() && page()->pageItem() && from != to) {
        BandDesignIntf* fromBand = page()->pageItem()->bandByIndex(from);
        reverceTo = fromBand->minChildIndex();
        if (fromBand){
            page()->pageItem()->moveBandFromTo(from, to);
            reverceFrom = fromBand->bandIndex();
            return true;
        }
    }
    return false;
}

void BandMoveFromToCommand::undoIt()
{
    if (page() && page()->pageItem())
        page()->pageItem()->moveBandFromTo(reverceFrom, reverceTo);
}

}

