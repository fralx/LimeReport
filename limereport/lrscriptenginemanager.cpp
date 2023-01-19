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
#include "lrscriptenginemanager.h"

#include <QDate>
#include <QStringList>
#include <QUuid>
#ifdef USE_QTSCRIPTENGINE
#include <QScriptValueIterator>
#endif
#include <QMessageBox>
#ifdef HAVE_UI_LOADER
#include <QUiLoader>
#include <QBuffer>
#include <QWidget>
#endif
#include "lrdatasourcemanager.h"
#include "lrbasedesignintf.h"
#include "lrbanddesignintf.h"
#include "lrpageitemdesignintf.h"

Q_DECLARE_METATYPE(QColor)
Q_DECLARE_METATYPE(QFont)
Q_DECLARE_METATYPE(LimeReport::ScriptEngineManager *)

#ifdef USE_QTSCRIPTENGINE
QScriptValue constructColor(QScriptContext *context, QScriptEngine *engine)
{
    QColor color(context->argument(0).toString());
    return engine->toScriptValue(color);
}
#endif

namespace LimeReport{

ScriptEngineNode::ScriptEngineNode(const QString &name, const QString &description,
                                   ScriptEngineNode::NodeType type, ScriptEngineNode *parent, const QIcon &icon)
    :m_name(name), m_description(description), m_icon(icon), m_type(type), m_parent(parent)
{}

ScriptEngineNode::~ScriptEngineNode()
{
    qDeleteAll(m_childs.begin(), m_childs.end());
}

ScriptEngineNode*ScriptEngineNode::addChild(const QString& name, const QString& description,
                                            ScriptEngineNode::NodeType type, const QIcon& icon)
{
    ScriptEngineNode* res = new ScriptEngineNode(name, description, type,this,icon);
    m_childs.push_back(res);
    return res;
}

int ScriptEngineNode::row()
{
    if (m_parent){
        return m_parent->m_childs.indexOf(const_cast<ScriptEngineNode*>(this));
    }
    return 0;
}

void ScriptEngineNode::clear()
{
    for (int i=0; i<m_childs.count(); ++i){
        delete m_childs[i];
    }
    m_childs.clear();
}

ScriptEngineModel::ScriptEngineModel(ScriptEngineManager* scriptManager)
    :m_rootNode(new ScriptEngineNode())
{
    setScriptEngineManager(scriptManager);
}

ScriptEngineModel::~ScriptEngineModel() {
    delete m_rootNode;
}

QModelIndex ScriptEngineModel::parent(const QModelIndex& child) const
{
    if (!child.isValid()) return QModelIndex();

    ScriptEngineNode* childNode = nodeFromIndex(child);
    if (!childNode) return QModelIndex();

    ScriptEngineNode* parentNode = childNode->parent();
    if ((parentNode == m_rootNode) || (!parentNode)) return QModelIndex();
    return createIndex(parentNode->row(),0,parentNode);
}

QModelIndex ScriptEngineModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!m_rootNode)
        return QModelIndex();

    if (!hasIndex(row,column,parent))
        return QModelIndex();

    ScriptEngineNode* parentNode;
    if (parent.isValid()){
        parentNode = nodeFromIndex(parent);
    } else {
        parentNode = m_rootNode;
    }

    ScriptEngineNode* childNode = parentNode->child(row);
    if (childNode){
        return createIndex(row,column,childNode);
    } else return QModelIndex();
}

int ScriptEngineModel::rowCount(const QModelIndex& parent) const
{
    if (!m_rootNode) return 0;
    ScriptEngineNode* parentNode;
    if (parent.isValid())
        parentNode = nodeFromIndex(parent);
    else
        parentNode = m_rootNode;
    return parentNode->childCount();
}

int ScriptEngineModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant ScriptEngineModel::data(const QModelIndex& index, int role) const
{
    ScriptEngineNode *node = nodeFromIndex(index);
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

void ScriptEngineModel::setScriptEngineManager(ScriptEngineManager* scriptManager)
{
    m_scriptManager = scriptManager;
    updateModel();
}

void ScriptEngineModel::slotScriptEngineChanged()
{
    updateModel();
}

ScriptEngineNode*ScriptEngineModel::nodeFromIndex(const QModelIndex& index) const
{
    if (index.isValid()){
        return static_cast<ScriptEngineNode*>(index.internalPointer());
    } else return m_rootNode;
}

void ScriptEngineModel::updateModel()
{
    beginResetModel();
    m_rootNode->clear();
    QMap<QString,ScriptEngineNode*> categories;
    foreach(ScriptFunctionDesc funcDesc, m_scriptManager->functionsDescribers()){
        ScriptEngineNode* categ;
        QString categoryName = (!funcDesc.category.isEmpty())?funcDesc.category:"NO CATEGORY";
        if (categories.contains(categoryName)){
            categ = categories.value(categoryName);
        } else {
            categ = m_rootNode->addChild(categoryName,"",ScriptEngineNode::Category,QIcon(":/report/images/folder"));
            categories.insert(categoryName,categ);
        }
        categ->addChild(funcDesc.name,funcDesc.description,ScriptEngineNode::Function,QIcon(":/report/images/function"));
    }
    endResetModel();
}

ScriptEngineManager::~ScriptEngineManager()
{
    delete m_model;
    m_model = 0;
    delete m_scriptEngine;
}

bool ScriptEngineManager::isFunctionExists(const QString &functionName) const
{
    return m_functions.contains(functionName);
    //    foreach (ScriptFunctionDesc desc, m_functions.values()) {
    //        if (desc.name.compare(functionName,Qt::CaseInsensitive)==0){
    //            return true;
    //        }
    //    }
    //    return false;
}

void ScriptEngineManager::deleteFunction(const QString &functionsName)
{
    m_functions.remove(functionsName);
}

bool ScriptEngineManager::addFunction(const JSFunctionDesc &functionDescriber)
{
    if (m_functions.contains(functionDescriber.name())) return false;
    ScriptValueType functionManager = scriptEngine()->globalObject().property(functionDescriber.managerName());
#ifdef USE_QJSENGINE
    if (functionManager.isUndefined()){
#else
    if (!functionManager.isValid()){
#endif
        functionManager = scriptEngine()->newQObject(functionDescriber.manager());
        scriptEngine()->globalObject().setProperty(
                    functionDescriber.managerName(),
                    functionManager
                    );
    }

    if (functionManager.toQObject() == functionDescriber.manager()){
        ScriptValueType checkWrapper = scriptEngine()->evaluate(functionDescriber.scriptWrapper());
        if (!checkWrapper.isError()){
            ScriptFunctionDesc funct;
            funct.name = functionDescriber.name();
            funct.description = functionDescriber.description();
            funct.category = functionDescriber.category();
            funct.type = ScriptFunctionDesc::Native;
            m_functions.insert(funct.name, funct);
            if (m_model)
                m_model->updateModel();
            return true;
        } else {
            m_lastError = checkWrapper.toString();
            return false;
        }
    } else {
        m_lastError = tr("Function manager with name \"%1\" already exists!");
        return false;
    }

}

#ifdef USE_QTSCRIPTENGINE
#if QT_VERSION > 0x050600
Q_DECL_DEPRECATED
#endif
bool ScriptEngineManager::addFunction(const QString& name,
                                      QScriptEngine::FunctionSignature function,
                                      const QString& category,
                                      const QString& description)
{
    if (!isFunctionExists(name)){
        ScriptFunctionDesc funct;
        funct.name = name;
        funct.description = description;
        funct.category = category;
        funct.scriptValue = scriptEngine()->newFunction(function);
        funct.scriptValue.setProperty("functionName", name);
        funct.scriptValue.setData(m_scriptEngine->toScriptValue(this));
        funct.type = ScriptFunctionDesc::Native;
        m_functions.insert(name, funct);
        if (m_model)
            m_model->updateModel();
        m_scriptEngine->globalObject().setProperty(funct.name, funct.scriptValue);
        return true;
    } else {
        return false;
    }
}
#endif

bool ScriptEngineManager::addFunction(const QString& name, const QString& script, const QString& category, const QString& description)
{
    ScriptValueType functionValue = m_scriptEngine->evaluate(script);
    if (!functionValue.isError()){
        ScriptFunctionDesc funct;
        funct.scriptValue = functionValue;
        funct.name =  name;
        funct.category = category;
        funct.description = description;
        funct.type = ScriptFunctionDesc::Script;
        m_functions.insert(name, funct);
        m_model->updateModel();
        return true;
    } else {
        m_lastError = functionValue.toString();
        return false;
    }
}

QStringList ScriptEngineManager::functionsNames()
{
    return m_functions.keys();
    //    QStringList res;
    //    foreach(ScriptFunctionDesc func, m_functions){
    //        res<<func.name;
    //    }
    //    return res;
}

void ScriptEngineManager::setDataManager(DataSourceManager *dataManager){
    if (dataManager && m_dataManager != dataManager){
        m_dataManager = dataManager;
        if (m_dataManager){
            foreach(QString func, m_dataManager->groupFunctionNames()){
                JSFunctionDesc describer(
                            func,
                            tr("GROUP FUNCTIONS"),
                            func+"(\""+tr("FieldName")+"\",\""+tr("BandName")+"\")",
                            LimeReport::Const::FUNCTION_MANAGER_NAME,
                            m_functionManager,
                            QString("function %1(fieldName, bandName, pageitem){\
                                    if (typeof pageitem == 'undefined') return %2.calcGroupFunction(\"%1\", fieldName, bandName); \
                                                                                                     else return %2.calcGroupFunction(\"%1\", fieldName, bandName, pageitem);}"
                                                                                                                                       ).arg(func)
                                                                                                     .arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                                                                                                     );
                            addFunction(describer);
            }
            moveQObjectToScript(new DatasourceFunctions(dataManager), LimeReport::Const::DATAFUNCTIONS_MANAGER_NAME);
        }
    }
}

QString ScriptEngineManager::expandUserVariables(QString context, RenderPass /* pass */, ExpandType expandType, QVariant &varValue)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    QRegExp rx(Const::VARIABLE_RX);
    if (context.contains(rx)){
        int pos = 0;
        while ((pos = rx.indexIn(context,pos))!=-1){
            QString variable=rx.cap(1);
            pos += rx.matchedLength();
            if (dataManager()->containsVariable(variable) ){
                try {

                    varValue = dataManager()->variable(variable);
                    switch (expandType){
                    case EscapeSymbols:
                        context.replace(rx.cap(0),escapeSimbols(varValue.toString()));
                        break;
                    case NoEscapeSymbols:
                        context.replace(rx.cap(0),varValue.toString());
                        break;
                    case ReplaceHTMLSymbols:
                        context.replace(rx.cap(0),replaceHTMLSymbols(varValue.toString()));
                        break;
                    }
                    pos=0;

                } catch (ReportError &e){
                    dataManager()->putError(e.what());
                    if (!dataManager()->reportSettings() || dataManager()->reportSettings()->suppressAbsentFieldsAndVarsWarnings())
                        context.replace(rx.cap(0),e.what());
                    else
                        context.replace(rx.cap(0),"");
                }
            } else {
                QString error;
                error = tr("Variable %1 not found").arg(variable);
                dataManager()->putError(error);
                if (!dataManager()->reportSettings() || dataManager()->reportSettings()->suppressAbsentFieldsAndVarsWarnings())
                    context.replace(rx.cap(0),error);
                else
                    context.replace(rx.cap(0),"");
            }
        }
    }
    return context;
#else
    QRegularExpression rx = getVariableRegEx();
    if (context.contains(rx)){
        int pos = 0;
        QRegularExpressionMatch match = rx.match(context, pos);
        while (match.hasMatch()){

            QString variable=match.captured(1);
            pos = match.capturedEnd();

            if (dataManager()->containsVariable(variable) ){
                try {

                    varValue = dataManager()->variable(variable);
                    switch (expandType){
                    case EscapeSymbols:
                        context.replace(match.captured(0), escapeSimbols(varValue.toString()));
                        break;
                    case NoEscapeSymbols:
                        context.replace(match.captured(0), varValue.toString());
                        break;
                    case ReplaceHTMLSymbols:
                        context.replace(match.captured(0), replaceHTMLSymbols(varValue.toString()));
                        break;
                    }

                    pos = 0;

                } catch (ReportError &e){
                    dataManager()->putError(e.what());
                    if (!dataManager()->reportSettings() || dataManager()->reportSettings()->suppressAbsentFieldsAndVarsWarnings())
                        context.replace(match.captured(0), e.what());
                    else
                        context.replace(match.captured(0), "");
                }
            } else {

                QString error;
                error = tr("Variable %1 not found").arg(variable);
                dataManager()->putError(error);
                if (!dataManager()->reportSettings() || dataManager()->reportSettings()->suppressAbsentFieldsAndVarsWarnings())
                    context.replace(match.captured(0), error);
                else
                    context.replace(match.captured(0), "");
            }

            match = rx.match(context, pos);
        }
    }
    return context;
#endif
}

QString ScriptEngineManager::expandDataFields(QString context, ExpandType expandType, QVariant &varValue, QObject *reportItem)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    QRegExp rx(Const::FIELD_RX);

    if (context.contains(rx)){
        while ((rx.indexIn(context))!=-1){
            QString field=rx.cap(1);

            if (dataManager()->containsField(field)) {
                QString fieldValue;
                varValue = dataManager()->fieldData(field);
                if (expandType == EscapeSymbols) {
                    if (varValue.isNull()) {
                        fieldValue="\"\"";
                    } else {
                        fieldValue = escapeSimbols(varValue.toString());
                        switch (dataManager()->fieldData(field).type()) {
                        case QVariant::Char:
                        case QVariant::String:
                        case QVariant::StringList:
                        case QVariant::Date:
                        case QVariant::DateTime:
                            fieldValue = "\""+fieldValue+"\"";
                            break;
                        default:
                            break;
                        }
                    }
                } else {
                    if (expandType == ReplaceHTMLSymbols)
                        fieldValue = replaceHTMLSymbols(varValue.toString());
                    else fieldValue = varValue.toString();
                }

                context.replace(rx.cap(0),fieldValue);

            } else {
                QString error;
                if (reportItem){
                    error = tr("Field %1 not found in %2!").arg(field).arg(reportItem->objectName());
                    dataManager()->putError(error);
                }
                varValue = QVariant();
                if (!dataManager()->reportSettings() || !dataManager()->reportSettings()->suppressAbsentFieldsAndVarsWarnings())
                    context.replace(rx.cap(0),error);
                else
                    context.replace(rx.cap(0),"");
            }
        }
    }

    return context;
#else
    QRegularExpression rx = getFieldRegEx();
    if (context.contains(rx)){
        QRegularExpressionMatch match = rx.match(context);
        while (match.hasMatch()){

            QString field=match.captured(1);

            if (dataManager()->containsField(field)) {
                QString fieldValue;
                varValue = dataManager()->fieldData(field);
                if (expandType == EscapeSymbols) {
                    if (varValue.isNull()) {
                        fieldValue="\"\"";
                    } else {
                        fieldValue = escapeSimbols(varValue.toString());
                        //TODO: Migrate to QMetaType
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                        switch (dataManager()->fieldData(field).typeId()) {
                            case QMetaType::QChar:
                            case QMetaType::QString:
                            case QMetaType::QStringList:
                            case QMetaType::QDate:
                            case QMetaType::QDateTime:
                                fieldValue = "\""+fieldValue+"\"";
                                break;
                            default:
                                break;
                        }
#else
                        switch (dataManager()->fieldData(field).type()) {
                            case QVariant::Char:
                            case QVariant::String:
                            case QVariant::StringList:
                            case QVariant::Date:
                            case QVariant::DateTime:
                                fieldValue = "\""+fieldValue+"\"";
                                break;
                            default:
                                break;
                        }
#endif
                    }
                } else {
                    if (expandType == ReplaceHTMLSymbols)
                        fieldValue = replaceHTMLSymbols(varValue.toString());
                    else fieldValue = varValue.toString();
                }

                context.replace(match.captured(0),fieldValue);

            } else {
                QString error;
                if (reportItem){
                    error = tr("Field %1 not found in %2!").arg(field).arg(reportItem->objectName());
                    dataManager()->putError(error);
                }
                varValue = QVariant();
                if (!dataManager()->reportSettings() || !dataManager()->reportSettings()->suppressAbsentFieldsAndVarsWarnings())
                    context.replace(match.captured(0), error);
                else
                    context.replace(match.captured(0), "");
            }
            match = rx.match(context);
        }
    }

    return context;
#endif
}

QString ScriptEngineManager::expandScripts(QString context, QVariant& varValue, QObject *reportItem)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    QRegExp rx(Const::SCRIPT_RX);

    if (context.contains(rx)){
#else
    QRegularExpression rx = getScriptRegEx();
    if(context.contains(rx)){
#endif

        if (ScriptEngineManager::instance().dataManager() != dataManager())
            ScriptEngineManager::instance().setDataManager(dataManager());

        ScriptEngineType* se = ScriptEngineManager::instance().scriptEngine();

        if (reportItem){
            ScriptValueType svThis;
#ifdef USE_QJSENGINE
            svThis = getJSValue(*se, reportItem);
            se->globalObject().setProperty("THIS",svThis);
#else
            svThis = se->globalObject().property("THIS");
            if (svThis.isValid()){
                se->newQObject(svThis, reportItem);
            } else {
                svThis = se->newQObject(reportItem);
                se->globalObject().setProperty("THIS",svThis);
            }
#endif
        }

        ScriptExtractor scriptExtractor(context);
        if (scriptExtractor.parse()){
            context = replaceScripts(context, varValue, reportItem, se, scriptExtractor.scriptTree());
        }

    }
    return context;
}

QString ScriptEngineManager::replaceScripts(QString context, QVariant &varValue, QObject *reportItem, ScriptEngineType* se, ScriptNode::Ptr scriptTree)
{
    foreach(ScriptNode::Ptr item, scriptTree->children()){
        QString scriptBody = expandDataFields(item->body(), EscapeSymbols, varValue, reportItem);
        if (item->children().size() > 0)
            scriptBody = replaceScripts(scriptBody, varValue, reportItem, se, item);
        scriptBody = expandUserVariables(scriptBody, FirstPass, EscapeSymbols, varValue);
        ScriptValueType value = se->evaluate(scriptBody);
#ifdef USE_QJSENGINE
        if (!value.isError()){
            varValue = value.toVariant();
            context.replace(item->script(), value.toString());
        } else {
            context.replace(item->script(), value.toString());
        }
#else
        if (!se->hasUncaughtException()) {
            varValue = value.toVariant();
            context.replace(item->script(), value.toString());
        } else {
            context.replace(item->script(), se->uncaughtException().toString());
        }
#endif
    }
    return context;
}

QVariant ScriptEngineManager::evaluateScript(const QString& script){

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    QRegExp rx(Const::SCRIPT_RX);
    QVariant varValue;

    if (script.contains(rx)){
#else    
    QRegularExpression rx = getScriptRegEx();
    QVariant varValue;

    if (script.contains(rx)){
#endif

        if (ScriptEngineManager::instance().dataManager()!=dataManager())
            ScriptEngineManager::instance().setDataManager(dataManager());

        ScriptEngineType* se = ScriptEngineManager::instance().scriptEngine();

        ScriptExtractor scriptExtractor(script);
        if (scriptExtractor.parse()){
            QString scriptBody = expandDataFields(scriptExtractor.scriptTree()->body(), EscapeSymbols, varValue, 0);
            scriptBody = expandUserVariables(scriptBody, FirstPass, EscapeSymbols, varValue);
            ScriptValueType value = se->evaluate(scriptBody);
#ifdef USE_QJSENGINE
            if (!value.isError()){
#else
            if (!se->hasUncaughtException()) {
#endif
                return value.toVariant();
            }
        }
    }
    return QVariant();
}

void ScriptEngineManager::addBookMark(const QString& uniqKey, const QString& content){
    Q_ASSERT(m_context != 0);
    if (m_context){
        BandDesignIntf* currentBand = m_context->currentBand();
        if (currentBand)
            currentBand->addBookmark(uniqKey, content);
        else if (m_context->currentPage()) {
            m_context->currentPage()->addBookmark(uniqKey, content);
        }
    }
}

int ScriptEngineManager::findPageIndexByBookmark(const QString &uniqKey)
{
    for (int i=0; i < m_context->reportPages()->size(); ++i){
        if (m_context->reportPages()->at(i)->bookmarks().contains(uniqKey))
            return i+1;
        foreach(BandDesignIntf* band, m_context->reportPages()->at(i)->bands()){
            if (band->bookmarks().contains(uniqKey))
                return i+1;
        }
    }
    return -1;
}

int ScriptEngineManager::getPageFreeSpace(PageItemDesignIntf* page){
    if (page){
        int height = 0;
        foreach(BandDesignIntf* band, page->bands()){
            if(band->type() == BandDesignIntf::Data)
            {
                height += band->geometry().height() * m_dataManager->dataSource(band->datasourceName())->model()->rowCount();
            }
            else height += band->height();
        }
        return page->height() - height - (page->pageFooter()?page->pageFooter()->height() : 0);
    } else return -1;
}

void ScriptEngineManager::addTableOfContentsItem(const QString& uniqKey, const QString& content, int indent)
{
    Q_ASSERT(m_context != 0);
    if (m_context){
        m_context->tableOfContents()->setItem(uniqKey, content, 0, indent);
        addBookMark(uniqKey, content);
    }
}

void ScriptEngineManager::clearTableOfContents(){
    if (m_context) {
        if (m_context->tableOfContents())
            m_context->tableOfContents()->clear();
    }
}

ScriptValueType ScriptEngineManager::moveQObjectToScript(QObject* object, const QString objectName)
{
    ScriptValueType obj = scriptEngine()->globalObject().property(objectName);
    if (!obj.isNull()) delete obj.toQObject();
    ScriptValueType result = scriptEngine()->newQObject(object);
    scriptEngine()->globalObject().setProperty(objectName, result);
    return result;
}

void ScriptEngineManager::updateModel()
{

}

bool ScriptEngineManager::createLineFunction()
{
    JSFunctionDesc fd;

    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("SYSTEM"));
    fd.setName("line");
    fd.setDescription("line(\""+tr("BandName")+"\")");
    fd.setScriptWrapper(QString("function line(bandName){ return %1.line(bandName);}").arg(LimeReport::Const::FUNCTION_MANAGER_NAME));

    return addFunction(fd);

}

bool ScriptEngineManager::createNumberFomatFunction()
{
    JSFunctionDesc fd;

    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("NUMBER"));
    fd.setName("numberFormat");
    fd.setDescription("numberFormat(\""+tr("Value")+"\",\""+tr("Format")+"\",\""+
                      tr("Precision")+"\",\""+
                      tr("Locale")+"\")"
                      );
    fd.setScriptWrapper(QString("function numberFormat(value, format, precision, locale){"
                                " if(typeof(format)==='undefined') format = \"f\"; "
                                " if(typeof(precision)==='undefined') precision=2; "
                                " if(typeof(locale)==='undefined') locale=\"\"; "
                                "return %1.numberFormat(value,format,precision,locale);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createDateFormatFunction(){
    JSFunctionDesc fd;

    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("DATE&TIME"));
    fd.setName("dateFormat");
    fd.setDescription("dateFormat(\""+tr("Value")+"\",\""+tr("Format")+"\", \""+tr("Locale")+"\")");
    fd.setScriptWrapper(QString("function dateFormat(value, format, locale){"
                                " if(typeof(format)==='undefined') format = \"dd.MM.yyyy\"; "
                                "return %1.dateFormat(value,format, locale);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createTimeFormatFunction(){
    JSFunctionDesc fd;

    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("DATE&TIME"));
    fd.setName("timeFormat");
    fd.setDescription("timeFormat(\""+tr("Value")+"\",\""+tr("Format")+"\")");
    fd.setScriptWrapper(QString("function timeFormat(value, format){"
                                " if(typeof(format)==='undefined') format = \"hh:mm\"; "
                                "return %1.timeFormat(value,format);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createDateTimeFormatFunction(){
    JSFunctionDesc fd;

    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("DATE&TIME"));
    fd.setName("dateTimeFormat");
    fd.setDescription("dateTimeFormat(\""+tr("Value")+"\",\""+tr("Format")+"\", \""+tr("Locale")+"\")");
    fd.setScriptWrapper(QString("function dateTimeFormat(value, format, locale){"
                                " if(typeof(format)==='undefined') format = \"dd.MM.yyyy hh:mm\"; "
                                "return %1.dateTimeFormat(value, format, locale);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createSectotimeFormatFunction()
{
    JSFunctionDesc fd;

    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("DATE&TIME"));
    fd.setName("sectotimeFormat");
    fd.setDescription("sectotimeFormat(\""+tr("Value")+"\",\""+tr("Format")+"\")");
    fd.setScriptWrapper(QString("function sectotimeFormat(value, format){"
                                " if(typeof(format)==='undefined') format = \"hh:mm:ss\"; "
                                "return %1.sectotimeFormat(value,format);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createDateFunction(){
    //    addFunction("date",date,"DATE&TIME","date()");
    JSFunctionDesc fd;

    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("DATE&TIME"));
    fd.setName("date");
    fd.setDescription("date()");
    fd.setScriptWrapper(QString("function date(){"
                                "return %1.date();}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}


bool ScriptEngineManager::createNowFunction(){
    //    addFunction("now",now,"DATE&TIME","now()");
    JSFunctionDesc fd;

    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("DATE&TIME"));
    fd.setName("now");
    fd.setDescription("now()");
    fd.setScriptWrapper(QString("function now(){"
                                "return %1.now();}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createCurrencyFormatFunction(){
    //    addFunction("currencyFormat",currencyFormat,"NUMBER","currencyFormat(\""+tr("Value")+"\",\""+tr("Locale")+"\")");
    JSFunctionDesc fd;

    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("NUMBER"));
    fd.setName("currencyFormat");
    fd.setDescription("currencyFormat(\""+tr("Value")+"\",\""+tr("Locale")+"\")");
    fd.setScriptWrapper(QString("function currencyFormat(value, locale){"
                                " if(typeof(locale)==='undefined') locale = \"\"; "
                                "return %1.currencyFormat(value,locale);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createCurrencyUSBasedFormatFunction(){
    //    addFunction("currencyUSBasedFormat",currencyUSBasedFormat,"NUMBER","currencyUSBasedFormat(\""+tr("Value")+",\""+tr("CurrencySymbol")+"\")");
    JSFunctionDesc fd;

    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("NUMBER"));
    fd.setName("currencyUSBasedFormat");
    fd.setDescription("currencyUSBasedFormat(\""+tr("Value")+",\""+tr("CurrencySymbol")+"\")");
    fd.setScriptWrapper(QString("function currencyUSBasedFormat(value, currencySymbol){"
                                " if(typeof(currencySymbol)==='undefined') currencySymbol = \"\"; "
                                "return %1.currencyUSBasedFormat(value,currencySymbol);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createSetVariableFunction(){
    //    addFunction("setVariable", setVariable, "GENERAL", "setVariable(\""+tr("Name")+"\",\""+tr("Value")+"\")");
    JSFunctionDesc fd;

    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("GENERAL"));
    fd.setName("setVariable");
    fd.setDescription("setVariable(\""+tr("Name")+"\",\""+tr("Value")+"\")");
    fd.setScriptWrapper(QString("function setVariable(name, value){"
                                "return %1.setVariable(name,value);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createGetVariableFunction()
{
    JSFunctionDesc fd;
    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("GENERAL"));
    fd.setName("getVariable");
    fd.setDescription("getVariable(\""+tr("Name")+"\")");
    fd.setScriptWrapper(QString("function getVariable(name){"
                                "return %1.getVariable(name);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createGetFieldFunction()
{
    JSFunctionDesc fd;
    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("GENERAL"));
    fd.setName("getField");
    fd.setDescription("getField(\""+tr("Name")+"\")");
    fd.setScriptWrapper(QString("function getField(name){"
                                "return %1.getField(name);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createGetFieldByKeyFunction()
{
    JSFunctionDesc fd;
    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("GENERAL"));
    fd.setName("getFieldByKeyField");
    fd.setDescription("getFieldByKeyField(\""+tr("Datasource")+"\", \""+
                      tr("ValueField")+"\",\""+
                      tr("KeyField")+"\", \""+
                      tr("KeyFieldValue")+"\")"
                      );
    fd.setScriptWrapper(QString("function getFieldByKeyField(datasource, valueFieldName, keyFieldName, keyValue){"
                                "return %1.getFieldByKeyField(datasource, valueFieldName, keyFieldName, keyValue);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createGetFieldByRowIndex()
{
    JSFunctionDesc fd;
    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("GENERAL"));
    fd.setName("getFieldByRowIndex");
    fd.setDescription("getFieldByRowIndex(\""+tr("FieldName")+"\", \""+
                      tr("RowIndex")+"\")"
                      );
    fd.setScriptWrapper(QString("function getFieldByRowIndex(fieldName, rowIndex){"
                                "return %1.getFieldByRowIndex(fieldName, rowIndex);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createAddBookmarkFunction()
{
    JSFunctionDesc fd;
    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("GENERAL"));
    fd.setName("addBookmark");
    fd.setDescription("addBookmark(\""+tr("Unique identifier")+" \""+tr("Content")+"\")");
    fd.setScriptWrapper(QString("function addBookmark(uniqKey, content){"
                                "return %1.addBookmark(uniqKey, content);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createFindPageIndexByBookmark()
{
    JSFunctionDesc fd;
    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("GENERAL"));
    fd.setName("findPageIndexByBookmark");
    fd.setDescription("findPageIndexByBookmark(\""+tr("Unique identifier")+"\")");
    fd.setScriptWrapper(QString("function findPageIndexByBookmark(uniqKey){"
                                "return %1.findPageIndexByBookmark(uniqKey);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createAddTableOfContentsItemFunction()
{
    JSFunctionDesc fd;
    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("GENERAL"));
    fd.setName("addTableOfContentsItem");
    fd.setDescription("addTableOfContentsItem(\""+tr("Unique identifier")+" \""+tr("Content")+"\", \""+tr("Indent")+"\")");
    fd.setScriptWrapper(QString("function addTableOfContentsItem(uniqKey, content, indent){"
                                "return %1.addTableOfContentsItem(uniqKey, content, indent);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createClearTableOfContentsFunction()
{
    JSFunctionDesc fd;
    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("GENERAL"));
    fd.setName("clearTableOfContents");
    fd.setDescription("clearTableOfContents()");
    fd.setScriptWrapper(QString("function clearTableOfContents(){"
                                "return %1.clearTableOfContents();}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

bool ScriptEngineManager::createReopenDatasourceFunction()
{
    JSFunctionDesc fd;
    fd.setManager(m_functionManager);
    fd.setManagerName(LimeReport::Const::FUNCTION_MANAGER_NAME);
    fd.setCategory(tr("GENERAL"));
    fd.setName("reopenDatasource");
    fd.setDescription("reopenDatasource(\""+tr("datasourceName")+"\")");
    fd.setScriptWrapper(QString("function reopenDatasource(datasourceName){"
                                "return %1.reopenDatasource(datasourceName);}"
                                ).arg(LimeReport::Const::FUNCTION_MANAGER_NAME)
                        );
    return addFunction(fd);
}

ScriptEngineManager::ScriptEngineManager()
    :m_model(0), m_context(0), m_dataManager(0)
{
    m_scriptEngine = new ScriptEngineType;
    m_functionManager = new ScriptFunctionsManager(this);
    m_functionManager->setScriptEngineManager(this);
#ifdef USE_QTSCRIPTENGINE
    m_scriptEngine->setDefaultPrototype(qMetaTypeId<QComboBox*>(),
                                        m_scriptEngine->newQObject(new ComboBoxPrototype()));
#endif
    createLineFunction();
    createNumberFomatFunction();
    createDateFormatFunction();
    createTimeFormatFunction();
    createDateTimeFormatFunction();
    createSectotimeFormatFunction();
    createDateFunction();
    createNowFunction();
#if QT_VERSION>0x040800
    createCurrencyFormatFunction();
    createCurrencyUSBasedFormatFunction();
#endif
    createSetVariableFunction();
    createGetFieldFunction();
    createGetFieldByRowIndex();
    createGetFieldByKeyFunction();
    createGetVariableFunction();
#ifdef USE_QTSCRIPTENGINE
    QScriptValue colorCtor = m_scriptEngine->newFunction(constructColor);
    m_scriptEngine->globalObject().setProperty("QColor", colorCtor);

    QScriptValue fontProto(m_scriptEngine->newQObject(new QFontPrototype,QScriptEngine::ScriptOwnership));
    m_scriptEngine->setDefaultPrototype(qMetaTypeId<QFont>(), fontProto);
    QScriptValue fontConstructor = m_scriptEngine->newFunction(QFontPrototype::constructorQFont, fontProto);
    m_scriptEngine->globalObject().setProperty("QFont", fontConstructor);
#endif
    createAddBookmarkFunction();
    createFindPageIndexByBookmark();
    createAddTableOfContentsItemFunction();
    createClearTableOfContentsFunction();
    createReopenDatasourceFunction();

    m_model = new ScriptEngineModel(this);
}

bool ScriptExtractor::parse()
{
    int currentPos = 0;
    parse(currentPos, None, m_scriptTree);
    return m_scriptTree->children().count() > 0;
}

bool ScriptExtractor::parse(int &curPos, const State& state, ScriptNode::Ptr scriptNode)
{
    while (curPos < m_context.length()){
        switch (state) {
        case OpenBracketFound:
            if (m_context[curPos]=='}'){
                return true;
            } else {
                if (m_context[curPos]=='{')
                    extractBracket(curPos, scriptNode);
            }
        case None:
            if (m_context[curPos]=='$'){
                int startPos = curPos;
                if (isStartScriptLexem(curPos))
                    extractScript(curPos, substring(m_context,startPos,curPos), scriptNode->createChildNode());
                if (isStartFieldLexem(curPos) || isStartVariableLexem(curPos))
                    skipField(curPos);
            }
        default:
            break;
        }
        curPos++;
    }
    return false;
}

void ScriptExtractor::extractScript(int &curPos, const QString& startStr, ScriptNode::Ptr scriptNode)
{
    int startPos = curPos;
    if (extractBracket(curPos, scriptNode)){
        QString scriptBody = substring(m_context,startPos+1,curPos);
        scriptNode->setBody(scriptBody);
        scriptNode->setStartLex(startStr+'{');
    }
}

void ScriptExtractor::skipField(int &curPos){
    while (curPos<m_context.length()) {
        if (m_context[curPos]=='}'){
            return;
        } else {
            curPos++;
        }
    }
}

bool ScriptExtractor::extractBracket(int &curPos, ScriptNode::Ptr scriptNode)
{
    curPos++;
    return parse(curPos,OpenBracketFound, scriptNode);
}

bool ScriptExtractor::isStartLexem(int& curPos, QChar value){
    int pos = curPos+1;
    State ls = BuksFound;
    while (pos<m_context.length()){
        switch (ls){
        case BuksFound:
            if (m_context[pos]==value){
                ls = SignFound;
            } else {
                if (m_context[pos]!=' ')
                    return false;
            }
            break;
        case SignFound:
            if (m_context[pos]=='{'){
                curPos=pos;
                return true;
            } else
                if (m_context[pos]!=' ')
                    return false;
        default:
            break;
        }
        pos++;
    }
    return false;
}

bool ScriptExtractor::isStartScriptLexem(int& curPos)
{
    return isStartLexem(curPos, Const::SCRIPT_SIGN);
}

bool ScriptExtractor::isStartFieldLexem(int& curPos){
    return isStartLexem(curPos, Const::FIELD_SIGN);
}

bool ScriptExtractor::isStartVariableLexem(int &curPos)
{
    return isStartLexem(curPos, Const::VARIABLE_SIGN);
}


QString ScriptExtractor::substring(const QString &value, int start, int end)
{
    return value.mid(start,end-start);
}

QString DialogDescriber::name() const
{
    return m_name;
}

void DialogDescriber::setName(const QString& name)
{
    m_name = name;
}

QByteArray DialogDescriber::description() const
{
    return m_description;
}

void DialogDescriber::setDescription(const QByteArray &description)
{
    m_description = description;
}

#ifdef HAVE_UI_LOADER
void ScriptEngineContext::addDialog(const QString& name, const QByteArray& description)
{
    m_dialogs.push_back(DialogDescriber::create(name,description));
    emit dialogAdded(name);
}

bool ScriptEngineContext::changeDialog(const QString& name, const QByteArray& description)
{
    foreach( DialogDescriber::Ptr describer, m_dialogs){
        if (describer->name().compare(name) == 0){
            describer->setDescription(description);
            {
                QList<DialogPtr>::Iterator it = m_createdDialogs.begin();
                while(it!=m_createdDialogs.end()){
                    if ((*it)->objectName()==name){
                        it = m_createdDialogs.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
            return true;
        }
    }
    return false;
}

bool ScriptEngineContext::changeDialogName(const QString& oldName, const QString& newName)
{
    foreach( DialogDescriber::Ptr describer, m_dialogs){
        if (describer->name().compare(oldName) == 0){
            describer->setName(newName);
            {
                QList<DialogPtr>::Iterator it = m_createdDialogs.begin();
                while(it!=m_createdDialogs.end()){
                    if ((*it)->objectName()==oldName){
                        it = m_createdDialogs.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
            return true;
        }
    }
    return false;
}

bool ScriptEngineContext::previewDialog(const QString& dialogName)
{
    QDialog* dialog = getDialog(dialogName);
    if (dialog) {
        dialog->exec();
        return true;
    } else {
        m_lastError = tr("Dialog with name: %1 can`t be created").arg(dialogName);
        return false;
    }
}

bool ScriptEngineContext::containsDialog(const QString& dialogName)
{
    foreach(DialogDescriber::Ptr dialog, m_dialogs){
        if (dialog->name()==dialogName)
            return true;
    }
    return false;
}

void ScriptEngineContext::deleteDialog(const QString& dialogName)
{
    {
        QVector<DialogDescriber::Ptr>::Iterator it = m_dialogs.begin();
        while(it!=m_dialogs.end()){
            if ((*it)->name()==dialogName){
                it = m_dialogs.erase(it);
                emit dialogDeleted(dialogName);
            } else {
                ++it;
            }
        }
    }
    {
        QList<DialogPtr>::Iterator it = m_createdDialogs.begin();
        while(it!=m_createdDialogs.end()){
            if ((*it)->objectName()==dialogName){
                it = m_createdDialogs.erase(it);
            } else {
                ++it;
            }
        }
    }
}

#endif

void ScriptEngineContext::clear()
{
#ifdef HAVE_UI_LOADER
    m_dialogs.clear();
    m_createdDialogs.clear();
#endif
    m_initScript.clear();
    m_tableOfContents->clear();
    m_lastError="";
}

QObject* ScriptEngineContext::createElement(const QString& collectionName, const QString& elementType)
{
    Q_UNUSED(elementType)
#ifdef HAVE_UI_LOADER
    if (collectionName.compare("dialogs",Qt::CaseInsensitive)==0){
        m_dialogs.push_back(DialogDescriber::create());
        return m_dialogs.at(m_dialogs.count()-1).data();
    }
#else
    Q_UNUSED(collectionName)
#endif
    return 0;
}

int ScriptEngineContext::elementsCount(const QString& collectionName)
{
#ifdef HAVE_UI_LOADER
    if (collectionName.compare("dialogs",Qt::CaseInsensitive)==0){
        return m_dialogs.count();
    };
#else
    Q_UNUSED(collectionName)
#endif
    return 0;
}

QObject* ScriptEngineContext::elementAt(const QString& collectionName, int index)
{
#ifdef HAVE_UI_LOADER
    if (collectionName.compare("dialogs",Qt::CaseInsensitive)==0){
        return m_dialogs.at(index).data();
    };
#else
    Q_UNUSED(collectionName)
    Q_UNUSED(index)
#endif
    return 0;
}

void ScriptEngineContext::collectionLoadFinished(const QString& collectionName)
{
    Q_UNUSED(collectionName);
}

ReportPages* ScriptEngineContext::reportPages() const
{
    return m_reportPages;
}

void ScriptEngineContext::setReportPages(ReportPages *value)
{
    m_reportPages = value;
}

#ifdef HAVE_UI_LOADER
QDialog* ScriptEngineContext::createDialog(DialogDescriber* cont)
{
    QUiLoader loader;
    QByteArray desc = cont->description();
    QBuffer buffer(&desc);
    buffer.open(QIODevice::ReadOnly);
    QDialog* dialog = dynamic_cast<QDialog*>(loader.load(&buffer));
    m_createdDialogs.push_back(QSharedPointer<QDialog>(dialog));
    if (cont->name().compare(dialog->objectName())){
        cont->setName(dialog->objectName());
        emit dialogNameChanged(dialog->objectName());
    }
    return dialog;
}

QDialog* ScriptEngineContext::findDialog(const QString& dialogName)
{
    foreach(DialogPtr dialog, m_createdDialogs){
        if (dialog->objectName()==dialogName)
            return dialog.data();
    }
    return 0;
}

DialogDescriber* ScriptEngineContext::findDialogContainer(const QString& dialogName)
{
    foreach (DialogDescriber::Ptr dialogCont , m_dialogs) {
        if (dialogCont->name().compare(dialogName,Qt::CaseInsensitive)==0){
            return dialogCont.data();
        }
    }
    return 0;
}

#endif

TableOfContents* ScriptEngineContext::tableOfContents() const
{
    return m_tableOfContents;
}

void ScriptEngineContext::setTableOfContents(TableOfContents* tableOfContents)
{
    m_tableOfContents = tableOfContents;
}

PageItemDesignIntf* ScriptEngineContext::currentPage() const
{
    return m_currentPage;
}

void ScriptEngineContext::setCurrentPage(PageItemDesignIntf* currentPage)
{
    m_currentPage = currentPage;
    m_currentBand = 0;
}

BandDesignIntf* ScriptEngineContext::currentBand() const
{
    return m_currentBand;
}

void ScriptEngineContext::setCurrentBand(BandDesignIntf* currentBand)
{
    m_currentBand = currentBand;
}

#ifdef HAVE_UI_LOADER

QDialog* ScriptEngineContext::getDialog(const QString& dialogName)
{
    QDialog* dialog = findDialog(dialogName);
    if (dialog){
        return dialog;
    } else {
        DialogDescriber* cont = findDialogContainer(dialogName);
        if (cont){
            dialog = createDialog(cont);
            if (dialog)
                return dialog;
        }
    }
    return 0;
}

QString ScriptEngineContext::getNewDialogName()
{
    QString result = "Dialog";
    int index = m_dialogs.size() - 1;
    while (containsDialog(result)){
        index++;
        result = QString("Dialog%1").arg(index);
    }
    return result;
}

#endif

void ScriptEngineContext::baseDesignIntfToScript(const QString& pageName, BaseDesignIntf* item)
{
    if ( item ) {
        if (item->metaObject()->indexOfSignal("beforeRender()")!=-1)
            item->disconnect(SIGNAL(beforeRender()));
        if (item->metaObject()->indexOfSignal("afterData()")!=-1)
            item->disconnect(SIGNAL(afterData()));
        if (item->metaObject()->indexOfSignal("afterRender()")!=-1)
            item->disconnect(SIGNAL(afterRender()));

        ScriptEngineType* engine = ScriptEngineManager::instance().scriptEngine();

#ifdef USE_QJSENGINE
        ScriptValueType sItem = getJSValue(*engine, item);
        QString on = item->patternName().compare(pageName) == 0 ? pageName : pageName+"_"+item->patternName();
        engine->globalObject().setProperty(on, sItem);
#else
        QString on = item->patternName().compare(pageName) == 0 ? pageName : pageName+"_"+item->patternName();
        ScriptValueType sItem = engine->globalObject().property(on);
        if (sItem.isValid()){
            engine->newQObject(sItem, item);
        } else {
            sItem = engine->newQObject(item);
            engine->globalObject().setProperty(on,sItem);
        }
#endif
        foreach(BaseDesignIntf* child, item->childBaseItems()){
            baseDesignIntfToScript(pageName, child);
        }
    }
}

void ScriptEngineContext::qobjectToScript(const QString& name, QObject *item)
{
    ScriptEngineType* engine = ScriptEngineManager::instance().scriptEngine();
#ifdef USE_QJSENGINE
    ScriptValueType sItem = getJSValue(*engine, item);
    engine->globalObject().setProperty(name, sItem);
#else
    ScriptValueType sItem = engine->globalObject().property(name);
    if (sItem.isValid()){
        engine->newQObject(sItem, item);
    } else {
        sItem = engine->newQObject(item);
        engine->globalObject().setProperty(name,sItem);
    }
#endif
}

#ifdef HAVE_UI_LOADER

#ifdef USE_QJSENGINE
void registerChildObjects(ScriptEngineType* se, ScriptValueType* root, QObject* currObj){
    foreach(QObject* obj, currObj->children()){
        if (!obj->objectName().isEmpty()){
            ScriptValueType child = se->newQObject(obj);
            root->setProperty(obj->objectName(),child);
        }
        registerChildObjects(se, root, obj);
    }
}
#endif

void ScriptEngineContext::initDialogs(){
    ScriptEngineType* se = ScriptEngineManager::instance().scriptEngine();
    foreach(DialogDescriber::Ptr dialog, dialogDescribers()){
        ScriptValueType sv = se->newQObject(getDialog(dialog->name()));
#ifdef USE_QJSENGINE
        registerChildObjects(se, &sv, sv.toQObject());
#endif
        se->globalObject().setProperty(dialog->name(),sv);
    }
}

#endif


bool ScriptEngineContext::runInitScript(){

    ScriptEngineType* engine = ScriptEngineManager::instance().scriptEngine();
    ScriptEngineManager::instance().setContext(this);
    m_tableOfContents->clear();

    ScriptValueType res = engine->evaluate(initScript());
    if (res.isBool()) return res.toBool();
#ifdef  USE_QJSENGINE
    if (res.isError()){
        QMessageBox::critical(0,tr("Error"),
                              QString("Line %1: %2 ").arg(res.property("lineNumber").toString())
                              .arg(res.toString())
                              );
        return false;
    }
#else
    if (engine->hasUncaughtException()) {
        QMessageBox::critical(0,tr("Error"),
                              QString("Line %1: %2 ").arg(engine->uncaughtExceptionLineNumber())
                              .arg(engine->uncaughtException().toString())
                              );
        return false;
    }
#endif
    return true;
}

QString ScriptEngineContext::initScript() const
{
    return m_initScript;
}

void ScriptEngineContext::setInitScript(const QString& initScript)
{
    if (m_initScript != initScript){
        m_initScript = initScript;
        m_hasChanges = true;
    }
}

DialogDescriber::Ptr DialogDescriber::create(const QString& name, const QByteArray& desc) {
    Ptr res(new DialogDescriber());
    res->setName(name);
    res->setDescription(desc);
    return res;
}

QString JSFunctionDesc::name() const
{
    return m_name;
}

void JSFunctionDesc::setName(const QString &name)
{
    m_name = name;
}

QString JSFunctionDesc::category() const
{
    return m_category;
}

void JSFunctionDesc::setCategory(const QString &category)
{
    m_category = category;
}

QString JSFunctionDesc::description() const
{
    return m_description;
}

void JSFunctionDesc::setDescription(const QString &description)
{
    m_description = description;
}

QString JSFunctionDesc::managerName() const
{
    return m_managerName;
}

void JSFunctionDesc::setManagerName(const QString &managerName)
{
    m_managerName = managerName;
}

QObject *JSFunctionDesc::manager() const
{
    return m_manager;
}

void JSFunctionDesc::setManager(QObject *manager)
{
    m_manager = manager;
}

QString JSFunctionDesc::scriptWrapper() const
{
    return m_scriptWrapper;
}

void JSFunctionDesc::setScriptWrapper(const QString &scriptWrapper)
{
    m_scriptWrapper = scriptWrapper;
}

QVariant ScriptFunctionsManager::calcGroupFunction(const QString &name, const QString &expressionID, const QString &bandName, QObject* currentPage)
{
    if (m_scriptEngineManager->dataManager()){
        PageItemDesignIntf* pageItem = dynamic_cast<PageItemDesignIntf*>(currentPage);
        QString expression = m_scriptEngineManager->dataManager()->getExpression(expressionID);
        GroupFunction* gf =  m_scriptEngineManager->dataManager()->groupFunction(name,expression,bandName);
        if (gf){
            if (gf->isValid()){
                return gf->calculate(pageItem);
            } else{
                return gf->error();
            }
        } else {
            return QString(QObject::tr("Function %1 not found or have wrong arguments").arg(name));
        }
    } else {
        return QString(QObject::tr("Datasource manager not found"));
    }
}

QVariant ScriptFunctionsManager::calcGroupFunction(const QString& name, const QString& expressionID, const QString& bandName)
{
    return calcGroupFunction(name, expressionID, bandName, 0);
}

QVariant ScriptFunctionsManager::line(const QString &bandName)
{
    QString varName = QLatin1String("line_")+bandName.toLower();
    QVariant res;
    if (scriptEngineManager()->dataManager()->variable(varName).isValid()){
        res=scriptEngineManager()->dataManager()->variable(varName);
    } else res=QString("Variable line for band %1 not found").arg(bandName);
    return res;
}

QVariant ScriptFunctionsManager::numberFormat(QVariant value, const char &format, int precision, const QString& locale)
{
    return (locale.isEmpty()) ? QString::number(value.toDouble(),format,precision):
                                QLocale(locale).toString(value.toDouble(),format,precision);
}

QVariant ScriptFunctionsManager::dateFormat(QVariant value, const QString &format, const QString& locale)
{
    return (locale.isEmpty()) ?  QLocale().toString(value.toDate(),format) :
                                 QLocale(locale).toString(value.toDate(),format);
}

QVariant ScriptFunctionsManager::timeFormat(QVariant value, const QString &format)
{
    return QLocale().toString(value.toTime(),format);
}

QVariant ScriptFunctionsManager::dateTimeFormat(QVariant value, const QString &format, const QString& locale)
{
    return (locale.isEmpty()) ? QLocale().toString(value.toDateTime(),format) :
                                QLocale(locale).toString(value.toDateTime(),format);
}

QVariant ScriptFunctionsManager::sectotimeFormat(QVariant value, const QString &format)
{
    int seconds = value.toInt();
    int minutes = seconds / 60;
    int hours = minutes / 60;

    QString result = format;
    bool hasHour = format.contains("h");
    bool hasMinute = format.contains("m");
    for(int len = 2; len; len--) {
        if(hasHour)   result.replace(QString('h').repeated(len), QString::number(hours).rightJustified(len, '0'));
        if(hasMinute) result.replace(QString('m').repeated(len), QString::number(hasHour ? minutes % 60 : minutes).rightJustified(len, '0'));
        result.replace(QString('s').repeated(len), QString::number(hasMinute ? seconds % 60 : seconds).rightJustified(len, '0'));
    }
    return result;
}

QVariant ScriptFunctionsManager::date()
{
    return QDate::currentDate();
}

QVariant ScriptFunctionsManager::now()
{
    return QDateTime::currentDateTime();
}

QVariant ScriptFunctionsManager::currencyFormat(QVariant value, const QString &locale)
{
    QString l = (!locale.isEmpty())?locale:QLocale::system().name();
    return QLocale(l).toCurrencyString(value.toDouble());
}

QVariant ScriptFunctionsManager::currencyUSBasedFormat(QVariant value, const QString &currencySymbol)
{
    QString CurrencySymbol = (!currencySymbol.isEmpty())?currencySymbol:QLocale::system().currencySymbol();
    // Format it using USA locale
    QString vTempStr=QLocale(QLocale::English, QLocale::UnitedStates).toCurrencyString(value.toDouble());
    // Replace currency symbol if necesarry
    if (CurrencySymbol!="") vTempStr.replace("$", CurrencySymbol);
    return vTempStr;
}

void ScriptFunctionsManager::setVariable(const QString &name, QVariant value)
{
    DataSourceManager* dm = scriptEngineManager()->dataManager();
    if (dm->containsVariable(name)){
        dm->changeVariable(name,value);
    } else {
        dm->addVariable(name, value, VarDesc::User);
    }
}

QVariant ScriptFunctionsManager::getVariable(const QString &name)
{
    DataSourceManager* dm = scriptEngineManager()->dataManager();
    return dm->variable(name);
}

QVariant ScriptFunctionsManager::getField(const QString &field)
{
    DataSourceManager* dm = scriptEngineManager()->dataManager();
    return dm->fieldData(field);
}

QVariant ScriptFunctionsManager::getFieldByKeyField(const QString& datasourceName, const QString& valueFieldName, const QString& keyFieldName, QVariant keyValue)
{
    DataSourceManager* dm = scriptEngineManager()->dataManager();
    return dm->fieldDataByKey(datasourceName, valueFieldName, keyFieldName, keyValue);
}

QVariant ScriptFunctionsManager::getFieldByRowIndex(const QString &fieldName, int rowIndex)
{
    DataSourceManager* dm = scriptEngineManager()->dataManager();
    return dm->fieldDataByRowIndex(fieldName, rowIndex);
}

void ScriptFunctionsManager::reopenDatasource(const QString& datasourceName)
{
    DataSourceManager* dm = scriptEngineManager()->dataManager();
    return dm->reopenDatasource(datasourceName);
}

void ScriptFunctionsManager::addBookmark(const QString &uniqKey, const QString &content)
{
    scriptEngineManager()->addBookMark(uniqKey, content);
}

int ScriptFunctionsManager::findPageIndexByBookmark(const QString &uniqKey)
{
    return scriptEngineManager()->findPageIndexByBookmark(uniqKey);
}

int ScriptFunctionsManager::getPageFreeSpace(QObject* page){
    return scriptEngineManager()->getPageFreeSpace(dynamic_cast<PageItemDesignIntf*>(page));
}

void ScriptFunctionsManager::addTableOfContentsItem(const QString& uniqKey, const QString& content, int indent)
{
    scriptEngineManager()->addTableOfContentsItem(uniqKey, content, indent);
}

void ScriptFunctionsManager::clearTableOfContents()
{
    scriptEngineManager()->clearTableOfContents();
}

QFont ScriptFunctionsManager::font(const QString &family, int pointSize, bool italic, bool bold, bool underLine)
{
    QFont result (family, pointSize);
    result.setBold(bold);
    result.setItalic(italic);
    result.setUnderline(underLine);
    return result;
}

#ifdef USE_QJSENGINE

void ScriptFunctionsManager::addItemsToComboBox(QJSValue object, const QStringList &values)
{
    QComboBox* comboBox = dynamic_cast<QComboBox*>(object.toQObject());
    if (comboBox){
        comboBox->addItems(values);
    }
}

void ScriptFunctionsManager::addItemToComboBox(QJSValue object, const QString &value)
{
    QComboBox* comboBox = dynamic_cast<QComboBox*>(object.toQObject());
    if (comboBox){
        comboBox->addItem(value);
    }
}

QJSValue ScriptFunctionsManager::createComboBoxWrapper(QJSValue comboBox)
{
    QComboBox* item = dynamic_cast<QComboBox*>(comboBox.toQObject());
    if (item){
        ComboBoxWrapper* wrapper = new ComboBoxWrapper(item);
        return m_scriptEngineManager->scriptEngine()->newQObject(wrapper);
    }
    return QJSValue();
}

QJSValue ScriptFunctionsManager::createWrapper(QJSValue item)
{
    QObject* object = item.toQObject();
    if (object){
        IWrapperCreator* wrapper = m_wrappersFactory.value(object->metaObject()->className());
        if (wrapper){
            return m_scriptEngineManager->scriptEngine()->newQObject(wrapper->createWrapper(item.toQObject()));
        }
    }
    return QJSValue();
}

#else

void ScriptFunctionsManager::addItemsToComboBox(QScriptValue object, const QStringList &values)
{
    QComboBox* comboBox = dynamic_cast<QComboBox*>(object.toQObject());
    if (comboBox){
        comboBox->addItems(values);
    }
}

void ScriptFunctionsManager::addItemToComboBox(QScriptValue object, const QString &value)
{
    QComboBox* comboBox = dynamic_cast<QComboBox*>(object.toQObject());
    if (comboBox){
        comboBox->addItem(value);
    }
}

QScriptValue ScriptFunctionsManager::createComboBoxWrapper(QScriptValue comboBox)
{
    QComboBox* item = dynamic_cast<QComboBox*>(comboBox.toQObject());
    if (item){
        ComboBoxWrapper* wrapper = new ComboBoxWrapper(item);
        return m_scriptEngineManager->scriptEngine()->newQObject(wrapper);
    }
    return QScriptValue();
}

QScriptValue ScriptFunctionsManager::createWrapper(QScriptValue item)
{
    QObject* object = item.toQObject();
    if (object){
        IWrapperCreator* wrapper = m_wrappersFactory.value(object->metaObject()->className());
        if (wrapper){
            return m_scriptEngineManager->scriptEngine()->newQObject(wrapper->createWrapper(item.toQObject()));
        }
    }
    return QScriptValue();
}

#endif

QFont ScriptFunctionsManager::font(QVariantMap params){
    if (!params.contains("family")){
        return QFont();
    } else {
        QFont result(params.value("family").toString());
        if (params.contains("pointSize"))
            result.setPointSize(params.value("pointSize").toInt());
        if (params.contains("bold"))
            result.setBold(params.value("bold").toBool());
        if (params.contains("italic"))
            result.setItalic(params.value("italic").toBool());
        if (params.contains("underline"))
            result.setUnderline(params.value("underline").toBool());
        return result;
    }
}

ScriptEngineManager *ScriptFunctionsManager::scriptEngineManager() const
{
    return m_scriptEngineManager;
}

void ScriptFunctionsManager::setScriptEngineManager(ScriptEngineManager *scriptEngineManager)
{
    m_scriptEngineManager = scriptEngineManager;
}

TableOfContents::~TableOfContents()
{
    clear();
}

void TableOfContents::setItem(const QString& uniqKey, const QString& content, int pageNumber, int indent)
{
    ContentItem * item = 0;
    if (m_hash.contains(uniqKey)){
        item = m_hash.value(uniqKey);
        item->content = content;
        item->pageNumber = pageNumber;
        if (indent>0)
            item->indent = indent;
    } else {
        item = new ContentItem;
        item->content = content;
        item->pageNumber = pageNumber;
        item->indent = indent;
        item->uniqKey = uniqKey;
        m_tableOfContents.append(item);
        m_hash.insert(uniqKey, item);
    }

}

void TableOfContents::slotOneSlotDS(CallbackInfo info, QVariant& data)
{
    QStringList columns;
    columns << "Content" << "Page number" << "Content Key";

    switch (info.dataType) {
    case LimeReport::CallbackInfo::RowCount:
        data = m_tableOfContents.count();
        break;
    case LimeReport::CallbackInfo::ColumnCount:
        data = columns.size();
        break;
    case LimeReport::CallbackInfo::ColumnHeaderData: {
        data = columns.at(info.index);
        break;
    }
    case LimeReport::CallbackInfo::ColumnData:
        if (info.index < m_tableOfContents.count()){
            ContentItem* item = m_tableOfContents.at(info.index);
            if (info.columnName.compare("Content",Qt::CaseInsensitive) == 0)
                data = item->content.rightJustified(item->indent+item->content.size());
            if (info.columnName.compare("Content Key",Qt::CaseInsensitive) == 0)
                data = item->uniqKey;
            if (info.columnName.compare("Page number",Qt::CaseInsensitive) == 0)
                data = QString::number(item->pageNumber);
        }
        break;
    default: break;
    }
}

void LimeReport::TableOfContents::clear(){

    m_hash.clear();
    foreach(ContentItem* item, m_tableOfContents){
        delete item;
    }
    m_tableOfContents.clear();

}

QObject* ComboBoxWrapperCreator::createWrapper(QObject *item)
{
    QComboBox* comboBox = dynamic_cast<QComboBox*>(item);
    if (comboBox){
        return  new ComboBoxWrapper(comboBox);
    }
    return 0;
}

bool DatasourceFunctions::first(const QString& datasourceName)
{
    if (m_dataManager && m_dataManager->dataSource(datasourceName)){
        m_dataManager->dataSource(datasourceName)->first();
        return true;
    }
    return false;
}

bool DatasourceFunctions::next(const QString &datasourceName){
    if (m_dataManager && m_dataManager->dataSource(datasourceName))
        return m_dataManager->dataSource(datasourceName)->next();
    return false;
}

bool DatasourceFunctions::prior(const QString& datasourceName)
{
    if (m_dataManager && m_dataManager->dataSource(datasourceName))
        return m_dataManager->dataSource(datasourceName)->prior();
    return false;
}

bool DatasourceFunctions::isEOF(const QString &datasourceName)
{
    if (m_dataManager && m_dataManager->dataSource(datasourceName))
        return m_dataManager->dataSource(datasourceName)->eof();
    return true;
}

int DatasourceFunctions::rowCount(const QString &datasourceName)
{
    if (m_dataManager && m_dataManager->dataSource(datasourceName))
        return m_dataManager->dataSource(datasourceName)->model()->rowCount();
    return 0;
}

bool DatasourceFunctions::invalidate(const QString& datasourceName)
{
    if (m_dataManager && m_dataManager->dataSource(datasourceName)){
        m_dataManager->dataSourceHolder(datasourceName)->invalidate(IDataSource::RENDER_MODE);
        return true;
    }
    return false;
}

QObject* DatasourceFunctions::createTableBuilder(QObject* horizontalLayout)
{
    LimeReport::HorizontalLayout* l = dynamic_cast<LimeReport::HorizontalLayout*>(horizontalLayout);
    if (l)
        return new TableBuilder(l, m_dataManager);
    return 0;
}

TableBuilder::TableBuilder(HorizontalLayout* layout, DataSourceManager* dataManager)
    : m_horizontalLayout(layout), m_baseLayout(0), m_dataManager(dataManager)
{
    if (m_horizontalLayout)
        m_patternLayout = dynamic_cast<HorizontalLayout*>(m_horizontalLayout->cloneItem(m_horizontalLayout->itemMode()));
}

QObject* TableBuilder::addRow()
{
    checkBaseLayout();
    if (m_baseLayout && m_patternLayout){
        HorizontalLayout* newRow = new HorizontalLayout(m_baseLayout, m_baseLayout);
        newRow->setLayoutSpacing(m_horizontalLayout->layoutSpacing());
        for(int i = 0; i < m_horizontalLayout->childrenCount(); ++i){
            BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(m_patternLayout->at(i));
            BaseDesignIntf* cloneItem = item->cloneItem(item->itemMode(), newRow, newRow);
            newRow->addChild(cloneItem);
        }
        m_baseLayout->addChild(newRow);
        return newRow;
    } else return 0;
}

QObject* TableBuilder::currentRow()
{
    checkBaseLayout();
    if (m_baseLayout && m_baseLayout->childrenCount()>0)
        return m_baseLayout->at(m_baseLayout->childrenCount()-1);
    return 0;
}

void TableBuilder::fillInRowData(QObject* row)
{
    HorizontalLayout* layout = dynamic_cast<HorizontalLayout*>(row);
    if (layout){
        for (int i = 0; i < layout->childrenCount(); ++i) {
            BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(layout->at(i));
            DataSourceManager* dm = dynamic_cast<DataSourceManager*>(m_dataManager);
            if (item && dm)
                item->updateItemSize(dm);
        }
    }
}

void TableBuilder::buildTable(const QString& datasourceName)
{
    checkBaseLayout();
    IDataSourceHolder* dh = m_dataManager->dataSourceHolder(datasourceName);
    if (dh) {
        dh->invalidate(IDataSource::RENDER_MODE);
        IDataSource* ds = m_dataManager->dataSource(datasourceName);
        if (ds){
            bool firstTime = true;
            QObject* row = m_horizontalLayout;
            while(!ds->eof()){
                if (!firstTime) row = addRow();
                else firstTime = false;
                fillInRowData(row);
                ds->next();
            }
        }
    }
}

void TableBuilder::checkBaseLayout()
{
    if (!m_baseLayout){
        m_baseLayout = dynamic_cast<VerticalLayout*>(m_horizontalLayout->parentItem());
        if (!m_baseLayout){
            m_baseLayout = new VerticalLayout(m_horizontalLayout->parent(), m_horizontalLayout->parentItem());
            m_baseLayout->setItemLocation(m_horizontalLayout->itemLocation());
            m_baseLayout->setPos(m_horizontalLayout->pos());
            m_baseLayout->setWidth(m_horizontalLayout->width());
            m_baseLayout->setHeight(0);
            m_baseLayout->addChild(m_horizontalLayout);
            m_baseLayout->setObjectName(QUuid::createUuid().toString());
            m_baseLayout->setItemTypeName("VerticalLayout");
        }
    }
}

#ifdef USE_QTSCRIPTENGINE
void ComboBoxPrototype::addItem(const QString &text)
{
    QComboBox* comboBox = qscriptvalue_cast<QComboBox*>(thisObject());
    if (comboBox){
        comboBox->addItem(text);
    }
}

void ComboBoxPrototype::addItems(const QStringList &texts)
{
    QComboBox* comboBox = qscriptvalue_cast<QComboBox*>(thisObject());
    if (comboBox){
        comboBox->addItems(texts);
    }
}
#endif

} //namespace LimeReport

