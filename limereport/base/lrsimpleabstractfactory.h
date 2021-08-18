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
#ifndef LRSIMPLEABSTRACTFACTORY_H
#define LRSIMPLEABSTRACTFACTORY_H

#include "lrsingleton.h"
#include "lrglobal.h"
#include <stdexcept>
#include <QHash>
#include <QMap>
#include <QString>
namespace LimeReport{

template
<
    typename AbstractProduct,
    typename IdentifierType,
    typename ProductCreator
>
class SimpleAbstractFactory
  : public Singleton< SimpleAbstractFactory< AbstractProduct,IdentifierType,ProductCreator > >
{
private:
    typedef QHash<IdentifierType,ProductCreator> FactoryMap;
    friend class Singleton< SimpleAbstractFactory< AbstractProduct,IdentifierType,ProductCreator > >;
public:
    bool registerCreator(const IdentifierType& id, ProductCreator creator){
        return (m_factoryMap.insert(id,creator).value()==creator);
    }
    bool unregisterCreator(const IdentifierType& id){
        return (m_factoryMap.remove(id)==1);
    }
    ProductCreator objectCreator(const IdentifierType& id){
        if (m_factoryMap.contains(id)){
            return m_factoryMap[id];
        } else {
            return 0;
        }
    }
    const FactoryMap& map(){return m_factoryMap;}
    int mapElementCount(){return m_factoryMap.count();}
private:
    FactoryMap m_factoryMap;
};

template
<
        typename AbstractProduct,
        typename IdentifierType,
        typename ProductCreator,
        typename Attribs
>
class AttribAbstractFactory : public SimpleAbstractFactory< AbstractProduct,IdentifierType,ProductCreator >{
    typedef SimpleAbstractFactory< AbstractProduct,IdentifierType,ProductCreator > SimpleFactory;
    typedef QMap<IdentifierType,Attribs> AliasMap;
    friend class Singleton<AttribAbstractFactory< AbstractProduct,IdentifierType,ProductCreator,Attribs > >;
public :
    bool registerCreator(const IdentifierType &id, Attribs attribs, ProductCreator creator){
        return SimpleFactory::registerCreator(id,creator) &&  (m_attribsMap.insert(id,attribs).value()==attribs);
    }
    bool unregisterCreator(const IdentifierType &id){
        return SimpleFactory::unregisterCreator(id)&&(m_attribsMap.remove(id)==1);
    }
    QString attribs(const IdentifierType& id){
        if (m_attribsMap.contains(id)){
            return m_attribsMap.value(id);
        } else return "";
    }
    const AliasMap& attribsMap(){return m_attribsMap;}
private:
    AliasMap m_attribsMap;
};

}

#endif //LRSIMPLEABSTRACTFACTORY_H
