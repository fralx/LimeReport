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
#include "lrqrealpropitem.h"

#include <QDoubleSpinBox>
#include <limits>

namespace{
    LimeReport::ObjectPropItem * createQRealPropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
    {
        return new LimeReport::QRealPropItem(object, objects, name, displayName, data, parent, readonly);
    }

    bool VARIABLE_IS_NOT_USED  registred = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("qreal",""),QObject::tr("qreal"),createQRealPropItem);
    bool VARIABLE_IS_NOT_USED  registredDouble = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("double",""),QObject::tr("qreal"),createQRealPropItem);    
}

namespace LimeReport{

QWidget *QRealPropItem::createProperyEditor(QWidget *parent) const
{
    QDoubleSpinBox *editor= new QDoubleSpinBox(parent);
    editor->setMaximum(std::numeric_limits<qreal>::max());
    editor->setMinimum(std::numeric_limits<qreal>::max()*-1);
    return editor;
}

void QRealPropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const
{
    QDoubleSpinBox *editor =qobject_cast<QDoubleSpinBox*>(propertyEditor);
    editor->setValue(propertyValue().toDouble());
}

void QRealPropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    model->setData(index,qobject_cast<QDoubleSpinBox*>(propertyEditor)->value());
    setValueToObject(propertyName(),propertyValue());
}

}
