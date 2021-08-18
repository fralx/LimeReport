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
#include "lrboolpropitem.h"
#include "lrobjectpropitem.h"
#include <QPainter>
#include <QStylePainter>
#include <QApplication>
#include <QBitmap>

#include "../editors/lrcheckboxeditor.h"

namespace{
    LimeReport::ObjectPropItem * createBoolPropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
    {
        return new LimeReport::BoolPropItem(object, objects, name, displayName, data, parent, readonly);
    }
    bool VARIABLE_IS_NOT_USED  registred = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("bool",""),QObject::tr("bool"),createBoolPropItem);
} // namespace

namespace LimeReport {

QWidget *BoolPropItem::createProperyEditor(QWidget *parent) const
{
    CheckBoxEditor *editor= new CheckBoxEditor(parent);
    return editor;
}

void BoolPropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const
{
    CheckBoxEditor *editor =qobject_cast<CheckBoxEditor *>(propertyEditor);
    editor->setEditing(true);
    editor->setChecked(propertyValue().toBool());
    editor->setEditing(false);
}

void BoolPropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    model->setData(index,qobject_cast<CheckBoxEditor*>(propertyEditor)->isChecked());
    setValueToObject(propertyName(),propertyValue());
}

QPixmap BoolPropItem::getIndicatorImage(const StyleOptionViewItem &option){
    QStyleOptionButton so;
    so.state = option.state;
    if (!isValueReadonly())
        so.state = QStyle::State_Enabled;
    else
        so.state &= ~QStyle::State_Enabled;
    so.state |= propertyValue().toBool() ? QStyle::State_On : QStyle::State_Off;
    so.rect = QRect(0,0,
                    QApplication::style()->pixelMetric(QStyle::PM_IndicatorWidth),
                    QApplication::style()->pixelMetric(QStyle::PM_IndicatorHeight));

    QPixmap pixmap(so.rect.width(),so.rect.height());
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    option.widget->style()->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck,&so, &p);
    return pixmap;
}

bool BoolPropItem::paint(QPainter *painter, const StyleOptionViewItem &option, const QModelIndex &index)
{

    QStyle* style = option.widget ? option.widget->style() : QApplication::style();

    if (index.column()==1){
        int border = (option.rect.height() - style->pixelMetric(QStyle::PM_IndicatorWidth))/2;
//        QStyleOptionButton so;
//        so.rect = option.rect.adjusted(border,border,0,-border);
//        so.rect.setWidth(style->pixelMetric(QStyle::PM_IndicatorWidth));
//        so.rect.setHeight(style->pixelMetric(QStyle::PM_IndicatorHeight));
//        so.state |= propertyValue().toBool() ? QStyle::State_On : QStyle::State_Off;
//        style->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck,&so,painter);
        painter->drawPixmap(option.rect.x()+border,option.rect.y()+border, getIndicatorImage(option));
        return true;
    } else return false;
}

} // namespace LimeReport

