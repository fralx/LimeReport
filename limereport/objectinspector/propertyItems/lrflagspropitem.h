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
#ifndef LRFLAGSPROPEDITOR_H
#define LRFLAGSPROPEDITOR_H

#include "lrobjectpropitem.h"
#include "lrboolpropitem.h"

namespace LimeReport{

class FlagsPropItem : public ObjectPropItem
{
    Q_OBJECT
public:
    FlagsPropItem():ObjectPropItem(){}
    FlagsPropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value,ObjectPropItem* parent, bool readonly);
    FlagsPropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value,ObjectPropItem* parent, bool readonly, QSet<int> acceptableValues);
    virtual QString displayValue() const;
    virtual void    setPropertyValue(QVariant propertyValue);
private slots:
    void slotEnumChanged(QString);
private:
    void translateFlagsItem();
private:
    QSet<int> m_acceptableValues;
    QString nameByType(int propertyValue) const;
    int     typeByName(QString propertyValue) const;
    void createChildren();
    void updateChildren();
};

class FlagPropItem : public BoolPropItem{
public:
    FlagPropItem():BoolPropItem(){}
    FlagPropItem(QObject* object, ObjectsList* objects, const QString& propName, const QString& displayName, const QVariant& propertyValue, ObjectPropItem* parent, bool readonly);
    virtual void setPropertyEditorData(QWidget * propertyEditor, const QModelIndex & ) const;
    virtual void setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &);
    virtual QString displayValue() const{return "";}
private:
    int valueByName(const QString &typeName);
};


} // namespace LimeReport

#endif // LRFLAGSPROPEDITOR_H
