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
#include <QFontDialog>

#include "lrfontpropitem.h"
#include "editors/lrbuttonlineeditor.h"
#include "editors/lrfonteditor.h"
#include "editors/lrcheckboxeditor.h"
#include "lrobjectitemmodel.h"

namespace{
    LimeReport::ObjectPropItem * createFontPropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
    {
        return new LimeReport::FontPropItem(object, objects, name, displayName, data, parent, readonly);
    }
    bool VARIABLE_IS_NOT_USED registredFontProp = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("QFont",""),QObject::tr("QFont"),createFontPropItem);
}


namespace LimeReport{

FontPropItem::FontPropItem(QObject *object, ObjectPropItem::ObjectsList *objects, const QString &name, const QString &displayName, const QVariant &value, ObjectPropItem *parent, bool readonly)
    :ObjectPropItem(object, objects, name, displayName, value, parent, readonly)
{

    m_bold = new FontAttribPropItem(object,objects,"bold",tr("bold"),propertyValue().value<QFont>().bold(),this,false);
    m_italic = new FontAttribPropItem(object,objects,"italic",tr("italic"),propertyValue().value<QFont>().italic(),this,false);
    m_underline = new FontAttribPropItem(object,objects,"underline",tr("underline"),propertyValue().value<QFont>().underline(),this,false);
    m_pointSize = new FontPointSizePropItem(object,0,"pointSize",tr("size"),propertyValue().value<QFont>().pointSize(),this,false);
    m_family = new FontFamilyPropItem(object,0,"family",tr("family"),propertyValue().value<QFont>(),this,false);

    this->appendItem(m_family);
    this->appendItem(m_pointSize);
    this->appendItem(m_bold);
    this->appendItem(m_italic);
    this->appendItem(m_underline);
}

QWidget *FontPropItem::createProperyEditor(QWidget *parent) const
{
    return new FontEditor(parent);
}

QString FontPropItem::displayValue() const
{
    return toString(propertyValue().value<QFont>());//propertyValue().toString();//toString(propertyValue().value<QFont>());
}

void FontPropItem::setPropertyEditorData(QWidget* propertyEditor, const QModelIndex &) const
{
    FontEditor *editor =qobject_cast<FontEditor*>(propertyEditor);
    editor->setFontValue(propertyValue().value<QFont>());
}

void FontPropItem::setModelData(QWidget* propertyEditor, QAbstractItemModel* model, const QModelIndex &index)
{

    QFont tmpFont = qobject_cast<FontEditor*>(propertyEditor)->fontValue();
    QFont font(tmpFont.family());
    font.setBold(tmpFont.bold());
    font.setPointSize(tmpFont.pointSize());
    font.setItalic(tmpFont.italic());
    font.setUnderline(tmpFont.underline());
    model->setData(index,font);
    setValueToObject(propertyName(),propertyValue());
}

void FontPropItem::setPropertyValue(QVariant value)
{
    ObjectPropItem::setPropertyValue(value);
    m_bold->setPropertyValue(value.value<QFont>().bold());
    m_italic->setPropertyValue(value.value<QFont>().italic());
    m_underline->setPropertyValue(value.value<QFont>().underline());
    m_pointSize->setPropertyValue(value.value<QFont>().pointSize());
    m_family->setPropertyValue(value.value<QFont>());
}

QString FontPropItem::toString(QFont value) const
{
    QString attribs="";
    if (value.bold()) (attribs=="") ? attribs+="b":attribs+=",b";
    if (value.italic()) (attribs=="") ? attribs+="i":attribs+=",i";
    if (attribs!="") attribs="["+attribs+"]";
    return "\""+ value.family()+"\" "+QString::number(value.pointSize())+" "+attribs;
}

QString FontFamilyPropItem::displayValue() const
{
    QFont font = propertyValue().value<QFont>();
    return font.family();
}

QWidget *FontFamilyPropItem::createProperyEditor(QWidget *parent) const
{
    FontFamilyEditor* editor = new FontFamilyEditor(parent);
//    QFontComboBox* editor = new QFontComboBox(parent);
    editor->setAutoFillBackground(true);
    editor->setFont(propertyValue().value<QFont>());
    return editor;
}

void FontFamilyPropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const
{
    FontFamilyEditor* editor = qobject_cast<FontFamilyEditor*>(propertyEditor);
//    QFontComboBox* editor = qobject_cast<QFontComboBox*>(propertyEditor);
    editor->setFont(propertyValue().value<QFont>());
}

void FontFamilyPropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    QFont font = object()->property(parent()->propertyName().toLatin1()).value<QFont>();
//    font.setFamily(qobject_cast<QFontComboBox*>(propertyEditor)->currentFont().family());
    font.setFamily(qobject_cast<FontFamilyEditor*>(propertyEditor)->currentFont().family());
    model->setData(index,font);
    setValueToObject(parent()->propertyName(),font);
}

void FontAttribPropItem::setModelData(QWidget *propertyEditor , QAbstractItemModel *model, const QModelIndex &index)
{
    model->setData(index,qobject_cast<CheckBoxEditor*>(propertyEditor)->isChecked());
    QFont font = object()->property(parent()->propertyName().toLatin1()).value<QFont>();
    if (propertyName()=="bold"){
        font.setBold(propertyValue().toBool());
    }
    if (propertyName()=="italic"){
        font.setItalic(propertyValue().toBool());
    }
    if (propertyName()=="underline"){
        font.setUnderline(propertyValue().toBool());
    }
    setValueToObject(parent()->propertyName(),font);
}

void FontPointSizePropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    model->setData(index,qobject_cast<SpinBoxEditor*>(propertyEditor)->value());
    QFont font = object()->property(parent()->propertyName().toLatin1()).value<QFont>();
    font.setPointSize(propertyValue().toInt());
    setValueToObject(parent()->propertyName(),font);
}

FontFamilyEditor::FontFamilyEditor(QWidget *parent)
    :QWidget(parent)
{
    m_valueEditor = new QFontComboBox(this);
    setFocusProxy(m_valueEditor);
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->addWidget(m_valueEditor);
    hLayout->setContentsMargins(1,1,1,1);
    hLayout->setSpacing(0);
    setAutoFillBackground(true);
}

QFont FontFamilyEditor::currentFont()
{
    return m_valueEditor->currentFont();
}

void FontFamilyEditor::setFont(QFont font)
{
    m_valueEditor->setCurrentFont(font);
    m_valueEditor->setFont(font);
}

}
