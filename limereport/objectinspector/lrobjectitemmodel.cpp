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
#include "lrobjectitemmodel.h"
#include <QMetaProperty>
#include <QPainter>
#include <QDebug>
#include <QMessageBox>

namespace LimeReport {

void QObjectPropertyModel::translatePropertyName()
{
    tr("leftMargin");
    tr("rightMargin");
    tr("topMargin");
    tr("bottomMargin");
    tr("objectName");
    tr("borders");
    tr("geometry");
    tr("itemAlign");
    tr("pageOrientation");
    tr("pageSize");
    tr("TopLine");
    tr("BottomLine");
    tr("LeftLine");
    tr("RightLine");
    tr("reprintOnEachPage");
    tr("borderLineSize");
    tr("autoHeight");
    tr("backgroundColor");
    tr("columnCount");
    tr("columnsFillDirection");
    tr("datasource");
    tr("keepBottomSpace");
    tr("keepFooterTogether");
    tr("keepSubdetailTogether");
    tr("printIfEmpty");
    tr("sliceLastRow");
    tr("splittable");
    tr("alignment");
    tr("angle");
    tr("autoWidth");
    tr("backgroundMode");
    tr("backgroundOpacity");
    tr("content");
    tr("font");
    tr("fontColor");
    tr("foregroundOpacity");
    tr("itemLocation");
    tr("margin");
    tr("stretchToMaxHeight");
    tr("trimValue");
    tr("lineWidth");
    tr("opacity");
    tr("penStyle");
    tr("shape");
    tr("shapeBrush");
    tr("shapeBrushColor");
    tr("gridStep");
    tr("fullPage");
    tr("oldPrintMode");
    tr("borderColor");
    tr("resetPageNumber");
    tr("alternateBackgroundColor");
    tr("backgroundBrushStyle");
    tr("columnCount");
    tr("startFromNewPage");
    tr("startNewPage");
    tr("adaptFontToSize");
    tr("allowHTML");
    tr("allowHTMLInFields");
    tr("backgroundBrushStyle");
    tr("followTo");
    tr("format");
    tr("lineSpacing");
    tr("textIndent");
    tr("textLayoutDirection");
    tr("underlineLineSize");
    tr("underlines");
    tr("valueType");
    tr("securityLevel");
    tr("testValue");
    tr("whitespace");
    tr("resourcePath");
    tr("scale");
    tr("cornerRadius");
    tr("shapeColor");
    tr("layoutType");
    tr("barcodeType");
    tr("barcodeWidth");
    tr("foregroundColor");
    tr("inputMode");
    tr("pdf417CodeWords");
    tr("autoSize");
    tr("center");
    tr("field");
    tr("image");
    tr("keepAspectRatio");
    tr("columnsCount");
    tr("useAlternateBackgroundColor");
    tr("printBeforePageHeader");
    tr("maxScalePercent");
    tr("printOnFirstPage");
    tr("printOnLastPage");
    tr("printAlways");
    tr("repeatOnEachRow");
    tr("condition");
    tr("groupFieldName");
    tr("keepGroupTogether");
    tr("endlessHeight");
    tr("extendedHeight");
    tr("isExtendedInDesignMode");
    tr("pageIsTOC");
    tr("setPageSizeToPrinter");
    tr("fillInSecondPass");
    tr("chartTitle");
    tr("chartType");
    tr("drawLegendBorder");
    tr("labelsField");
    tr("xAxisField");
    tr("legendAlign");
    tr("series");
    tr("titleAlign");
    tr("watermark");
    tr("keepTopSpace");
    tr("printable");
    tr("variable");
    tr("replaceCRwithBR");
    tr("hideIfEmpty");
    tr("hideEmptyItems");
    tr("useExternalPainter");
    tr("layoutSpacing");
    tr("printerName");
    tr("fontLetterSpacing");
    tr("hideText");
    tr("option3");
    tr("units");
    tr("geometryLocked");
    tr("printBehavior");
    tr("shiftItems");
    tr("showLegend");
    tr("seriesLineWidth");
    tr("drawPoints");
    tr("removeGap");
    tr("dropPrinterMargins");
    tr("notPrintIfEmpty");
    tr("gridChartLines");
    tr("horizontalAxisOnTop");
    tr("mixWithPriorPage");
}

void QObjectPropertyModel::clearObjectsList()
{
    m_objects.clear();
}

QObjectPropertyModel::QObjectPropertyModel(QObject *parent/*=0*/)
    :QAbstractItemModel(parent),m_rootNode(0), m_object(0), m_dataChanging(false),
     m_subclassesAsLevel(true), m_validator(0), m_translateProperties(true)
{}

QObjectPropertyModel::~QObjectPropertyModel()
{
    delete m_rootNode;
}

void QObjectPropertyModel::initModel()
{
    beginResetModel();
    delete m_rootNode;
    m_rootNode=0;
    if (m_object) {
        connect(m_object,SIGNAL(destroyed(QObject*)),this,SLOT(slotObjectDestroyed(QObject*)));
        m_rootNode=new ObjectPropItem(0,0,"root","root",QVariant(),0);
        m_rootNode->setModel(this);
        foreach(QObject* item, m_objects)
            connect(item,SIGNAL(destroyed(QObject*)),this,SLOT(slotObjectDestroyed(QObject*)));
        addObjectProperties(m_object->metaObject(), m_object, &m_objects);
    }
    endResetModel();
}

void QObjectPropertyModel::setObject(QObject *object)
{
    m_objects.clear();
    if (m_object!=object){
        submit();
        m_object=object;
        initModel();
    }
}

void QObjectPropertyModel::setMultiObjects(QList<QObject *>* list)
{
    m_objects.clear();
    submit();

    if (list->isEmpty()) {
        return;
    }

    if (!list->contains(m_object)){
        m_object=list->at(0);
        list->removeAt(0);
    } else {
        list->removeOne(m_object);
    }

    foreach(QObject* item, *list)
        m_objects.append(item);
    //initModel();
}

void QObjectPropertyModel::slotObjectDestroyed(QObject *obj)
{
    m_objects.removeOne(obj);
    if (m_object == obj){
       m_object=0;
       initModel();
    }
}

void QObjectPropertyModel::slotPropertyChanged(const QString &propertyName, const QVariant& oldValue, const QVariant& newValue)
{
    Q_UNUSED(oldValue);
    Q_UNUSED(newValue);
    if (m_object)
        updateProperty(propertyName);
}

void QObjectPropertyModel::slotPropertyObjectNameChanged(const QString &oldName, const QString &newName)
{
    Q_UNUSED(oldName)
    Q_UNUSED(newName)
    if (m_object)
        updateProperty("objectName");
}


int QObjectPropertyModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 2;
}

QVariant QObjectPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Horizontal&&role==Qt::DisplayRole){
        if (section==0) return tr("Property Name");
        else return tr("Property value");
    } else return QVariant();
}

ObjectPropItem * QObjectPropertyModel::nodeFromIndex(const QModelIndex &index) const
{
    if(index.isValid()){
        return static_cast<ObjectPropItem*>(index.internalPointer());
    } else return m_rootNode;
}

void QObjectPropertyModel::updateProperty(const QString &propertyName)
{
    if (!m_dataChanging&&m_rootNode){
        ObjectPropItem* propItem = m_rootNode->findPropertyItem(propertyName);
        if (propItem)
           propItem->updatePropertyValue();
    }
}

void QObjectPropertyModel::setSubclassesAsLevel(bool value)
{
    m_subclassesAsLevel = value;
}

int QObjectPropertyModel::rowCount(const QModelIndex &parent) const
{
    if (!m_rootNode) return 0;
    ObjectPropItem *parentNode;
    if (parent.isValid())
        parentNode = nodeFromIndex(parent);
    else
        parentNode = m_rootNode;
    return parentNode->childCount();
}

QVariant QObjectPropertyModel::data(const QModelIndex &index, int role) const
{
    ObjectPropItem *node = nodeFromIndex(index);
    switch (role) {
    case Qt::DisplayRole:
        if (!node) return QVariant();
        node->setTranslateProperty(isTranslateProperties());
        if (index.column()==0){
            return node->displayName();
        } else {
            return node->displayValue();
        }
    case Qt::DecorationRole :
        if (!node) return QIcon();
        if (index.column()==1){
            return node->iconValue();
        } else return QIcon();
    case Qt::UserRole:
        return QVariant::fromValue(node);
    default:
        return QVariant();
    }
}

QModelIndex QObjectPropertyModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!m_rootNode)
        return QModelIndex();

    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ObjectPropItem *parentNode;
    if (parent.isValid())
        parentNode = nodeFromIndex(parent);
    else
        parentNode = m_rootNode;

    ObjectPropItem *childItem=parentNode->child(row);

    if (childItem){
        QModelIndex modelIndex=createIndex(row,column,childItem);
        if (column==1){
            if (childItem->modelIndex()!=modelIndex){
                childItem->setModelIndex(modelIndex);
            }
        }
        return modelIndex;
    }
    else return QModelIndex();
}

QModelIndex QObjectPropertyModel::parent(const QModelIndex &child) const
{  
    if (!child.isValid()) return QModelIndex();

    ObjectPropItem *childNode = nodeFromIndex(child);
    if (!childNode) return QModelIndex();

    ObjectPropItem *parentNode = childNode->parent();
    if ((parentNode == m_rootNode) || (!parentNode)) return QModelIndex();

    return createIndex(parentNode->row(),0,parentNode);
}


Qt::ItemFlags QObjectPropertyModel::flags(const QModelIndex &index) const
{
    if ((index.column() == 1) && (!nodeFromIndex(index)->isValueReadonly())) return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    else return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

CreatePropItem QObjectPropertyModel::propertyItemCreator(QMetaProperty prop)
{
    CreatePropItem creator = 0;
    creator = ObjectPropFactory::instance().objectCreator(APropIdent(prop.name(),prop.enclosingMetaObject()->className()));
    if (!creator){
        if (prop.isFlagType()){
            creator=ObjectPropFactory::instance().objectCreator(APropIdent("flags",""));
            if (creator){
              return creator;
            } else {
                qDebug()<<"flags prop editor not found";
                return 0;
            }
        }
        if (prop.isEnumType()){       
            creator=ObjectPropFactory::instance().objectCreator(APropIdent("enum",""));
            if (creator){
                return creator;
            } else {
                qDebug()<<"enum prop editor not found";
                return 0;
            }
        }
        creator = ObjectPropFactory::instance().objectCreator(APropIdent(prop.typeName(),""));
        if (!creator) {qDebug()<<"Editor for propperty name = \""<<prop.name()<<"\" & property type =\""<<prop.typeName()<<"\" not found!";}
    }
    return creator;
}

ObjectPropItem * QObjectPropertyModel::createPropertyItem(QMetaProperty prop, QObject *object, ObjectPropItem::ObjectsList *objects, ObjectPropItem *parent)
{
    ObjectPropItem* propertyItem=0;
    CreatePropItem creator=propertyItemCreator(prop);

    if (creator) {
        propertyItem=creator(
                object,
                objects,
                QString(prop.name()),
                QString(tr(prop.name())),
                object->property(prop.name()),
                parent,
                !(prop.isWritable() && prop.isDesignable())
             );
    } else {
        propertyItem=new ObjectPropItem(
                    0,
                    0,
                    QString(prop.name()),
                    QString(tr(prop.name())),
                    object->property(prop.name()),
                    parent
                 );
    }
    return propertyItem;
}

bool QObjectPropertyModel::isTranslateProperties() const
{
    return m_translateProperties;
}

void QObjectPropertyModel::setTranslateProperties(bool translateProperties)
{
    m_translateProperties = translateProperties;
}
ValidatorIntf *QObjectPropertyModel::validator() const
{
    return m_validator;
}

void QObjectPropertyModel::setValidator(ValidatorIntf *validator)
{
    m_validator = validator;
}

void QObjectPropertyModel::addObjectProperties(const QMetaObject *metaObject, QObject *object, ObjectPropItem::ObjectsList *objects, int level)
{
    if (metaObject->propertyCount()>metaObject->propertyOffset()){
        ObjectPropItem* objectNode;
        if (m_subclassesAsLevel){
            objectNode=new ObjectPropItem(0,0,metaObject->className(),metaObject->className(),m_rootNode,true);
            m_rootNode->appendItem(objectNode);
        } else {
            objectNode = m_rootNode;
        }

        for (int i=metaObject->propertyOffset();i<metaObject->propertyCount();i++){
            if (metaObject->property(i).isDesignable()){
                ObjectPropItem* prop=createPropertyItem(metaObject->property(i),object,objects,objectNode);
                objectNode->appendItem(prop);
            }
        }
        if (m_subclassesAsLevel){
            objectNode->setColorIndex(level);
            objectNode->sortItem();
            level++;
        }
    }

    if (metaObject->superClass()) addObjectProperties(metaObject->superClass(),object,objects,level);
    m_rootNode->sortItem();
}

bool QObjectPropertyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid()&&role==Qt::EditRole){
        m_dataChanging=true;
        ObjectPropItem * propItem = nodeFromIndex(index);
        if (propItem->propertyValue()!=value){
            QString msg;
            if (validator() && !validator()->validate(propItem->propertyName(),value.toString(),m_object,msg)){
                QMessageBox::information(0,tr("Warning"),msg);
                return true;
            }
            QVariant oldValue=propItem->propertyValue();
            propItem->setPropertyValue(value);
            emit dataChanged(index,index);
            emit objectPropetyChanged(propItem->propertyName(),oldValue,propItem->propertyValue());
        }
        m_dataChanging=false;
        return true;
    }
    return false;
}

void QObjectPropertyModel::itemDataChanged(const QModelIndex &index)
{
    emit dataChanged(index,index);
}

}

