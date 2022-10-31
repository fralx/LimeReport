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
#ifndef LRSTORAGEINTF_H
#define LRSTORAGEINTF_H

#include "lrglobal.h"
#include <QSharedPointer>

class QString;
class QObject;

namespace LimeReport{

class LIMEREPORT_EXPORT ObjectLoadingStateIntf{
public:
    virtual bool isLoading() = 0;
    virtual void objectLoadStarted() = 0;
    virtual void objectLoadFinished() = 0;
};

class LIMEREPORT_EXPORT ItemsWriterIntf
{
public:
    virtual void putItem(QObject* item) = 0;
    virtual bool saveToFile(QString fileName) = 0;
    virtual QString saveToString() = 0;
    virtual QByteArray saveToByteArray() = 0;
    virtual void setPassPhrase(const QString& passPhrase) = 0;
    virtual ~ItemsWriterIntf(){}
};

class LIMEREPORT_EXPORT ItemsReaderIntf
{
public:
    typedef QSharedPointer<ItemsReaderIntf> Ptr;
    virtual bool first() = 0;
    virtual bool next() = 0;
    virtual bool prior() = 0;
    virtual QString itemType() = 0;
    virtual QString itemClassName() = 0;
    virtual bool readItem(QObject *item) = 0;
    virtual int firstLevelItemsCount() = 0;
    virtual QString lastError() = 0;
    virtual void setPassPhrase(const QString& passPhrase) = 0;
    virtual ~ItemsReaderIntf(){}
};

}
#endif // LRSTORAGEINTF_H
