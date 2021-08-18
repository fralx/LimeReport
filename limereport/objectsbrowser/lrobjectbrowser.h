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
#ifndef LROBJECTBROWSER_H
#define LROBJECTBROWSER_H

#include <QWidget>
#include <QTreeWidget>
#include "lrreportdesignwidget.h"
#include "lrbasedesignintf.h"

namespace LimeReport{

class ObjectBrowserNode :public QTreeWidgetItem{
public:
    void setObject(QObject* value);
    QObject* object() const;
    explicit ObjectBrowserNode(QTreeWidget *view);
    explicit ObjectBrowserNode(QTreeWidgetItem *parent = 0);
    bool operator <(const QTreeWidgetItem& other) const;
private:
    QObject* m_object;
};

class ObjectBrowser :public QWidget
{
    Q_OBJECT
public:
    ObjectBrowser(QWidget *parent=0);
    void setReportEditor(LimeReport::ReportDesignWidget* designerWidget);
    void setMainWindow(QMainWindow* mainWindow);
protected:
    void fillNode(QTreeWidgetItem *parentNode, BaseDesignIntf *reportItem, BaseDesignIntf* ignoredItem = 0);
    void buildTree(BaseDesignIntf *ignoredItem = 0);
    void removeItem(BaseDesignIntf* item);
    void findAndRemove(QTreeWidgetItem *node, BaseDesignIntf *item);
    void moveItemNode(BaseDesignIntf* item, BaseDesignIntf* parent);
private slots:
//    void slotObjectNameChanged(const QString& objectName);
    void slotPropertyObjectNameChanged(const QString& oldName, const QString& newName);
    void slotClear();
    void slotReportLoaded();
    void slotActivePageChanged();
    void slotBandAdded(LimeReport::PageDesignIntf*, LimeReport::BandDesignIntf*);
    void slotBandDeleted(LimeReport::PageDesignIntf*, LimeReport::BandDesignIntf*item);
    void slotItemAdded(LimeReport::PageDesignIntf*page, LimeReport::BaseDesignIntf*);
    void slotItemDeleted(LimeReport::PageDesignIntf*, LimeReport::BaseDesignIntf*item);
    void slotObjectTreeItemSelectionChanged();
    void slotItemSelected(LimeReport::BaseDesignIntf* item);
    void slotMultiItemSelected();
    void slotItemDoubleClicked(QTreeWidgetItem* item,int);
    void slotActivePageUpdated(LimeReport::PageDesignIntf*);
    void slotItemParentChanged(BaseDesignIntf* item, BaseDesignIntf* parent);
private:
    ReportDesignWidget* m_designerWidget;
    QMainWindow*        m_mainWindow;
    QTreeWidget*        m_treeView;
    QMap<QObject*, ObjectBrowserNode*> m_itemsMap;
    bool m_changingItemSelection;
    bool m_movingItem;
    QList<QObject*> m_defferedItems;
};

} //namespace LimeReport
#endif // LROBJECTBROWSER_H
