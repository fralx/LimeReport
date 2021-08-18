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
#include "lralignpropitem.h"
#include "objectinspector/propertyItems/lrenumpropitem.h"
#include "objectinspector/editors/lrcomboboxeditor.h"
#include "lrtextitem.h"

namespace{
    LimeReport::ObjectPropItem * createAlignItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly
    ){
        return new LimeReport::AlignmentPropItem(object, objects, name, displayName, data, parent, readonly);
    }

    bool VARIABLE_IS_NOT_USED registred = LimeReport::ObjectPropFactory::instance().registerCreator(
                LimeReport::APropIdent("alignment","LimeReport::TextItem"),
                QObject::tr("alignment"),
                createAlignItem
                );
}

namespace LimeReport{

QString AlignmentPropItem::associateValue(int value, const AlignMap* map) const
{
    QString result;
    QMap<QString,Qt::Alignment>::const_iterator it = map->constBegin();
    for(;it!= map->constEnd();++it){
        if ((value & it.value())) {
            if (result.isEmpty()) result+=it.key();
            else result=result+" | "+it.key();
        }
    }
    return result;
}

AlignmentPropItem::AlignmentPropItem(QObject *object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value, ObjectPropItem *parent, bool readonly)
    :ObjectPropItem(object, objects, name,displayName,value,parent,readonly)
{

    m_horizMap.insert(tr("Left"),Qt::AlignLeft);
    m_horizMap.insert(tr("Right"),Qt::AlignRight);
    m_horizMap.insert(tr("Center"),Qt::AlignHCenter);
    m_horizMap.insert(tr("Justify"),Qt::AlignJustify);

    m_vertMap.insert(tr("Top"),Qt::AlignTop);
    m_vertMap.insert(tr("Center"),Qt::AlignVCenter);
    m_vertMap.insert(tr("Botom"),Qt::AlignBottom);

    m_horizEditor = new AlignmentItemEditor(object, objects, name,tr("horizontal"),value.toInt(),this,false,m_horizMap);
    m_vertEditor = new AlignmentItemEditor(object, objects, name,tr("vertical"), value.toInt(),this,false,m_vertMap);

    this->appendItem(m_horizEditor);
    this->appendItem(m_vertEditor);
}

QString AlignmentPropItem::displayValue() const
{
    return associateValue(propertyValue().toInt(),&m_horizMap)+" | "+
            associateValue(propertyValue().toInt(),&m_vertMap);
}

void AlignmentPropItem::setPropertyValue(QVariant value)
{
    m_horizEditor->setPropertyValue(value);
    m_vertEditor->setPropertyValue(value);
    ObjectPropItem::setPropertyValue(value);
}

AlignmentItemEditor::AlignmentItemEditor(QObject *object, ObjectsList* objects, const QString &name, const QString &displayName, const QVariant &value, ObjectPropItem *parent,
                                         bool readonly, AlignMap acceptableValues)
    :ObjectPropItem(object, objects, name, displayName, value, parent, readonly),m_acceptableValues(acceptableValues)
{
    if (! extractAcceptableValue(value.toInt()).isEmpty())
        setPropertyValue(extractAcceptableValue(value.toInt())[0]);
    else setPropertyValue(0);
}

void AlignmentItemEditor::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    int flags = object()->property(propertyName().toLatin1()).toInt();
    int align = m_acceptableValues.value(qobject_cast<ComboBoxEditor*>(propertyEditor)->text());
    flags=clearAcceptableValues(flags) | align;
    object()->setProperty(propertyName().toLatin1(),flags);
    if (objects())
        foreach(QObject* item,*objects()){item->setProperty(propertyName().toLatin1(),flags);}
    parent()->setPropertyValue(flags);
    model->setData(index,align);
}

QVector<int> AlignmentItemEditor::extractAcceptableValue(int flags)
{
    QVector<int> result;
    AlignMap::const_iterator it = m_acceptableValues.constBegin();
    for (;it != m_acceptableValues.constEnd();++it)
    {
        if (flags & it.value()) result<<it.value();
    }
    return result;
}


int AlignmentItemEditor::clearAcceptableValues(int flags)
{
    AlignMap::const_iterator it = m_acceptableValues.constBegin();
    for (;it != m_acceptableValues.constEnd();++it)
    {
        if (flags & it.value())
           flags=flags^it.value();
    }
    return flags;
}

QWidget *AlignmentItemEditor::createProperyEditor(QWidget *parent) const
{
    ComboBoxEditor *editor = new ComboBoxEditor(parent);
    QStringList enumValues;
    enumValues<<m_acceptableValues.keys();
    editor->addItems(enumValues);
    return editor;
}

void AlignmentItemEditor::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const
{
    ComboBoxEditor *editor=qobject_cast<ComboBoxEditor *>(propertyEditor);
    editor->setTextValue(m_acceptableValues.key(Qt::Alignment(propertyValue().toInt())));
}

QString AlignmentItemEditor::displayValue() const
{
    return m_acceptableValues.key(Qt::Alignment(propertyValue().toInt()));
}

void AlignmentItemEditor::setPropertyValue(QVariant value)
{
    QVector<int> _accpepttableValueList= extractAcceptableValue(value.toInt());
    if(_accpepttableValueList.isEmpty()) return;

    ObjectPropItem::setPropertyValue(_accpepttableValueList[0]);
}

}
