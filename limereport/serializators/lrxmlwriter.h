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
#ifndef LRXMLWRITER_H
#define LRXMLWRITER_H

#include <QtXml>
#include "serializators/lrstorageintf.h"
#include "serializators/lrxmlserializatorsfactory.h"
#include "lrbasedesignintf.h"

namespace LimeReport{

class XMLWriter : public ItemsWriterIntf
{
public:
    XMLWriter();
    XMLWriter(QSharedPointer<QDomDocument> doc);
    ~XMLWriter() {}
private:
    // ItemsWriterIntf interface
    void  putItem(QObject* item);
    bool  saveToFile(QString fileName);
    QString saveToString();
    QByteArray saveToByteArray();
    void setPassPhrase(const QString &passPhrase);

    void init();
    QDomElement putQObjectItem(QString name, QObject* item);
    void putChildQObjectItem(QString name, QObject* item, QDomElement* parentNode);
    void putCollectionItem(QObject* item, QDomElement* parentNode=0);
    void putQObjectProperty(QString propertyName, QObject *item, QDomElement* parentNode=0);
    void saveProperties(QObject* item, QDomElement* node);
    bool setContent(QString fileName);
    void saveProperty(QString name, QObject* item, QDomElement* node);
    bool enumOrFlag(QString name, QObject* item);
    QString extractClassName(QObject* item);
    bool isCollection(QString propertyName, QObject *item);
    bool isTranslation(QString propertyName, QObject *item);
    void saveCollection(QString propertyName, QObject *item, QDomElement *node);
    void saveTranslation(QString propertyName, QObject *item, QDomElement *node);
    bool isQObject(QString propertyName, QObject *item);
    bool replaceNode(QDomElement node, QObject *item);
private:
    QSharedPointer<QDomDocument> m_doc;
    QString m_fileName;
    QDomElement m_rootElement;
    QString m_passPhrase;
};

}

#endif // LRXMLWRITER_H
