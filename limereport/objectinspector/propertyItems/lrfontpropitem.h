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
#ifndef LRFONTPROPITEM_H
#define LRFONTPROPITEM_H

#include <QFontComboBox>

#include "lrobjectpropitem.h"
#include "lrboolpropitem.h"
#include "lrintpropitem.h"

namespace LimeReport{

class FontFamilyEditor : public QWidget{
    Q_OBJECT
public:
    FontFamilyEditor(QWidget* parent);
    QFont currentFont();
    void setFont(QFont font);
signals:
    void editingFinished();
private:
    QFontComboBox* m_valueEditor;
};

class FontFamilyPropItem : public ObjectPropItem
{
    Q_OBJECT
public:
    FontFamilyPropItem():ObjectPropItem(){}
    FontFamilyPropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value,ObjectPropItem* parent, bool readonly=true)
    :ObjectPropItem(object, objects, name, displayName, value, parent, readonly){}
    QString displayValue() const;
    QWidget* createProperyEditor(QWidget *parent) const;
    void setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const;
    void setModelData(QWidget *propertyEditor , QAbstractItemModel *model, const QModelIndex &index);
};

class FontPointSizePropItem : public IntPropItem
{
    Q_OBJECT
public:
    FontPointSizePropItem():IntPropItem(){}
    FontPointSizePropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value,ObjectPropItem* parent, bool readonly=true)
    :IntPropItem(object, objects, name, displayName, value, parent, readonly){}
    void setModelData(QWidget *propertyEditor , QAbstractItemModel *model, const QModelIndex &index);
};

class FontAttribPropItem : public BoolPropItem
{
    Q_OBJECT
public:
    FontAttribPropItem():BoolPropItem(){}
    FontAttribPropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value,ObjectPropItem* parent, bool readonly=true)
        :BoolPropItem(object, objects, name, displayName, value, parent, readonly){}
    void setModelData(QWidget *propertyEditor , QAbstractItemModel *model, const QModelIndex &index);
};

class FontPropItem : public ObjectPropItem
{
    Q_OBJECT
public:
    FontPropItem():ObjectPropItem(), m_pointSize(NULL), m_bold(NULL), m_italic(NULL), m_underline(NULL), m_family(NULL) {}
    FontPropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value,ObjectPropItem* parent, bool readonly);
    QWidget* createProperyEditor(QWidget *parent) const;
    QString displayValue() const;
    void setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const;
    void setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index);
    void setPropertyValue(QVariant value);
protected:
    QString toString(QFont value) const;
    FontPointSizePropItem* m_pointSize;
    FontAttribPropItem *m_bold;
    FontAttribPropItem *m_italic;
    FontAttribPropItem *m_underline;
    FontFamilyPropItem *m_family;
};

}
#endif // LRFONTPROPITEM_H
