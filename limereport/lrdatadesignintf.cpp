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
#include "lrdatadesignintf.h"

#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>
#include <stdexcept>
#include <QStringList>
#include "lrdatasourcemanager.h"

namespace LimeReport{

ModelHolder::ModelHolder(QAbstractItemModel *model, bool owned /*false*/)
{
    ModelToDataSource* mh = new ModelToDataSource(model,owned);
    m_dataSource = mh;
    m_owned=owned;
    connect(mh, SIGNAL(modelStateChanged()), this, SIGNAL(modelStateChanged()));
}

ModelHolder::~ModelHolder(){
    delete m_dataSource;
}

IDataSource * ModelHolder::dataSource(IDataSource::DatasourceMode mode)
{
    Q_UNUSED(mode);
    return m_dataSource;
}

QueryHolder::QueryHolder(QString queryText, QString connectionName, DataSourceManager *dataManager)
    : m_queryText(queryText), m_connectionName(connectionName),
      m_mode(IDataSource::RENDER_MODE), m_dataManager(dataManager), m_prepared(true)
{
    extractParams();
}

QueryHolder::~QueryHolder(){}

bool QueryHolder::runQuery(IDataSource::DatasourceMode mode)
{
    m_mode = mode;

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);

    if (!db.isValid()) {
        setLastError(QObject::tr("Invalid connection! %1").arg(m_connectionName));
        return false;
    }

    extractParams();
    if (!m_prepared) return false;

    query.prepare(m_preparedSQL);
    fillParams(&query);
    query.exec();

    QSqlQueryModel *model = new QSqlQueryModel;

#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
    model->setQuery(std::move(query));
#else
    model->setQuery(query);
#endif

    while (model->canFetchMore())
        model->fetchMore();

    if (model->lastError().isValid()){
        if (m_dataSource)
           m_dataSource.clear();
        setLastError(model->lastError().text());
        delete model;
        return false;
    } else { setLastError("");}

    setDatasource(IDataSource::Ptr(new ModelToDataSource(model,true)));
    return true;
}

QString QueryHolder::connectionName()
{
    return m_connectionName;
}

void QueryHolder::setConnectionName(QString connectionName)
{
    m_connectionName=connectionName;
}

void QueryHolder::invalidate(IDataSource::DatasourceMode mode, bool dbWillBeClosed){
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isValid() || dbWillBeClosed){
        setLastError(QObject::tr("Invalid connection! %1").arg(m_connectionName));
        m_dataSource.clear();
    } else {
        runQuery(mode);
    }

}

void QueryHolder::update()
{
    runQuery(m_mode);
}

void QueryHolder::setDatasource(IDataSource::Ptr value){
    m_dataSource.clear();
    m_dataSource=value;
}

void QueryHolder::fillParams(QSqlQuery *query)
{
    foreach(QString param,m_aliasesToParam.keys()){
        QVariant value;
        if (param.contains(".")){
            value = dataManager()->fieldData(m_aliasesToParam.value(param));
            param=param.right(param.length()-param.indexOf('.')-1);
        } else {
            value = dataManager()->variable(m_aliasesToParam.value(param));
        }
        if (value.isValid() || m_mode == IDataSource::DESIGN_MODE)
            query->bindValue(':'+param,value);
    }
}

void QueryHolder::extractParams()
{
    m_preparedSQL =  dataManager()->replaceVariables(m_queryText, m_aliasesToParam);
    m_prepared = true;
}

QString QueryHolder::replaceVariables(QString query)
{
    return dataManager()->replaceVariables(query, m_aliasesToParam);
}

QString QueryHolder::queryText()
{
    return m_queryText;
}

void QueryHolder::setQueryText(QString queryText)
{
    m_queryText=queryText;
    m_prepared = false;
}

IDataSource* QueryHolder::dataSource(IDataSource::DatasourceMode mode)
{
    if ((m_mode != mode && m_mode == IDataSource::DESIGN_MODE) || m_dataSource==0) {
        m_mode = mode;
        runQuery(mode);
    }
    if (m_dataSource)
        return m_dataSource.data();
    else return 0;
}

// QueryHolder

// ModelToDataSource

ModelToDataSource::ModelToDataSource(QAbstractItemModel* model, bool owned)
    : QObject(), m_model(model), m_owned(owned), m_curRow(-1), m_lastError("")
{
    Q_ASSERT(model);
    if (model){
        while (model->canFetchMore(QModelIndex())){
            model->fetchMore(QModelIndex());
            if (model->rowCount() <= 0) break;
        }
        connect(model, SIGNAL(destroyed()), this, SLOT(slotModelDestroed()));
        connect(model, SIGNAL(modelReset()), this, SIGNAL(modelStateChanged()));
    }
}

ModelToDataSource::~ModelToDataSource()
{
    if ((m_owned) && m_model!=0)
        delete m_model;
}

bool ModelToDataSource::next()
{
    if (isInvalid()) return false;
    if (m_curRow<(m_model->rowCount())) {
        if (bof()) m_curRow++;
        m_curRow++;
        return true;
    } else return false;
}

bool ModelToDataSource::hasNext()
{
    if (isInvalid()) return false;
    return m_curRow<m_model->rowCount()-1;
}

bool ModelToDataSource::prior()
{
    if (isInvalid()) return false;
    if (m_curRow>-1){
        if (eof()) m_curRow--;
        m_curRow--;
        return true;
    } else return false;
}

void ModelToDataSource::first()
{
    m_curRow=0;
}

void ModelToDataSource::last()
{
    if (isInvalid()) m_curRow=0;
    else m_curRow=m_model->rowCount()-1;
}

bool ModelToDataSource::eof()
{
    if (isInvalid()) return true;
    return (m_curRow==m_model->rowCount())||(m_model->rowCount()==0);
}

bool ModelToDataSource::bof()
{
    if (isInvalid()) return true;
    return (m_curRow==-1)||(m_model->rowCount()==0);
}

QVariant ModelToDataSource::data(const QString &columnName)
{
    if (isInvalid()) return QVariant();
    return m_model->data(m_model->index(currentRow(),columnIndexByName(columnName)));
}

QVariant ModelToDataSource::dataByRowIndex(const QString &columnName, int rowIndex)
{
    if (m_model->rowCount() > rowIndex)
        return m_model->data(m_model->index(rowIndex, columnIndexByName(columnName)));
    return QVariant();
}

QVariant ModelToDataSource::dataByKeyField(const QString& columnName, const QString& keyColumnName, QVariant keyData)
{
   for( int i=0; i < m_model->rowCount(); ++i ){
      if (m_model->data(m_model->index(i, columnIndexByName(keyColumnName))) == keyData){
          return m_model->data(m_model->index(i, columnIndexByName(columnName)));
      }
   }
   return QVariant();
}

int ModelToDataSource::columnCount()
{
    if (isInvalid()) return 0;
    return m_model->columnCount();
}

QString ModelToDataSource::columnNameByIndex(int columnIndex)
{
    if (isInvalid()) return "";
    QString result = m_model->headerData(columnIndex,Qt::Horizontal, Qt::UserRole).isValid()?
                     m_model->headerData(columnIndex,Qt::Horizontal, Qt::UserRole).toString():
                     m_model->headerData(columnIndex,Qt::Horizontal).toString();
    return  result;
}

int ModelToDataSource::columnIndexByName(QString name)
{
    if (isInvalid()) return 0;
    for(int i=0;i<m_model->columnCount();i++){
        QString columnName = m_model->headerData(i,Qt::Horizontal, Qt::UserRole).isValid()?
                    m_model->headerData(i,Qt::Horizontal, Qt::UserRole).toString():
                    m_model->headerData(i,Qt::Horizontal).toString();
        if (columnName.compare(name,Qt::CaseInsensitive)==0)
            return i;
    }
    return -1;
}

QString ModelToDataSource::lastError()
{
    return m_lastError;
}

QAbstractItemModel * ModelToDataSource::model()
{
    return m_model;
}

int ModelToDataSource::currentRow()
{
    if (eof()) return m_curRow-1;
    if (bof()) return m_curRow+1;
    return m_curRow;
}

bool ModelToDataSource::isInvalid() const
{
    return m_model==0;
}

void ModelToDataSource::slotModelDestroed()
{
    m_model = 0;
    m_lastError = tr("model is destroyed");
    emit modelStateChanged();
}

ConnectionDesc::ConnectionDesc(QSqlDatabase db, QObject *parent)
    : QObject(parent), m_connectionName(db.connectionName()), m_connectionHost(db.hostName()), m_connectionDriver(db.driverName()),
      m_databaseName(db.databaseName()), m_user(db.userName()), m_password(db.password()), m_port(""), m_autoconnect(false),
      m_internal(false), m_keepDBCredentials(true)
{}

ConnectionDesc::ConnectionDesc(QObject *parent)
    :QObject(parent),m_connectionName(""),m_connectionHost(""), m_connectionDriver(""),
      m_databaseName(""), m_user(""), m_password(""), m_port(""), m_autoconnect(false),
      m_internal(false), m_keepDBCredentials(true)
{}

ConnectionDesc::Ptr ConnectionDesc::create(QSqlDatabase db, QObject *parent)
{
    return Ptr(new ConnectionDesc(db,parent));
}

void ConnectionDesc::setName(const QString &value)
{
    if (m_connectionName!=value) emit nameChanged(m_connectionName,value);
    m_connectionName=value;
}

bool ConnectionDesc::isEqual(const QSqlDatabase &db)
{
    return (db.databaseName() == m_databaseName) &&
           (db.driverName() == m_connectionDriver) &&
           (db.hostName() == m_connectionHost) &&
           (db.connectionName() == m_connectionName) &&
           (db.userName() == m_user) &&
            (db.password() == m_password);
}

QString ConnectionDesc::connectionNameForUser(const QString &connectionName)
{
    return connectionName.compare(QSqlDatabase::defaultConnection) == 0 ? tr("defaultConnection") : connectionName;
}

QString ConnectionDesc::connectionNameForReport(const QString &connectionName)
{
    return connectionName.compare(tr("defaultConnection")) == 0 ? QSqlDatabase::defaultConnection : connectionName;
}

QString ConnectionDesc::port() const
{
    return m_port;
}

void ConnectionDesc::setPort(QString port)
{
    m_port = port;
}

bool ConnectionDesc::keepDBCredentials() const
{
    return m_keepDBCredentials;
}

void ConnectionDesc::setKeepDBCredentials(bool keepDBCredentals)
{
    m_keepDBCredentials = keepDBCredentals;
}

QueryDesc::QueryDesc(QString queryName, QString queryText, QString connection)
    :m_queryName(queryName), m_queryText(queryText), m_connectionName(connection)
{}

SubQueryHolder::SubQueryHolder(QString queryText, QString connectionName, QString masterDatasource, DataSourceManager* dataManager)
    : QueryHolder(queryText, connectionName, dataManager), m_masterDatasource(masterDatasource)/*, m_invalid(false)*/
{
    extractParams();
}

void SubQueryHolder::setMasterDatasource(const QString &value)
{
    if (dataManager()->dataSource(value)){
        m_masterDatasource = value;
    }
}

void SubQueryHolder::extractParams()
{
    if (!dataManager()->containsDatasource(m_masterDatasource)){
        setLastError(QObject::tr("Master datasource \"%1\" not found!").arg(m_masterDatasource));
        setPrepared(false);
    } else {
        m_preparedSQL = replaceFields(replaceVariables(queryText()));
        setPrepared(true);
    }
}

QString SubQueryHolder::extractField(QString source)
{
    if (source.contains('.')) {
        return source.right(source.length()-(source.indexOf('.')+1));
    }
    return source;
}

QString SubQueryHolder::replaceFields(QString query)
{
    return dataManager()->replaceFields(query, m_aliasesToParam);
}

SubQueryDesc::SubQueryDesc(QString queryName, QString queryText, QString connection, QString masterDatasourceName)
    :QueryDesc(queryName,queryText,connection), m_masterDatasourceName(masterDatasourceName)
{
}

QObject *ProxyDesc::createElement(const QString &collectionName, const QString &)
{
    if (collectionName=="fields"){
        FieldMapDesc* fieldMapDesc = new FieldMapDesc;
        m_maps.append(fieldMapDesc);
        return fieldMapDesc;
    }
    return 0;
}

int ProxyDesc::elementsCount(const QString &collectionName)
{
    Q_UNUSED(collectionName)
    return m_maps.count();
}

QObject *ProxyDesc::elementAt(const QString &collectionName, int index)
{
    Q_UNUSED(collectionName)
    return m_maps.at(index);
}

ProxyHolder::ProxyHolder(ProxyDesc* desc, DataSourceManager* dataManager)
    :m_model(0), m_desc(desc), m_lastError(""), m_mode(IDataSource::RENDER_MODE),
     m_invalid(false), m_dataManager(dataManager)
{}

QString ProxyHolder::masterDatasource()
{
    if (m_desc) return m_desc->master();
    return QString();
}

void ProxyHolder::filterModel()
{
    if (!m_datasource){

        if (dataManager()){
            IDataSource* master = dataManager()->dataSource(m_desc->master());
            IDataSource* child = dataManager()->dataSource(m_desc->child());
            if (master&&child){
                m_model = new MasterDetailProxyModel(dataManager());
                connect(child->model(),SIGNAL(destroyed()), this, SLOT(slotChildModelDestoroyed()));
                m_model->setSourceModel(child->model());
                m_model->setMaster(m_desc->master());
                m_model->setChildName(m_desc->child());
                m_model->setFieldsMap(m_desc->fieldsMap());
                try{
                    m_model->rowCount();
                    m_datasource = IDataSource::Ptr(new ModelToDataSource(m_model,true));
                } catch (ReportError& exception) {
                    m_lastError = exception.what();
                }
                m_invalid = false;
                m_lastError.clear();
            } else {
                m_lastError.clear();
                if(!master) m_lastError+=QObject::tr("Master datasouce \"%1\" not found!").arg(m_desc->master());
                if(!child) m_lastError+=((m_lastError.isEmpty())?QObject::tr("Child"):QObject::tr(" and child "))+
                                          QObject::tr("datasouce \"%1\" not found!").arg(m_desc->child());
            }
        }
    } else {
        if (!isInvalid()){
            m_model->invalidate();
            m_datasource->first();
        }
    }
}

IDataSource *ProxyHolder::dataSource(IDataSource::DatasourceMode mode)
{
    if ((m_mode != mode && m_mode == IDataSource::DESIGN_MODE) || m_datasource==0) {
        m_mode = mode;
        m_datasource.clear();
        filterModel();
    }
    return m_datasource.data();
}

void ProxyHolder::invalidate(IDataSource::DatasourceMode mode, bool dbWillBeClosed)
{
    Q_UNUSED(mode)
    Q_UNUSED(dbWillBeClosed);
    if (m_model && m_model->isInvalid()){
        m_invalid = true;
        m_lastError = tr("Datasource has been invalidated");
    } else {
        filterModel();
    }
}

void ProxyHolder::slotChildModelDestoroyed(){
    m_datasource.clear();
    m_model = 0;
}

void ProxyDesc::addFieldsCorrelation(const FieldsCorrelation& fieldsCorrelation)
{
    m_maps.append(new FieldMapDesc(fieldsCorrelation));
}

void MasterDetailProxyModel::setMaster(QString name){
    m_masterName=name;
}

bool MasterDetailProxyModel::isInvalid() const
{
    if (m_masterName.isEmpty() || m_childName.isEmpty()) return true;
    IDataSource* masterData = dataManager()->dataSource(m_masterName);
    IDataSource* childData = dataManager()->dataSource(m_childName);
    if (!masterData || !childData) return true;
    return masterData->isInvalid() || childData->isInvalid();
}

bool MasterDetailProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)
    foreach (FieldMapDesc* fieldCorrelation, *m_maps) {
        QVariant master = masterData(fieldCorrelation->master());
        QVariant detail = sourceData(fieldCorrelation->detail(),source_row);
        if (master==detail)
            return true;
    }
    return false;
}

int MasterDetailProxyModel::fieldIndexByName(QString fieldName) const
{
    for(int i=0;i<sourceModel()->columnCount();++i){
        QString sourceFieldName = sourceModel()->headerData(i,Qt::Horizontal,Qt::UserRole).isValid()?
                            sourceModel()->headerData(i,Qt::Horizontal,Qt::UserRole).toString():
                            sourceModel()->headerData(i,Qt::Horizontal).toString();
        if (sourceFieldName.compare(fieldName,Qt::CaseInsensitive)==0){
           return i;
        }
    }
    return -1;
}

QVariant MasterDetailProxyModel::sourceData(QString fieldName, int row) const
{
    int fieldIndex = fieldIndexByName(fieldName);
    if (fieldIndex!=-1){
        return sourceModel()->index(row,fieldIndex).data();
    } else {
        throw ReportError(
            tr("Field: \"%1\" not found in \"%2\" child datasource").arg(fieldName).arg(m_childName)
        );
    }
}

QVariant MasterDetailProxyModel::masterData(QString fieldName) const
{
    IDataSource* master = dataManager()->dataSource(m_masterName);
    if (master){
        int columnIndex = master->columnIndexByName(fieldName);
        if (columnIndex!=-1){
            return master->data(fieldName);
        } else {
            throw ReportError(
                tr("Field: \"%1\" not found in \"%2\" master datasource").arg(fieldName).arg(m_masterName)
            );
        }
    }
    return QVariant();
}

bool CallbackDatasource::next(){
    if (!m_eof){
        bool nextRowExists = checkNextRecord(m_currentRow);
        if (m_currentRow>-1){
            if (!m_getDataFromCache && nextRowExists){
                for (int i = 0; i < m_columnCount; ++i ){
                    m_valuesCache[columnNameByIndex(i)] = data(columnNameByIndex(i));
                }

            }
        }
        if (!nextRowExists){
            m_eof = true;
            return false;
        }
        m_currentRow++;
        bool result = true;
        if (!m_getDataFromCache)
            emit changePos(CallbackInfo::Next,result);
        m_getDataFromCache = false;
        if (m_rowCount != -1){
            if (m_rowCount > 0 && m_currentRow < m_rowCount){
                m_eof = false;
            } else {
                m_eof = true;
            }
            return !m_eof;
        } else {
            m_eof = !result;
            return result;
        }
    } else return false;
}

bool CallbackDatasource::prior(){
     if (m_currentRow !=-1) {
        if (!m_getDataFromCache && !m_valuesCache.isEmpty()){
            m_getDataFromCache = true;
            m_currentRow--;
            m_eof = false;
            return true;
        } else {
            return false;
        }
     } else {
         return false;
     }
}

void CallbackDatasource::first(){
    m_currentRow = 0;
    m_getDataFromCache = false;
    m_eof=checkIfEmpty();
    bool result=false;

    QVariant rowCount;
    CallbackInfo info;
    info.dataType = CallbackInfo::RowCount;
    emit getCallbackData(info,rowCount);
    if (rowCount.isValid()) m_rowCount = rowCount.toInt();

    emit changePos(CallbackInfo::First,result);
    if (m_rowCount>0) m_eof = false;
    else m_eof = !result;
}

QVariant CallbackDatasource::callbackData(const QString& columnName, int row)
{
    CallbackInfo info;
    QVariant result;
    info.dataType = CallbackInfo::ColumnData;
    info.columnName = columnName;
    info.index = row;
    emit getCallbackData(info, result);
    return result;
}

QVariant CallbackDatasource::data(const QString& columnName)
{
    QVariant result;
    if (!bof())
    {
        if (!m_getDataFromCache){
            result = callbackData(columnName, m_currentRow);
        } else {
            result = m_valuesCache[columnName];
        }
    }
    return result;
}

QVariant CallbackDatasource::dataByRowIndex(const QString &columnName, int rowIndex)
{
    int backupCurrentRow = m_currentRow;
    QVariant result = QVariant();
    first();
    for (int i = 0; i < rowIndex && !eof(); ++i, next()){}
    if (!eof()) result = callbackData(columnName, rowIndex);
    first();
    if (backupCurrentRow != -1){
        for (int i = 0; i < backupCurrentRow; ++i)
            next();
    }
    return result;
}

QVariant CallbackDatasource::dataByKeyField(const QString& columnName, const QString& keyColumnName, QVariant keyData)
{
    int backupCurrentRow = m_currentRow;
    QVariant result = QVariant();

    m_currentRow = m_lastKeyRow;
    if (next()){
        for (int i = 0; i < 10; ++i){
            QVariant key = callbackData(keyColumnName, m_currentRow);
            if (key == keyData){
                result = callbackData(columnName, m_currentRow);
                m_lastKeyRow = m_currentRow;
                m_currentRow = backupCurrentRow;
                return result;
            }
            if (!next()) break;
        }
    }

    first();
    if (!checkIfEmpty()){
        do {
            QVariant key = callbackData(keyColumnName, m_currentRow);
            if (key == keyData){
                result = callbackData(columnName, m_currentRow);
                m_lastKeyRow = m_currentRow;
                m_currentRow = backupCurrentRow;
                return result;
            }
        } while (next());
    }

    m_currentRow = backupCurrentRow;
    return result;
}

int CallbackDatasource::columnCount(){
    CallbackInfo info;
    if (m_columnCount == -1){
        QVariant columnCount;
        info.dataType = CallbackInfo::ColumnCount;
        emit getCallbackData(info,columnCount);
        if (columnCount.isValid()){
            m_columnCount = columnCount.toInt();
        }
        if (m_columnCount != -1){
            for(int i=0;i<m_columnCount;++i) {
                QVariant columnName;
                info.dataType = CallbackInfo::ColumnHeaderData;
                info.index = i;
                emit getCallbackData(info,columnName);
                if (columnName.isValid())
                    m_headers.append(columnName.toString());
            }
        } else {
            int currIndex = 0;
            do {
                QVariant columnName;
                info.dataType = CallbackInfo::ColumnHeaderData;
                info.index = currIndex;
                emit getCallbackData(info,columnName);
                if (columnName.isValid()){
                    m_headers.append(columnName.toString());
                    currIndex++;
                } else break;
            } while (true);
        }
    }
    if (m_headers.size()>0) m_columnCount = m_headers.size();
    return m_columnCount;
}

QString CallbackDatasource::columnNameByIndex(int columnIndex)
{
    if (columnIndex < m_headers.size())
        return m_headers[columnIndex];
    else return QString();
}

int CallbackDatasource::columnIndexByName(QString name)
{
    for (int i=0;i<m_headers.size();++i) {
        if (m_headers[i].compare(name, Qt::CaseInsensitive) == 0)
            return i;
    }
//    if (m_headers.size()==0){
//        QVariant data;
//        bool hasNextRow;
//        emit getData(m_currentRow, name, data, hasNextRow);
//        if (data.isValid()) return 0;
//    }
    return -1;
}

bool CallbackDatasource::checkNextRecord(int recordNum){
    if (bof()) checkIfEmpty();
    if (m_rowCount > 0) {
        return (recordNum < (m_rowCount-1));
    } else {
        QVariant result = false;
        CallbackInfo info;
        info.dataType = CallbackInfo::HasNext;
        info.index = recordNum;
        emit getCallbackData(info,result);
        return result.toBool();
    }
}

bool CallbackDatasource::checkIfEmpty(){
    if (m_rowCount == 0) {
        return true;
    } else {
        QVariant isEmpty = true;
        QVariant recordCount = 0;
        CallbackInfo info;
        info.dataType = CallbackInfo::RowCount;
        emit getCallbackData(info, recordCount);
        if (recordCount.toInt()>0) {
            m_rowCount = recordCount.toInt();
            return false;
        }
        info.dataType = CallbackInfo::IsEmpty;
        emit getCallbackData(info,isEmpty);
        return isEmpty.toBool();
    }
}

QString CSVDesc::name() const
{
    return m_csvName;
}

void CSVDesc::setName(const QString &csvName)
{
    m_csvName = csvName;
}

QString CSVDesc::csvText() const
{
    return m_csvText;
}

void CSVDesc::setCsvText(const QString &csvText)
{
    m_csvText = csvText;
    emit cvsTextChanged(m_csvName, m_csvText);
}

QString CSVDesc::separator() const
{
    return m_separator;
}

void CSVDesc::setSeparator(const QString &separator)
{
    m_separator = separator;
}

bool CSVDesc::firstRowIsHeader() const
{
    return m_firstRowIsHeader;
}

void CSVDesc::setFirstRowIsHeader(bool firstRowIsHeader)
{
    m_firstRowIsHeader = firstRowIsHeader;
}

void CSVHolder::updateModel()
{
    m_model.clear();
    QString sep = (separator().compare("\\t") == 0) ? "\t" : separator();
    bool firstRow = true;
    QList<QStandardItem*> columns;
    QStringList headers;
    foreach(QString line, m_csvText.split('\n')){
        columns.clear();
        foreach(QString item, line.split(sep)){
            columns.append(new QStandardItem(item));
            if (firstRow && m_firstRowIsHeader) headers.append(item);
        }

        if (firstRow){
            if (!headers.isEmpty()){
                m_model.setHorizontalHeaderLabels(headers);
                firstRow = false;
            } else {
                m_model.appendRow(columns);
            }
        } else {
            m_model.appendRow(columns);
        }

    }


}

bool CSVHolder::firsRowIsHeader() const
{
    return m_firstRowIsHeader;
}

void CSVHolder::setFirsRowIsHeader(bool firstRowIsHeader)
{
    m_firstRowIsHeader = firstRowIsHeader;
}

CSVHolder::CSVHolder(const CSVDesc &desc, DataSourceManager *dataManager)
    : m_csvText(desc.csvText()),
      m_separator(desc.separator()),
      m_dataManager(dataManager),
      m_firstRowIsHeader(desc.firstRowIsHeader())
{
    m_dataSource = IDataSource::Ptr(new ModelToDataSource(&m_model, false));
    updateModel();
}

void CSVHolder::setCSVText(QString csvText)
{
    m_csvText = csvText;
    updateModel();
}

QString CSVHolder::separator() const
{
    return m_separator;
}

void CSVHolder::setSeparator(const QString &separator)
{
    m_separator = separator;
    updateModel();
}

IDataSource *CSVHolder::dataSource(IDataSource::DatasourceMode mode)
{
    Q_UNUSED(mode);
    return m_dataSource.data();
}

} //namespace LimeReport
