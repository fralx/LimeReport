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
#include "lrdatasourcemanager.h"

Q_DECLARE_METATYPE(QColor)
Q_DECLARE_METATYPE(QFont)

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

ScriptEngineNode*ScriptEngineNode::addChild(const QString& name, ScriptEngineNode::NodeType type, const QIcon& icon)
{
    ScriptEngineNode* res = new ScriptEngineNode(name,type,this,icon);
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
            categ = m_rootNode->addChild(categoryName,ScriptEngineNode::Category,QIcon(":/report/images/folder"));
            categories.insert(categoryName,categ);
        }
        categ->addChild(funcDesc.name,ScriptEngineNode::Function,QIcon(":/report/images/function"));
    }
    //reset();
    endResetModel();
}

QScriptValue dateToStr(QScriptContext* pcontext, QScriptEngine* pengine){
    DataSourceManager* dm = DataSourceManager::instance();
    QString field = pcontext->argument(0).toString();
    QString format = pcontext->argument(1).toString();
    QScriptValue res;
    if (dm->containsField(field)){
        res=pengine->newVariant(QLocale().toString(dm->fieldData(field).toDate(),format));
    } else {
        QString error = (!dm->lastError().isNull())?dm->lastError():QString("Field %1 not found").arg(field);
        res=pengine->newVariant(error);
    }
    return res;
}

QScriptValue line(QScriptContext* pcontext, QScriptEngine* pengine){
    DataSourceManager* dm=DataSourceManager::instance();
    QString band = pcontext->argument(0).toString();
    QScriptValue res;
    QString varName = QLatin1String("line_")+band.toLower();
    if (dm->variable(varName).isValid()){
        res=pengine->newVariant(dm->variable(varName));
    } else res=pengine->newVariant(QString("Variable line for band %1 not found").arg(band));
    return res;
}

QScriptValue numberFormat(QScriptContext* pcontext, QScriptEngine* pengine){
    QVariant value = pcontext->argument(0).toVariant();
    char format = (pcontext->argumentCount()>1)?pcontext->argument(1).toString()[0].toLatin1():'f';
    int precision = (pcontext->argumentCount()>2)?pcontext->argument(2).toInt32():2;
    QScriptValue res = pengine->newVariant(QString::number(value.toDouble(),format,precision));
    return res;
}

QScriptValue dateFormat(QScriptContext* pcontext, QScriptEngine* pengine){
    QVariant value = pcontext->argument(0).toVariant();
    QString format = (pcontext->argumentCount()>1)?pcontext->argument(1).toString().toLatin1():"dd.MM.yyyy";
    QScriptValue res = pengine->newVariant(QLocale().toString(value.toDate(),format));
    return res;
}

QScriptValue now(QScriptContext* /*pcontext*/, QScriptEngine* pengine){
    return pengine->newVariant(QDate::currentDate());
}

QScriptValue callGroupFunction(const QString& functionName, QScriptContext* pcontext, QScriptEngine* pengine){

    DataSourceManager* dm=DataSourceManager::instance();
    QString expression = pcontext->argument(0).toString();
    QString band = pcontext->argument(1).toString();
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
    if (m_model){
        delete m_model;
        m_model=0;
    }
    delete m_scriptEngine;
}

QScriptValue ScriptEngineManager::addFunction(const QString& name,
                                              QScriptEngine::FunctionSignature function,
                                              const QString& category,
                                              const QString& description)
{
    ScriptFunctionDesc funct;
    funct.name = name;
    funct.description = description;
    funct.category = category;
    funct.scriptValue = scriptEngine()->newFunction(function);
    funct.scriptValue.setProperty("functionName",name);
    funct.type = ScriptFunctionDesc::Native;
    m_functions.append(funct);
    if (m_model)
        m_model->updateModel();
    return funct.scriptValue;
}

QScriptValue ScriptEngineManager::addFunction(const QString& name,const QString& script, const QString& category, const QString& description)
{
    QScriptSyntaxCheckResult cr = m_scriptEngine->checkSyntax(script);
    if (cr.state() == QScriptSyntaxCheckResult::Valid){
        ScriptFunctionDesc funct;
        funct.scriptValue = m_scriptEngine->evaluate(script);
        funct.name =  name;
        funct.category = category;
        funct.description = description;
        funct.type = ScriptFunctionDesc::Script;
        m_functions.append(funct);
        m_model->updateModel();
        return funct.scriptValue;
    } else {
        m_lastError = cr.errorMessage();
        return QScriptValue();
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

ScriptEngineManager::ScriptEngineManager()
    :m_model(0)
{
    m_scriptEngine = new QScriptEngine;

    addFunction("dateToStr",dateToStr,"DATE");
    addFunction("line",line,"SYSTEM");
    addFunction("numberFormat",numberFormat,"NUMBER");
    addFunction("dateFormat",dateFormat,"DATE");
    addFunction("now",now,"DATE");

    QScriptValue colorCtor = m_scriptEngine->newFunction(constructColor);
    m_scriptEngine->globalObject().setProperty("QColor", colorCtor);

    QScriptValue fontProto(m_scriptEngine->newQObject(new QFontPrototype,QScriptEngine::ScriptOwnership));
    m_scriptEngine->setDefaultPrototype(qMetaTypeId<QFont>(), fontProto);
    QScriptValue fontConstructor = m_scriptEngine->newFunction(QFontPrototype::constructorQFont, fontProto);
    m_scriptEngine->globalObject().setProperty("QFont", fontConstructor);

    DataSourceManager* dm=DataSourceManager::instance();
    foreach(QString func, dm->groupFunctionNames()){
        addFunction(func,groupFunction,"GROUP FUNCTIONS");
    }

    foreach(ScriptFunctionDesc func, m_functions){
        if (func.type==ScriptFunctionDesc::Native)
            m_scriptEngine->globalObject().setProperty(func.name,func.scriptValue);
    }

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

} //namespace LimeReport

