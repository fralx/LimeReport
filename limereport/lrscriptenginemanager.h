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
#ifndef LRSCRIPTENGINEMANAGER_H
#define LRSCRIPTENGINEMANAGER_H

#include <QtScript/QScriptEngine>
#include <QVector>
#include <QIcon>
#include <QAbstractItemModel>
#include <QDebug>
#include <QtGlobal>
#include <QScriptable>
#include <QFont>

#ifdef HAVE_UI_LOADER
#include <QDialog>
#endif

#include "base/lrsingleton.h"
#include "lrscriptenginemanagerintf.h"
#include "lrcollection.h"

namespace LimeReport{

class DataSourceManager;

struct ScriptFunctionDesc{
    enum FuncType {Native,Script};
    QScriptValue scriptValue;
    QString name;
    QString description;
    QString category;
    FuncType type;
};

class ScriptEngineNode {
public:
    enum NodeType{Root,Category,Function,Dialog,DialogElement};
    ScriptEngineNode(const QString& name="", const QString& description ="", NodeType type=Root, ScriptEngineNode* parent=0, const QIcon& icon=QIcon())
        :m_name(name), m_description(description), m_icon(icon), m_type(type), m_parent(parent){}
    virtual ~ScriptEngineNode();
    int       childCount(){return m_childs.count();}
    ScriptEngineNode* child(int index){return m_childs[index];}
    ScriptEngineNode* parent(){return m_parent;}
    ScriptEngineNode* addChild(const QString& name="", const QString &description="", NodeType type=Root, const QIcon& icon=QIcon());
    int       row();
    QString   name(){return m_name;}
    QString   description(){return m_description;}
    QIcon     icon(){return m_icon;}
    void      clear();
    NodeType  type(){return m_type;}
private:
    QString     m_name;
    QString     m_description;
    QIcon       m_icon;
    NodeType    m_type;
    ScriptEngineNode*   m_parent;
    QVector<ScriptEngineNode*> m_childs;
};

class ScriptEngineManager;

class ScriptEngineModel : public QAbstractItemModel{
    Q_OBJECT
public:
    friend class ScriptEngineManager;
    explicit ScriptEngineModel():m_scriptManager(0), m_rootNode(new ScriptEngineNode()){}
    explicit ScriptEngineModel(ScriptEngineManager* scriptManager);
    ~ScriptEngineModel();
    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void setScriptEngineManager(ScriptEngineManager* scriptManager);
    inline ScriptEngineManager* scriptEngineManager(){return m_scriptManager;}
private slots:
    void slotScriptEngineChanged();
private:
    ScriptEngineNode* nodeFromIndex(const QModelIndex &index) const;
    void updateModel();
private:
    ScriptEngineManager* m_scriptManager;
    ScriptEngineNode* m_rootNode;
};

class DialogDescriber : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QByteArray description READ description WRITE setDescription)
public:
    typedef QSharedPointer<DialogDescriber> Ptr;
    static Ptr create(const QString& name, const QByteArray &desc);
    static Ptr create(){return Ptr(new DialogDescriber);}
    QString name() const;
    void setName(const QString& name);
    QByteArray description() const;
    void setDescription(const QByteArray& description);
private :
    QString m_name;
    QByteArray m_description;
};

class ScriptEngineContext : public QObject, public ICollectionContainer
{
    Q_OBJECT
    Q_PROPERTY(ACollectionProperty dialogs READ fakeCollectionReader)
    Q_PROPERTY(QString initScript READ initScript WRITE setInitScript)
public:
#ifdef HAVE_UI_LOADER
    typedef QSharedPointer<QDialog> DialogPtr;
#endif
    explicit ScriptEngineContext(QObject* parent=0):QObject(parent){}
#ifdef HAVE_UI_LOADER
    void  addDialog(const QString& name, const QByteArray &description);
    bool    previewDialog(const QString& dialogName);
    bool    containsDialog(const QString& dialogName);
    const   QVector<DialogDescriber::Ptr>& dialogsDescriber(){return m_dialogs;}
    void    deleteDialog(const QString& dialogName);
    QDialog *getDialog(const QString &dialogName);
#endif
    void clear();
    QString initScript() const;
    void setInitScript(const QString& initScript);
protected:
    QObject* createElement(const QString& collectionName,const QString& elementType);
    int elementsCount(const QString& collectionName);
    QObject* elementAt(const QString& collectionName,int index);
    void collectionLoadFinished(const QString &collectionName);
#ifdef HAVE_UI_LOADER
    QDialog *createDialog(DialogDescriber *cont);
    QDialog *findDialog(const QString &dialogName);
    DialogDescriber* findDialogContainer(const QString& dialogName);
#endif
private:
#ifdef HAVE_UI_LOADER
    QVector<DialogDescriber::Ptr> m_dialogs;
    QList<DialogPtr> m_createdDialogs;
#endif
    QString m_lastError;
    QString m_initScript;
};

class ScriptEngineManager : public QObject, public Singleton<ScriptEngineManager>, public IScriptEngineManager
{    
    Q_OBJECT
public:
    QScriptEngine* scriptEngine(){return m_scriptEngine;}
    ~ScriptEngineManager();
    friend class Singleton<ScriptEngineManager>;
    bool isFunctionExists(const QString& functionName) const;
    void deleteFunction(const QString& functionsName);
    bool addFunction(const QString& name, QScriptEngine::FunctionSignature function,
                             const QString& category="", const QString& description="");
    bool addFunction(const QString &name, const QString& script,
                             const QString &category="", const QString &description="");
    const QString& lastError() const {return m_lastError;}
    QStringList functionsNames();
    const QList<ScriptFunctionDesc>& functionsDescriber(){return m_functions;}
    ScriptEngineModel* model(){return m_model;}
    void setContext(ScriptEngineContext* context){m_context=context;}
    DataSourceManager* dataManager() const {return m_dataManager;}
    void setDataManager(DataSourceManager* dataManager);

    QString expandUserVariables(QString context, RenderPass pass, ExpandType expandType, QVariant &varValue);
    QString expandDataFields(QString context, ExpandType expandType, QVariant &varValue, QObject* reportItem);
    QString expandScripts(QString context, QVariant &varValue, QObject* reportItem);
    QVariant evaluateScript(const QString &script);

protected:
    void updateModel();
    bool containsFunction(const QString &functionName);
private:
    Q_DISABLE_COPY(ScriptEngineManager)
private:
    ScriptEngineManager();
    QScriptEngine*  m_scriptEngine;
    QString m_lastError;
    QList<ScriptFunctionDesc> m_functions;
    ScriptEngineModel* m_model;
    ScriptEngineContext* m_context;
    DataSourceManager* m_dataManager;
};

class ScriptExtractor
{
public:
    enum State{None,BuksFound,SFound,StartScriptFound,OpenBracketFound,CloseBracketFound,DFound,VFound, SignFound};
    explicit ScriptExtractor(const QString& value):m_context(value){}
    bool parse();
    int count(){return m_scriptsBody.count();}
    QString bodyAt(int index){return m_scriptsBody[index];}
    QString scriptAt(int index){return m_scriptsStartLex[index]+m_scriptsBody[index]+'}';}

private:
    static const char SCRIPT_SIGN = 'S';
    static const char FIELD_SIGN = 'D';
    static const char VARIABLE_SIGN = 'V';

    bool isStartLexem(int &curPos, QChar value);
    bool parse(int& curPos, const State &state);
    void skipField(int &curPos);
    void extractScript(int& curPos, const QString &startStr);
    bool extractBracket(int& curPos);
    bool isStartScriptLexem(int &curPos);
    bool isStartFieldLexem(int &curPos);
    bool isStartVariableLexem(int &curPos);
    QString substring(const QString& value, int start, int end);
private:
    QString m_context;
    QVector<QString> m_scriptsBody;
    QVector<QString> m_scriptsStartLex;

};

class QFontPrototype : public QObject, public QScriptable {
    Q_OBJECT
    Q_PROPERTY(QString family READ family)
    Q_PROPERTY(int size READ size)
    Q_PROPERTY(bool bold READ bold)
    Q_PROPERTY(bool italic READ italic)
    Q_PROPERTY(bool underline READ underline)
public:
    QFontPrototype(QObject * parent = NULL) : QObject(parent) , QScriptable() {
        this->setObjectName("QFontPrototype");
    }
    QString family() const {
        QFont font(qScriptValueToValue<QFont>(this->thisObject()));
        return font.family();
    }
    int size(){
        QFont font = qScriptValueToValue<QFont>(thisObject());
        return font.pointSize();
    }
    bool bold(){
        QFont font = qScriptValueToValue<QFont>(thisObject());
        return font.bold();
    }
    bool italic(){
        QFont font = qScriptValueToValue<QFont>(thisObject());
        return font.italic();
    }
    bool underline(){
        QFont font = qScriptValueToValue<QFont>(thisObject());
        return font.underline();
    }
    static QScriptValue constructorQFont(QScriptContext * context, QScriptEngine * engine) {
        QFont font;
        switch (context->argumentCount()) {
        case 5: font.setUnderline(qScriptValueToValue<bool>(context->argument(4)));
        case 4: font.setBold(qScriptValueToValue<bool>(context->argument(3)));
        case 3: font.setItalic(qScriptValueToValue<bool>(context->argument(2)));
        case 2: font.setPointSize(qScriptValueToValue<int>(context->argument(1)));
        case 1: font.setFamily(qScriptValueToValue<QString>(context->argument(0)));
        case 0: break;
        default:
            break;
        }
        return qScriptValueFromValue<QFont>(engine, font);
    }
};

}
#endif // LRSCRIPTENGINEMANAGER_H
