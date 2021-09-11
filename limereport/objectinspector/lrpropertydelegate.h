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
#ifndef LRPROPERTYDELEGATE_H
#define LRPROPERTYDELEGATE_H

#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QTreeView>
#include <QObject>

#include "lrobjectitemmodel.h"

namespace LimeReport{

class ObjectInspectorTreeView;
//class PropertyDelegate : public QItemDelegate
class PropertyDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    PropertyDelegate(QObject *parent=0);
    void setObjectInspector(ObjectInspectorTreeView* objectInspector);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    LimeReport::ObjectPropItem* editingItem();
    bool isEditing(){return m_isEditing;}
private slots:
    void commitAndCloseEditor();
    void slotEditorDeleted();
    void slotItemDeleted(QObject* item);
private:
    LimeReport::ObjectInspectorTreeView* m_objectInspector;
    mutable LimeReport::ObjectPropItem* m_editingItem;
    mutable bool m_isEditing;
};
}
#endif // LRPROPERTYDELEGATE_H
