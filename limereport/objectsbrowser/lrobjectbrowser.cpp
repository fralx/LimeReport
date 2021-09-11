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
#include "lrobjectbrowser.h"
#include "lritemdesignintf.h"
#include "lrbanddesignintf.h"
#include <QVBoxLayout>

namespace LimeReport{

ObjectBrowser::ObjectBrowser(QWidget *parent)
    :QWidget(parent), m_designerWidget(NULL), m_mainWindow(NULL),
      m_changingItemSelection(false), m_movingItem(false)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);
    int margin = Const::DOCKWIDGET_MARGINS;
    layout->setContentsMargins(margin, margin, margin, margin);
    m_treeView = new QTreeWidget(this);
    layout->addWidget(m_treeView);
    m_treeView->headerItem()->setText(0,tr("Objects"));
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void ObjectBrowser::setReportEditor(ReportDesignWidget *designerWidget)
{
    m_designerWidget=designerWidget;
    connect(m_designerWidget,SIGNAL(cleared()),this,SLOT(slotClear()));
    connect(m_designerWidget, SIGNAL(loadFinished()), this, SLOT(slotReportLoaded()));
    connect(m_designerWidget, SIGNAL(activePageChanged()), this, SLOT(slotActivePageChanged()));

    connect(m_designerWidget,SIGNAL(itemAdded(LimeReport::PageDesignIntf*,LimeReport::BaseDesignIntf*)),
            this, SLOT(slotItemAdded(LimeReport::PageDesignIntf*,LimeReport::BaseDesignIntf*)));
    connect(m_designerWidget, SIGNAL(itemDeleted(LimeReport::PageDesignIntf*,LimeReport::BaseDesignIntf*)),
            this, SLOT(slotItemDeleted(LimeReport::PageDesignIntf*,LimeReport::BaseDesignIntf*)));
    connect(m_designerWidget, SIGNAL(bandAdded(LimeReport::PageDesignIntf*,LimeReport::BandDesignIntf*)),
            this, SLOT(slotBandAdded(LimeReport::PageDesignIntf*,LimeReport::BandDesignIntf*)));
    connect(m_designerWidget, SIGNAL(bandDeleted(LimeReport::PageDesignIntf*,LimeReport::BandDesignIntf*)),
            this, SLOT(slotBandDeleted(LimeReport::PageDesignIntf*,LimeReport::BandDesignIntf*)));
    connect(m_treeView, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotObjectTreeItemSelectionChanged()) );
    connect(m_designerWidget, SIGNAL(itemSelected(LimeReport::BaseDesignIntf*)),
            this, SLOT(slotItemSelected(LimeReport::BaseDesignIntf*)));
    connect(m_designerWidget, SIGNAL(multiItemSelected()),
            this, SLOT(slotMultiItemSelected()) );
    connect(m_designerWidget, SIGNAL(activePageUpdated(LimeReport::PageDesignIntf*)),
            this, SLOT(slotActivePageUpdated(LimeReport::PageDesignIntf*)));
    connect(m_treeView, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotItemDoubleClicked(QTreeWidgetItem*,int)));

    buildTree();
}

void ObjectBrowser::setMainWindow(QMainWindow *mainWindow)
{
    m_mainWindow=mainWindow;
}

void ObjectBrowser::slotClear()
{

}

void ObjectBrowser::fillNode(QTreeWidgetItem* parentNode, BaseDesignIntf* reportItem, BaseDesignIntf *ignoredItem){

    foreach (BaseDesignIntf* item, reportItem->childBaseItems()) {
        if (item != ignoredItem){
            ObjectBrowserNode* treeItem = new ObjectBrowserNode(/*parentNode*/);
            treeItem->setText(0,item->objectName());
            treeItem->setObject(item);
            treeItem->setIcon(0,QIcon(":/items/"+extractClassName(item->metaObject()->className())));
            connect(item, SIGNAL(propertyObjectNameChanged(QString,QString)),
                    this, SLOT(slotPropertyObjectNameChanged(QString,QString)), Qt::UniqueConnection);
            ItemDesignIntf* i = dynamic_cast<ItemDesignIntf*>(item);
            if (i){
                connect(i, SIGNAL(itemLocationChanged(BaseDesignIntf*,BaseDesignIntf*)),
                        this, SLOT(slotItemParentChanged(BaseDesignIntf*,BaseDesignIntf*)), Qt::UniqueConnection);
            }
            m_itemsMap.insert(item,treeItem);

            BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(item);

            QSet<BandDesignIntf::BandsType> subBands;
            subBands << BandDesignIntf::SubDetailBand <<
                        BandDesignIntf::SubDetailHeader <<
                        BandDesignIntf::SubDetailFooter;

            if (band && subBands.contains(band->bandType())){
                ObjectBrowserNode* parentBandNode = 0;
                if (band->bandType() == BandDesignIntf::SubDetailBand){
                    parentBandNode = m_itemsMap.value(band->parentBand());
                } else {
                    parentBandNode = m_itemsMap.value(band->parentBand()->parentBand());
                }
                if(parentBandNode)
                    parentBandNode->addChild(treeItem);
            } else {
                parentNode->addChild(treeItem);
            }
            if (!item->childBaseItems().isEmpty())
                fillNode(treeItem,item, ignoredItem);
        }
    }
}

void ObjectBrowser::buildTree(BaseDesignIntf* ignoredItem){

    m_treeView->clear();
    m_itemsMap.clear();
    if (!m_designerWidget->activePage()) return;

    ObjectBrowserNode *topLevelItem=new ObjectBrowserNode(m_treeView);
    topLevelItem->setText(0,m_designerWidget->activePage()->objectName());
    topLevelItem->setObject(m_designerWidget->activePage());
    m_itemsMap.insert(m_designerWidget->activePage(),topLevelItem);

    m_treeView->addTopLevelItem(topLevelItem);
    QList<QGraphicsItem*> itemsList = m_designerWidget->activePage()->items();
    foreach (QGraphicsItem* item, itemsList) {
        if (item != ignoredItem){
            BaseDesignIntf* reportItem = dynamic_cast<BaseDesignIntf*>(item);
            if (reportItem && reportItem->parentItem()==0){
                ObjectBrowserNode* tItem = new ObjectBrowserNode(topLevelItem);
                tItem->setText(0,reportItem->objectName());
                tItem->setObject(reportItem);
                tItem->setIcon(0,QIcon(":/items/"+extractClassName(reportItem->metaObject()->className())));
                connect(reportItem, SIGNAL(propertyObjectNameChanged(QString,QString)),
                        this, SLOT(slotPropertyObjectNameChanged(QString,QString)));
                m_itemsMap.insert(reportItem,tItem);
                fillNode(tItem,reportItem, ignoredItem);
                topLevelItem->addChild(tItem);
            }
        }
    }
    m_treeView->sortItems(0,Qt::AscendingOrder);
    m_treeView->expandAll();
}

void ObjectBrowser::findAndRemove(QTreeWidgetItem* node, BaseDesignIntf* item){

    for (int i=0;i<node->childCount();i++){
        QTreeWidgetItem* treeItem = node->child(i);
        if (treeItem->text(0)==item->objectName()){
            node->removeChild(treeItem);
            break;
        } else {
            if (treeItem->childCount()>0) findAndRemove(treeItem, item);
        }
    }

}

void ObjectBrowser::slotPropertyObjectNameChanged(const QString &oldName, const QString &newName)
{
    Q_UNUSED(oldName)
    if (m_itemsMap.contains(sender())){
        m_itemsMap.value(sender())->setText(0,newName);
    }
}

//void ObjectBrowser::slotObjectNameChanged(const QString &objectName)
//{
//    if (m_itemsMap.contains(sender())){
//        m_itemsMap.value(sender())->setText(0,objectName);
//    }
//}

void ObjectBrowser::removeItem(BaseDesignIntf *item)
{
    findAndRemove(m_treeView->topLevelItem(0),item);
}

void ObjectBrowser::slotReportLoaded()
{
    buildTree();
}

void ObjectBrowser::slotActivePageChanged()
{
    buildTree();
}

void ObjectBrowser::slotBandAdded(LimeReport::PageDesignIntf *, BandDesignIntf *)
{
    buildTree();
}

void ObjectBrowser::slotBandDeleted(PageDesignIntf *, BandDesignIntf * item)
{
    buildTree(item);
}

void ObjectBrowser::slotItemAdded(PageDesignIntf *page, BaseDesignIntf *)
{
    if (!page->isUpdating()) buildTree();
}

void ObjectBrowser::slotItemDeleted(PageDesignIntf *, BaseDesignIntf *item)
{
    if (dynamic_cast<LayoutDesignIntf*>(item)){
        buildTree(item);
    } else {
        removeItem(item);
    }
}

void ObjectBrowser::slotObjectTreeItemSelectionChanged()
{
    if (!m_changingItemSelection  && m_designerWidget->activePage()){
        m_changingItemSelection = true;
        m_designerWidget->activePage()->clearSelection();
        foreach(QTreeWidgetItem* item, m_treeView->selectedItems()){
            ObjectBrowserNode* tn = dynamic_cast<ObjectBrowserNode*>(item);
            if (tn){
                BaseDesignIntf* si = dynamic_cast<BaseDesignIntf*>(tn->object());
                if (si) {
                    m_designerWidget->activePage()->animateItem(si);
                    si->setSelected(true);
                    QPointF p = si->mapToScene(si->pos());
                    if (si->parentItem())
                        p = si->parentItem()->mapToScene(si->pos());
                    m_designerWidget->activeView()->centerOn(p);
                }
            }
        }
        m_changingItemSelection = false;
    }
}

void ObjectBrowser::slotItemSelected(LimeReport::BaseDesignIntf *item)
{
    if (!m_changingItemSelection){
        m_changingItemSelection = true;

        m_treeView->selectionModel()->clear();
        BaseDesignIntf* bg = dynamic_cast<BaseDesignIntf*>(item);
        if (bg){
            if (m_itemsMap.value(bg))
                m_itemsMap.value(bg)->setSelected(true);
        }

        m_changingItemSelection = false;
    }
}

void ObjectBrowser::slotMultiItemSelected()
{
    if (!m_changingItemSelection){
        m_changingItemSelection = true;

        m_treeView->selectionModel()->clear();

        foreach(QGraphicsItem* item, m_designerWidget->activePage()->selectedItems()){
            BaseDesignIntf* bg = dynamic_cast<BaseDesignIntf*>(item);
            if (bg){
                ObjectBrowserNode* node = m_itemsMap.value(bg);
                if (node)
                  node->setSelected(true);
            }
        }

        m_changingItemSelection = false;
    }
}

void ObjectBrowser::slotItemDoubleClicked(QTreeWidgetItem *item, int)
{
    ObjectBrowserNode* node = dynamic_cast<ObjectBrowserNode*>(item);
    if (node){
        BaseDesignIntf* baseItem = dynamic_cast<BaseDesignIntf*>(node->object());
        if (baseItem) {
            baseItem->showEditorDialog();
        }
    }
}

void ObjectBrowser::slotActivePageUpdated(LimeReport::PageDesignIntf *)
{
    buildTree();
}



void ObjectBrowser::moveItemNode(BaseDesignIntf* item, BaseDesignIntf* parent)
{
    if (m_itemsMap.contains(item) && m_itemsMap.contains(parent)){
        m_itemsMap.value(item)->parent()->removeChild(m_itemsMap.value(item));
        m_itemsMap.value(parent)->addChild(m_itemsMap.value(item));
        m_changingItemSelection = true;
        m_itemsMap.value(item)->setSelected(true);
        item->setSelected(true);
        m_changingItemSelection = false;
    }
}

void ObjectBrowser::slotItemParentChanged(BaseDesignIntf* item, BaseDesignIntf* parent)
{
    if (!m_movingItem){
        m_movingItem = true;
        moveItemNode(item, parent);
        m_movingItem = false;
        foreach(QObject* di, m_defferedItems){
            BaseDesignIntf* b = dynamic_cast<BaseDesignIntf*>(di);
            if (b)
                moveItemNode(b, parent);
        }
        m_defferedItems.clear();
    } else {
        if (!m_defferedItems.contains(item))
            m_defferedItems.append(item);
    }
}

void ObjectBrowserNode::setObject(QObject *value)
{
    m_object = value;
}

QObject *ObjectBrowserNode::object() const
{
    return m_object;
}

ObjectBrowserNode::ObjectBrowserNode(QTreeWidget *view)
    :QTreeWidgetItem(view), m_object(0){}

ObjectBrowserNode::ObjectBrowserNode(QTreeWidgetItem *parent)
    :QTreeWidgetItem(parent), m_object(0){}

bool ObjectBrowserNode::operator <(const QTreeWidgetItem &other) const
{
    BandDesignIntf* band1 = dynamic_cast<BandDesignIntf*>(m_object);
    BandDesignIntf* band2 = dynamic_cast<BandDesignIntf*>(dynamic_cast<const ObjectBrowserNode&>(other).object());
    if (band1 && band2) return band1->bandIndex()<band2->bandIndex();
    return false;
}

} //namespace LimeReport
