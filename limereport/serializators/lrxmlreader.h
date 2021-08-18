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
#ifndef LRXMLREADER_H
#define LRXMLREADER_H

#include <QString>
#include <QtXml>

#include "serializators/lrxmlwriter.h"
#include "lrdesignelementsfactory.h"

namespace LimeReport{

class XMLReader : public ItemsReaderIntf
{
public:
    XMLReader();
    XMLReader(QSharedPointer<QDomDocument> doc);
protected:
//ItemsReaderIntf interface
    bool first();
    bool next();
    bool prior();
    QString itemType();
    QString itemClassName();
    bool readItem(QObject *item);
    int firstLevelItemsCount();
    QString lastError();
    void setPassPhrase(const QString &passPhrase);

    virtual bool prepareReader(QDomDocument *doc);

    void readItemFromNode(QObject *item, QDomElement *node);
    void readProperty(QObject *item, QDomElement *node);
    void readQObject(QObject *item, QDomElement *node);
    void readCollection(QObject *item, QDomElement *node);
    void readTranslation(QObject *item, QDomElement *node);
    QVariant getValue(QDomElement *node);

protected:
    bool extractFirstNode();
    QString m_error;
private:
    QSharedPointer<QDomDocument> m_doc;
    QDomElement m_curNode;
    QDomElement m_firstNode;
    QString m_passPhrase;
};

class FileXMLReader : public XMLReader{
public:
    static ItemsReaderIntf::Ptr create(QString fileName){ return ItemsReaderIntf::Ptr(new FileXMLReader(fileName));}
protected:
    virtual bool prepareReader(QDomDocument *doc);
private:
    FileXMLReader(QString fileName);
    QString m_fileName;
};

class StringXMLreader : public XMLReader{
public:
    static ItemsReaderIntf::Ptr create(QString content){ return ItemsReaderIntf::Ptr(new StringXMLreader(content));}
protected:
    virtual bool prepareReader(QDomDocument *doc);
private:
    StringXMLreader(QString content) : m_content(content){}
    QString m_content;
};

class ByteArrayXMLReader : public XMLReader{
public:
    static ItemsReaderIntf::Ptr create(QByteArray* content){ return ItemsReaderIntf::Ptr(new ByteArrayXMLReader(content));}
protected:
    virtual bool prepareReader(QDomDocument *doc);
private:
    ByteArrayXMLReader(QByteArray* content): m_content(content){}
    QByteArray* m_content;
};

}
#endif // LRXMLREADER_H
