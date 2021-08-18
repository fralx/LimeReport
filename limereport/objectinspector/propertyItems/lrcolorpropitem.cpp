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
#include "lrcolorpropitem.h"
#include "lrglobal.h"
#include "editors/lrcoloreditor.h"
#include <QPainter>
#include <QApplication>

namespace{
    LimeReport::ObjectPropItem * createColorPropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
    {
        return new LimeReport::ColorPropItem(object, objects, name, displayName, data, parent, readonly);
    }
    bool VARIABLE_IS_NOT_USED registredColorProp = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("QColor",""),QObject::tr("QColor"),createColorPropItem);
}

namespace LimeReport{

void ColorPropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const
{
    ColorEditor *editor =qobject_cast<ColorEditor*>(propertyEditor);
    editor->setColor(propertyValue().value<QColor>());
}

void ColorPropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    model->setData(index,qobject_cast<ColorEditor*>(propertyEditor)->color());
    setValueToObject(propertyName(),propertyValue());
}

bool ColorPropItem::paint(QPainter *painter, const StyleOptionViewItem &option, const QModelIndex &index)
{
    if (index.column()==1){
        painter->save();

        QStyle* style = option.widget ? option.widget->style() : QApplication::style();
        QPen pen;
        QColor penColor = isColorDark(propertyValue().value<QColor>()) ? Qt::transparent : Qt::darkGray;
        pen.setColor(penColor);
        painter->setPen(pen);
        painter->setBrush(propertyValue().value<QColor>());
        int border = (option.rect.height() - style->pixelMetric(QStyle::PM_IndicatorWidth))/2;

        QRect rect(option.rect.x()+border,option.rect.y()+border,
                   style->pixelMetric(QStyle::PM_IndicatorWidth),
                   style->pixelMetric(QStyle::PM_IndicatorWidth));
        painter->drawRect(rect);

        painter->restore();
        return true;
    } else return false;
}

QWidget *ColorPropItem::createProperyEditor(QWidget *parent) const
{
    return new ColorEditor(parent);
}

}
