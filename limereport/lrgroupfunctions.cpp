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
#include "lrgroupfunctions.h"
#include "lrdatasourcemanager.h"
#include "lrbanddesignintf.h"
#include "lritemdesignintf.h"
#include "lrscriptenginemanager.h"
#include "lrpageitemdesignintf.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
#include <QRegExp>
#endif

namespace LimeReport {

void GroupFunction::slotBandRendered(BandDesignIntf *band)
{
    ScriptEngineManager& sm = ScriptEngineManager::instance();

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    QRegExp rxField(Const::FIELD_RX);
    QRegExp rxVar(Const::VARIABLE_RX);
#else
    QRegularExpression rxField = getFieldRegEx();
    QRegularExpression rxVar = getVariableRegEx();
#endif

    switch (m_dataType){
    case Field:{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
        if (rxField.indexIn(m_data) != -1){
            QString field = rxField.cap(1);
#else
        QRegularExpressionMatch matchField = rxField.match(m_data);
        if(matchField.hasMatch()){
            QString field = matchField.captured(1);
#endif
            if (m_dataManager->containsField(field)){
                m_values.push_back(m_dataManager->fieldData(field));
                m_valuesByBand.insert(band, m_dataManager->fieldData(field));
            } else {
                setInvalid(tr("Field \"%1\" not found").arg(m_data));
            }
        }
        break;
    }
    case Variable:{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
        if (rxVar.indexIn(m_data) != -1){
            QString var = rxVar.cap(1);
#else
        QRegularExpressionMatch matchVar = rxVar.match(m_data);
        if(matchVar.hasMatch()){
            QString var = matchVar.captured(1);
#endif
            if (m_dataManager->containsVariable(var)){
                m_values.push_back(m_dataManager->variable(var));
                m_valuesByBand.insert(band, m_dataManager->variable(var));
            } else {
                setInvalid(tr("Variable \"%1\" not found").arg(m_data));
            }
        }
        break;
    }
    case Script:
    {
        QVariant value = sm.evaluateScript(m_data);
        if (value.isValid()){
            m_values.push_back(value);
            m_valuesByBand.insert(band, value);
        } else {
            setInvalid(tr("Wrong script syntax \"%1\" ").arg(m_data));
        }
        break;
    }
    case ContentItem:{
        QString itemName = m_data;
        ContentItemDesignIntf* item = dynamic_cast<ContentItemDesignIntf*>(band->childByName(itemName.remove('"')));
        if (item){
            m_values.push_back(item->content());
            m_valuesByBand.insert(band, item->content());
        } else if (m_name.compare("COUNT",Qt::CaseInsensitive) == 0) {
            m_values.push_back(1);
            m_valuesByBand.insert(band, 1);
        } else setInvalid(tr("Item \"%1\" not found").arg(m_data));

        break;
    }
    default:
        break;
    }
}

void GroupFunction::slotBandReRendered(BandDesignIntf *oldBand, BandDesignIntf *newBand)
{
    if (m_valuesByBand.contains(oldBand)){
        m_valuesByBand.insert(newBand, m_valuesByBand.value(oldBand));
        m_valuesByBand.remove(oldBand);
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
    :m_data(expression), m_dataBandName(dataBandName), m_dataManager(dataManager), m_isValid(true), m_errorMessage("")
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    QRegExp rxField(Const::FIELD_RX,Qt::CaseInsensitive);
    QRegExp rxVariable(Const::VARIABLE_RX,Qt::CaseInsensitive);
    QRegExp rxScript(Const::SCRIPT_RX,Qt::CaseInsensitive);
#else
    QRegularExpression rxField = getFieldRegEx();
    QRegularExpression rxVariable = getVariableRegEx();
    QRegularExpression rxScript = getScriptRegEx();
#endif
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    if (rxScript.indexIn(expression) != -1){
#else
    QRegularExpressionMatch matchScript = rxScript.match(expression);
    if(matchScript.hasMatch()){
#endif
        m_dataType = Script;
        return;
    }

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    if (rxField.indexIn(expression) != -1){
#else
    QRegularExpressionMatch matchField = rxField.match(expression);
    if(matchField.hasMatch()){
#endif
        m_dataType=Field;
        return;
    }

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    if (rxVariable.indexIn(expression) != -1){
#else
    QRegularExpressionMatch matchVariable = rxVariable.match(expression);
    if(matchVariable.hasMatch()){
#endif
        m_dataType = Variable;
        return;
    }

    m_dataType = ContentItem;
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

QVariant SumGroupFunction::calculate(PageItemDesignIntf *page)
{
    QVariant res = 0;
    if (!page){
        foreach(QVariant value,values()){
            res = addition(res,value);
        }
    } else {
        foreach (BandDesignIntf* band, page->bands()) {
            res = addition(res, m_valuesByBand.value(band));
        }
    }
    return res;
}

QVariant AvgGroupFunction::calculate(PageItemDesignIntf *page)
{
    QVariant res = QVariant();
    if (!page){
        foreach(QVariant value,values()){
            res=addition(res,value);
        }
    } else {
        foreach (BandDesignIntf* band, page->bands()) {
            res = addition(res, m_valuesByBand.value(band));
        }
    }
    if (!res.isNull()&&(values().count()>0)){
        res=division(res,values().count());
    }
    return res;
}

QVariant MinGroupFunction::calculate(PageItemDesignIntf *page)
{
    //TODO: check variant type
    QVariant res = QVariant();
    if (!page){
        if (!values().empty()) res = values().at(0);
        foreach(QVariant value, values()){
            if (res.toDouble() > value.toDouble()) res = value;
        }
    } else {
        if (!page->bands().empty()) res = m_valuesByBand.value(page->bands().at(0));
        foreach (BandDesignIntf* band, page->bands()) {
            if (res.toDouble() > m_valuesByBand.value(band).toDouble()) res = m_valuesByBand.value(band);
        }
    }

    return res;
}

QVariant MaxGroupFunction::calculate(PageItemDesignIntf *page)
{
    //TODO: check variant type
    QVariant res = QVariant();

    if (!page){
        if (!values().empty()) res = values().at(0);
        foreach(QVariant value, values()){
            if (res.toDouble() < value.toDouble()) res = value;
        }
    } else {
        if (!page->bands().empty()) res = m_valuesByBand.value(page->bands().at(0));
        foreach (BandDesignIntf* band, page->bands()) {
            if (res.toDouble() < m_valuesByBand.value(band).toDouble()) res = m_valuesByBand.value(band);
        }
    }

    return res;
}

QVariant CountGroupFunction::calculate(PageItemDesignIntf *page){
    if (!page){
        return values().count();
    } else {
        int res = 0;
        foreach (BandDesignIntf* band, page->bands()) {
            if (!m_valuesByBand.value(band).isNull()){
                res++;
            }
        }
        return res;
    }
}

} //namespace LimeReport

