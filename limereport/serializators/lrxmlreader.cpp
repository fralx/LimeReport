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
#include "stdexcept"

#include "lrxmlreader.h"
#include "lrbasedesignintf.h"
#include "lrdesignelementsfactory.h"
#include "lrcollection.h"

#include <QDebug>

namespace LimeReport{

XMLReader::XMLReader()
    : m_doc(new QDomDocument){}
XMLReader::XMLReader(QSharedPointer<QDomDocument> doc)
    : m_doc(doc){}

bool XMLReader::prepareReader(QDomDocument* doc)
{
    Q_UNUSED(doc);
    m_error=QLatin1String("Context not initialized");
    return false;
}

int XMLReader::firstLevelItemsCount()
{
    if (m_firstNode.childNodes().count()==0) return 0;
    QDomElement tmpNode = m_firstNode;
    int res=0;
    while (!tmpNode.isNull()){
        res++;
        tmpNode=tmpNode.nextSiblingElement();
    }
    return res;
}

bool XMLReader::first()
{
    if (extractFirstNode()){
        m_curNode = m_firstNode;
        return !m_curNode.isNull();
    }
    return false;
}

bool XMLReader::next()
{
    m_curNode = m_curNode.nextSiblingElement();
    return !m_curNode.isNull();
}

bool XMLReader::prior()
{
    m_curNode = m_curNode.previousSiblingElement();
    return !m_curNode.isNull();
}

QString XMLReader::itemType()
{
    return m_curNode.attribute("Type");
}

QString XMLReader::itemClassName()
{
    return m_curNode.attribute("ClassName");
}

bool XMLReader::readItem(QObject *item)
{
    if (!m_curNode.isNull()){
        readItemFromNode(item,&m_curNode);
    } else {
        m_error=QString("Object %1 not founded").arg(item->objectName());
        return false;
    }
    return true;
}

void XMLReader::readItemFromNode(QObject* item,QDomElement *node)
{
    ObjectLoadingStateIntf* lf = dynamic_cast<ObjectLoadingStateIntf*>(item);
    if(lf) lf->objectLoadStarted();
    for (int i=0;i<node->childNodes().count();i++){
        QDomElement currentNode =node->childNodes().at(i).toElement();
        if (currentNode.attribute("Type")=="Object"){
            readQObject(item,&currentNode);
        }else if (currentNode.attribute("Type")=="Collection")
        {
            readCollection(item,&currentNode);
        }
        else readProperty(item,&currentNode);
    }
    if (lf) lf->objectLoadFinished();

    BaseDesignIntf* baseObj = dynamic_cast<BaseDesignIntf*>(item);
    if(baseObj) {
        foreach(QGraphicsItem* childItem,baseObj->childItems()){
            BaseDesignIntf* baseItem = dynamic_cast<BaseDesignIntf*>(childItem);
            if (baseItem) baseItem->parentObjectLoadFinished();
        }
    }
}

QString XMLReader::lastError()
{
    return m_error;
}

void XMLReader::setPassPhrase(const QString &passPhrase)
{
    m_passPhrase = passPhrase;
}

bool XMLReader::extractFirstNode()
{
    if (m_firstNode.isNull()){
        if (m_doc->childNodes().count()==0){
                if (!prepareReader(m_doc.data())) return false;
        }
        m_firstNode = m_doc->documentElement();
        if (!m_firstNode.hasAttributes())
          m_firstNode = m_firstNode.firstChildElement();
    }
    return !m_firstNode.isNull();
}

void XMLReader::readProperty(QObject *item, QDomElement *node)
{
    item->setProperty(node->nodeName().toLatin1(),getValue(node));
}

QVariant XMLReader::getValue(QDomElement *node)
{
    CreateSerializator creator = 0;
    try {
        creator=XMLAbstractSerializatorFactory::instance().objectCreator(
                node->attribute("Type")
                );
    } catch(ReportError &e){
        qDebug()<<node->attribute("Type")<<e.what();
    }

    if (creator) {
        QScopedPointer<SerializatorIntf>serializator(creator(m_doc.data(),node));
        CryptedSerializator* cs = dynamic_cast<CryptedSerializator*>(serializator.data());
        if (cs){
            cs->setPassPhrase(m_passPhrase);
        }
        return serializator->loadValue();
    }
    return QVariant();
}

void XMLReader::readQObject(QObject* item, QDomElement* node)
{
    QObject* childItem = qvariant_cast<QObject*>(item->property(node->nodeName().toLatin1()));
    if (childItem)
        readItemFromNode(childItem,node);
}

void XMLReader::readCollection(QObject *item, QDomElement *node)
{
    ICollectionContainer* collection = dynamic_cast<ICollectionContainer*>(item);
    if (collection){
        QString collectionName = node->nodeName();
        for(int i=0;i<node->childNodes().count();i++){
            QDomElement currentNode =node->childNodes().at(i).toElement();
            QObject* obj = collection->createElement(collectionName,currentNode.attribute("ClassName"));
            if (obj)
                readItemFromNode(obj,&currentNode);
        }
        collection->collectionLoadFinished(collectionName);
    }
}

FileXMLReader::FileXMLReader(QString fileName)
    : m_fileName(fileName)
{
}

bool FileXMLReader::prepareReader(QDomDocument *doc)
{
    if (!m_fileName.isEmpty()){
        QFile source(m_fileName);
        if (source.open(QFile::ReadOnly)) {
            doc->setContent(&source);
            if (doc->documentElement().nodeName()!="Report") {
                m_error = QString(QObject::tr("Wrong file format"));
                return false;
            }
        } else {m_error=QString(QObject::tr("File %1 not opened")).arg(m_fileName); return false;}
    }
    return true;
}

bool StringXMLreader::prepareReader(QDomDocument *doc)
{
    if (!m_content.isEmpty()){
        doc->setContent(m_content);
    }else {m_error = QString(QObject::tr("Content string is empty")); return false;}
    return true;
}

bool ByteArrayXMLReader::prepareReader(QDomDocument *doc)
{
    if (m_content){
        doc->setContent(*m_content);
    } else {m_error = QString(QObject::tr("Content is empty")); return false;}
    return true;
}

}
