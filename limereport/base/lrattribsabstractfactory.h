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
#ifndef LRATTRIBABSTRACTFACTORY_H
#define LRATTRIBABSTRACTFACTORY_H

#include "lrsingleton.h"
#include "lrglobal.h"
#include <stdexcept>
#include <QMap>
#include <QString>

namespace LimeReport{

template
<
    typename AbstractProduct,
    typename IdentifierType,
    typename ProductCreator,
    typename Attribs
>
class AttribsAbstractFactory
  : public Singleton< AttribsAbstractFactory< AbstractProduct,IdentifierType,ProductCreator,Attribs > >
{
private:
    typedef QMap<IdentifierType,ProductCreator> FactoryMap;
    typedef QMap<IdentifierType,Attribs> AliasMap;
    friend class Singleton< AttribsAbstractFactory< AbstractProduct,IdentifierType,ProductCreator,Attribs > >;
public:
    bool registerCreator(const IdentifierType& id, Attribs attribs, ProductCreator creator){
        return (m_factoryMap.insert(id,creator).value() == creator) &&
               (m_attribsMap.insert(id,attribs).value() == attribs);
    }
    bool unregisterCreator(const IdentifierType& id){
        return (m_factoryMap.remove(id) == 1) && (m_attribsMap.remove(id) == 1);
    }
    ProductCreator objectCreator(const IdentifierType& id){
        if (m_factoryMap.contains(id)){
            return m_factoryMap[id];
        } else return 0;
    }
    QString attribs(const IdentifierType& id){
        if (m_attribsMap.contains(id)){
            return m_attribsMap.value(id);
        } else return "";
    }
    const FactoryMap& map(){return m_factoryMap;}
    const AliasMap& attribsMap(){return m_attribsMap;}
    int mapElementCount(){return m_factoryMap.count();}
private:    
    FactoryMap m_factoryMap;
    AliasMap m_attribsMap;
};

}
#endif // LRATTRIBABSTRACTFACTORY_H
