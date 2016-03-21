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
#ifndef LROBJECTINSPECTORWIDGET_H
#define LROBJECTINSPECTORWIDGET_H

#include <QTreeView>
#include <QMap>
#include "lrobjectitemmodel.h"
#include "lrpropertydelegate.h"

namespace LimeReport{

class ObjectInspectorWidget : public QTreeView
{
    Q_OBJECT
public:
    ObjectInspectorWidget(QWidget * parent=0);
    ~ObjectInspectorWidget();
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

} //namespace LimeReport

#endif // LROBJECTINSPECTORWIDGET_H
