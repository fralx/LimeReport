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
#include "lrflagspropitem.h"
#include "lrenumpropitem.h"
#include "lrboolpropitem.h"
#include "../editors/lrcheckboxeditor.h"
#include "lrobjectitemmodel.h"
#include <QIcon>
#include <QImage>
#include <QPainter>
#include <QStyle>
#include <QStylePainter>

namespace {

LimeReport::ObjectPropItem * createFlagsPropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::FlagsPropItem(object, objects, name, displayName, data, parent, readonly);
}
bool VARIABLE_IS_NOT_USED registred = LimeReport::ObjectPropFactory::instance().registerCreator(
    LimeReport::APropIdent("flags",""),QObject::tr("flags"),createFlagsPropItem
);

} // namespace

namespace LimeReport {

void FlagsPropItem::createChildren()
{
    QMetaEnum propEnum = object()->metaObject()->property(object()->metaObject()->indexOfProperty(propertyName().toLatin1())).enumerator();
    for (int i=0;i<propEnum.keyCount();i++)
    {
        if ( propEnum.keyToValue(propEnum.key(i)) !=0 ) {
            this->appendItem(new LimeReport::FlagPropItem(
                                 object(), objects(), QString(propEnum.key(i)), tr(propEnum.key(i)),
                                 bool((propertyValue().toInt() & propEnum.keyToValue(propEnum.key(i)))==propEnum.keyToValue(propEnum.key(i))),
                                 this, false
                                 )
                             );
        }
    }
}

void FlagsPropItem::updateChildren()
{
    QMetaEnum propEnum = object()->metaObject()->property(object()->metaObject()->indexOfProperty(propertyName().toLatin1())).enumerator();
    for (int i=0;i<propEnum.keyCount();i++)
    {
        ObjectPropItem* property = findChild(QString(propEnum.key(i)));
        if (property)
            property->setPropertyValue(bool((propertyValue().toInt() & propEnum.keyToValue(propEnum.key(i)))==propEnum.keyToValue(propEnum.key(i))));
    }
}

FlagsPropItem::FlagsPropItem(QObject *object, ObjectPropItem::ObjectsList *objects, const QString &name, const QString &displayName, const QVariant &value, ObjectPropItem *parent, bool readonly)
    :ObjectPropItem(object, objects, name, displayName, value, parent, readonly)
{
    createChildren();
}

FlagsPropItem::FlagsPropItem(QObject *object, ObjectPropItem::ObjectsList *objects, const QString &name, const QString &displayName, const QVariant &value, ObjectPropItem *parent, bool readonly, QSet<int> acceptableValues)
    :ObjectPropItem(object, objects, name, displayName, value, parent, readonly),m_acceptableValues(acceptableValues){}

QString FlagsPropItem::displayValue() const
{
    QString result;
    QMetaEnum propEnum = object()->metaObject()->property(object()->metaObject()->indexOfProperty(propertyName().toLatin1())).enumerator();
    for (int i=0;i<propEnum.keyCount();i++)
    {
        if ((propEnum.keyToValue(propEnum.key(i)) == 0) ? propertyValue().toInt() == 0 : (propertyValue().toInt() & propEnum.keyToValue(propEnum.key(i))) == propEnum.keyToValue(propEnum.key(i)))
        {
            if (result.isEmpty()) result+= isTranslateProperty() ? tr(propEnum.key(i)) : propEnum.key(i);
            else result=result+" | "+ (isTranslateProperty() ? tr(propEnum.key(i)) : propEnum.key(i));
        }

    }
    return result;
}

void FlagsPropItem::setPropertyValue(QVariant value)
{
    ObjectPropItem::setPropertyValue(value);
    updateChildren();
}

void FlagsPropItem::slotEnumChanged(QString /*text*/)
{
}

void FlagsPropItem::translateFlagsItem()
{
    tr("NoLine");
    tr("TopLine");
    tr("BottomLine");
    tr("LeftLine");
    tr("RightLine");
    tr("AllLines");
}

FlagPropItem::FlagPropItem(QObject* object, ObjectsList* objects, const QString &propName, const QString &displayName, const QVariant &value, ObjectPropItem* parent, bool readonly)
    :BoolPropItem(object, objects, propName,displayName,value,parent,readonly)
{
}

void FlagPropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &/*index*/) const
{
    CheckBoxEditor *editor = qobject_cast<CheckBoxEditor*>(propertyEditor);
    editor->setChecked(propertyValue().toBool());
}

void FlagPropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    bool value = qobject_cast<CheckBoxEditor*>(propertyEditor)->isChecked();
    model->setData(index,value);
    int flags = object()->property(parent()->propertyName().toLatin1()).toInt();
    if (value) flags = flags | valueByName(propertyName());
    else if (flags & valueByName(propertyName())) flags = flags ^ valueByName(propertyName());
    setValueToObject(parent()->propertyName(),flags);
    parent()->setPropertyValue(flags);
}

int FlagPropItem::valueByName(const QString& typeName)
{
    QMetaEnum propEnum = object()->metaObject()->property(object()->metaObject()->indexOfProperty(parent()->propertyName().toLatin1())).enumerator();
    return propEnum.keyToValue(typeName.toLatin1());
}

} // namespace LimeReport


