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
#ifndef VARIABLEHOLDER_H
#define VARIABLEHOLDER_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QVariant>
#include "lrglobal.h"

namespace LimeReport{

class VarDesc : public QObject{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    enum VarType {System, User, Report};
    void setVarType(VarType value){m_varType=value;}
    VarType varType(){return m_varType;}
    void setRenderPass(RenderPass value){m_varPass=value;}
    RenderPass renderPass(){return m_varPass;}
    void setName(QString value){m_name=value;}
    QString name(){return m_name;}
    void setValue(QVariant value){m_value=value;}
    QVariant value(){return m_value;}
private:
    VarType     m_varType;
    RenderPass  m_varPass;
    QString     m_name;
    QVariant    m_value;
};

class IVariablesContainer
{
public:
    virtual ~IVariablesContainer(){}
    virtual void addVariable(const QString &name, const QVariant &value, VarDesc::VarType type=VarDesc::User, RenderPass pass=FirstPass)=0;
    virtual void deleteVariable(const QString &name)=0;
    virtual void changeVariable(const QString &name, const QVariant &value)=0;
    virtual void clearUserVariables()=0;
    virtual QVariant variable(const QString &name)=0;
    virtual VarDesc::VarType variableType(const QString &name)=0;
    virtual RenderPass variablePass(const QString &name)=0;
    virtual bool containsVariable(const QString &name)=0;
    virtual QStringList variableNames()=0;
};

class VariablesHolder : public QObject, public IVariablesContainer
{
    Q_OBJECT
public:
    explicit VariablesHolder(QObject *parent = 0);
    ~VariablesHolder();
    void addVariable(const QString &name, const QVariant &value, VarDesc::VarType type=VarDesc::User, RenderPass pass=FirstPass);
    void deleteVariable(const QString &name);
    void changeVariable(const QString &name, const QVariant &value);
    void clearUserVariables();
    QVariant variable(const QString &name);
    VarDesc::VarType variableType(const QString &name);
    RenderPass variablePass(const QString &name);
    bool containsVariable(const QString &name);
    QStringList variableNames();
    int userVariablesCount();
    VarDesc* userVariableAt(int index);
signals:
private:
    QMap<QString,VarDesc*> m_varNames;
    QList<VarDesc*> m_userVariables;
};

}// namespace LimeReport

#endif // VARIABLEHOLDER_H
