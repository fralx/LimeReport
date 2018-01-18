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
#include "lrdatasourcemanager.h"
#include "lrdatadesignintf.h"
#include <QStringList>
#include <QSqlQuery>
#include <QRegExp>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QFileInfo>
#include <stdexcept>

namespace LimeReport{

DataNode::~DataNode()
{
    for (int i = 0; i<m_childs.count(); ++i){
        delete m_childs[i];
    }
}

DataNode*DataNode::addChild(const QString& name, DataNode::NodeType type, const QIcon& icon)
{
    DataNode* res = new DataNode(name,type,this,icon);
    m_childs.push_back(res);
    return res;
}

int DataNode::row()
{
    if (m_parent){
        return m_parent->m_childs.indexOf(const_cast<DataNode*>(this));
    }
    return 0;
}

void DataNode::clear()
{
    for (int i=0; i<m_childs.count(); ++i){
        delete m_childs[i];
    }
    m_childs.clear();
}


DataSourceModel::DataSourceModel(DataSourceManager* dataManager)
    :m_dataManager(0), m_rootNode(new DataNode())
{
    setDataSourceManager(dataManager);
}

DataSourceModel::~DataSourceModel()
{
    delete m_rootNode;
}

QModelIndex DataSourceModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!m_rootNode)
        return QModelIndex();

    if (!hasIndex(row,column,parent))
        return QModelIndex();

    DataNode* parentNode;
    if (parent.isValid()){
        parentNode = nodeFromIndex(parent);
    } else {
        parentNode = m_rootNode;
    }

    DataNode* childNode = parentNode->child(row);
    if (childNode){
        return createIndex(row,column,childNode);
    } else return QModelIndex();
}

QModelIndex DataSourceModel::parent(const QModelIndex& child) const
{
    if (!child.isValid()) return QModelIndex();

    DataNode* childNode = nodeFromIndex(child);
    if (!childNode) return QModelIndex();

    DataNode* parentNode = childNode->parent();
    if ((parentNode == m_rootNode) || (!parentNode)) return QModelIndex();
    return createIndex(parentNode->row(),0,parentNode);
}

int DataSourceModel::rowCount(const QModelIndex& parent) const
{
    if (!m_rootNode) return 0;
    DataNode* parentNode;
    if (parent.isValid())
        parentNode = nodeFromIndex(parent);
    else
        parentNode = m_rootNode;
    return parentNode->childCount();
}

int DataSourceModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant DataSourceModel::data(const QModelIndex& index, int role) const
{
    DataNode *node = nodeFromIndex(index);
    switch (role) {
    case Qt::DisplayRole:
        if (!node) return QVariant();
        return node->name();
        break;
    case Qt::DecorationRole :
        if (!node) return QIcon();
        return node->icon();
        break;
    default:
        return QVariant();
    }
}

void DataSourceModel::setDataSourceManager(DataSourceManager* dataManager)
{
    m_dataManager = dataManager;
    connect(m_dataManager,SIGNAL(datasourcesChanged()),this,SLOT(slotDatasourcesChanged()));
    updateModel();
}

void DataSourceModel::slotDatasourcesChanged()
{
    beginResetModel();
    updateModel();
    endResetModel();
}

DataNode* DataSourceModel::nodeFromIndex(const QModelIndex& index) const
{
    if (index.isValid()){
        return static_cast<DataNode*>(index.internalPointer());
    } else return m_rootNode;
}

void DataSourceModel::fillFields(DataNode* parent)
{
    foreach(QString name, m_dataManager->fieldNames(parent->name())){
        parent->addChild(name,DataNode::Field,QIcon(":/report/images/field"));
    }
}

void DataSourceModel::updateModel()
{
    QMap<QString,DataNode*> connections;

    m_rootNode->clear();
    DataNode* ds = m_rootNode->addChild(tr("Datasources"),DataNode::DataSources,QIcon(":/report/images/databases"));


    foreach (QString name,m_dataManager->connectionNames()) {
        DataNode* connection = ds->addChild(name,DataNode::Connection,QIcon(":/report/images/database"));
        connections.insert(name,connection);
    }

    foreach (QString name, m_dataManager->dataSourceNames()) {
        DataNode* datasource;
        if (m_dataManager->isQuery(name)){
            DataNode* connection = connections.value(m_dataManager->queryByName(name)->connectionName());
            if (connection) datasource=connection->addChild(name,DataNode::Query,QIcon(":/report/images/table"));
            else datasource=ds->addChild(name,DataNode::Query);
        } else if (m_dataManager->isSubQuery(name)){
            DataNode* connection = connections.value(m_dataManager->subQueryByName(name)->connectionName());
            if (connection) datasource=connection->addChild(name,DataNode::SubQuery,QIcon(":/report/images/table"));
            else datasource=ds->addChild(name,DataNode::SubQuery);
        } else {
            datasource=ds->addChild(name,DataNode::Model,QIcon(":/report/images/table"));
        }
        fillFields(datasource);
    }

    DataNode* vars = m_rootNode->addChild(tr("Variables"),DataNode::Variables,QIcon(":/report/images/folder"));
    foreach (QString name, m_dataManager->variableNames()){
        vars->addChild(name,DataNode::Variable,QIcon(":/report/images/value"));
    }

    vars = m_rootNode->addChild(tr("External variables"),DataNode::Variables,QIcon(":/report/images/folder"));
    foreach (QString name, m_dataManager->namesOfUserVariables()){
        vars->addChild(name,DataNode::Variable,QIcon(":/report/images/value"));
    }
}

DataSourceManager::DataSourceManager(QObject *parent) :
    QObject(parent), m_lastError(""), m_designTime(true), m_needUpdate(false), m_dbCredentialsProvider(0)
{
    m_groupFunctionFactory.registerFunctionCreator(QLatin1String("COUNT"),new ConstructorGroupFunctionCreator<CountGroupFunction>);
    m_groupFunctionFactory.registerFunctionCreator(QLatin1String("SUM"),new ConstructorGroupFunctionCreator<SumGroupFunction>);
    m_groupFunctionFactory.registerFunctionCreator(QLatin1String("AVG"),new ConstructorGroupFunctionCreator<AvgGroupFunction>);
    m_groupFunctionFactory.registerFunctionCreator(QLatin1String("MIN"),new ConstructorGroupFunctionCreator<MinGroupFunction>);
    m_groupFunctionFactory.registerFunctionCreator(QLatin1String("MAX"),new ConstructorGroupFunctionCreator<MaxGroupFunction>);
    setSystemVariable(QLatin1String("#PAGE"),1,FirstPass);
    setSystemVariable(QLatin1String("#PAGE_COUNT"),0,SecondPass);
    setSystemVariable(QLatin1String("#IS_LAST_PAGEFOOTER"),false,FirstPass);
    setSystemVariable(QLatin1String("#IS_FIRST_PAGEFOOTER"),false,FirstPass);

    connect(&m_reportVariables, SIGNAL(variableHasBeenAdded(QString)),
            this, SLOT(slotVariableHasBeenAdded(QString)) );
    connect(&m_reportVariables, SIGNAL(variableHasBeenChanged(QString)),
            this, SLOT(slotVariableHasBeenChanged(QString)));
    connect(&m_userVariables, SIGNAL(variableHasBeenAdded(QString)),
            this, SLOT(slotVariableHasBeenAdded(QString)) );
    connect(&m_userVariables, SIGNAL(variableHasBeenChanged(QString)),
            this, SLOT(slotVariableHasBeenChanged(QString)));

    m_datasourcesModel.setDataSourceManager(this);
}

QString DataSourceManager::defaultDatabasePath() const
{
    return m_defaultDatabasePath;
}

void DataSourceManager::setDefaultDatabasePath(const QString &defaultDatabasePath)
{
    m_defaultDatabasePath = defaultDatabasePath;
}

QString DataSourceManager::putGroupFunctionsExpressions(QString expression)
{
    if (m_groupFunctionsExpressionsMap.contains(expression)){
        return QString::number(m_groupFunctionsExpressionsMap.value(expression));
    } else {
        m_groupFunctionsExpressions.append(expression);
        m_groupFunctionsExpressionsMap.insert(expression, m_groupFunctionsExpressions.size()-1);
        return QString::number(m_groupFunctionsExpressions.size()-1);
    }
}

void DataSourceManager::clearGroupFuntionsExpressions()
{
    m_groupFunctionsExpressionsMap.clear();
    m_groupFunctionsExpressions.clear();
}

QString DataSourceManager::getExpression(QString index)
{
    bool ok = false;
    int i = index.toInt(&ok);
    if (ok && m_groupFunctionsExpressions.size()>i)
        return m_groupFunctionsExpressions.at(index.toInt());
    else return "";
}

bool DataSourceManager::designTime() const
{
    return m_designTime;
}

void DataSourceManager::setDesignTime(bool designTime)
{
    m_designTime = designTime;
}


DataSourceManager::~DataSourceManager()
{
    clear(All);
    clearGroupFunction();
}

void DataSourceManager::connectAllDatabases()
{
    foreach(ConnectionDesc* conn,m_connections){
        try{
            connectConnection(conn);
        } catch (ReportError e){
            putError(e.what());
            setLastError(e.what());
            qDebug()<<e.what();
        }
    }
}

bool DataSourceManager::addModel(const QString &name, QAbstractItemModel *model, bool owned)
{
    if (m_datasources.contains(name.toLower()))
        removeDatasource(name.toLower());
    ModelHolder* mh = new ModelHolder(model,owned);
    try{
        putHolder(name, mh);
        connect(mh, SIGNAL(modelStateChanged()), this, SIGNAL(datasourcesChanged()));
    } catch (ReportError e){
        putError(e.what());
        setLastError(e.what());
        return false;
    }
    emit datasourcesChanged();
    return true;
}

void DataSourceManager::removeModel(const QString &name)
{
    if (m_datasources.contains(name.toLower()))
        removeDatasource(name.toLower());
}

ICallbackDatasource *DataSourceManager::createCallbackDatasource(const QString& name)
{
    ICallbackDatasource* ds = new CallbackDatasource();
    IDataSourceHolder* holder = new CallbackDatasourceHolder(dynamic_cast<IDataSource*>(ds),true);
    putHolder(name,holder);
    emit datasourcesChanged();
    m_needUpdate = true;
    return ds;
}

void DataSourceManager::registerDbCredentialsProvider(IDbCredentialsProvider *provider)
{
    m_dbCredentialsProvider = provider;
}

void DataSourceManager::addCallbackDatasource(ICallbackDatasource *datasource, const QString& name)
{
    IDataSource* datasourceIntf = dynamic_cast<IDataSource*>(datasource);
    if (datasourceIntf){
        IDataSourceHolder* holder = new CallbackDatasourceHolder(datasourceIntf,true);
        putHolder(name,holder);
        emit datasourcesChanged();
    }
}

QSharedPointer<QAbstractItemModel>DataSourceManager::previewSQL(const QString &connectionName, const QString &sqlText, QString masterDatasource)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);

    if (db.isValid() && db.isOpen()){

        QSqlQueryModel* model = new QSqlQueryModel();
        QMap<QString,QString> aliasesToParam;
        QString queryText = replaceVariables(sqlText,aliasesToParam);
        queryText = replaceFields(queryText,aliasesToParam,masterDatasource);
        QSqlQuery query(db);
        query.prepare(queryText);

        foreach(QString param,aliasesToParam.keys()){
            QVariant value;
            if (param.contains(".")){
                value = fieldData(aliasesToParam.value(param));
                param=param.right(param.length()-param.indexOf('.')-1);
            } else {
                value = variable(aliasesToParam.value(param));
            }
            if (value.isValid() || m_designTime)
                query.bindValue(':'+param,value);
        }

        query.exec();
        model->setQuery(query);
        m_lastError = model->lastError().text();
        putError(m_lastError);
        if (model->query().isActive())
            return QSharedPointer<QAbstractItemModel>(model);
        else
            return QSharedPointer<QAbstractItemModel>(0);
    }
    if (!db.isOpen()){
        m_lastError = tr("Connection \"%1\" is not open").arg(connectionName);
        putError(m_lastError);
    }
    return QSharedPointer<QAbstractItemModel>(0);
}

void DataSourceManager::updateDatasourceModel()
{
    m_datasourcesModel.updateModel();
    m_needUpdate = false;
}

QString DataSourceManager::extractField(QString source)
{
    if (source.contains('.')) {
        return source.right(source.length()-(source.indexOf('.')+1));
    }
    return source;
}

QString DataSourceManager::replaceVariables(QString value){
    QRegExp rx(Const::VARIABLE_RX);

    if (value.contains(rx)){
        int pos = -1;
        while ((pos=rx.indexIn(value))!=-1){
            QString var=rx.cap(0);
            var.remove("$V{");
            var.remove("}");

            if (variable(var).isValid()){
                value.replace(pos,rx.cap(0).length(),variable(var).toString());
            } else {
                value.replace(pos,rx.cap(0).length(),QString(tr("Variable \"%1\" not found!").arg(var)));
            }
        }
    }
    return value;
}

QString DataSourceManager::replaceVariables(QString query, QMap<QString,QString> &aliasesToParam)
{
    QRegExp rx(Const::VARIABLE_RX);
    int curentAliasIndex = 0;
    if (query.contains(rx)){
        int pos = -1;
        while ((pos=rx.indexIn(query))!=-1){

            QString var=rx.cap(0);
            var.remove("$V{");
            var.remove("}");

            if (aliasesToParam.contains(var)){
                curentAliasIndex++;
                aliasesToParam.insert(var+"_v_alias"+QString::number(curentAliasIndex),var);
                var += "_v_alias"+QString::number(curentAliasIndex);
            } else {
                aliasesToParam.insert(var,var);
            }

            query.replace(pos,rx.cap(0).length(),":"+var);

        }
    }
    return query;
}

QString DataSourceManager::replaceFields(QString query, QMap<QString,QString> &aliasesToParam, QString masterDatasource)
{
    QRegExp rx(Const::FIELD_RX);
    int curentAliasIndex=0;
    if (query.contains(rx)){
        int pos;
        while ((pos=rx.indexIn(query))!=-1){
            QString field=rx.cap(0);
            field.remove("$D{");
            field.remove("}");

            if (!aliasesToParam.contains(field)){
                if (field.contains("."))
                    aliasesToParam.insert(field,field);
                else
                    aliasesToParam.insert(field,masterDatasource+"."+field);
            } else {
                curentAliasIndex++;
                if (field.contains("."))
                    aliasesToParam.insert(field+"_f_alias"+QString::number(curentAliasIndex),field);
                else
                    aliasesToParam.insert(field+"_f_alias"+QString::number(curentAliasIndex),masterDatasource+"."+field);
                field+="_f_alias"+QString::number(curentAliasIndex);
            }
            query.replace(pos,rx.cap(0).length(),":"+extractField(field));
        }
    }
    return query;
}

void DataSourceManager::setReportVariable(const QString &name, const QVariant &value)
{ 
    if (!containsVariable(name)){
        addVariable(name,value);
    } else changeVariable(name,value);
}

void DataSourceManager::addQuery(const QString &name, const QString &sqlText, const QString &connectionName)
{
    QueryDesc *queryDecs = new QueryDesc(name,sqlText,connectionName);
    putQueryDesc(queryDecs);
    putHolder(name,new QueryHolder(sqlText, connectionName, this));
    emit datasourcesChanged();
}

void DataSourceManager::addSubQuery(const QString &name, const QString &sqlText, const QString &connectionName, const QString &masterDatasource)
{
    SubQueryDesc *subQueryDesc = new SubQueryDesc(name.toLower(),sqlText,connectionName,masterDatasource);
    putSubQueryDesc(subQueryDesc);
    putHolder(name,new SubQueryHolder(sqlText, connectionName, masterDatasource, this));
    emit datasourcesChanged();
}

void DataSourceManager::addProxy(const QString &name, QString master, QString detail, QList<FieldsCorrelation> fields)
{
    ProxyDesc *proxyDesc = new ProxyDesc();
    proxyDesc->setName(name);
    proxyDesc->setMaster(master);
    proxyDesc->setDetail(detail);
    foreach(FieldsCorrelation correlation,fields){
        proxyDesc->addFieldsCorrelation(correlation);
    }
    putProxyDesc(proxyDesc);
    putHolder(name,new ProxyHolder(proxyDesc, this));
    emit datasourcesChanged();
}

QString DataSourceManager::queryText(const QString &dataSourceName)
{
    if (isQuery(dataSourceName)) return queryByName(dataSourceName)->queryText();
    if (isSubQuery(dataSourceName)) return subQueryByName(dataSourceName)->queryText();
    else return QString();
}

QueryDesc *DataSourceManager::queryByName(const QString &dataSourceName)
{
    int queryIndex = queryIndexByName(dataSourceName);
    if (queryIndex!=-1) return m_queries.at(queryIndex);
    return 0;
}

SubQueryDesc* DataSourceManager::subQueryByName(const QString &dataSourceName)
{
    int queryIndex = subQueryIndexByName(dataSourceName);
    if (queryIndex!=-1) return m_subqueries.at(queryIndex);
    return 0;
}

ConnectionDesc* DataSourceManager::connectionByName(const QString &connectionName)
{
    int queryIndex = connectionIndexByName(connectionName);
    if (queryIndex!=-1) return m_connections.at(queryIndex);
    return 0;
}

int DataSourceManager::queryIndexByName(const QString &dataSourceName)
{
    for(int i=0;i<m_queries.count();i++){
        QueryDesc* desc=m_queries.at(i);
        if (desc->queryName().compare(dataSourceName,Qt::CaseInsensitive)==0) return i;
    }
    return -1;
}

int DataSourceManager::subQueryIndexByName(const QString &dataSourceName)
{
    for(int i=0;i<m_subqueries.count();++i){
        QueryDesc* desc=m_subqueries.at(i);
        if (desc->queryName().compare(dataSourceName,Qt::CaseInsensitive)==0) return i;
    }
    return -1;
}

int DataSourceManager::proxyIndexByName(const QString &dataSourceName)
{
    for(int i=0;i<m_proxies.count();++i){
        ProxyDesc* desc=m_proxies.at(i);
        if (desc->name().compare(dataSourceName,Qt::CaseInsensitive)==0) return i;
    }
    return -1;
}

int DataSourceManager::connectionIndexByName(const QString &connectionName)
{
    for(int i=0;i<m_connections.count();++i){
        ConnectionDesc* desc=m_connections.at(i);
        if (desc->name().compare(connectionName,Qt::CaseInsensitive)==0) return i;
    }
    return -1;
}

QList<ConnectionDesc *>& DataSourceManager::conections()
{
    return m_connections;
}

bool DataSourceManager::dataSourceIsValid(const QString &name)
{
    if (m_datasources.value(name.toLower())) return !m_datasources.value(name.toLower())->isInvalid();
    else throw ReportError(tr("Datasource \"%1\" not found!").arg(name));
}

bool DataSourceManager::isQuery(const QString &dataSourceName)
{
    return (queryByName(dataSourceName));
}

QString DataSourceManager::connectionName(const QString &dataSourceName)
{
    if (isQuery(dataSourceName)) return queryByName(dataSourceName)->connectionName();
    if (isSubQuery(dataSourceName)) return subQueryByName(dataSourceName)->connectionName();
    return QString();
}

ProxyDesc *DataSourceManager::proxyByName(QString datasourceName)
{
    int proxyIndex = proxyIndexByName(datasourceName);
    if (proxyIndex>-1) return m_proxies.at(proxyIndex);
    else return 0;
}

void DataSourceManager::removeDatasource(const QString &name)
{
    invalidateLinkedDatasources(name);

    if (m_datasources.contains(name)){
        IDataSourceHolder *holder;
        holder=m_datasources.value(name);
        m_datasources.remove(name);
        delete holder;
    }
    if (isQuery(name)){
        int queryIndex=queryIndexByName(name);
        delete m_queries.at(queryIndex);
        m_queries.removeAt(queryIndex);
    }
    if (isSubQuery(name)){
        int queryIndex=subQueryIndexByName(name);
        delete m_subqueries.at(queryIndex);
        m_subqueries.removeAt(queryIndex);
    }
    if (isProxy(name)){
        int proxyIndex=proxyIndexByName(name);
        delete m_proxies.at(proxyIndex);
        m_proxies.removeAt(proxyIndex);
    }
    emit datasourcesChanged();
}

void DataSourceManager::removeConnection(const QString &connectionName)
{
    QList<ConnectionDesc*>::iterator cit = m_connections.begin();
    while( cit != m_connections.end() ){
        if ( ((*cit)->name().compare(connectionName) == 0) ){
            if ((*cit)->isInternal())
            {
                QSqlDatabase db = QSqlDatabase::database(connectionName);
                db.close();
            }
            QSqlDatabase::removeDatabase(connectionName);
            delete (*cit);
            cit = m_connections.erase(cit);
        } else {
            cit++;
        }
    }
    emit datasourcesChanged();
}

void DataSourceManager::addConnectionDesc(ConnectionDesc * connection)
{
    if (!isConnection(connection->name())) {
        connect(connection,SIGNAL(nameChanged(QString,QString)),this,SLOT(slotConnectionRenamed(QString,QString)));
        m_connections.append(connection);
        if (connection->autoconnect()){
            try{
              connectConnection(connection);
            } catch(ReportError &exception) {
                qDebug()<<exception.what();
            }
        }
    } else {
       throw ReportError(tr("Connection with name \"%1\" already exists!").arg(connection->name()));
    }
}

bool DataSourceManager::checkConnectionDesc(ConnectionDesc *connection)
{
    if (connectConnection(connection)){
        if (connection->isInternal()){
            QSqlDatabase::removeDatabase(connection->name());
            if (designTime()) emit datasourcesChanged();
        }
        return true;
    }
    if (connection->isInternal())
        QSqlDatabase::removeDatabase(connection->name());
    return false;
}

void DataSourceManager::putHolder(const QString& name, IDataSourceHolder *dataSource)
{
    if (!m_datasources.contains(name.toLower())){
        m_datasources.insert(
            name.toLower(),
            dataSource
        );
    } else throw ReportError(tr("Datasource with name \"%1\" already exists!").arg(name));
}

void DataSourceManager::putQueryDesc(QueryDesc* queryDesc)
{
    if (!containsDatasource(queryDesc->queryName())){
        m_queries.append(queryDesc);
        connect(queryDesc, SIGNAL(queryTextChanged(QString,QString)),
                this, SLOT(slotQueryTextChanged(QString,QString)));
    } else throw ReportError(tr("Datasource with name \"%1\" already exists!").arg(queryDesc->queryName()));
}

void DataSourceManager::putSubQueryDesc(SubQueryDesc *subQueryDesc)
{
    if (!containsDatasource(subQueryDesc->queryName())){
        m_subqueries.append(subQueryDesc);
        connect(subQueryDesc, SIGNAL(queryTextChanged(QString,QString)),
                this, SLOT(slotQueryTextChanged(QString,QString)));
    } else throw ReportError(tr("Datasource with name \"%1\" already exists!").arg(subQueryDesc->queryName()));
}

void DataSourceManager::putProxyDesc(ProxyDesc *proxyDesc)
{
    if (!containsDatasource(proxyDesc->name())){
        m_proxies.append(proxyDesc);
    } else throw ReportError(tr("Datasource with name \"%1\" already exists!").arg(proxyDesc->name()));
}

bool DataSourceManager::initAndOpenDB(QSqlDatabase& db, ConnectionDesc& connectionDesc){

    bool connected = false;


    db.setHostName(replaceVariables(connectionDesc.host()));
    db.setUserName(replaceVariables(connectionDesc.userName()));
    db.setPassword(replaceVariables(connectionDesc.password()));
    if (connectionDesc.port()!=-1)
        db.setPort(connectionDesc.port());

    if (!connectionDesc.keepDBCredentials() && m_dbCredentialsProvider){
        if (!m_dbCredentialsProvider->getUserName(connectionDesc.name()).isEmpty())
            db.setUserName(m_dbCredentialsProvider->getUserName(connectionDesc.name()));
        if (!m_dbCredentialsProvider->getPassword(connectionDesc.name()).isEmpty())
            db.setPassword(m_dbCredentialsProvider->getPassword(connectionDesc.name()));
    }

    QString dbName = replaceVariables(connectionDesc.databaseName());
    if (connectionDesc.driver().compare("QSQLITE")==0){
        if (!defaultDatabasePath().isEmpty()){
            dbName = !QFileInfo(dbName).exists() ?
                    defaultDatabasePath()+QFileInfo(dbName).fileName() :
                    dbName;
        }
        if (QFileInfo(dbName).exists()){
            db.setDatabaseName(dbName);
        } else {
            setLastError(tr("Database \"%1\" not found").arg(dbName));
            return false;
        }
    } else {
        db.setDatabaseName(dbName);
    }

    connected=db.open();
    if (!connected) setLastError(db.lastError().text());
    return  connected;
}

ReportSettings *DataSourceManager::reportSettings() const
{
    return m_reportSettings;
}

void DataSourceManager::setReportSettings(ReportSettings *reportSettings)
{
    m_reportSettings = reportSettings;
}

bool DataSourceManager::checkConnection(QSqlDatabase db){
    QSqlQuery query("Select 1",db);
    return query.first();
}

bool DataSourceManager::connectConnection(ConnectionDesc *connectionDesc)
{

    bool connected = false;
    clearErrors();
    QString lastError ="";

    foreach(QString datasourceName, dataSourceNames()){
        dataSourceHolder(datasourceName)->clearErrors();
    }

    if (!QSqlDatabase::contains(connectionDesc->name())){
        QString dbError;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(connectionDesc->driver(),connectionDesc->name());
            connectionDesc->setInternal(true);
            connected=initAndOpenDB(db, *connectionDesc);
            dbError = db.lastError().text();
        }
        if (!connected){
            if (!dbError.trimmed().isEmpty())
                setLastError(dbError);
            QSqlDatabase::removeDatabase(connectionDesc->name());
            return false;
        }
    } else {
        QSqlDatabase db = QSqlDatabase::database(connectionDesc->name());
        if (!connectionDesc->isEqual(db) && connectionDesc->isInternal()){
            db.close();
            connected = initAndOpenDB(db, *connectionDesc);
        } else {
            connected = checkConnection(db);
            if (!connected && connectionDesc->isInternal())
                connected = initAndOpenDB(db, *connectionDesc);
        }
    }

    if (!connected) {
        if (connectionDesc->isInternal())
            QSqlDatabase::removeDatabase(connectionDesc->name());
        return false;
    } else {
        foreach(QString datasourceName, dataSourceNames()){
            if (isQuery(datasourceName)){
               QueryHolder* qh = dynamic_cast<QueryHolder*>(dataSourceHolder(datasourceName));
               if (qh){
                   qh->invalidate(designTime()?IDataSource::DESIGN_MODE:IDataSource::RENDER_MODE);
                   invalidateChildren(datasourceName);
               }
            }
        }
        foreach(QString datasourceName, dataSourceNames()){
            if (isProxy(datasourceName)){
               ProxyHolder* ph = dynamic_cast<ProxyHolder*>(dataSourceHolder(datasourceName));
               if (ph){
                   ph->invalidate(designTime()?IDataSource::DESIGN_MODE:IDataSource::RENDER_MODE);
               }
            }
        }
        if (designTime()) emit datasourcesChanged();
    }
    return true;
}

void DataSourceManager::clearReportVariables()
{
    m_reportVariables.clearUserVariables();
}

void DataSourceManager::connectAutoConnections()
{
    foreach(ConnectionDesc* conn,m_connections){
        if (conn->autoconnect()) {
            try {
                connectConnection(conn);
            } catch(ReportError e){
                setLastError(e.what());
                putError(e.what());
                qDebug()<<e.what();
            }
        }
    }
}

QList<QString> DataSourceManager::childDatasources(const QString &parentDatasourceName)
{
    QList<QString> result;
    foreach(QString datasourceName, dataSourceNames()){
        if (isSubQuery(datasourceName)){
            SubQueryHolder* sh = dynamic_cast<SubQueryHolder*>(dataSourceHolder(datasourceName));
            if (sh && sh->masterDatasource().compare(parentDatasourceName,Qt::CaseInsensitive)==0){
                result.append(datasourceName);
            }
        }
    }
    return result;
}

void DataSourceManager::invalidateChildren(const QString &parentDatasourceName)
{
    foreach(QString datasourceName, childDatasources(parentDatasourceName)){
        SubQueryHolder* sh = dynamic_cast<SubQueryHolder*>(dataSourceHolder(datasourceName));
        if (sh)
            sh->invalidate(designTime()?IDataSource::DESIGN_MODE:IDataSource::RENDER_MODE);
        invalidateChildren(datasourceName);
    }
}

bool DataSourceManager::containsDatasource(const QString &dataSourceName)
{
    return m_datasources.contains(dataSourceName.toLower());
}

bool DataSourceManager::isSubQuery(const QString &dataSourceName)
{
    return subQueryIndexByName(dataSourceName.toLower())!=-1;
}

bool DataSourceManager::isProxy(const QString &dataSourceName)
{
    return proxyIndexByName(dataSourceName)!=-1;
}

bool DataSourceManager::isConnection(const QString &connectionName)
{
    return connectionIndexByName(connectionName)!=-1;
}

bool DataSourceManager::isConnectionConnected(const QString &connectionName)
{
    if (isConnection(connectionName) && QSqlDatabase::contains(connectionName)){
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        return db.isValid() && QSqlDatabase::database(connectionName).isOpen();
    }
    return false;
}

bool DataSourceManager::connectConnection(const QString& connectionName)
{
    return connectConnection(connectionByName(connectionName));
}

void DataSourceManager::disconnectConnection(const QString& connectionName)
{
    foreach(QString datasourceName, dataSourceNames()){
        if (isQuery(datasourceName) || isSubQuery(datasourceName)){
            QueryHolder* qh = dynamic_cast<QueryHolder*>(dataSourceHolder(datasourceName));
            if (qh && qh->connectionName().compare(connectionName,Qt::CaseInsensitive)==0){
                qh->invalidate(designTime()?IDataSource::DESIGN_MODE:IDataSource::RENDER_MODE);
                qh->setLastError(tr("invalid connection"));
            }
        }
    }

    ConnectionDesc* connectionDesc = connectionByName(connectionName);
    if (connectionDesc->isInternal()){
        {
            QSqlDatabase db = QSqlDatabase::database(connectionName);
            if (db.isOpen()) db.close();
        }
        if (QSqlDatabase::contains(connectionName)) QSqlDatabase::removeDatabase(connectionName);
    }

}

IDataSource *DataSourceManager::dataSource(const QString &name)
{
    IDataSourceHolder* holder = m_datasources.value(name.toLower());
    if (holder) {
        if (holder->isInvalid()) {
            setLastError(name+" : "+holder->lastError());
            return 0;
        } else {
            return holder->dataSource(designTime()?IDataSource::DESIGN_MODE:IDataSource::RENDER_MODE);
        }
    } else {
        setLastError(tr("Datasource \"%1\" not found!").arg(name));
        return 0;
    }
}

IDataSourceHolder *DataSourceManager::dataSourceHolder(const QString &name)
{
    if (m_datasources.value(name.toLower())) return m_datasources.value(name.toLower());
    else return 0;
}

QStringList DataSourceManager::dataSourceNames()
{
    QStringList result;
    for (int i=0;i<m_datasources.keys().count();i++){
       result.append(m_datasources.keys().at(i));
    }
    result.sort();
    return result;
}

QStringList DataSourceManager::dataSourceNames(const QString& connectionName)
{
    QStringList result;
    foreach (QueryDesc* query, m_queries) {
        if (query->connectionName().compare(connectionName,Qt::CaseInsensitive)==0){
            result.append(query->queryName());
        }
    }
    foreach (QueryDesc* query, m_subqueries) {
        if (query->connectionName().compare(connectionName,Qt::CaseInsensitive)==0){
            result.append(query->queryName());
        }
    }
    return result;
}

QStringList DataSourceManager::connectionNames()
{
    QStringList result;
    foreach(ConnectionDesc* conDesc,m_connections){
       result.append(conDesc->name());
    }
    return result;
}

QStringList DataSourceManager::fieldNames(const QString &datasourceName)
{
    QStringList result;
    IDataSource* ds = dataSource(datasourceName);
    if (ds && !ds->isInvalid()){
        for(int i=0;i<ds->columnCount();i++){
            result.append(ds->columnNameByIndex(i));
        }
        result.sort();
    }
    return result;
}

void DataSourceManager::addConnection(const QString &connectionName)
{
    addConnectionDesc(new ConnectionDesc(QSqlDatabase::database(connectionName)));
    emit datasourcesChanged();
}

QObject *DataSourceManager::createElement(const QString& collectionName, const QString&)
{
    if (collectionName=="connections"){
        ConnectionDesc* connection = new ConnectionDesc;
        m_connections.append(connection);
        return connection;
    }
    if (collectionName=="queries"){
        QueryDesc* queryDesc = new QueryDesc;
        m_queries.append(queryDesc);
        return queryDesc;
    }
    if (collectionName=="subqueries"){
        SubQueryDesc* subQueryDesc = new SubQueryDesc;
        m_subqueries.append(subQueryDesc);
        return subQueryDesc;
    }
    if (collectionName=="subproxies"){
        ProxyDesc* proxyDesc = new ProxyDesc;
        m_proxies.append(proxyDesc);
        return proxyDesc;
    }

    if (collectionName=="variables"){
        VarDesc* var = new VarDesc;
        m_tempVars.append(var);
        return var;
    }

    return 0;
}

int DataSourceManager::elementsCount(const QString &collectionName)
{
    if (collectionName=="connections"){
       return m_connections.count();
    }
    if (collectionName=="queries"){
        return m_queries.count();
    }
    if (collectionName=="subqueries"){
        return m_subqueries.count();
    }
    if (collectionName=="subproxies"){
        return m_proxies.count();
    }
    if (collectionName=="variables"){
        return m_reportVariables.userVariablesCount();
    }
    return 0;
}

QObject* DataSourceManager::elementAt(const QString &collectionName, int index)
{
    if (collectionName=="connections"){
        return m_connections.at(index);
    }
    if (collectionName=="queries"){
        return m_queries.at(index);
    }
    if (collectionName=="subqueries"){
        return m_subqueries.at(index);
    }
    if (collectionName=="subproxies"){
        return m_proxies.at(index);
    }
    if (collectionName=="variables"){
        return m_reportVariables.userVariableAt(index);
    }
    return 0;
}

void DataSourceManager::collectionLoadFinished(const QString &collectionName)
{

    if (collectionName.compare("connections",Qt::CaseInsensitive) == 0){

    }

    if (collectionName.compare("queries",Qt::CaseInsensitive) == 0){

        QMutableListIterator<QueryDesc*> it(m_queries);
        while (it.hasNext()){
            it.next();
            if (!m_datasources.contains(it.value()->queryName().toLower())){
                connect(it.value(), SIGNAL(queryTextChanged(QString,QString)),
                        this, SLOT(slotQueryTextChanged(QString,QString)));
                putHolder(it.value()->queryName(),new QueryHolder(it.value()->queryText(), it.value()->connectionName(), this));
            } else {
                delete it.value();
                it.remove();
            }
        }

    }

    if (collectionName.compare("subqueries",Qt::CaseInsensitive) == 0){

        QMutableListIterator<SubQueryDesc*> it(m_subqueries);
        while (it.hasNext()){
            it.next();
            if (!m_datasources.contains(it.value()->queryName().toLower())){
                connect(it.value(), SIGNAL(queryTextChanged(QString,QString)),
                        this, SLOT(slotQueryTextChanged(QString,QString)));
                putHolder(it.value()->queryName(),new SubQueryHolder(
                              it.value()->queryText(),
                              it.value()->connectionName(),
                              it.value()->master(),
                              this)
                );
            } else {
                delete it.value();
                it.remove();
            }
        }

    }

    if (collectionName.compare("subproxies",Qt::CaseInsensitive) == 0){
        QMutableListIterator<ProxyDesc*> it(m_proxies);
        while (it.hasNext()){
            it.next();
            if (!m_datasources.contains(it.value()->name().toLower())){
                putHolder(it.value()->name(),new ProxyHolder(it.value(), this));
            } else {
                delete it.value();
                it.remove();
            }
        }
    }

    if (collectionName.compare("variables",Qt::CaseInsensitive) == 0){
        foreach (VarDesc* item, m_tempVars) {
            if (!m_reportVariables.containsVariable(item->name())){
                m_reportVariables.addVariable(item->name(),item->value(),VarDesc::Report,FirstPass);
            }
            delete item;
        }
        m_tempVars.clear();
    }

    emit datasourcesChanged();
    emit loadCollectionFinished(collectionName);
}

void DataSourceManager::addVariable(const QString &name, const QVariant &value, VarDesc::VarType type, RenderPass pass)
{
    if (type == VarDesc::User){
        m_userVariables.addVariable(name, value, type, pass);
    } else {
        m_reportVariables.addVariable(name,value,type,pass);
    }
    if (designTime())
      emit datasourcesChanged();
}

void DataSourceManager::deleteVariable(const QString& name)
{
    m_userVariables.deleteVariable(name);
    if (m_reportVariables.containsVariable(name)&&m_reportVariables.variableType(name)==VarDesc::Report){
        m_reportVariables.deleteVariable(name);
        if (designTime())
          emit datasourcesChanged();
    }
}

void DataSourceManager::changeVariable(const QString& name,const QVariant& value)
{
    if (m_userVariables.containsVariable(name)){
        m_userVariables.changeVariable(name,value);
    }
    if (m_reportVariables.containsVariable(name)){
        m_reportVariables.changeVariable(name,value);
    }
}

void DataSourceManager::setSystemVariable(const QString &name, const QVariant &value, RenderPass pass)
{
    addVariable(name,value,VarDesc::System,pass);
}

void DataSourceManager::setLastError(const QString &value){
    m_lastError =  value;
    if (!value.isEmpty() && !m_errorsList.contains(value)) {
        m_errorsList.append(value);
    }
}

void DataSourceManager::invalidateLinkedDatasources(QString datasourceName)
{
    foreach(QString name, dataSourceNames()){
        if (isSubQuery(name)){
           if (subQueryByName(name)->master() == datasourceName)
               dataSourceHolder(name)->invalidate(designTime()?IDataSource::DESIGN_MODE:IDataSource::RENDER_MODE);
        }
        if (isProxy(name)){
            ProxyDesc* proxy = proxyByName(name);
            if ((proxy->master() == datasourceName) || (proxy->child() == datasourceName))
                dataSourceHolder(name)->invalidate(designTime()?IDataSource::DESIGN_MODE:IDataSource::RENDER_MODE);

        }
    }
}

void DataSourceManager::slotConnectionRenamed(const QString &oldName, const QString &newName)
{
    foreach(QueryDesc* query, m_queries){
        if (query->connectionName().compare(oldName,Qt::CaseInsensitive)==0) query->setConnectionName(newName);
    }
    foreach(SubQueryDesc* query, m_subqueries){
        if (query->connectionName().compare(oldName,Qt::CaseInsensitive)==0) query->setConnectionName(newName);
    }
}

void DataSourceManager::slotQueryTextChanged(const QString &queryName, const QString &queryText)
{
    QueryHolder* holder = dynamic_cast<QueryHolder*>(m_datasources.value(queryName));
    if (holder){
        holder->setQueryText(queryText);
    }
}

void DataSourceManager::invalidateQueriesContainsVariable(const QString& variableName)
{
    if (!variableIsSystem(variableName)){
        foreach (const QString& datasourceName, dataSourceNames()){
            QueryHolder* holder = dynamic_cast<QueryHolder*>(m_datasources.value(datasourceName));
            if (holder){
                QRegExp rx(QString(Const::NAMED_VARIABLE_RX).arg(variableName));
                if  (holder->queryText().contains(rx))
                    holder->invalidate(designTime()?IDataSource::DESIGN_MODE:IDataSource::RENDER_MODE);
            }
        }
    }
}

void DataSourceManager::slotVariableHasBeenAdded(const QString& variableName)
{
    //qDebug()<< "variable has been added"<< variableName;
    invalidateQueriesContainsVariable(variableName);
}

void DataSourceManager::slotVariableHasBeenChanged(const QString& variableName)
{
    //qDebug()<< "variable has been changed"<< variableName;
    invalidateQueriesContainsVariable(variableName);
}

void DataSourceManager::clear(ClearMethod method)
{
    DataSourcesMap::iterator dit;
    for( dit = m_datasources.begin(); dit != m_datasources.end(); ){
        bool owned = (*dit)->isOwned() && (*dit)->isRemovable();
        switch(method){
        case All:
            invalidateLinkedDatasources(dit.key());
            delete dit.value();
            m_datasources.erase(dit++);
            break;
        default:
            if (owned){
                invalidateLinkedDatasources(dit.key());
                delete dit.value();
                m_datasources.erase(dit++);
            } else {
                ++dit;
            }
        }

    }

    QList<ConnectionDesc*>::iterator cit = m_connections.begin();
    while( cit != m_connections.end() ){
        if ( (*cit)->isInternal() )
            QSqlDatabase::removeDatabase( (*cit)->name() );
        delete (*cit);
        cit = m_connections.erase(cit);
    }

    //TODO: add smart pointes to collections
    foreach(QueryDesc *desc, m_queries) delete desc;
    foreach(SubQueryDesc* desc, m_subqueries) delete desc;
    foreach(ProxyDesc* desc, m_proxies) delete desc;

    m_queries.clear();
    m_subqueries.clear();
    m_proxies.clear();
//    if (method == All)
//        clearUserVariables();
    clearReportVariables();

    emit cleared();
}

void DataSourceManager::clearGroupFunction()
{
    foreach(GroupFunction* gf ,m_groupFunctions.values()){
        delete gf;
    }
    m_groupFunctions.clear();
}

void DataSourceManager::clearGroupFunctionValues(const QString& bandObjectName)
{
    foreach(GroupFunction* gf, m_groupFunctions.values(bandObjectName)){
        gf->values().clear();
    }
}

GroupFunction* DataSourceManager::addGroupFunction(const QString &name, const QString &expression, const QString &band, const QString& dataBand)
{
    GroupFunction* gf = m_groupFunctionFactory.createGroupFunction(name,expression,dataBand,this);
    if (gf){
        m_groupFunctions.insert(band,gf);
    }
    return gf;
}

GroupFunction *DataSourceManager::groupFunction(const QString &name, const QString &expression, const QString &band)
{
    foreach(GroupFunction* gf,m_groupFunctions.values(band)){
        if ((gf->name().compare(name,Qt::CaseInsensitive)==0)&&
            (gf->data().compare(expression,Qt::CaseInsensitive)==0)
           ) return gf;
    }
    return 0;
}

QList<GroupFunction *> DataSourceManager::groupFunctionsByBand(const QString &band)
{
    return m_groupFunctions.values(band);
}

void DataSourceManager::updateChildrenData(const QString &datasourceName)
{
    foreach(SubQueryDesc* subquery,m_subqueries){
        if (subquery->master().compare(datasourceName,Qt::CaseInsensitive)==0){
            SubQueryHolder* holder=dynamic_cast<SubQueryHolder*>(dataSourceHolder(subquery->queryName()));
            if (holder) holder->runQuery();
        }
    }
    foreach(ProxyDesc* subproxy,m_proxies){
        if(subproxy->master().compare(datasourceName,Qt::CaseInsensitive)==0){
            ProxyHolder* holder = dynamic_cast<ProxyHolder*>(dataSourceHolder(subproxy->name()));
            holder->filterModel();
        }
    }
}

QString DataSourceManager::extractDataSource(const QString &fieldName)
{
    return fieldName.left(fieldName.indexOf('.'));
}

QString DataSourceManager::extractFieldName(const QString &fieldName)
{
    return fieldName.right((fieldName.length()-fieldName.indexOf('.'))-1);
}

bool DataSourceManager::containsField(const QString &fieldName)
{
    IDataSource* ds = dataSource(extractDataSource(fieldName));
    if (ds) {
        return ds->columnIndexByName(extractFieldName(fieldName))!=-1;
    }
    return false;
}

bool DataSourceManager::containsVariable(const QString& variableName)
{
    if (m_userVariables.containsVariable(variableName)) return true;
    return m_reportVariables.containsVariable(variableName);
}

void DataSourceManager::clearUserVariables()
{
    m_userVariables.clearUserVariables();
    m_reportVariables.clearUserVariables();
}

QVariant DataSourceManager::fieldData(const QString &fieldName)
{
    if (containsField(fieldName)){
        IDataSource* ds = dataSource(extractDataSource(fieldName));
        if (ds) return ds->data(extractFieldName(fieldName));
    }
    return QVariant();
}

QVariant DataSourceManager::variable(const QString &variableName)
{
    if (m_userVariables.containsVariable(variableName))
        return m_userVariables.variable(variableName);
    return m_reportVariables.variable(variableName);
}

RenderPass DataSourceManager::variablePass(const QString &name)
{
    if (m_userVariables.containsVariable(name))
        return m_userVariables.variablePass(name);
    return m_reportVariables.variablePass(name);
}

bool DataSourceManager::variableIsSystem(const QString &name)
{
    if (m_reportVariables.containsVariable(name))
        return (m_reportVariables.variableType(name)==VarDesc::System);
    return false;
}

QStringList DataSourceManager::variableNames()
{
    return m_reportVariables.variableNames();
}

QStringList DataSourceManager::namesOfUserVariables(){
    return m_userVariables.variableNames();
}

VarDesc::VarType DataSourceManager::variableType(const QString &name)
{
    if (m_reportVariables.containsVariable(name))
        return m_reportVariables.variableType(name);
    return VarDesc::User;
}

void DataSourceManager::setAllDatasourcesToFirst()
{
    foreach(IDataSourceHolder* ds,m_datasources.values()) {
        if (ds->dataSource()) ds->dataSource()->first();
    }
}

} //namespace LimeReport
