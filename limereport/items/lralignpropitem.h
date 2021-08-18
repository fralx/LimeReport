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
#ifndef LRALIGNPROPITEM_H
#define LRALIGNPROPITEM_H

#include "lrobjectpropitem.h"
#include "objectinspector/propertyItems/lrenumpropitem.h"

namespace LimeReport{

typedef QMap<QString,Qt::Alignment> AlignMap;

class AlignmentItemEditor;

class AlignmentPropItem : public ObjectPropItem
{
    Q_OBJECT
public:
    AlignmentPropItem():ObjectPropItem(),m_horizEditor(NULL),m_vertEditor(NULL){}
    AlignmentPropItem(QObject *object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value, ObjectPropItem* parent, bool readonly=true);
    QString displayValue() const;
    void setPropertyValue(QVariant value);
private:
    AlignMap m_vertMap;
    AlignMap m_horizMap;
    AlignmentItemEditor* m_horizEditor;
    AlignmentItemEditor* m_vertEditor;
    QString associateValue(int value, const AlignMap *map) const;
};

class AlignmentItemEditor : public ObjectPropItem
{
    Q_OBJECT
public:
    AlignmentItemEditor():ObjectPropItem(){}
    AlignmentItemEditor(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value, ObjectPropItem* parent, bool readonly,
                        AlignMap acceptableValues);
    void        setModelData(QWidget * propertyEditor, QAbstractItemModel * model, const QModelIndex & index);
    void        setPropertyEditorData(QWidget * propertyEditor, const QModelIndex &) const;
    QWidget*    createProperyEditor(QWidget *parent) const;
    QString     displayValue() const;
    void setPropertyValue(QVariant value);
protected:
    QVector<int>    extractAcceptableValue(int flags);
    int             clearAcceptableValues(int flags);
    AlignMap        acceptableValues() const {return m_acceptableValues;}
private:
    AlignMap m_acceptableValues;
};


}
#endif // LRALIGNPROPITEM_H
