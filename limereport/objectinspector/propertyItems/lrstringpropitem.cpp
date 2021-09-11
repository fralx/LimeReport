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
#include <QLineEdit>
#include <QDebug>
#include <QString>

#include "lrstringpropitem.h"
#include "lrobjectpropitem.h"
#include "objectinspector/editors/lrbuttonlineeditor.h"

namespace{
    LimeReport::ObjectPropItem * createStringPropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
    {
        return new LimeReport::StringPropItem(object, objects, name, displayName, data, parent, readonly);
    }
    bool VARIABLE_IS_NOT_USED registredStringProp = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("QString",""),QObject::tr("QString"),createStringPropItem);
} // namespace

namespace LimeReport{


QWidget * StringPropItem::createProperyEditor(QWidget *parent) const
{
    return new ButtonLineEditor(object()->objectName()+"."+displayName(),parent);
}

void StringPropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const
{
    ButtonLineEditor *editor = qobject_cast<ButtonLineEditor *>(propertyEditor);
    editor->setText(propertyValue().toString());
}

void StringPropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    model->setData(index,qobject_cast<ButtonLineEditor*>(propertyEditor)->text());
    object()->setProperty(propertyName().toLatin1(),propertyValue());
}

QString StringPropItem::displayValue() const
{
    return propertyValue().toString().simplified();
}

} // namespace LimeReport
