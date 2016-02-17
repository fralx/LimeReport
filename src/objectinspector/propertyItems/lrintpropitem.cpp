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
#include "lrintpropitem.h"
#include <limits>
#include <QDoubleSpinBox>

namespace{
    LimeReport::ObjectPropItem * createIntPropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
    {
        return new LimeReport::IntPropItem(object, objects, name, displayName, data, parent, readonly);
    }
    bool registred = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("int",""),QObject::tr("int"),createIntPropItem);
} // namespace

namespace LimeReport{

QWidget *IntPropItem::createProperyEditor(QWidget *parent) const
{
    QSpinBox *editor= new QSpinBox(parent);
    editor->setMaximum(std::numeric_limits<int>::max());
    editor->setMinimum(std::numeric_limits<int>::min());
    return editor;
}

void IntPropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const
{
    QSpinBox *editor =qobject_cast<QSpinBox *>(propertyEditor);
    editor->setValue(propertyValue().toInt());
}

void IntPropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    model->setData(index,qobject_cast<QSpinBox*>(propertyEditor)->value());
    object()->setProperty(propertyName().toLatin1(),propertyValue());
    foreach(QObject* item, *objects()){
        if (item->metaObject()->indexOfProperty(propertyName().toLatin1())!=-1){
            item->setProperty(propertyName().toLatin1(),propertyValue());
        }
    }
}

} // namespace LimeReport
