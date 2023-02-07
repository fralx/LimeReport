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
#ifndef LRCOLLECTION_H
#define LRCOLLECTION_H

#include <QObject>
#include <QMetaType>

#include "lrglobal.h"

class LIMEREPORT_EXPORT ACollectionProperty{
public:
    ACollectionProperty(){}
    ACollectionProperty(const ACollectionProperty& ){}
    virtual ~ACollectionProperty(){}
};
Q_DECLARE_METATYPE(ACollectionProperty)

namespace LimeReport{
#if __cplusplus >= 201703L
const int inline COLLECTION_TYPE_ID = qMetaTypeId<ACollectionProperty>();
#else
const int COLLECTION_TYPE_ID = qMetaTypeId<ACollectionProperty>();
#endif
class LIMEREPORT_EXPORT ICollectionContainer{
public:
    virtual QObject* createElement(const QString& collectionName,const QString& elementType)=0;
    virtual int elementsCount(const QString& collectionName)=0;
    virtual QObject* elementAt(const QString& collectionName,int index)=0;
    virtual void collectionLoadFinished(const QString& collectionName){Q_UNUSED(collectionName)}
    ACollectionProperty fakeCollectionReader(){return ACollectionProperty();}
};

}
#endif // LRCOLLECTION_H
