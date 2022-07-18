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
#include "lrrectproptem.h"
#include "lrobjectpropitem.h"
#include "lrbanddesignintf.h"
#include "lrpageitemdesignintf.h"
#include "lrglobal.h"
#include "lrobjectitemmodel.h"

#include <QAbstractItemModel>
#include <QRect>
#include <QDoubleSpinBox>
#include <QDebug>

namespace{
    LimeReport::ObjectPropItem * createReqtItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly
    ){
        return new LimeReport::RectPropItem(object, objects, name, displayName, data, parent, readonly);
    }
    LimeReport::ObjectPropItem * createReqtUnitItem(
        QObject*object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly
    ){
        return new LimeReport::RectUnitPropItem(object, objects, name, displayName, data, parent, readonly);
    }
    bool VARIABLE_IS_NOT_USED registredRectProp = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("QRect",""),QObject::tr("QRect"),createReqtItem);
    bool VARIABLE_IS_NOT_USED registredRectFProp = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("QRectF",""),QObject::tr("QRectF"),createReqtItem);
    bool VARIABLE_IS_NOT_USED registredRectMMProp = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("geometry","LimeReport::BaseDesignIntf"),QObject::tr("geometry"),createReqtUnitItem);
}

namespace LimeReport{

template<class T> QString rectToString(T rect)
{
    return QString("[%1,%2] %3x%4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
}

QRectF modifyRect(QRectF rect, const QString& name, qreal itemValue){
    if (name=="x"){qreal width=rect.width(); rect.setX(itemValue); rect.setWidth(width);}
    if (name=="y"){qreal heigh=rect.height(); rect.setY(itemValue); rect.setHeight(heigh);}
    if (name=="height"){rect.setHeight(itemValue);}
    if (name=="width"){rect.setWidth(itemValue);}

    return rect;
}

}

LimeReport::RectPropItem::RectPropItem(QObject *object, ObjectsList* objects, const QString &name, const QString &displayName, const QVariant &value, ObjectPropItem *parent, bool readonly):
    ObjectPropItem(object, objects, name, displayName, value,parent,readonly)
{
    QRect rect=value.toRect();
    this->appendItem(new ObjectPropItem(object, objects, "x","x", rect.x(),this));
    this->appendItem(new ObjectPropItem(object, objects, "y","x", rect.y(),this));
    this->appendItem(new ObjectPropItem(object, objects, "width",tr("width"), rect.width(),this));
    this->appendItem(new ObjectPropItem(object, objects, "heigh",tr("height"),rect.height(),this));
}

QString LimeReport::RectPropItem::displayValue() const
{
    //TODO: Migrate to QMetaType
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    switch(propertyValue().typeId()){
        case QMetaType::QRect:
            return rectToString(propertyValue().toRect());
        case QMetaType::QRectF:
            return rectToString(propertyValue().toRect());
        default :
            return ObjectPropItem::displayValue();
    }
#else
    switch(propertyValue().type()){
        case QVariant::Rect:
            return rectToString(propertyValue().toRect());
        case QVariant::RectF:
            return rectToString(propertyValue().toRect());
        default :
            return ObjectPropItem::displayValue();
    }
#endif
}

LimeReport::RectUnitPropItem::RectUnitPropItem(QObject *object, ObjectsList* objects, const QString &name, const QString &displayName, const QVariant &value, ObjectPropItem *parent, bool /*readonly*/):
    ObjectPropItem(object, objects, name, displayName, value,parent)
{
    QRectF rect= value.toRect();
    LimeReport::BandDesignIntf* band = dynamic_cast<LimeReport::BandDesignIntf*>(object);
    LimeReport::PageItemDesignIntf* page = dynamic_cast<LimeReport::PageItemDesignIntf*>(object);
    LimeReport::BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(object);

    if (band){
        this->appendItem(new LimeReport::RectUnitValuePropItem(object, objects, "x", "x", rect.x(), this, true));
        this->appendItem(new LimeReport::RectUnitValuePropItem(object, objects, "y", "y", rect.y(), this, true));
        this->appendItem(new LimeReport::RectUnitValuePropItem(object, objects, "width", tr("width"), rect.width(), this, true));
        this->appendItem(new LimeReport::RectUnitValuePropItem(object, objects, "height", tr("height"), rect.height(), this, false));
    } else if (page){
        this->appendItem(new LimeReport::RectUnitValuePropItem(object, 0, "x", "x", rect.x(), this, true));
        this->appendItem(new LimeReport::RectUnitValuePropItem(object, 0, "y", "y",rect.y(), this, true));
        this->appendItem(new LimeReport::RectUnitValuePropItem(object, 0,"width", tr("width"), rect.width(), this, false));
        this->appendItem(new LimeReport::RectUnitValuePropItem(object, 0, "height", tr("height"), rect.height(), this, false));
    } else {
        this->appendItem(new LimeReport::RectUnitValuePropItem(object, objects, "x", "x", rect.x(), this, false));
        this->appendItem(new LimeReport::RectUnitValuePropItem(object, objects, "y", "y", rect.y(), this, false));
        this->appendItem(new LimeReport::RectUnitValuePropItem(object, objects, "width", tr("width"), rect.width(), this, false));
        this->appendItem(new LimeReport::RectUnitValuePropItem(object, objects, "height", tr("height"), rect.height(), this, false));
    }

    if (item){
        connect(item,SIGNAL(geometryChanged(QObject*,QRectF,QRectF)),this,SLOT(itemGeometryChanged(QObject*,QRectF,QRectF)));
        connect(item,SIGNAL(posChanged(QObject*,QPointF,QPointF)),this,SLOT(itemPosChanged(QObject*,QPointF,QPointF)));
        connect(item,SIGNAL(posChanging(QObject*,QPointF,QPointF)),this,SLOT(itemPosChanged(QObject*,QPointF,QPointF)));
    }

}
QString LimeReport::RectUnitPropItem::displayValue() const
{
    QRectF rect = rectInUnits(propertyValue().toRectF());
    return QString("[%1,%2] %3x%4 %5")
            .arg(rect.x(), 0, 'f', 2)
            .arg(rect.y(), 0,'f', 2)
            .arg(rect.width(), 0, 'f', 2)
            .arg(rect.height(), 0, 'f', 2)
            .arg(unitShortName());
}

LimeReport::RectUnitValuePropItem::RectUnitValuePropItem(QObject *object, ObjectsList* objects, const QString &name, const QString &displayName, const QVariant &value, ObjectPropItem *parent, bool readonly
):ObjectPropItem(object, objects, name, displayName, value,parent,readonly){}

QWidget * LimeReport::RectUnitValuePropItem::createProperyEditor(QWidget *parent) const
{
    QDoubleSpinBox *editor= new QDoubleSpinBox(parent);
    editor->setMaximum(100000);
    editor->setSuffix(" "+unitShortName());
    return editor;
}

void LimeReport::RectUnitValuePropItem::setPropertyEditorData(QWidget *propertyEditor, const QModelIndex &) const
{
    QDoubleSpinBox *editor = qobject_cast<QDoubleSpinBox*>(propertyEditor);
    editor->setValue(valueInUnits(propertyValue().toReal()));
}

void LimeReport::RectUnitValuePropItem::setModelData(QWidget *propertyEditor, QAbstractItemModel *model, const QModelIndex &index)
{
    model->setData(index,valueInReportUnits(qobject_cast<QDoubleSpinBox*>(propertyEditor)->value()));
    QRectF rect=object()->property(parent()->propertyName().toLatin1()).toRectF();
    object()->setProperty(parent()->propertyName().toLatin1(), modifyRect(rect, propertyName(), propertyValue().toReal()));
}

qreal LimeReport::RectUnitValuePropItem::valueInUnits(qreal value) const
{
    BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(object());
    switch (item->unitType()) {
    case LimeReport::BaseDesignIntf::Millimeters:
        return value / item->unitFactor();
    case LimeReport::BaseDesignIntf::Inches:
        return value / (item->unitFactor() * 10);
    }
    return 0;
}

qreal LimeReport::RectUnitValuePropItem::valueInReportUnits(qreal value) const
{
    BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(object());
    switch (item->unitType()) {
    case LimeReport::BaseDesignIntf::Millimeters:
        return value * item->unitFactor();
    case LimeReport::BaseDesignIntf::Inches:
        return value * (item->unitFactor() * 10);
    }
    return 0;
}

QString LimeReport::RectUnitValuePropItem::unitShortName() const
{
    BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(object());
    switch (item->unitType()) {
    case LimeReport::BaseDesignIntf::Millimeters:
        return QObject::tr("mm");
    case LimeReport::BaseDesignIntf::Inches:
        return QObject::tr("''");
    }
    return QString();
}

QString LimeReport::RectUnitValuePropItem::displayValue() const
{
    return QString("%1 %2").arg(valueInUnits(propertyValue().toReal()), 0, 'f', 2).arg(unitShortName());
}

void LimeReport::RectUnitPropItem::itemPosChanged(QObject* /*object*/, QPointF newPos, QPointF oldPos)
{
    if (newPos.x() != oldPos.x()){
        setValue("x", newPos.x());
    }
    if (newPos.y() != oldPos.y()){
        setValue("y", newPos.y());
    }
}

void LimeReport::RectUnitPropItem::itemGeometryChanged(QObject * /*object*/, QRectF newGeometry, QRectF oldGeometry)
{
    if (newGeometry.x() != oldGeometry.x()){
        setValue("x", newGeometry.x());
    }
    if (newGeometry.y() != oldGeometry.y()){
        setValue("y", newGeometry.y());
    }
    if (newGeometry.width() != oldGeometry.width()){
        setValue("width", newGeometry.width());
    }
    if (newGeometry.height() != oldGeometry.height()){
        setValue("height", newGeometry.height());
    }
}

void LimeReport::RectUnitPropItem::setValue(const QString &name, qreal value)
{
    if (name != ""){
        LimeReport::ObjectPropItem* propItem = findChild(name);
        if (propItem) {
            propItem->setPropertyValue(value);
            setPropertyValue(LimeReport::modifyRect(propertyValue().toRectF(), name, value));
            LimeReport::QObjectPropertyModel *itemModel = dynamic_cast<LimeReport::QObjectPropertyModel *>(model());
            if (itemModel) {
                itemModel->itemDataChanged(modelIndex());
                if (propItem->modelIndex().isValid())
                    itemModel->itemDataChanged(propItem->modelIndex());
            }
        }
    }
}

QRectF LimeReport::RectUnitPropItem::rectInUnits(QRectF rect) const
{
    BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(object());
    switch (item->unitType()) {
    case LimeReport::BaseDesignIntf::Millimeters:
        return QRectF(rect.x() / item->unitFactor(),
                      rect.y() / item->unitFactor(),
                      rect.width() / item->unitFactor(),
                      rect.height() / item->unitFactor());
    case LimeReport::BaseDesignIntf::Inches:
        return QRectF(rect.x() / (item->unitFactor() * 10),
                      rect.y() / (item->unitFactor() * 10),
                      rect.width() / (item->unitFactor() * 10),
                      rect.height() / (item->unitFactor() * 10));
    }
    return QRectF();
}

QString LimeReport::RectUnitPropItem::unitShortName() const
{
    BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(object());
    switch (item->unitType()) {
    case LimeReport::BaseDesignIntf::Millimeters:
        return QObject::tr("mm");
    case LimeReport::BaseDesignIntf::Inches:
        return QObject::tr("''");
    }
    return QString();
}
