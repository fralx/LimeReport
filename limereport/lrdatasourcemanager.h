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
#ifndef LRDATASOURCEMANAGER_H
#define LRDATASOURCEMANAGER_H

#include <QObject>
#include <QIcon>
#include "lrdatadesignintf.h"
#include "lrcollection.h"
#include "lrglobal.h"
#include "lrvariablesholder.h"
#include "lrgroupfunctions.h"
#include "lrdatasourcemanagerintf.h"
#include "lrdatasourceintf.h"

namespace LimeReport{


class DataSourceManager;

class DataNode {
public:
    enum NodeType{Root,Connection,DataSources,Query,SubQuery,Model,Field,Variables,Variable};
    DataNode(const QString& name="", NodeType type=Root, DataNode* parent=0, const QIcon& icon=QIcon())
        :m_name(name), m_icon(icon), m_type(type), m_parent(parent){}
    virtual ~DataNode();
    int       childCount(){return m_childs.count();}
    DataNode* child(int index){return m_childs[index];}
    DataNode* parent(){return m_parent;}
    DataNode* addChild(const QString& name="", NodeType type=Root, const QIcon& icon=QIcon());
    int       row();
    QString   name(){return m_name;}
    QIcon     icon(){return m_icon;}
    void      clear();
    NodeType  type(){return m_type;}
private:
    QString     m_name;
    QIcon       m_icon;
    NodeType    m_type;
    DataNode*   m_parent;
    QVector<DataNode*> m_childs;
};

class DataSourceModel : public QAbstractItemModel{
    Q_OBJECT
    friend class DataSourceManager;
public:
    DataSourceModel():m_dataManager(NULL),m_rootNode(new DataNode()){}
    DataSourceModel(DataSourceManager* dataManager);
    ~DataSourceModel();
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void setDataSourceManager(DataSourceManager* dataManager);
private slots:
    void slotDatasourcesChanged();
private:
    DataNode* nodeFromIndex(const QModelIndex &index) const;
    void fillFields(DataNode* parent);
    void updateModel();
private:
    DataSourceManager* m_dataManager;
    DataNode* m_rootNode;
};

class DataSourceManager : public QObject, public ICollectionContainer, public IVariablesContainer, public IDataSourceManager
{
    Q_OBJECT
    Q_PROPERTY(ACollectionProperty connections READ fakeCollectionReader)
    Q_PROPERTY(ACollectionProperty queries READ fakeCollectionReader)
    Q_PROPERTY(ACollectionProperty subqueries READ fakeCollectionReader)
    Q_PROPERTY(ACollectionProperty subproxies READ fakeCollectionReader)
    Q_PROPERTY(ACollectionProperty variables READ fakeCollectionReader)
    Q_PROPERTY(ACollectionProperty csvs READ fakeCollectionReader)
    friend class ReportEnginePrivate;
    friend class ReportRender;
public:
    typedef QHash<QString,IDataSourceHolder*> DataSourcesMap;
    enum ClearMethod {All,Owned};
    ~DataSourceManager();
    void connectAllDatabases();
    void addConnection(const QString& connectionName);
    void addConnectionDesc(ConnectionDesc *);
    bool checkConnectionDesc(ConnectionDesc *connection);
    void addQuery(const QString& name, const QString& sqlText, const QString& connectionName="");
    void addSubQuery(const QString& name, const QString& sqlText, const QString& connectionName, const QString& masterDatasource);
    void addProxy(const QString& name, const QString& master, const QString& detail, QList<FieldsCorrelation> fields);
    void addCSV(const QString& name, const QString& csvText, const QString& separator, bool firstRowIsHeader);
    bool addModel(const QString& name, QAbstractItemModel *model, bool owned);
    void removeModel(const QString& name);
    ICallbackDatasource* createCallbackDatasource(const QString &name);
    void registerDbCredentialsProvider(IDbCredentialsProvider *provider);
    void addCallbackDatasource(ICallbackDatasource *datasource, const QString &name);
    void setReportVariable(const QString& name, const QVariant& value);
    void deleteVariable(const QString& name);
    bool containsVariable(const QString& variableName);
    void clearUserVariables();
    void addVariable(const QString& name, const QVariant& value, VarDesc::VarType type=VarDesc::User, RenderPass pass=FirstPass);
    void changeVariable(const QString& name,const QVariant& value);
    QVariant    variable(const QString& variableName);
    RenderPass  variablePass(const QString& name);
    QStringList variableNames();
    QStringList variableNamesByRenderPass(RenderPass pass);
    QStringList userVariableNames();
    VarDesc::VarType   variableType(const QString& name);
    VariableDataType   variableDataType(const QString& name);
    void setVariableDataType(const QString &name, VariableDataType value);
    bool variableIsSystem(const QString& name);
    bool variableIsMandatory(const QString& name);
    void setVarableMandatory(const QString &name, bool value);
    QString queryText(const QString& dataSourceName);
    QString connectionName(const QString& dataSourceName);
    void removeDatasource(const QString& name);
    void removeConnection(const QString& connectionName);
    bool isQuery(const QString& dataSourceName);
    bool containsDatasource(const QString& dataSourceName);
    bool isSubQuery(const QString& dataSourceName);
    bool isProxy(const QString& dataSourceName);
    bool isCSV(const QString& datasourceName);
    bool isConnection(const QString& connectionName);
    bool isConnectionConnected(const QString& connectionName);
    bool connectConnection(const QString &connectionName);
    void connectAutoConnections();
    void disconnectConnection(const QString &connectionName);
    QueryDesc* queryByName(const QString& datasourceName);
    SubQueryDesc* subQueryByName(const QString& datasourceName);
    ProxyDesc* proxyByName(const QString& datasourceName);
    CSVDesc* csvByName(const QString& datasourceName);
    ConnectionDesc *connectionByName(const QString& connectionName);
    int queryIndexByName(const QString& dataSourceName);
    int subQueryIndexByName(const QString& dataSourceName);
    int proxyIndexByName(const QString& dataSourceName);
    int csvIndexByName(const QString& dataSourceName);
    int connectionIndexByName(const QString& connectionName);

    QList<ConnectionDesc *> &conections();
    bool dataSourceIsValid(const QString& name);
    IDataSource* dataSource(const QString& name);
    IDataSourceHolder* dataSourceHolder(const QString& name);
    QStringList dataSourceNames();
    QStringList dataSourceNames(const QString& connectionName);
    QStringList connectionNames();
    QStringList fieldNames(const QString& datasourceName);
    bool        containsField(const QString& fieldName);
    QVariant    fieldData(const QString& fieldName);
    QVariant    fieldDataByRowIndex(const QString& fieldName, int rowIndex);
    QVariant    fieldDataByKey(
            const QString& datasourceName,
            const QString& valueFieldName,
            const QString& keyFieldName,
            QVariant keyValue
    );
    void    reopenDatasource(const QString& datasourceName);

    QString extractDataSource(const QString& fieldName);
    QString extractFieldName(const QString& fieldName);
    void setAllDatasourcesToFirst();
    void clear(ClearMethod method);
    void clearGroupFunction();
    void clearGroupFunctionValues(const QString &bandObjectName);

    QList<QString> groupFunctionNames(){return m_groupFunctionFactory.functionNames();}
    GroupFunction* addGroupFunction(const QString& name, const QString& expression, const QString& band, const QString &dataBand);
    GroupFunction* groupFunction(const QString& name, const QString &expression, const QString& band);
    QList<GroupFunction*> groupFunctionsByBand(const QString& band);
    void updateChildrenData(const QString& datasourceName);

    DataSourceModel* datasourcesModel(){return &m_datasourcesModel;}
    QString lastError() const { return m_lastError;}

    void putError(QString error){ if (!m_errorsList.contains(error)) m_errorsList.append(error);}
    void clearErrors(){ m_errorsList.clear(); m_lastError = "";}
    QStringList errorsList(){ return m_errorsList;}
    bool designTime() const;
    void setDesignTime(bool designTime);

    QString extractField(QString source);
    QString replaceVariables(QString query, QMap<QString, QString> &aliasesToParam);
    QString replaceVariables(QString value);
    QString replaceFields(QString query, QMap<QString, QString> &aliasesToParam, QString masterDatasource = "");
    QSharedPointer<QAbstractItemModel> previewSQL(const QString& connectionName, const QString& sqlText, QString masterDatasource="");
    void updateDatasourceModel();
    bool isNeedUpdateDatasourceModel(){ return m_needUpdate;}
    QString defaultDatabasePath() const;
    void setDefaultDatabasePath(const QString &defaultDatabasePath);

    QString putGroupFunctionsExpressions(QString expression);
    void    clearGroupFuntionsExpressions();
    QString getExpression(QString index);

    ReportSettings *reportSettings() const;
    void setReportSettings(ReportSettings *reportSettings);

    bool hasChanges(){ return m_hasChanges; }
    void dropChanges(){ m_hasChanges = false; }
signals:
    void loadCollectionFinished(const QString& collectionName);
    void cleared();
    void datasourcesChanged();
protected:
    void putHolder(const QString& name, LimeReport::IDataSourceHolder* dataSource);
    void putQueryDesc(QueryDesc *queryDesc);
    void putSubQueryDesc(SubQueryDesc *subQueryDesc);
    void putProxyDesc(ProxyDesc *proxyDesc);
    void putCSVDesc(CSVDesc* csvDesc);
    bool connectConnection(ConnectionDesc* connectionDesc);
    void clearReportVariables();
    QList<QString> childDatasources(const QString& datasourceName);
    void invalidateChildren(const QString& parentDatasourceName);
    //ICollectionContainer
    virtual QObject* createElement(const QString& collectionName,const QString&);
    virtual int elementsCount(const QString& collectionName);
    virtual QObject *elementAt(const QString& collectionName,int index);
    virtual void collectionLoadFinished(const QString& collectionName);

    void setSystemVariable(const QString& name, const QVariant& value, RenderPass pass);
    void setLastError(const QString& value);
    void invalidateLinkedDatasources(QString datasourceName);
    bool checkConnection(QSqlDatabase db);
    void invalidateQueriesContainsVariable(const QString& variableName);
private slots:
    void slotConnectionRenamed(const QString& oldName,const QString& newName);
    void slotQueryTextChanged(const QString& queryName, const QString& queryText);
    void slotVariableHasBeenAdded(const QString& variableName);
    void slotVariableHasBeenChanged(const QString& variableName);
    void slotCSVTextChanged(const QString& csvName, const QString& csvText);
private:
    explicit DataSourceManager(QObject *parent = 0);
    bool initAndOpenDB(QSqlDatabase &db, ConnectionDesc &connectionDesc);
    Q_DISABLE_COPY(DataSourceManager)
private:
    QList<ConnectionDesc*> m_connections;
    QList<QueryDesc*> m_queries;
    QList<SubQueryDesc*> m_subqueries;
    QList<ProxyDesc*> m_proxies;
    QList<VarDesc*> m_tempVars;
    QList<CSVDesc*> m_csvs;

    QMultiMap<QString,GroupFunction*> m_groupFunctions;
    GroupFunctionFactory m_groupFunctionFactory;
    VariablesHolder m_reportVariables;
    VariablesHolder m_userVariables;
    DataSourcesMap m_datasources;
    DataSourceModel m_datasourcesModel;
    QString m_lastError;
    QStringList m_errorsList;
    bool m_designTime;
    bool m_needUpdate;
    QString m_defaultDatabasePath;
    ReportSettings* m_reportSettings;
    QHash<QString,int> m_groupFunctionsExpressionsMap;
    QVector<QString> m_groupFunctionsExpressions;
    IDbCredentialsProvider* m_dbCredentialsProvider;

    QMap< QString, QVector<QString> > m_varToDataSource;

    bool m_hasChanges;
};

}
#endif // LRDATASOURCEMANAGER_H

