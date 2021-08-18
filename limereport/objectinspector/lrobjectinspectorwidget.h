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
#ifndef LROBJECTINSPECTORWIDGET_H
#define LROBJECTINSPECTORWIDGET_H

#include <QTreeView>
#include <QMap>
#include <QSortFilterProxyModel>

#include "lrobjectitemmodel.h"
#include "lrbasedesignobjectmodel.h"
#include "lrpropertydelegate.h"

namespace LimeReport{

class ObjectInspectorTreeView : public QTreeView
{
    Q_OBJECT
public:
    ObjectInspectorTreeView(QWidget * parent=0);
    ~ObjectInspectorTreeView();
    QColor getColor(const int index) const;
    virtual void reset();
    virtual void commitActiveEditorData();
protected:
    void mousePressEvent(QMouseEvent *event);
    void drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const;
    void keyPressEvent(QKeyEvent *event);
private:
    void initColorMap();
    LimeReport::ObjectPropItem* nodeFromIndex(QModelIndex index) const;

private:
    QVector<QColor> m_colors;
    PropertyDelegate *m_propertyDelegate;
};

class PropertyFilterModel: public QSortFilterProxyModel{
public:
    PropertyFilterModel(QObject* parent = 0): QSortFilterProxyModel(parent){}
protected:
    bool filterAcceptsRow(int sourceRow,const QModelIndex &sourceParent) const;
};

class ObjectInspectorWidget: public QWidget{
    Q_OBJECT
public:
    ObjectInspectorWidget(QWidget* parent = 0);
    void setModel(QAbstractItemModel* model);
    void setAlternatingRowColors(bool value);
    void setRootIsDecorated(bool value);
    void setColumnWidth(int column, int width);
    int  columnWidth(int column);
    void expandToDepth(int depth);
    void commitActiveEditorData();
    void setValidator(ValidatorIntf *validator);
    bool subclassesAsLevel();
    void setSubclassesAsLevel(bool value);
    bool translateProperties();
    void setTranslateProperties(bool value);
    void setObject(QObject* setObject);
    const QObject* object();
    void setMultiObjects(QList<QObject *>* list);
    void clearObjectsList();
    void updateProperty(const QString &propertyName);
private slots:
    void slotFilterTextChanged(const QString& filter);
    void slotTranslatePropertiesChecked(bool value);
private:
    ObjectInspectorTreeView* m_objectInspectorView;
    QSortFilterProxyModel* m_filterModel;
    BaseDesignPropertyModel* m_propertyModel;
    QAction* m_translateProperties;

};

} //namespace LimeReport

#endif // LROBJECTINSPECTORWIDGET_H
