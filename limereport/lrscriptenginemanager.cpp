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
#include "lrscriptenginemanager.h"

#include <QDate>
#include <QStringList>
#include <QScriptValueIterator>
#ifdef HAVE_UI_LOADER
#include <QUiLoader>
#include <QBuffer>
#include <QWidget>
#endif
#include "lrdatasourcemanager.h"

Q_DECLARE_METATYPE(QColor)
Q_DECLARE_METATYPE(QFont)
Q_DECLARE_METATYPE(LimeReport::ScriptEngineManager *)

QScriptValue constructColor(QScriptContext *context, QScriptEngine *engine)
{
     QColor color(context->argument(0).toString());
     return engine->toScriptValue(color);
}

namespace LimeReport{

ScriptEngineNode::~ScriptEngineNode()
{
    for (int i = 0; i<m_childs.count(); ++i){
        delete m_childs[i];
    }
}

ScriptEngineNode*ScriptEngineNode::addChild(const QString& name, const QString& description,  ScriptEngineNode::NodeType type, const QIcon& icon)
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
    foreach(ScriptFunctionDesc funcDesc, m_scriptManager->functionsDescriber()){
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
    //reset();
    endResetModel();
}

QScriptValue line(QScriptContext* pcontext, QScriptEngine* pengine){    
    ScriptEngineManager* sm = qscriptvalue_cast<ScriptEngineManager*>(pcontext->callee().data());
    DataSourceManager* dm = sm->dataManager();
    QString band = pcontext->argument(0).toString();
    QScriptValue res;
    QString varName = QLatin1String("line_")+band.toLower();
    if (dm->variable(varName).isValid()){
        res=pengine->newVariant(dm->variable(varName));
    } else res=pengine->newVariant(QString("Variable line for band %1 not found").arg(band));
    return res;
}

QScriptValue setVariable(QScriptContext* pcontext, QScriptEngine* /*pengine*/){

    QString name = pcontext->argument(0).toString();
    QVariant value = pcontext->argument(1).toVariant();

    ScriptEngineManager* sm = qscriptvalue_cast<ScriptEngineManager*>(pcontext->callee().data());
    DataSourceManager* dm = sm->dataManager();

    if (dm->containsVariable(name))
        dm->changeVariable(name,value);
    else
        dm->addVariable(name, value);

    return QScriptValue();
}

QScriptValue getVariable(QScriptContext* pcontext, QScriptEngine* pengine){

    QString name = pcontext->argument(0).toString();

    ScriptEngineManager* sm = qscriptvalue_cast<ScriptEngineManager*>(pcontext->callee().data());
    DataSourceManager* dm = sm->dataManager();
    QScriptValue res = pengine->newVariant(dm->variable(name));

    return res;
}

QScriptValue getField(QScriptContext* pcontext, QScriptEngine* pengine){

    QString name = pcontext->argument(0).toString();

    ScriptEngineManager* sm = qscriptvalue_cast<ScriptEngineManager*>(pcontext->callee().data());
    DataSourceManager* dm = sm->dataManager();
    QScriptValue res = pengine->newVariant(dm->fieldData(name));

    return res;
}

QScriptValue numberFormat(QScriptContext* pcontext, QScriptEngine* pengine){
    QVariant value = pcontext->argument(0).toVariant();
    char format = (pcontext->argumentCount()>1)?pcontext->argument(1).toString()[0].toLatin1():'f';
    int precision = (pcontext->argumentCount()>2)?pcontext->argument(2).toInt32():2;
    QString locale = (pcontext->argumentCount()>3)?pcontext->argument(3).toString():"";
    QScriptValue res = (locale.isEmpty())?pengine->newVariant(QString::number(value.toDouble(),format,precision)):
                                          pengine->newVariant(QLocale(locale).toString(value.toDouble(),format,precision));
    return res;
}
#if QT_VERSION>0x040800
QScriptValue currencyFormat(QScriptContext* pcontext, QScriptEngine* pengine){
    QVariant value = pcontext->argument(0).toVariant();
    QString locale = (pcontext->argumentCount()>1)?pcontext->argument(1).toString():QLocale::system().name();
    return pengine->newVariant(QLocale(locale).toCurrencyString(value.toDouble()));
}

QScriptValue currencyUSBasedFormat(QScriptContext* pcontext, QScriptEngine* pengine){
    QVariant value = pcontext->argument(0).toVariant();
    QString CurrencySymbol = (pcontext->argumentCount()>1)?pcontext->argument(1).toString():QLocale::system().currencySymbol();
    // Format it using USA locale
    QString vTempStr=QLocale(QLocale::English, QLocale::UnitedStates).toCurrencyString(value.toDouble());
    // Replace currency symbol if necesarry
    if (CurrencySymbol!="") vTempStr.replace("$", CurrencySymbol);
    return pengine->newVariant(vTempStr);
}
#endif
QScriptValue dateFormat(QScriptContext* pcontext, QScriptEngine* pengine){
    QVariant value = pcontext->argument(0).toVariant();
    QString format = (pcontext->argumentCount()>1)?pcontext->argument(1).toString().toLatin1():"dd.MM.yyyy";
    QScriptValue res = pengine->newVariant(QLocale().toString(value.toDate(),format));
    return res;
}

QScriptValue timeFormat(QScriptContext* pcontext, QScriptEngine* pengine){
    QVariant value = pcontext->argument(0).toVariant();
    QString format = (pcontext->argumentCount()>1)?pcontext->argument(1).toString().toLatin1():"hh:mm";
    QScriptValue res = pengine->newVariant(QLocale().toString(value.toTime(),format));
    return res;
}

QScriptValue dateTimeFormat(QScriptContext* pcontext, QScriptEngine* pengine){
    QVariant value = pcontext->argument(0).toVariant();
    QString format = (pcontext->argumentCount()>1)?pcontext->argument(1).toString().toLatin1():"dd.MM.yyyy hh:mm";
    QScriptValue res = pengine->newVariant(QLocale().toString(value.toDateTime(),format));
    return res;
}

QScriptValue now(QScriptContext* /*pcontext*/, QScriptEngine* pengine){
    return pengine->newVariant(QDateTime::currentDateTime());
}

QScriptValue date(QScriptContext* /*pcontext*/, QScriptEngine* pengine){
    return pengine->newVariant(QDate::currentDate());
}

QScriptValue callGroupFunction(const QString& functionName, QScriptContext* pcontext, QScriptEngine* pengine){

    ScriptEngineManager* sm = qscriptvalue_cast<ScriptEngineManager*>(pcontext->callee().data());
    DataSourceManager* dm = sm->dataManager();

    QString expression;
    QString band;

    if (functionName.compare("COUNT",Qt::CaseInsensitive) == 0 && pcontext->argumentCount()==1){
        expression = " ";
        band = pcontext->argument(0).toString();
    } else {
        expression = dm->getExpression(pcontext->argument(0).toString());
        band = pcontext->argument(1).toString();
    }

    QScriptValue res;
    GroupFunction* gf = dm->groupFunction(functionName,expression,band);
    if (gf){
        if (gf->isValid()){
            res=pengine->newVariant(gf->calculate());
        }else{
            res=pengine->newVariant(gf->error());
        }
    }
    else {
        res=pengine->newVariant(QString(QObject::tr("Function %1 not found or have wrong arguments").arg(functionName)));
    }
    return res;
}

QScriptValue groupFunction(QScriptContext* pcontext, QScriptEngine* pengine){
    return callGroupFunction(pcontext->callee().property("functionName").toString(),pcontext,pengine);
}

ScriptEngineManager::~ScriptEngineManager()
{
    delete m_model;
    m_model = 0;
    delete m_scriptEngine;
}

bool ScriptEngineManager::isFunctionExists(const QString &functionName) const
{
    foreach (ScriptFunctionDesc desc, m_functions) {
        if (desc.name.compare(functionName,Qt::CaseInsensitive)==0){
            return true;
        }
    }
    return false;
}

void ScriptEngineManager::deleteFunction(const QString &functionsName)
{
    QMutableListIterator<ScriptFunctionDesc> it(m_functions);
    while(it.hasNext()){
        if (it.next().name.compare(functionsName, Qt::CaseInsensitive)==0){
            it.remove();
        }
    }
}

bool ScriptEngineManager::containsFunction(const QString& functionName){
    foreach (ScriptFunctionDesc funct, m_functions) {
        if (funct.name.compare(functionName)== 0){
            return true;
        }
    }
    return false;
}

bool ScriptEngineManager::addFunction(const QString& name,
                                              QScriptEngine::FunctionSignature function,
                                              const QString& category,
                                              const QString& description)
{
    if (!containsFunction(name)){
        ScriptFunctionDesc funct;
        funct.name = name;
        funct.description = description;
        funct.category = category;
        funct.scriptValue = scriptEngine()->newFunction(function);
        funct.scriptValue.setProperty("functionName",name);
        funct.scriptValue.setData(m_scriptEngine->toScriptValue(this));
        funct.type = ScriptFunctionDesc::Native;
        m_functions.append(funct);
        if (m_model)
            m_model->updateModel();
        m_scriptEngine->globalObject().setProperty(funct.name,funct.scriptValue);
        return true;
    } else {
        return false;
    }
}

bool ScriptEngineManager::addFunction(const QString& name, const QString& script, const QString& category, const QString& description)
{
    QScriptSyntaxCheckResult cr = m_scriptEngine->checkSyntax(script);
    if (cr.state() == QScriptSyntaxCheckResult::Valid){
        ScriptFunctionDesc funct;
        funct.scriptValue = m_scriptEngine->evaluate(script);
        funct.name =  name;
        funct.category = category;
        funct.description = description;
        funct.type = ScriptFunctionDesc::Script;
        funct.scriptValue.setData(m_scriptEngine->toScriptValue(this));
        m_functions.append(funct);
        m_model->updateModel();        
        return true;
    } else {
        m_lastError = cr.errorMessage();
        return false;
    }
}

QStringList ScriptEngineManager::functionsNames()
{
    QStringList res;
    foreach(ScriptFunctionDesc func, m_functions){
        res<<func.name;
    }
    return res;
}

void ScriptEngineManager::setDataManager(DataSourceManager *dataManager){
    if (m_dataManager != dataManager){
        m_dataManager =  dataManager;
        if (m_dataManager){
            foreach(QString func, m_dataManager->groupFunctionNames()){
                if (isFunctionExists(func)) deleteFunction(func);
                addFunction(func, groupFunction,tr("GROUP FUNCTIONS"), func+"(\""+tr("Value")+"\",\""+tr("BandName")+"\")");
            }
            foreach(ScriptFunctionDesc func, m_functions){
                if (func.type==ScriptFunctionDesc::Native)
                    m_scriptEngine->globalObject().setProperty(func.name,func.scriptValue);
            }
        }
    }
}

QString ScriptEngineManager::expandUserVariables(QString context, RenderPass pass, ExpandType expandType, QVariant &varValue)
{
    QRegExp rx(Const::VARIABLE_RX);
    if (context.contains(rx)){
        int pos = 0;
        while ((pos = rx.indexIn(context,pos))!=-1){
            QString variable=rx.cap(1);
            pos += rx.matchedLength();
            if (dataManager()->containsVariable(variable) ){
                try {
                    if (pass==dataManager()->variablePass(variable)){
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
                    }
                } catch (ReportError e){
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
}

QString ScriptEngineManager::expandDataFields(QString context, ExpandType expandType, QVariant &varValue, QObject *reportItem)
{
    QRegExp rx(Const::FIELD_RX);

    if (context.contains(rx)){
        while ((rx.indexIn(context))!=-1){
            QString field=rx.cap(1);

            if (dataManager()->containsField(field)) {
                QString fieldValue;
                varValue = dataManager()->fieldData(field);
                if (expandType == EscapeSymbols) {
                    if (dataManager()->fieldData(field).isNull()) {
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
                    error = QString("Field %1 not found in %2 !!! ").arg(field).arg(reportItem->objectName());
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
}

QString ScriptEngineManager::expandScripts(QString context, QVariant& varValue, QObject *reportItem)
{
    QRegExp rx(Const::SCRIPT_RX);

    if (context.contains(rx)){

        if (ScriptEngineManager::instance().dataManager()!=dataManager())
            ScriptEngineManager::instance().setDataManager(dataManager());

        QScriptEngine* se = ScriptEngineManager::instance().scriptEngine();

        if (reportItem){
            QScriptValue svThis =  se->globalObject().property("THIS");
            if (svThis.isValid()){
                se->newQObject(svThis, reportItem);
            } else {
                svThis = se->newQObject(reportItem);
                se->globalObject().setProperty("THIS",svThis);
            }
        }

        ScriptExtractor scriptExtractor(context);
        if (scriptExtractor.parse()){
            for(int i=0; i<scriptExtractor.count();++i){
                QString scriptBody = expandDataFields(scriptExtractor.bodyAt(i),EscapeSymbols, varValue, reportItem);
                scriptBody = expandUserVariables(scriptBody, FirstPass, EscapeSymbols, varValue);
                QScriptValue value = se->evaluate(scriptBody);
                if (!se->hasUncaughtException()) {
                    varValue = value.toVariant();
                    context.replace(scriptExtractor.scriptAt(i),value.toString());
                } else {
                    context.replace(scriptExtractor.scriptAt(i),se->uncaughtException().toString());
                }
            }
        }
    }
    return context;
}

QVariant ScriptEngineManager::evaluateScript(const QString& script){

    QRegExp rx(Const::SCRIPT_RX);
    QVariant varValue;

    if (script.contains(rx)){

        if (ScriptEngineManager::instance().dataManager()!=dataManager())
            ScriptEngineManager::instance().setDataManager(dataManager());

        QScriptEngine* se = ScriptEngineManager::instance().scriptEngine();

        ScriptExtractor scriptExtractor(script);
        if (scriptExtractor.parse()){
            QString scriptBody = expandDataFields(scriptExtractor.bodyAt(0),EscapeSymbols, varValue, 0);
            scriptBody = expandUserVariables(scriptBody, FirstPass, EscapeSymbols, varValue);
            QScriptValue value = se->evaluate(scriptBody);
            if (!se->hasUncaughtException()) {
                return value.toVariant();
            }
        }
    }
    return QVariant();
}

void ScriptEngineManager::updateModel()
{

}

ScriptEngineManager::ScriptEngineManager()
    :m_model(0), m_dataManager(0)
{
    m_scriptEngine = new QScriptEngine;

    //addFunction("dateToStr",dateToStr,"DATE", "dateToStr(\"value\",\"format\")");
    addFunction("line",line,tr("SYSTEM"), "line(\""+tr("BandName")+"\")");
    addFunction("numberFormat",numberFormat,tr("NUMBER"), "numberFormat(\""+tr("Value")+"\",\""+tr("Format")+"\",\""+
                tr("Precision")+"\",\""+
                tr("Locale")+"\")");
    addFunction("dateFormat",dateFormat,tr("DATE&TIME"), "dateFormat(\""+tr("Value")+"\",\""+tr("Format")+"\")");
    addFunction("timeFormat",timeFormat,tr("DATE&TIME"), "dateFormat(\""+tr("Value")+"\",\""+tr("Format")+"\")");
    addFunction("dateTimeFormat", dateTimeFormat, tr("DATE&TIME"), "dateTimeFormat(\""+tr("Value")+"\",\""+tr("Format")+"\")");
    addFunction("date",date,tr("DATE&TIME"),"date()");
    addFunction("now",now,tr("DATE&TIME"),"now()");
#if QT_VERSION>0x040800
    addFunction("currencyFormat",currencyFormat,tr("NUMBER"),"currencyFormat(\""+tr("Value")+"\",\""+tr("Locale")+"\")");
    addFunction("currencyUSBasedFormat",currencyUSBasedFormat,tr("NUMBER"),"currencyUSBasedFormat(\""+tr("Value")+",\""+tr("CurrencySymbol")+"\")");
#endif
    addFunction("setVariable", setVariable, tr("GENERAL"), "setVariable(\""+tr("Name")+"\",\""+tr("Value")+"\")");
    addFunction("getVariable", getVariable, tr("GENERAL"), "getVariable(\""+tr("Name")+"\")");
    addFunction("getField", getField, tr("GENERAL"), "getField(\""+tr("Name")+"\")");

    QScriptValue colorCtor = m_scriptEngine->newFunction(constructColor);
    m_scriptEngine->globalObject().setProperty("QColor", colorCtor);

    QScriptValue fontProto(m_scriptEngine->newQObject(new QFontPrototype,QScriptEngine::ScriptOwnership));
    m_scriptEngine->setDefaultPrototype(qMetaTypeId<QFont>(), fontProto);
    QScriptValue fontConstructor = m_scriptEngine->newFunction(QFontPrototype::constructorQFont, fontProto);
    m_scriptEngine->globalObject().setProperty("QFont", fontConstructor);

//    foreach(QString func, dataManager()->groupFunctionNames()){
//        addFunction(func, groupFunction,"GROUP FUNCTIONS", func+"(\""+tr("FieldName")+"\",\""+tr("BandName")+"\")");
//    }

//    foreach(ScriptFunctionDesc func, m_functions){
//        if (func.type==ScriptFunctionDesc::Native)
//            m_scriptEngine->globalObject().setProperty(func.name,func.scriptValue);
//    }

    m_model = new ScriptEngineModel(this);

}

bool ScriptExtractor::parse()
{
    int currentPos = 0;
    parse(currentPos,None);
    return m_scriptsBody.count()>0;

}

bool ScriptExtractor::parse(int &curPos,const State& state)
{
    while (curPos<m_context.length()){
        switch (state) {
        case OpenBracketFound:
            if (m_context[curPos]=='}'){
                return true;
            } else {
                if (m_context[curPos]=='{')
                   extractBracket(curPos);
            }
        case None:
            if (m_context[curPos]=='$'){
                int startPos = curPos;
                if (isStartScriptLexem(curPos))
                    extractScript(curPos,substring(m_context,startPos,curPos));
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

void ScriptExtractor::extractScript(int &curPos, const QString& startStr)
{
    int startPos = curPos;
    if (extractBracket(curPos)){
        QString scriptBody = substring(m_context,startPos+1,curPos);
        m_scriptsBody.push_back(scriptBody);
        m_scriptsStartLex.push_back(startStr+'{');
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

bool ScriptExtractor::extractBracket(int &curPos)
{
    curPos++;
    return parse(curPos,OpenBracketFound);
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
    return isStartLexem(curPos, SCRIPT_SIGN);
}

bool ScriptExtractor::isStartFieldLexem(int& curPos){
    return isStartLexem(curPos, FIELD_SIGN);
}

bool ScriptExtractor::isStartVariableLexem(int &curPos)
{
    return isStartLexem(curPos, VARIABLE_SIGN);
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

#ifdef HAVE_UI_LOADER
QDialog* ScriptEngineContext::createDialog(DialogDescriber* cont)
{
    QUiLoader loader;
    QByteArray desc = cont->description();
    QBuffer buffer(&desc);
    buffer.open(QIODevice::ReadOnly);
    QDialog* dialog = dynamic_cast<QDialog*>(loader.load(&buffer));
    m_createdDialogs.push_back(QSharedPointer<QDialog>(dialog));
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
#endif
QString ScriptEngineContext::initScript() const
{
    return m_initScript;
}

void ScriptEngineContext::setInitScript(const QString& initScript)
{
    m_initScript = initScript;
}

DialogDescriber::Ptr DialogDescriber::create(const QString& name, const QByteArray& desc) {
    Ptr res(new DialogDescriber());
    res->setName(name);
    res->setDescription(desc);
    return res;
}

} //namespace LimeReport

