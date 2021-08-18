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
#ifndef LRSUBITEMPARENTPROPITEM_H
#define LRSUBITEMPARENTPROPITEM_H

#include <QMap>

#include "lrobjectpropitem.h"
#include "lritemdesignintf.h"

namespace LimeReport{
class ItemLocationPropItem : public LimeReport::ObjectPropItem{
    Q_OBJECT
public:
    ItemLocationPropItem():ObjectPropItem(){}
    ItemLocationPropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value, ObjectPropItem* parent, bool readonly);
    QWidget* createProperyEditor(QWidget *parent) const;
    QString displayValue() const;
    void setPropertyEditorData(QWidget *, const QModelIndex &) const;
    void setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &);
private slots:
    void slotLocationChanged(const QString& text);
private:
    QString locationToString(LimeReport::ItemDesignIntf::LocationType location) const;
    QString locationToString(int location) const {return locationToString(static_cast<LimeReport::ItemDesignIntf::LocationType>(location));}
    LimeReport::ItemDesignIntf::LocationType stringToLocation(const QString& locationName);
private:
    QMap<QString,LimeReport::ItemDesignIntf::LocationType> m_locationMap;
};
}
#endif // LRSUBITEMPARENTPROPITEM_H
