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
#include "lrobjectpropitem.h"
#include "lrobjectitemmodel.h"
#ifdef INSPECT_BASEDESIGN
#include "lrbasedesignintf.h"
#endif
#include <QDebug>

namespace LimeReport {

bool lesThen(ObjectPropItem* v1, ObjectPropItem* v2){
    return v1->displayName().compare(v2->displayName())<0;
}

ObjectPropItem::ObjectPropItem(QObject *object, ObjectsList* objects, const QString &name, const QString &displayName, ObjectPropItem *parent, bool isClass)
    :m_object(object), m_name(name), m_displayName(displayName), m_haveValue(false), m_parent(parent), m_colorIndex(-1),
      m_readonly(true), m_model(0), m_isClass(isClass), m_changingValue(false),
      m_translatePropperty(true)
{
    if (parent) setModel(parent->model());
    m_index=QModelIndex();
    //if (objects) foreach(QObject* item, *objects) m_objects.append(item);
    m_objects = objects;
#ifdef INSPECT_BASEDESIGN
    BaseDesignIntf * item = dynamic_cast<BaseDesignIntf*>(object);
    if (item){
        connect(item,SIGNAL(propertyChanged(QString,QVariant,QVariant)),this,SLOT(slotPropertyChanged(QString,QVariant,QVariant)));
        connect(item,SIGNAL(propertyObjectNameChanged(QString,QString)),this,SLOT(slotPropertyObjectName(QString,QString)));
    }
#endif
}

ObjectPropItem::ObjectPropItem(QObject *object, ObjectsList* objects, const QString &name, const QString &displayName, const QVariant &value, ObjectPropItem *parent, bool readonly)
    :m_object(object), m_name(name), m_displayName(displayName), m_value(value),
     m_haveValue(true), m_parent(parent), m_colorIndex(-1),
     m_readonly(readonly), m_model(0), m_isClass(false), m_changingValue(false),
     m_translatePropperty(true)
{
    if (parent) setModel(parent->model());
    m_index=QModelIndex();
    //if (objects) foreach(QObject* item, *objects) m_objects.append(item);
    m_objects = objects;
#ifdef INSPECT_BASEDESIGN
    BaseDesignIntf * item = dynamic_cast<BaseDesignIntf*>(object);
    if (item){
        connect(item,SIGNAL(propertyChanged(QString,QVariant,QVariant)),this,SLOT(slotPropertyChanged(QString,QVariant,QVariant)));
        connect(item,SIGNAL(propertyObjectNameChanged(QString,QString)),this,SLOT(slotPropertyObjectName(QString,QString)));
    }
#endif
}

ObjectPropItem::~ObjectPropItem(){
  qDeleteAll(m_childItems);
}


int ObjectPropItem::childCount(){
    return m_childItems.count();
}

void ObjectPropItem::appendItem(ObjectPropItem *item){
    m_childItems.append(item); 
    if (m_parent && (!item->isClass())) m_parent->m_globalPropList.append(item);
}

void ObjectPropItem::sortItem()
{
    std::sort(m_childItems.begin(), m_childItems.end(), lesThen);
}

QVariant ObjectPropItem::propertyValue() const {
    return m_value;
}

void ObjectPropItem::setPropertyValue(QVariant value){
    m_value=value;
    LimeReport::QObjectPropertyModel *itemModel=dynamic_cast<LimeReport::QObjectPropertyModel *>(model());
    if (itemModel){
        itemModel->itemDataChanged(modelIndex());
        foreach(ObjectPropItem*item, children()){
            if (item->modelIndex().isValid()) itemModel->itemDataChanged(item->modelIndex());
        }
    }
}

QString ObjectPropItem::displayName() const {
    return isTranslateProperty() ? m_displayName : propertyName();
}

int ObjectPropItem::row(){
    if (m_parent)
        return m_parent->m_childItems.indexOf(const_cast<ObjectPropItem*>(this));
    return 0;
}

ObjectPropItem * ObjectPropItem::child(int row){
    return m_childItems[row];
}

void ObjectPropItem::setColorIndex(int value)
{
    m_colorIndex=value;
    for (int i=0;i<m_childItems.count();i++){
        m_childItems[i]->setColorIndex(value);
    }
}

#ifdef INSPECT_BASEDESIGN
void ObjectPropItem::slotPropertyChanged(const QString &name, QVariant, QVariant newValue)
{
    if (name.compare(propertyName(),Qt::CaseInsensitive)==0 && !isValueChanging()){
        setPropertyValue(newValue);
    }
}

void ObjectPropItem::slotPropertyObjectName(const QString &oldValue, const QString &newValue)
{
    Q_UNUSED(oldValue)
    if (propertyName().compare("objectName",Qt::CaseInsensitive)==0 && !isValueChanging()){
        setPropertyValue(newValue);
    }
}
#endif

void ObjectPropItem::setValueToObject(const QString &propertyName, QVariant propertyValue)
{
    object()->setProperty(propertyName.toLatin1(),propertyValue);
    if (objects()){
        foreach (QObject* item, *objects()) {
            if (item->metaObject()->indexOfProperty(propertyName.toLatin1())!=-1)
                item->setProperty(propertyName.toLatin1(), propertyValue);
        }
    }
}

bool ObjectPropItem::isTranslateProperty() const
{
    return m_translatePropperty;
}

void ObjectPropItem::setTranslateProperty(bool translatePropperty)
{
    m_translatePropperty = translatePropperty;
}

ObjectPropItem * ObjectPropItem::findChild(const QString &name)
{
    foreach(ObjectPropItem* item,m_childItems){
        if (item->propertyName()==name) return item;
    }
    return 0;
}

ObjectPropItem *ObjectPropItem::findPropertyItem(const QString &propertyName)
{
    foreach(ObjectPropItem* item,m_globalPropList){
        if (item->propertyName()==propertyName) return item;
    }
    return 0;
}

void ObjectPropItem::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}

void ObjectPropItem::updatePropertyValue()
{
    m_model->setData(m_index,m_object->property(m_name.toLatin1()));
}

bool ObjectPropItem::paint(QPainter *, const StyleOptionViewItem &, const QModelIndex &)
{
    return false;
}

QString ObjectPropItem::displayValue() const
{
    return m_value.toString();
}

}
