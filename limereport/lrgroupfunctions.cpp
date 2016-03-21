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
#include "lrgroupfunctions.h"
#include "lrdatasourcemanager.h"
#include "lrbanddesignintf.h"
#include "lritemdesignintf.h"

#include <QRegExp>

namespace LimeReport {

void GroupFunction::slotBandRendered(BandDesignIntf *band)
{
    switch (m_dataType){
    case Field:
        if (m_dataManager->containsField(m_data)){
            m_values.push_back(m_dataManager->fieldData(m_data));
        } else {
            setInvalid(tr("Field \"%1\" not found").arg(m_data));
        }
        break;
    case Variable:
        if (m_dataManager->containsVariable(m_data)){
            m_values.push_back(m_dataManager->variable(m_data));
        } else {
            setInvalid(tr("Variable \"%1\" not found").arg(m_data));
        }
        break;
    case ContentItem:{
        ContentItemDesignIntf* item = dynamic_cast<ContentItemDesignIntf*>(band->childByName(m_data));
        if (item)
            m_values.push_back(item->content());
        else setInvalid(tr("Item \"%1\" not found").arg(m_data));
        break;
    }
    default:
        break;
    }
}

QVariant GroupFunction::addition(QVariant value1, QVariant value2)
{
    return value1.toDouble()+value2.toDouble();
}

QVariant GroupFunction::subtraction(QVariant value1, QVariant value2)
{
    return value1.toDouble()-value2.toDouble();
}

QVariant GroupFunction::division(QVariant value1, QVariant value2)
{
    return value1.toDouble() / value2.toDouble();
}

QVariant GroupFunction::multiplication(QVariant value1, QVariant value2)
{
    return value1.toDouble() * value2.toDouble();
}

GroupFunction::GroupFunction(const QString &expression, const QString &dataBandName, DataSourceManager* dataManager)
    :m_dataBandName(dataBandName), m_dataManager(dataManager),m_isValid(true), m_errorMessage("")
{
    QRegExp rxField(Const::FIELD_RX,Qt::CaseInsensitive);
    if (rxField.indexIn(expression)>=0){
        m_dataType=Field;
        m_data = rxField.cap(1);
        return;
    }

    QRegExp rxVariable(Const::VARIABLE_RX,Qt::CaseInsensitive);
    if (rxVariable.indexIn(expression)>=0){
        m_dataType=Variable;
        m_data = rxVariable.cap(1);
        return;
    }

    m_dataType = ContentItem;
    m_data = expression;
    m_data = m_data.remove('"');

}

GroupFunction *GroupFunctionFactory::createGroupFunction(const QString &functionName, const QString &expression, const QString& dataBandName, DataSourceManager *dataManager)
{
    if (m_creators.contains(functionName)){
        return m_creators.value(functionName)->createFunction(expression, dataBandName, dataManager);
    }
    return 0;
}

GroupFunctionFactory::~GroupFunctionFactory()
{
    foreach(GroupFunctionCreator* creator, m_creators.values()){
        delete creator;
    }
    m_creators.clear();
}

QVariant SumGroupFunction::calculate()
{
    QVariant res=0;
    foreach(QVariant value,values()){
        res=addition(res,value);
    }
    return res;
}

QVariant AvgGroupFunction::calculate()
{
    QVariant res=QVariant();
    foreach(QVariant value,values()){
        res=addition(res,value);
    }
    if (!res.isNull()&&(values().count()>0)){
        res=division(res,values().count());
    }
    return res;
}

QVariant MinGroupFunction::calculate()
{
    //TODO: check variant type
    QVariant res = QVariant();
    if (!values().empty()) res = values().at(0);
    foreach(QVariant value, values()){
        if (res.toDouble()>value.toDouble()) res = value;
    }

    return res;
}

QVariant MaxGroupFunction::calculate()
{
    //TODO: check variant type
    QVariant res = QVariant();
    if (!values().empty()) res = values().at(0);
    foreach(QVariant value, values()){
        if (res.toDouble()<value.toDouble()) res = value;
    }

    return res;
}

} //namespace LimeReport

