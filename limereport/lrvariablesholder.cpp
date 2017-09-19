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
#include "lrvariablesholder.h"
#include <stdexcept>
#include <QStringList>
#include <QDebug>

namespace LimeReport{

VariablesHolder::VariablesHolder(QObject *parent) :
    QObject(parent)
{
}

VariablesHolder::~VariablesHolder()
{
    QMap<QString,VarDesc*>::iterator it = m_varNames.begin();
    while(it!=m_varNames.end()){
        delete *it;
        it++;
    }
    m_varNames.clear();
    m_userVariables.clear();
}

void VariablesHolder::addVariable(const QString& name, const QVariant& value, VarDesc::VarType type, RenderPass pass)
{
    if (!m_varNames.contains(name)){
        VarDesc* varValue = new VarDesc;
        varValue->setName(name);
        varValue->setValue(value);
        varValue->setVarType(type);
        varValue->setRenderPass(pass);
        m_varNames.insert(name,varValue);
        if (type==VarDesc::Report)
            m_userVariables.append(varValue);
        emit variableHasBeenAdded(name);
    } else {
        throw ReportError(tr("variable with name ")+name+tr(" already exists!"));
    }
}

QVariant VariablesHolder::variable(const QString &name)
{
    if (m_varNames.contains(name))
        return m_varNames.value(name)->value();
    else return QVariant();
}

VarDesc::VarType VariablesHolder::variableType(const QString &name)
{
    if (m_varNames.contains(name))
        return m_varNames.value(name)->varType();
    else throw ReportError(tr("variable with name ")+name+tr(" does not exists!"));
}

void VariablesHolder::deleteVariable(const QString &name)
{
    if (m_varNames.contains(name)) {
        m_userVariables.removeOne(m_varNames.value(name));
        delete m_varNames.value(name);
        m_varNames.remove(name);
        emit variableHasBennDeleted(name);
    }
}

void VariablesHolder::changeVariable(const QString &name, const QVariant &value)
{
    if(m_varNames.contains(name)) {
        m_varNames.value(name)->setValue(value);
        emit variableHasBeenChanged(name);
    } else
        throw ReportError(tr("variable with name ")+name+tr(" does not exists!"));
}

void VariablesHolder::clearUserVariables()
{
    QMap<QString,VarDesc*>::iterator it = m_varNames.begin();
    while (it != m_varNames.end()){
        if (it.value()->varType()==VarDesc::User ||
            it.value()->varType()==VarDesc::Report){
            m_userVariables.removeAll(it.value());
            delete it.value();
            it = m_varNames.erase(it);
        } else {
            ++it;
        }

    }

}

bool VariablesHolder::containsVariable(const QString &name)
{
    return m_varNames.contains(name);
}

int VariablesHolder::userVariablesCount()
{
    return m_userVariables.count();
}

VarDesc *VariablesHolder::userVariableAt(int index)
{
    return m_userVariables.at(index);
}

QStringList VariablesHolder::variableNames()
{
    QStringList result;
    foreach(QString varName,m_varNames.keys()){
        result<<varName;
    }
    return result;
}

RenderPass VariablesHolder::variablePass(const QString &name)
{
    if (m_varNames.contains(name))
        return m_varNames.value(name)->renderPass();
    else throw ReportError(tr("variable with name ")+name+tr(" does not exists!"));
}

}// namespace LimeReport
