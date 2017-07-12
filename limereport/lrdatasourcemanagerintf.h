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
#ifndef LRDATASOURCEMANAGERINTF_H
#define LRDATASOURCEMANAGERINTF_H

#include "lrcallbackdatasourceintf.h"

class QVariant;
class QString;
class QAbstractItemModel;
namespace LimeReport{

class IDbCredentialsProvider{
public:
    virtual QString getUserName(const QString& connectionName) = 0;
    virtual QString getPassword(const QString& connectionName) = 0;
};

class IDataSourceManager{
public:
    virtual void setReportVariable(const QString& name, const QVariant& value) = 0;
    virtual void setDefaultDatabasePath(const QString &defaultDatabasePath) = 0;
    virtual void deleteVariable(const QString& name) = 0;
    virtual bool containsVariable(const QString& variableName) = 0;
    virtual QVariant variable(const QString& variableName) = 0;
    virtual bool addModel(const QString& name, QAbstractItemModel *model, bool owned) = 0;
    virtual void removeModel(const QString& name) = 0;
    virtual bool containsDatasource(const QString& dataSourceName) = 0;
    virtual void clearUserVariables()=0;
    virtual ICallbackDatasource* createCallbackDatasource(const QString& name) = 0;
    virtual void registerDbCredentialsProvider(IDbCredentialsProvider* provider) = 0;
    //virtual void addCallbackDatasource(ICallbackDatasource* datasource, const QString& name) = 0;
};

}
#endif // LRDATASOURCEMANAGERINTF_H
