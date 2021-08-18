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
#ifndef LROBJECTPROPITEM_H
#define LROBJECTPROPITEM_H

#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QMetaProperty>
#include <QAbstractItemModel>
#include <QDebug>

#include "lrattribsabstractfactory.h"
#include "lrsingleton.h"

namespace LimeReport{

    class ObjectPropItem : public QObject
    {
        Q_OBJECT

    public:
        typedef QList< QObject* > ObjectsList;
        ObjectPropItem(){invalidate();}
        ObjectPropItem(QObject *object, ObjectsList* objects, const QString& propertyName, const QString& displayName, const QVariant& propertyValue, ObjectPropItem* parent, bool readonly=true);
        ObjectPropItem(QObject *object, ObjectsList* objects, const QString& propertyName, const QString& displayName, ObjectPropItem *parent, bool isClass = false);
        ~ObjectPropItem();

        virtual QVariant propertyValue() const;
        virtual void     setPropertyValue(QVariant value);
        virtual QString  propertyName() const {return m_name;}
        virtual QString  displayName() const;
        virtual QString  displayValue() const;
        virtual QIcon    iconValue() const{return QIcon();}
        virtual bool     isHaveChildren() const {return m_childItems.count()>0;}
        virtual bool     isHaveValue() const {return m_haveValue;}
        virtual bool     isValueReadonly() const {return m_readonly;}
                void     setValueReadOnly(bool value){m_readonly=value;}
        virtual bool     isValueModified() const {return false;}
        virtual QWidget* createProperyEditor(QWidget * /*parent*/) const {return 0;}
        virtual void     setPropertyEditorData(QWidget *, const QModelIndex &) const{}
        virtual void     setModelData(QWidget * /*editor*/, QAbstractItemModel * /*model*/, const QModelIndex &/*index*/){}
        virtual void     updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const;
        virtual void     updatePropertyValue();
        virtual bool     paint(QPainter *, const StyleOptionViewItem &, const QModelIndex &);

        ObjectPropItem*  parent() const{ return m_parent;}
        QObject* object() const{return m_object;}
        ObjectsList* objects()  {return m_objects;}
        ObjectPropItem*  child(int row);
        QList<ObjectPropItem*> children(){return m_childItems;}
        ObjectPropItem*  findChild(const QString& propertyName);
        ObjectPropItem*  findPropertyItem(const QString& propertyName);
        int  childCount();
        void appendItem(ObjectPropItem* item);
        void sortItem();
        int  row();
        bool isValid(){return m_valid;}
        int  colorIndex(){return m_colorIndex;}
        void setColorIndex(int propertyValue);
        void setModel(QAbstractItemModel* model){m_model=model;}
        QAbstractItemModel* model(){return m_model;}
        void setModelIndex(const QModelIndex& index){m_index=index;}
        QModelIndex modelIndex(){return m_index;}
        bool isClass(){return m_isClass;}
        bool isTranslateProperty() const;
        void setTranslateProperty(bool translatePropperty);
#ifdef INSPECT_BASEDESIGN
    private slots:
        void slotPropertyChanged(const QString& name, QVariant, QVariant newValue);
        void slotPropertyObjectName(const QString& oldValue, const QString& newValue);
#endif
    private:
        bool m_valid;
        void invalidate(){m_object=0; m_objects=0; m_valid = false; m_name = ""; m_value=QVariant(); m_isClass=false;}

    protected:
        void beginChangeValue(){ m_changingValue = true; }
        void endChangeValue(){ m_changingValue = false; }
        bool isValueChanging(){ return m_changingValue; }
        void setValueToObject(const QString& propertyName, QVariant propertyValue);
    private:
        QObject* m_object;
        ObjectsList* m_objects;
        QString m_name;
        QString m_displayName;
        QVariant m_value;
        bool m_haveValue;
        ObjectPropItem* m_parent;
        QList<ObjectPropItem*> m_childItems;
        QList<ObjectPropItem*> m_globalPropList;
        int m_colorIndex;
        bool m_readonly;
        QAbstractItemModel* m_model;
        QModelIndex m_index;
        bool m_isClass;
        bool m_changingValue;
        bool m_translatePropperty;
    };

    typedef QPair<QString,QString> APropIdent;
    typedef ObjectPropItem* (*CreatePropItem)(QObject *object, ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value, ObjectPropItem* parent, bool readonly);

    class ObjectPropFactory : public AttribsAbstractFactory<LimeReport::ObjectPropItem, APropIdent, CreatePropItem, QString>
    {
    private:
        friend class Singleton<ObjectPropFactory>;
    private:
        ObjectPropFactory(){}
        ~ObjectPropFactory(){}
        ObjectPropFactory(const ObjectPropFactory&){}
        ObjectPropFactory& operator = (const ObjectPropFactory&){return *this;}
    };

}

Q_DECLARE_METATYPE(LimeReport::ObjectPropItem*)

#endif // LROBJECTPROPITEM_H
