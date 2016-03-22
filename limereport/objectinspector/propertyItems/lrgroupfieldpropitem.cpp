/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2015 by Alexander Arin                                  *
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
#include "lrgroupfieldpropitem.h"
#include "../editors/lrcomboboxeditor.h"
#include "lrgroupbands.h"
#include "lrreportengine_p.h"

namespace  {
    LimeReport::ObjectPropItem* createFieldPropItem(QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly){
        return new LimeReport::GroupFieldPropItem(object, objects, name, displayName, data, parent, readonly);
    }
    bool registredGroupFieldProp = LimeReport::ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("groupFieldName","LimeReport::GroupBandHeader"),QObject::tr("field"),createFieldPropItem
    );
}
namespace LimeReport {

QWidget *GroupFieldPropItem::createProperyEditor(QWidget *parent) const
{
    ComboBoxEditor *editor = new ComboBoxEditor(parent,true);
    editor->setEditable(true);
    GroupBandHeader *item=dynamic_cast<GroupBandHeader*>(object());
    if (item){
        DataBandDesignIntf* dataBand = dynamic_cast<DataBandDesignIntf*>(item->parentBand());
        if (dataBand){
            int propertyIndex = dataBand->metaObject()->indexOfProperty("datasource");

            if (item && propertyIndex>0){
               editor->addItems(item->reportEditor()->dataManager()->fieldNames(dataBand->property("datasource").toString()));
            }
        }
    }
    return editor;
}

void GroupFieldPropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const
{
    ComboBoxEditor *editor=qobject_cast<ComboBoxEditor *>(propertyEditor);
    editor->setTextValue(propertyValue().toString());
}

void GroupFieldPropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    model->setData(index,qobject_cast<ComboBoxEditor*>(propertyEditor)->text());
    object()->setProperty(propertyName().toLatin1(),propertyValue());
}

}

