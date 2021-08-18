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
#ifndef LRGROUPFUNCTIONS_H
#define LRGROUPFUNCTIONS_H

#include <QString>
#include <QVariant>
#include <QVector>

namespace LimeReport{

class DataSourceManager;
class BandDesignIntf;
class PageItemDesignIntf;

class GroupFunction : public QObject{
    Q_OBJECT
public:
    enum DataType{Variable, Field, Script, ContentItem};
    GroupFunction(const QString& expression, const QString& dataBandName, DataSourceManager *dataManager);
    bool isValid(){return m_isValid;}
    void setInvalid(QString message){m_isValid=false,m_errorMessage=message;}
    const QString& name(){return m_name;}
    const QString& data(){return m_data;}
    const QString& error(){return m_errorMessage;}
    QVector<QVariant>& values(){return m_values;}
    QHash<BandDesignIntf*, QVariant> m_valuesByBand;
    const QString& dataBandName(){return m_dataBandName;}
    virtual QVariant calculate(PageItemDesignIntf* page = 0)=0;
public slots:
    void slotBandRendered(BandDesignIntf* band);
    void slotBandReRendered(BandDesignIntf* oldBand, BandDesignIntf* newBand);
protected:
    void setName(const QString& value){m_name=value;}
    QVariant addition(QVariant value1, QVariant value2);
    QVariant subtraction(QVariant value1, QVariant value2);
    QVariant division(QVariant value1, QVariant value2);
    QVariant multiplication(QVariant value1, QVariant value2);
private:
    QString m_data;
    QString m_name;
    DataType m_dataType;
    QString m_dataBandName;
    QVector<QVariant> m_values;
    DataSourceManager* m_dataManager;
    bool m_isValid;
    QString m_errorMessage;
};

class GroupFunctionCreator{
public:
    virtual GroupFunction* createFunction(const QString&, const QString&, DataSourceManager*)=0;
    virtual ~GroupFunctionCreator(){}
};

class GroupFunctionFactory{
public:
    void registerFunctionCreator(const QString& functionName,GroupFunctionCreator* creator){
        m_creators.insert(functionName,creator);
    }
    QList<QString> functionNames(){return m_creators.keys();}
    GroupFunction* createGroupFunction(const QString& functionName, const QString& expression, const QString& dataBandName, DataSourceManager* dataManager);
    ~GroupFunctionFactory();
private:
    QMap<QString,GroupFunctionCreator*> m_creators;
};

class CountGroupFunction :public GroupFunction{
    Q_OBJECT
public:
    CountGroupFunction(const QString& expression, const QString& dataBandName, DataSourceManager *dataManager)
        :GroupFunction(expression, dataBandName, dataManager){setName("COUNT");}
protected:
    virtual QVariant calculate(PageItemDesignIntf* page = 0);
};

class SumGroupFunction :public GroupFunction{
    Q_OBJECT
public:
    SumGroupFunction(const QString& expression, const QString& dataBandName, DataSourceManager *dataManager)
        :GroupFunction(expression, dataBandName, dataManager){setName("SUM");}
protected:
    virtual QVariant calculate(PageItemDesignIntf* page = 0);
};

class AvgGroupFunction :public GroupFunction{
    Q_OBJECT
public:
    AvgGroupFunction(const QString& expression, const QString& dataBandName, DataSourceManager *dataManager)
        :GroupFunction(expression, dataBandName, dataManager){setName("AVG");}
protected:
    virtual QVariant calculate(PageItemDesignIntf* page = 0);
};

class MinGroupFunction :public GroupFunction{
    Q_OBJECT
public:
    MinGroupFunction(const QString& expression, const QString& dataBandName, DataSourceManager *dataManager)
        :GroupFunction(expression, dataBandName, dataManager){setName("MIN");}
protected:
    virtual QVariant calculate(PageItemDesignIntf* page = 0);
};

class MaxGroupFunction :public GroupFunction{
    Q_OBJECT
public:
    MaxGroupFunction(const QString& expression, const QString& dataBandName, DataSourceManager *dataManager)
        :GroupFunction(expression, dataBandName, dataManager){setName("MAX");}
protected:
    virtual QVariant calculate(PageItemDesignIntf* page = 0);
};

template <typename T>
class ConstructorGroupFunctionCreator : public GroupFunctionCreator{
    virtual GroupFunction* createFunction(const QString& expression, const QString &dataBandName, DataSourceManager* dataManager);
};

template< typename T >
GroupFunction* ConstructorGroupFunctionCreator< T >::createFunction(const QString &expression, const QString &dataBandName, DataSourceManager *dataManager)
{
    return new T(expression, dataBandName, dataManager);
}

} //namespace LimeReport

#endif // LRGROUPFUNCTIONS_H
