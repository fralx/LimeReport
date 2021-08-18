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
#include "lrsubitemparentpropitem.h"
#include "../objectinspector/editors/lrcomboboxeditor.h"
#include "lrbasedesignintf.h"
#include "lritemdesignintf.h"
#include "lrpagedesignintf.h"
#include "lrobjectpropitem.h"


namespace{
    LimeReport::ObjectPropItem * createLocationPropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
    {
        return new LimeReport::ItemLocationPropItem(object, objects, name, displayName, data, parent, readonly);
    }
    bool VARIABLE_IS_NOT_USED registred = LimeReport::ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("itemLocation","LimeReport::ItemDesignIntf"),QObject::tr("itemLocation"),createLocationPropItem
    );
}

LimeReport::ItemLocationPropItem::ItemLocationPropItem(QObject* object, ObjectsList* objects, const QString &name, const QString &displayName, const QVariant &value, ObjectPropItem* parent, bool readonly)
  :LimeReport::ObjectPropItem(object, objects, name, displayName, value, parent, readonly){
    m_locationMap.insert(tr("Band"),LimeReport::ItemDesignIntf::Band);
    m_locationMap.insert(tr("Page"),LimeReport::ItemDesignIntf::Page);
}


QWidget * LimeReport::ItemLocationPropItem::createProperyEditor(QWidget *parent) const{
    ComboBoxEditor *editor = new ComboBoxEditor(parent);
    connect(editor,SIGNAL(currentIndexChanged(QString)),this,SLOT(slotLocationChanged(QString)));
    LimeReport::BaseDesignIntf *item=dynamic_cast<LimeReport::BaseDesignIntf*>(object());
    if (item){
        QStringList locationTypes;
        foreach(QString location,m_locationMap.keys()){
            locationTypes.append(location);
        }
        editor->addItems(locationTypes);
    }
    return editor;
}

QString LimeReport::ItemLocationPropItem::displayValue() const{
    return locationToString(static_cast<LimeReport::ItemDesignIntf::LocationType>(propertyValue().toInt()));
}

void LimeReport::ItemLocationPropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const{
    ComboBoxEditor *editor=qobject_cast<ComboBoxEditor *>(propertyEditor);
    editor->setTextValue(locationToString(propertyValue().toInt()));
}

void LimeReport::ItemLocationPropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index){
    Q_UNUSED(propertyEditor)
    model->setData(index,object()->property(propertyName().toLatin1()));
    setValueToObject(propertyName(), propertyValue());
}

QString LimeReport::ItemLocationPropItem::locationToString(LimeReport::ItemDesignIntf::LocationType location) const{
    return m_locationMap.key(location);
}

LimeReport::ItemDesignIntf::LocationType LimeReport::ItemLocationPropItem::stringToLocation(const QString &locationName){
    return m_locationMap.value(locationName);
}

void LimeReport::ItemLocationPropItem::slotLocationChanged(const QString &text){
    if ( locationToString(object()->property(propertyName().toLatin1()).toInt())!=text){
      object()->setProperty(propertyName().toLatin1(),stringToLocation(text));
    }
}
