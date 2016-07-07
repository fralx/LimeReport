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
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

#include "lrglobal.h"
#include "lrobjectinspectorwidget.h"
#include "lrobjectitemmodel.h"

namespace LimeReport{

ObjectInspectorWidget::ObjectInspectorWidget(QWidget *parent)
    :QTreeView(parent), m_propertyDelegate(0)
{
    setRootIsDecorated(false);
    initColorMap();
    setEditTriggers(
        QAbstractItemView::DoubleClicked |
        QAbstractItemView::SelectedClicked |
        QAbstractItemView::EditKeyPressed
    );
    m_propertyDelegate = new PropertyDelegate(this);
    setItemDelegate(m_propertyDelegate);
    QPalette p = palette();
    p.setColor(QPalette::AlternateBase,QColor(229,255,214));
    setPalette(p);
}

ObjectInspectorWidget::~ObjectInspectorWidget(){}

void ObjectInspectorWidget::drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const
{
    ObjectPropItem *node = nodeFromIndex(index);
    StyleOptionViewItem so = options;
    bool alternate = so.features & StyleOptionViewItem::Alternate;
    if (node){
        if ((!node->isHaveValue())){
            const QColor c = options.palette.color(QPalette::Dark);
            painter->fillRect(options.rect,c);
            so.palette.setColor(QPalette::AlternateBase,c);
        } else {
            if ( index.isValid()&&(nodeFromIndex(index)->colorIndex()!=-1) ){
                QColor fillColor(getColor(nodeFromIndex(index)->colorIndex()%m_colors.count()));
                so.palette.setColor(QPalette::AlternateBase,fillColor.lighter(115));
                if (!alternate){
                    painter->fillRect(options.rect,fillColor);
                }
            }
        }
    }
    QTreeView::drawRow(painter,so,index);
    painter->save();
    QColor gridLineColor = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor,&so));
    painter->setPen(gridLineColor);
    painter->drawLine(so.rect.x(),so.rect.bottom(),so.rect.right(),so.rect.bottom());
    painter->restore();
}

void ObjectInspectorWidget::mousePressEvent(QMouseEvent *event)
{

    if ((event->button()==Qt::LeftButton)){
        QModelIndex index=indexAt(event->pos());
        if (index.isValid()){
            if (event->pos().x()<indentation()) {
                if (!nodeFromIndex(index)->isHaveValue())
                    setExpanded(index,!isExpanded(index));
            } else {
                if ((index.column()==1)&&(!nodeFromIndex(index)->isHaveChildren())) {
                    setCurrentIndex(index);
                    edit(index);
                    return ;
                }
            }
        }
    }

    QTreeView::mousePressEvent(event);
}

void ObjectInspectorWidget::initColorMap()
{
    m_colors.reserve(6);
    m_colors.push_back(QColor(255,230,191));
    m_colors.push_back(QColor(255,255,191));
    m_colors.push_back(QColor(191,255,191));
    m_colors.push_back(QColor(199,255,255));
    m_colors.push_back(QColor(234,191,255));
    m_colors.push_back(QColor(255,191,239));
}

QColor ObjectInspectorWidget::getColor(const int index) const
{
    return m_colors[index];
}

void ObjectInspectorWidget::reset()
{
    QTreeView::reset();
    for (int i=0;i<model()->rowCount();i++){
        if (!nodeFromIndex(model()->index(i,0))->isHaveValue()){
          setFirstColumnSpanned(i,model()->index(i,0).parent(),true);
        }
    }
}

ObjectPropItem * ObjectInspectorWidget::nodeFromIndex(QModelIndex index) const
{
    return static_cast<LimeReport::ObjectPropItem*>(index.internalPointer());
}

void ObjectInspectorWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_Return){
        if(!m_propertyDelegate->isEditing()){
            QModelIndex index = currentIndex().model()->index(currentIndex().row(),
                                                              1,currentIndex().parent());
            edit(index);
            event->accept();
        }
    } else QTreeView::keyPressEvent(event);
}

void ObjectInspectorWidget::commitActiveEditorData(){
    if (state()==QAbstractItemView::EditingState){
        commitData(indexWidget(currentIndex()));
    }
}

} //namespace LimeReport
