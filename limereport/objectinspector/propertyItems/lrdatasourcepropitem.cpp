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
#include "lrdatasourcepropitem.h"
#include "lrobjectpropitem.h"
#include "lrbasedesignintf.h"
#include "lrreportengine_p.h"
#include "../editors/lrcomboboxeditor.h"
#include <QComboBox>

namespace{
    LimeReport::ObjectPropItem * createDatasourcePropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
    {
        return new LimeReport::DatasourcePropItem(object, objects, name, displayName, data, parent, readonly);
    }

    LimeReport::ObjectPropItem* createFieldPropItem(QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly){
        return new LimeReport::FieldPropItem(object, objects, name, displayName, data, parent, readonly);
    }

    bool VARIABLE_IS_NOT_USED registredDatasouceProp = LimeReport::ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("datasource","LimeReport::DataBandDesignIntf"),QObject::tr("datasource"),createDatasourcePropItem
    );
    
    bool VARIABLE_IS_NOT_USED registredImageDatasouceProp = LimeReport::ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("datasource","LimeReport::ImageItem"),QObject::tr("datasource"),createDatasourcePropItem
    );
    
    bool VARIABLE_IS_NOT_USED registredImageFieldProp = LimeReport::ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("field","LimeReport::ImageItem"),QObject::tr("field"),createFieldPropItem
    );
    
    bool VARIABLE_IS_NOT_USED registredChartDatasouceProp = LimeReport::ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("datasource","LimeReport::ChartItem"),QObject::tr("datasource"),createDatasourcePropItem
	);
	
    bool VARIABLE_IS_NOT_USED registredBarcodeDatasouceProp = LimeReport::ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("datasource","LimeReport::BarcodeItem"),QObject::tr("datasource"),createDatasourcePropItem
    );
    
    bool VARIABLE_IS_NOT_USED registredBarcodeFieldProp = LimeReport::ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("field","LimeReport::BarcodeItem"),QObject::tr("field"),createFieldPropItem
    );

    bool VARIABLE_IS_NOT_USED registredSVGItemDatasouceProp = LimeReport::ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("datasource","LimeReport::SVGItem"),QObject::tr("datasource"),createDatasourcePropItem
    );

    bool VARIABLE_IS_NOT_USED registredSVGFieldProp = LimeReport::ObjectPropFactory::instance().registerCreator(
        LimeReport::APropIdent("field","LimeReport::SVGItem"),QObject::tr("field"),createFieldPropItem
    );
}

QWidget* LimeReport::DatasourcePropItem::createProperyEditor(QWidget *parent) const{
    ComboBoxEditor *editor = new ComboBoxEditor(parent,true);
    editor->setEditable(true);
    LimeReport::BaseDesignIntf *item=dynamic_cast<LimeReport::BaseDesignIntf*>(object());
    if (item){
       editor->addItems(item->reportEditor()->dataManager()->dataSourceNames());
    }
    return editor;
}

void LimeReport::DatasourcePropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const{
    ComboBoxEditor *editor=qobject_cast<ComboBoxEditor *>(propertyEditor);
    editor->setTextValue(propertyValue().toString());
}

void LimeReport::DatasourcePropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index){
    model->setData(index,qobject_cast<ComboBoxEditor*>(propertyEditor)->text());
    object()->setProperty(propertyName().toLatin1(),propertyValue());
}


QWidget *LimeReport::FieldPropItem::createProperyEditor(QWidget *parent) const
{
    ComboBoxEditor *editor = new ComboBoxEditor(parent);
    editor->setEditable(true);
    LimeReport::BaseDesignIntf *item=dynamic_cast<LimeReport::BaseDesignIntf*>(object());
    int propertyIndex = object()->metaObject()->indexOfProperty("datasource");

    if (item && propertyIndex>0){
       editor->addItems(item->reportEditor()->dataManager()->fieldNames(object()->property("datasource").toString()));
    }
    return editor;
}

void LimeReport::FieldPropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const
{
    ComboBoxEditor *editor=qobject_cast<ComboBoxEditor *>(propertyEditor);
    editor->setTextValue(propertyValue().toString());
}

void LimeReport::FieldPropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    model->setData(index,qobject_cast<ComboBoxEditor*>(propertyEditor)->text());
    object()->setProperty(propertyName().toLatin1(),propertyValue());
}
