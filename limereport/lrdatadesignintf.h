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
#ifndef LRDATADESIGNINTF_H
#define LRDATADESIGNINTF_H

#include <QObject>
#include <QMap>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QVariant>
#include "lrcollection.h"
#include "lrcallbackdatasourceintf.h"
#include "lrdatasourceintf.h"

namespace LimeReport{

class DataSourceManager;

class ModelHolder: public QObject, public IDataSourceHolder{
    Q_OBJECT
public:
    ModelHolder(QAbstractItemModel* model, bool owned=false);
    ~ModelHolder();
    IDataSource* dataSource(IDataSource::DatasourceMode mode);
    bool isOwned() const { return m_owned; }
    bool isInvalid() const { return m_dataSource->isInvalid(); }
    QString lastError() const { return m_dataSource->lastError(); }
    bool isEditable() const { return false; }
    bool isRemovable() const { return false; }
    void invalidate(IDataSource::DatasourceMode mode, bool dbWillBeClosed = false){Q_UNUSED(mode) Q_UNUSED(dbWillBeClosed)}
    void update(){}
    void clearErrors(){}
signals:
    void modelStateChanged();
private:
    IDataSource* m_dataSource;
    bool m_owned;
};

class ConnectionDesc : public QObject{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString driver READ driver WRITE setDriver)
    Q_PROPERTY(QString databaseName READ databaseName WRITE setDatabaseName)
    Q_PROPERTY(QString userName READ userName WRITE setUserName)
    Q_PROPERTY(QString password READ password WRITE setPassword)
    Q_PROPERTY(QString host READ host WRITE setHost)
    Q_PROPERTY(bool autoconnect READ autoconnect WRITE setAutoconnect)
    Q_PROPERTY(bool keepDBCredentials READ keepDBCredentials WRITE setKeepDBCredentials)
    Q_PROPERTY(QString port READ port WRITE setPort)
public:
    typedef QSharedPointer<ConnectionDesc> Ptr;
    ConnectionDesc(QSqlDatabase db, QObject* parent=0);
    ConnectionDesc(QObject* parent=0);
    Ptr     create(QSqlDatabase db, QObject* parent=0);
    void    setName(const QString &value);
    QString name(){return m_connectionName;}
    void    setDriver(const QString &value){m_connectionDriver=value;}
    QString driver(){return m_connectionDriver;}
    void    setHost(const QString &value){m_connectionHost=value;}
    QString host(){return m_connectionHost;}
    void    setDatabaseName(const QString &value){m_databaseName=value;}
    QString databaseName(){return m_databaseName;}
    void    setUserName(const QString &value){m_user=value;}
    QString userName(){ return m_user; }
    void    setPassword(const QString &value){m_password=value;}
    QString password(){ return m_password; }
    void    setAutoconnect(bool value){m_autoconnect=value;}
    bool    autoconnect(){return m_autoconnect;}
    bool    isEqual(const QSqlDatabase& db);
    bool    isInternal(){ return m_internal; }
    void    setInternal(bool value) {m_internal = value;}
    bool    keepDBCredentials() const;
    void    setKeepDBCredentials(bool keepDBCredentials);
    QString port() const;
    void    setPort(QString port);
public:
    static QString connectionNameForUser(const QString& connectionName);
    static QString connectionNameForReport(const QString& connectionName);    
signals:
    void nameChanged(const QString& oldName,const QString& newName);
private:
    QString m_connectionName;
    QString m_connectionHost;
    QString m_connectionDriver;
    QString m_databaseName;
    QString m_user;
    QString m_password;
    QString m_port;
    bool    m_autoconnect;
    bool    m_internal;
    bool    m_keepDBCredentials;
};

class IConnectionController{
public:
    virtual void addConnectionDesc(ConnectionDesc* connection) = 0;
    virtual void changeConnectionDesc(ConnectionDesc* connection) = 0;
    virtual bool checkConnectionDesc(ConnectionDesc* connection) = 0;
    virtual bool containsDefaultConnection() = 0;
    virtual QString lastError() const = 0;
};

class CSVDesc: public QObject{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString csvText READ csvText WRITE setCsvText)
    Q_PROPERTY(QString separator READ separator WRITE setSeparator)
    Q_PROPERTY(bool firstRowIsHeader READ firstRowIsHeader WRITE setFirstRowIsHeader)
public:
    CSVDesc(const QString name, const QString csvText, QString separator, bool firstRowIsHeader)
        : m_csvName(name), m_csvText(csvText), m_separator(separator), m_firstRowIsHeader(firstRowIsHeader){}
    explicit CSVDesc(QObject* parent = 0):QObject(parent) {}
    QString name() const;
    void setName(const QString &name);
    QString csvText() const;
    void setCsvText(const QString &csvText);
    QString separator() const;
    void setSeparator(const QString &separator);
    bool firstRowIsHeader() const;
    void setFirstRowIsHeader(bool firstRowIsHeader);
signals:
    void cvsTextChanged(const QString& cvsName, const QString& cvsText);
private:
    QString m_csvName;
    QString m_csvText;
    QString m_separator;
    bool m_firstRowIsHeader;
};

class CSVHolder: public IDataSourceHolder{
public:
    CSVHolder(const CSVDesc& desc, DataSourceManager* dataManager);
    void setCSVText(QString csvText);
    QString csvText() { return m_csvText;}
    QString separator() const;
    void setSeparator(const QString &separator);
    bool firsRowIsHeader() const;
    void setFirsRowIsHeader(bool firstRowIsHeader);
    // IDataSourceHolder interface
public:
    IDataSource *dataSource(IDataSource::DatasourceMode mode = IDataSource::RENDER_MODE);
    QString lastError() const {return "";}
    bool isInvalid() const {return false;}
    bool isOwned() const {return true;}
    bool isEditable() const {return true;}
    bool isRemovable() const {return true;}
    void invalidate(IDataSource::DatasourceMode /*mode*/, bool /*dbWillBeClosed*/){ updateModel();}
    void update(){ updateModel(); }
    void clearErrors(){}
private:
    void updateModel();
private:
    QString m_csvText;
    QStandardItemModel m_model;
    QString m_separator;
    IDataSource::Ptr m_dataSource;
    DataSourceManager* m_dataManager;
    bool m_firstRowIsHeader;
};

class QueryDesc : public QObject{
    Q_OBJECT
    Q_PROPERTY(QString queryName READ queryName WRITE setQueryName)
    Q_PROPERTY(QString queryText READ queryText WRITE setQueryText)
    Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName)
public:
    QueryDesc(QString queryName, QString queryText, QString connection);
    explicit QueryDesc(QObject* parent=0):QObject(parent){}
    void    setQueryName(QString value){m_queryName=value;}
    QString queryName() const {return m_queryName;}
    void    setQueryText(QString value){m_queryText=value; emit queryTextChanged(m_queryName, m_queryText);}
    QString queryText() const {return m_queryText;}
    void    setConnectionName(QString value){m_connectionName=value;}
    QString connectionName() const {return m_connectionName;}
signals:
    void queryTextChanged(const QString& queryName, const QString& queryText);
private:
    QString m_queryName;
    QString m_queryText;
    QString m_connectionName;
};

class QueryHolder:public IDataSourceHolder{
public:
    QueryHolder(QString queryText, QString connectionName, DataSourceManager* dataManager);
    ~QueryHolder();
    virtual bool runQuery(IDataSource::DatasourceMode mode = IDataSource::RENDER_MODE);
    IDataSource* dataSource(IDataSource::DatasourceMode mode = IDataSource::RENDER_MODE);
    QString connectionName();
    QString queryText();
    void setQueryText(QString queryText);
    void setConnectionName(QString connectionName);
    bool isOwned() const { return true; }
    bool isInvalid() const { return !m_lastError.isEmpty(); }
    bool isEditable() const { return true; }
    bool isRemovable() const { return true; }
    bool isPrepared() const {return m_prepared;}
    QString lastError() const { return m_lastError; }
    void setLastError(QString value){m_lastError=value;}
    void invalidate(IDataSource::DatasourceMode mode, bool dbWillBeClosed = false);
    void update();
    void clearErrors(){setLastError("");}
    DataSourceManager* dataManager() const {return m_dataManager;}
protected:
    void setDatasource(IDataSource::Ptr value);
    void setPrepared(bool prepared){ m_prepared = prepared;}
    virtual void fillParams(QSqlQuery* query);
    virtual void extractParams();
    QString replaceVariables(QString query);
    QMap<QString,QString> m_aliasesToParam;
    QString m_preparedSQL;
private:
    QString m_queryText;
    QString m_connectionName;
    QString m_lastError;
    IDataSource::Ptr m_dataSource;
    IDataSource::DatasourceMode m_mode;
    DataSourceManager* m_dataManager;
    bool m_prepared;
};

class SubQueryDesc : public QueryDesc{
    Q_OBJECT
    Q_PROPERTY(QString master READ master WRITE setMaster)
public:
    SubQueryDesc(QString queryName, QString queryText, QString connection, QString master);
    explicit SubQueryDesc(QObject* parent=0):QueryDesc(parent){}
    void setMaster(QString value){m_masterDatasourceName=value;}
    QString master(){return m_masterDatasourceName;}
private:
    QString m_masterDatasourceName;
};

class SubQueryHolder:public QueryHolder{
public:
    SubQueryHolder(QString queryText, QString connectionName, QString masterDatasource, DataSourceManager *dataManager);
    QString masterDatasource(){return m_masterDatasource;}
    void setMasterDatasource(const QString& value);
    //void invalidate(){m_invalid = true;}
    bool isInvalid() const{ return QueryHolder::isInvalid(); /*|| m_invalid;*/}
protected:
    void extractParams();
    QString extractField(QString source);
    QString replaceFields(QString query);
private:
    QString m_masterDatasource;
    //bool m_invalid;
};

struct FieldsCorrelation{
    QString master;
    QString detail;
};

class FieldMapDesc : public QObject{
    Q_OBJECT
    Q_PROPERTY(QString master READ master WRITE setMaster)
    Q_PROPERTY(QString detail READ detail WRITE setDetail)
public:
    FieldMapDesc(){}
    FieldMapDesc(FieldsCorrelation correlation):m_fieldsCorrelation(correlation){}
    QString master(){return m_fieldsCorrelation.master;}
    void setMaster(QString value){m_fieldsCorrelation.master=value;}
    QString detail(){return m_fieldsCorrelation.detail;}
    void setDetail(QString value){m_fieldsCorrelation.detail=value;}
private:
    FieldsCorrelation m_fieldsCorrelation;
};

class ProxyDesc : public QObject, public ICollectionContainer{
    Q_OBJECT
    Q_PROPERTY(ACollectionProperty fields READ fakeCollectionReader)
    Q_PROPERTY(QString master READ master WRITE setMaster)
    Q_PROPERTY(QString child READ child WRITE setDetail)
    Q_PROPERTY(QString name READ name WRITE setName)
public:
    void setMaster(QString value){m_master=value;}
    QString master(){return m_master;}
    void setDetail(QString value){m_detail=value;}
    QString child(){return m_detail;}
    QString name(){return m_name;}
    void setName(QString value){m_name=value;}
    void addFieldsCorrelation(const FieldsCorrelation &fieldsCorrelation);
    QList<FieldMapDesc*>* fieldsMap(){return &m_maps;}
    //ICollectionContainer
    virtual QObject* createElement(const QString& collectionName,const QString&);
    virtual int elementsCount(const QString& collectionName);
    virtual QObject *elementAt(const QString& collectionName,int index);
private:
    QList<FieldMapDesc*> m_maps;
    QString m_master;
    QString m_detail;
    QString m_name;
};

class MasterDetailProxyModel : public QSortFilterProxyModel{    
    Q_OBJECT
public:
    MasterDetailProxyModel(DataSourceManager* dataManager):m_maps(0),m_dataManager(dataManager){}
    void setMaster(QString name);
    void setChildName(QString name){m_childName=name;}
    void setFieldsMap(QList<FieldMapDesc*> *fieldsMap){m_maps=fieldsMap;}
    bool isInvalid() const;
    DataSourceManager* dataManager() const {return m_dataManager;}
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    int fieldIndexByName(QString fieldName) const;
    QVariant sourceData(QString fieldName, int row) const;
    QVariant masterData(QString fieldName) const;
private:
    QList<FieldMapDesc*>* m_maps;
    QString m_masterName;
    QString m_childName;
    DataSourceManager* m_dataManager;
};

class ProxyHolder: public QObject, public IDataSourceHolder{
    Q_OBJECT
public:
    ProxyHolder(ProxyDesc *desc, DataSourceManager *dataManager);
    QString masterDatasource();
    void filterModel();
    IDataSource* dataSource(IDataSource::DatasourceMode mode = IDataSource::RENDER_MODE);
    bool isOwned() const { return true; }
    bool isInvalid() const { return !m_lastError.isEmpty() || m_invalid; }
    bool isEditable() const { return true; }
    bool isRemovable() const { return true; }
    QString lastError() const { return m_lastError; }
    void invalidate(IDataSource::DatasourceMode mode, bool dbWillBeClosed = false);
    void update(){}
    void clearErrors(){m_lastError = "";}
    DataSourceManager* dataManager() const {return m_dataManager;}
private slots:
    void slotChildModelDestoroyed();
private:
    IDataSource::Ptr m_datasource;
    MasterDetailProxyModel  *m_model;
    ProxyDesc *m_desc;
    QString m_lastError;
    IDataSource::DatasourceMode m_mode;
    bool m_invalid;
    DataSourceManager* m_dataManager;
};

class ModelToDataSource : public QObject, public IDataSource{
    Q_OBJECT
public:
    ModelToDataSource(QAbstractItemModel* model, bool owned);
    ~ModelToDataSource();
    bool next();
    bool hasNext();
    bool prior();
    void first();
    void last();
    bool eof();
    bool bof();
    QVariant data(const QString& columnName);
    QVariant dataByRowIndex(const QString &columnName, int rowIndex);
    QVariant dataByKeyField(const QString& columnName, const QString& keyColumnName, QVariant keyData);
    int columnCount();
    QString columnNameByIndex(int columnIndex);
    int columnIndexByName(QString name);
    QString lastError();
    virtual QAbstractItemModel* model();
    int currentRow();
    bool isInvalid() const;
signals:
    void modelStateChanged();
private slots:
    void slotModelDestroed();
private:
    QAbstractItemModel* m_model;
    bool m_owned;
    int  m_curRow;
    QString m_lastError;
};

class CallbackDatasource :public ICallbackDatasource, public IDataSource {
    Q_OBJECT
public:
    CallbackDatasource():  m_currentRow(-1), m_eof(false), m_columnCount(-1),
                           m_rowCount(-1), m_getDataFromCache(false), m_lastKeyRow(0){}
    bool next();
    bool hasNext(){ if (!m_eof) return checkNextRecord(m_currentRow); else return false;}
    bool prior();
    void first();
    void last(){}
    bool bof(){return m_currentRow == -1;}
    bool eof(){return m_eof;}
    QVariant data(const QString &columnName);
    QVariant dataByRowIndex(const QString& columnName, int rowIndex);
    QVariant dataByKeyField(const QString& columnName, const QString& keyColumnName, QVariant keyData);
    int columnCount();
    QString columnNameByIndex(int columnIndex);
    int columnIndexByName(QString name);
    bool isInvalid() const{ return false;}
    QString lastError(){ return "";}
    QAbstractItemModel *model(){return 0;}
private:
    bool checkNextRecord(int recordNum);
    bool checkIfEmpty();
    QVariant callbackData(const QString& columnName, int row);
private:
    QVector<QString> m_headers;
    int m_currentRow;
    bool m_eof;
    int m_columnCount;
    int m_rowCount;
    QHash<QString, QVariant> m_valuesCache;
    bool m_getDataFromCache;
    int m_lastKeyRow;
};

class CallbackDatasourceHolder :public QObject, public IDataSourceHolder{
    Q_OBJECT
    // IDataSourceHolder interface
public:
    CallbackDatasourceHolder(IDataSource* datasource, bool owned):m_owned(owned){ m_datasource = datasource;}
    IDataSource *dataSource(IDataSource::DatasourceMode){ return m_datasource;}
    QString lastError() const{ return m_datasource->lastError();}
    bool isInvalid() const {return m_datasource->isInvalid();}
    bool isOwned() const {return m_owned;}
    bool isEditable() const {return false;}
    bool isRemovable() const {return false;}
    void invalidate(IDataSource::DatasourceMode mode, bool dbWillBeClosed = false){Q_UNUSED(mode) Q_UNUSED(dbWillBeClosed)}
    ~CallbackDatasourceHolder(){delete m_datasource;}
    void update(){}
    void clearErrors(){}
private:
    IDataSource* m_datasource;
    bool m_owned;
};

}
#endif // LRDATADESIGNINTF_H
