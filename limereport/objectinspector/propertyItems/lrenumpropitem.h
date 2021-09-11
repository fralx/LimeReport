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
#ifndef LRENUMPROPITEM_H
#define LRENUMPROPITEM_H

#include "lrobjectpropitem.h"
#include <QMap>

namespace LimeReport{
class EnumPropItem : public ObjectPropItem
{
    Q_OBJECT
public:
    EnumPropItem():ObjectPropItem(), m_settingValue(false){initTranslation();}
    EnumPropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value,ObjectPropItem* parent, bool readonly)
        :ObjectPropItem(object, objects, name, displayName, value, parent, readonly),m_settingValue(false){initTranslation();}
    EnumPropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value,ObjectPropItem* parent, bool readonly, QVector<int> acceptableValues)
        :ObjectPropItem(object, objects, name, displayName, value, parent, readonly),m_acceptableValues(acceptableValues),m_settingValue(false){initTranslation();}
    QWidget*    createProperyEditor(QWidget *parent) const;
    QString     displayValue() const;
    void        setPropertyEditorData(QWidget * propertyEditor, const QModelIndex &) const;
    void        setModelData(QWidget * propertyEditor, QAbstractItemModel * model, const QModelIndex & index);
    QVector<int>        acceptableValues() const {return m_acceptableValues;}
protected:
    QString nameByType(int propertyValue) const;
    int     typeByName(const QString& propertyValue) const;
private slots:
    void slotEnumChanged(const QString& text);
private:
    void initTranslation();
    void translateEnumItemName();
private:
    QVector<int> m_acceptableValues;
    bool m_settingValue;
    QMap<QString, QString> m_translation;
};
}
#endif // LRENUMPROPITEM_H
