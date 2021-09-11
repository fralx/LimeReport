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
#include "lrenumpropitem.h"
#include "../editors/lrcomboboxeditor.h"
#include "lrbanddesignintf.h"

namespace {

LimeReport::ObjectPropItem * createEnumPropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
{
    return new LimeReport::EnumPropItem(object, objects, name, displayName, data, parent, readonly);
}
bool VARIABLE_IS_NOT_USED registred = LimeReport::ObjectPropFactory::instance().registerCreator(
    LimeReport::APropIdent("enum",""),QObject::tr("enum"),createEnumPropItem
);

}

namespace LimeReport {

QWidget *EnumPropItem::createProperyEditor(QWidget *parent) const
{
    ComboBoxEditor *editor = new ComboBoxEditor(parent,false);
    connect(editor,SIGNAL(currentIndexChanged(QString)),this,SLOT(slotEnumChanged(QString)));

    QStringList enumValues;
    QMetaEnum propEnum = object()->metaObject()->property(object()->metaObject()->indexOfProperty(propertyName().toLatin1())).enumerator();
    for (int i=0;i<propEnum.keyCount();i++){
        if (m_acceptableValues.isEmpty()) enumValues.append(tr(propEnum.key(i)));
        else {
            if (m_acceptableValues.contains(propEnum.value(i))){
                enumValues.append(tr(propEnum.key(i)));
            }
        }
    }
    editor->addItems(enumValues);
    return editor;
}

void EnumPropItem::slotEnumChanged(const QString &text)
{
    if ( nameByType(object()->property(propertyName().toLatin1()).toInt())!=text){
        beginChangeValue();
        setPropertyValue(typeByName(text));
        setValueToObject(propertyName(),typeByName(text));
        endChangeValue();
    }
}

void EnumPropItem::initTranslation()
{
    QMetaEnum propEnum = object()->metaObject()->property(object()->metaObject()->indexOfProperty(propertyName().toLatin1())).enumerator();
    for (int i=0;i<propEnum.keyCount();i++){
        m_translation.insert(QString(tr(propEnum.key(i))), QString(propEnum.key(i)));
    }
}

void EnumPropItem::translateEnumItemName()
{
    tr("Default");
    tr("Portrait");
    tr("Landscape");
    tr("NoneAutoWidth");
    tr("MaxWordLength");
    tr("MaxStringLength");
    tr("TransparentMode");
    tr("OpaqueMode");
    tr("Angle0");
    tr("Angle90");
    tr("Angle180");
    tr("Angle270");
    tr("Angle45");
    tr("Angle315");
    tr("DateTime");
    tr("Double");
    tr("NoBrush");
    tr("SolidPattern");
    tr("Dense1Pattern");
    tr("Dense2Pattern");
    tr("Dense3Pattern");
    tr("Dense4Pattern");
    tr("Dense5Pattern");
    tr("Dense6Pattern");
    tr("Dense7Pattern");
    tr("HorPattern");
    tr("VerPattern");
    tr("CrossPattern");
    tr("BDiagPattern");
    tr("FDiagPattern");
    tr("LeftToRight");
    tr("RightToLeft");
    tr("LayoutDirectionAuto");
    tr("LeftItemAlign");
    tr("RightItemAlign");
    tr("CenterItemAlign");
    tr("ParentWidthItemAlign");
    tr("DesignedItemAlign");
    tr("HorizontalLine");
    tr("VerticalLine");
    tr("Ellipse");
    tr("Rectangle");
    tr("Page");
    tr("Band");
    tr("Horizontal");
    tr("Vertical");
    tr("VerticalUniform");
    tr("Pie");
    tr("VerticalBar");
    tr("HorizontalBar");
    tr("LegendAlignTop");
    tr("LegendAlignCenter");
    tr("LegendAlignBottom");
    tr("TitleAlignLeft");
    tr("TitleAlignRight");
    tr("TitleAlignCenter");
    tr("Layout");
    tr("Table");
    tr("Millimeters");
    tr("Inches");
    tr("Scale");
    tr("Split");
}

void EnumPropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const
{
    ComboBoxEditor *editor=qobject_cast<ComboBoxEditor *>(propertyEditor);
    editor->setTextValue(nameByType(propertyValue().toInt()));
}

void EnumPropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    setValueToObject(propertyName(),typeByName(qobject_cast<ComboBoxEditor*>(propertyEditor)->text()));
    model->setData(index,object()->property(propertyName().toLatin1()));
}

QString EnumPropItem::nameByType(int value) const
{
    QMetaEnum propEnum = object()->metaObject()->property(object()->metaObject()->indexOfProperty(propertyName().toLatin1())).enumerator();
    return isTranslateProperty() ? tr(propEnum.valueToKey(value)) : propEnum.valueToKey(value);
}

int EnumPropItem::typeByName(const QString &value) const
{
    QMetaEnum propEnum = object()->metaObject()->property(object()->metaObject()->indexOfProperty(propertyName().toLatin1())).enumerator();
    return propEnum.keyToValue(m_translation.value(value).toLatin1());
}

QString EnumPropItem::displayValue() const
{
    return nameByType((propertyValue().toInt()));
}

}
