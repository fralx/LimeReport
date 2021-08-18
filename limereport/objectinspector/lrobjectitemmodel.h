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
#ifndef LROBJECTITEMMODEL_H
#define LROBJECTITEMMODEL_H

#include <QAbstractItemModel>
#include <QVariant>
#include <QObject>
#include "lrobjectpropitem.h"

namespace LimeReport{


class ValidatorIntf {
public:
    virtual bool validate(const QString& propName, const QVariant& propValue, QObject* object, QString& msg) = 0;
    virtual ~ValidatorIntf(){}
};

class QObjectPropertyModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QObjectPropertyModel(QObject *parent=0);
    ~QObjectPropertyModel();
    virtual void                setObject(QObject *object);
    virtual void                setMultiObjects(QList<QObject*>* list);
    virtual QModelIndex         index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex         parent(const QModelIndex &child) const;
    virtual int                 columnCount(const QModelIndex &parent) const;
    virtual int                 rowCount(const QModelIndex &parent) const;
    virtual QVariant            data(const QModelIndex &index, int role) const;
    virtual QVariant            headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags       flags(const QModelIndex &index) const;
    virtual bool                setData(const QModelIndex &index, const QVariant &value, int role);
    void                        itemDataChanged(const QModelIndex &index);
    void                        initModel();
    const QObject*              currentObject(){return m_object;}
    LimeReport::ObjectPropItem*   nodeFromIndex(const QModelIndex &index) const;
    LimeReport::ObjectPropItem*   rootNode(){return m_rootNode;}
    void                        updateProperty(const QString& propertyName);
    void                        setSubclassesAsLevel(bool value);
    bool                        subclassesAsLevel(){return m_subclassesAsLevel;}
    ValidatorIntf*              validator() const;
    void                        setValidator(ValidatorIntf* validator);
    void                        translatePropertyName();
    void                        clearObjectsList();
    bool                        isTranslateProperties() const;
    void                        setTranslateProperties(bool isTranslateProperties);

signals:
    void objectPropetyChanged(const QString& , const QVariant&, const QVariant&);
private slots:
    void slotObjectDestroyed(QObject* obj);
    void slotPropertyChanged(const QString& propertyName, const QVariant &oldValue, const QVariant &newValue);
    void slotPropertyObjectNameChanged(const QString& oldName, const QString& newName);
private:
    void                        addObjectProperties(const QMetaObject *metaObject, QObject *object, ObjectPropItem::ObjectsList* objects, int level=0);
    LimeReport::CreatePropItem    propertyItemCreator(QMetaProperty prop);
    LimeReport::ObjectPropItem*   createPropertyItem(QMetaProperty prop, QObject *object, ObjectPropItem::ObjectsList* objects, ObjectPropItem* parent);
private:
    LimeReport::ObjectPropItem*     m_rootNode;
    QObject*                        m_object;
    QList<QObject*>                 m_objects;
    bool                            m_dataChanging;
    bool                            m_subclassesAsLevel;
    ValidatorIntf*                  m_validator;
    bool                            m_translateProperties;
};

}
#endif // LROBJECTITEMMODEL_H
