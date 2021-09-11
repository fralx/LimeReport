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
#include "lrgroupfieldpropitem.h"
#include "../editors/lrcomboboxeditor.h"
#include "lrgroupbands.h"
#include "lrreportengine_p.h"

namespace  {
    LimeReport::ObjectPropItem* createFieldPropItem(QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly){
        return new LimeReport::GroupFieldPropItem(object, objects, name, displayName, data, parent, readonly);
    }
    bool VARIABLE_IS_NOT_USED registredGroupFieldProp = LimeReport::ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("groupFieldName","LimeReport::GroupBandHeader"),QObject::tr("field"),createFieldPropItem
    );
}
namespace LimeReport {


QString findDatasourceName(BandDesignIntf* band){
    if (!band) return "";
    if (!band->datasourceName().isEmpty()) return band->datasourceName();
    else return findDatasourceName(band->parentBand());
}

QWidget *GroupFieldPropItem::createProperyEditor(QWidget *parent) const
{
    ComboBoxEditor *editor = new ComboBoxEditor(parent,true);
    editor->setEditable(true);
    GroupBandHeader *item=dynamic_cast<GroupBandHeader*>(object());
    if (item){
        BandDesignIntf* dataBand = dynamic_cast<BandDesignIntf*>(item->parentBand());
        if (dataBand){
            QString datasourceName = findDatasourceName(dataBand);
            if (!datasourceName.isEmpty()){
               editor->addItems(item->reportEditor()->dataManager()->fieldNames(datasourceName));
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

