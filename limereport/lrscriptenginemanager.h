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
#ifndef LRSCRIPTENGINEMANAGER_H
#define LRSCRIPTENGINEMANAGER_H
#ifdef USE_QTSCRIPTENGINE
#include <QtScript/QScriptEngine>
#include <QScriptable>
#endif
#include <QVector>
#include <QIcon>
#include <QAbstractItemModel>
#include <QDebug>
#include <QtGlobal>
#include <QFont>
#include <QComboBox>

//#include <QJSEngine>

#ifdef HAVE_UI_LOADER
#include <QDialog>
#endif

#include "base/lrsingleton.h"
#include "lrglobal.h"
#include "lrscriptenginemanagerintf.h"
#include "lrcallbackdatasourceintf.h"
#include "lrcollection.h"
#include "lrdatasourceintf.h"
#include "lrdatasourcemanagerintf.h"
#include "lrhorizontallayout.h"
#include "lrverticallayout.h"

namespace LimeReport{

class DataSourceManager;
class BaseDesignIntf;
class PageItemDesignIntf;
class BandDesignIntf;

struct ContentItem {
    QString content;
    int     indent;
    int     pageNumber;
    QString uniqKey;
};

class TableOfContents : public QObject{
    Q_OBJECT
public:
    TableOfContents(QObject* parent = 0):QObject(parent){}
    ~TableOfContents();
    void setItem(const QString& uniqKey, const QString& content, int pageNumber, int indent = 0);
    void clear();
    bool isEmpty(){ return m_tableOfContents.isEmpty();}
private slots:
   void slotOneSlotDS(LimeReport::CallbackInfo info, QVariant &data);
private:
   QVector<ContentItem*> m_tableOfContents;
   QHash<QString, ContentItem* > m_hash;
};


struct ScriptFunctionDesc{
    enum FuncType {Native,Script};
    ScriptValueType scriptValue;
    QString name;
    QString description;
    QString category;
    FuncType type;
};

class ScriptEngineNode {
public:
    enum NodeType{Root, Category, Function, Dialog, DialogElement};
    ScriptEngineNode(const QString& name = "", const QString& description = "", NodeType type = Root,
                     ScriptEngineNode* parent = 0, const QIcon& icon = QIcon());
    virtual ~ScriptEngineNode();
    int       childCount(){return m_childs.count();}
    ScriptEngineNode* child(int index){return m_childs[index];}
    ScriptEngineNode* parent(){return m_parent;}
    ScriptEngineNode* addChild(const QString& name = "", const QString &description = "",
                               NodeType type = Root, const QIcon& icon = QIcon());
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

typedef QList< QSharedPointer<PageItemDesignIntf> > ReportPages;

class ScriptEngineContext : public QObject, public ICollectionContainer
{
    Q_OBJECT
    Q_PROPERTY(ACollectionProperty dialogs READ fakeCollectionReader)
    Q_PROPERTY(QString initScript READ initScript WRITE setInitScript)
public:
#ifdef HAVE_UI_LOADER
    typedef QSharedPointer<QDialog> DialogPtr;
#endif
    explicit ScriptEngineContext(QObject* parent=0):
        QObject(parent), m_currentBand(0), m_currentPage(0),
        m_tableOfContents(new TableOfContents(this)), m_hasChanges(false) {}
#ifdef HAVE_UI_LOADER
    void    addDialog(const QString& name, const QByteArray& description);
    bool    changeDialog(const QString& name, const QByteArray &description);
    bool    changeDialogName(const QString& oldName, const QString& newName);
    bool    previewDialog(const QString& dialogName);
    bool    containsDialog(const QString& dialogName);
    const   QVector<DialogDescriber::Ptr>& dialogDescribers(){return m_dialogs;}
    void    deleteDialog(const QString& dialogName);
    QDialog *getDialog(const QString &dialogName);
    QString getNewDialogName();
    void    initDialogs();
#endif
    void    baseDesignIntfToScript(const QString& pageName, BaseDesignIntf *item);
    void    qobjectToScript(const QString &name, QObject* item);
    void    clear();
    QString initScript() const;
    void    setInitScript(const QString& initScript);
    bool    runInitScript();

    BandDesignIntf* currentBand() const;
    void setCurrentBand(BandDesignIntf* currentBand);
    PageItemDesignIntf* currentPage() const;
    void setCurrentPage(PageItemDesignIntf* currentPage);
    TableOfContents* tableOfContents() const;
    void setTableOfContents(TableOfContents* tableOfContents);
    void dropChanges(){ m_hasChanges = false;}
    bool hasChanges(){ return m_hasChanges;}
    ReportPages* reportPages() const;
    void setReportPages(ReportPages* value);
#ifdef HAVE_UI_LOADER
signals:
    void    dialogNameChanged(QString dialogName);
    void    dialogDeleted(QString dialogName);
    void    dialogAdded(QString dialogName);
#endif
protected:
    QObject* createElement(const QString& collectionName,const QString& elementType);
    int      elementsCount(const QString& collectionName);
    QObject* elementAt(const QString& collectionName,int index);
    void     collectionLoadFinished(const QString &collectionName);
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
    BandDesignIntf* m_currentBand;
    PageItemDesignIntf* m_currentPage;
    TableOfContents* m_tableOfContents;
    bool m_hasChanges;
    ReportPages* m_reportPages;
};

class JSFunctionDesc{
public:
    JSFunctionDesc(){}
    JSFunctionDesc(const QString& functionName,
                   const QString& functionCategory,
                   const QString& functionDescription,
                   const QString& functionManagerName,
                   QObject* functionManager,
                   const QString& functionScriptWrapper
                   ): m_name(functionName), m_category(functionCategory), m_description(functionDescription),
                      m_managerName(functionManagerName), m_manager(functionManager), m_scriptWrapper(functionScriptWrapper)
    {}
    QString name() const;
    void setName(const QString &name);

    QString category() const;
    void setCategory(const QString &category);

    QString description() const;
    void setDescription(const QString &description);

    QString managerName() const;
    void setManagerName(const QString &managerName);

    QObject *manager() const;
    void setManager(QObject *manager);

    QString scriptWrapper() const;
    void setScriptWrapper(const QString &scriptWrapper);

private:
    QString  m_name;
    QString  m_category;
    QString  m_description;
    QString  m_managerName;
    QObject* m_manager;
    QString  m_scriptWrapper;
};

#ifdef USE_QTSCRIPTENGINE
class ComboBoxPrototype : public QObject, public QScriptable{
    Q_OBJECT
public:
    ComboBoxPrototype(QObject* parent = 0):QObject(parent){}
public slots:
    void addItem( const QString& text);
    void addItems(const QStringList& texts);
};
#endif

class IWrapperCreator{
public:
    virtual QObject* createWrapper(QObject* item) = 0;
    virtual ~IWrapperCreator(){}
};

class ComboBoxWrapper : public QObject{
    Q_OBJECT
public:
    ComboBoxWrapper(QComboBox* comboBox, QObject* parent = 0) : QObject(parent), m_comboBox(comboBox){}
    Q_INVOKABLE void addItems(const QStringList& texts){ m_comboBox->addItems(texts);}
    Q_INVOKABLE void addItem(const QString& text){ m_comboBox->addItem(text);}
private:
    QComboBox* m_comboBox;
};

class ComboBoxWrapperCreator: public IWrapperCreator{
private:
    QObject* createWrapper(QObject* item);
};

class TableBuilder: public QObject{
    Q_OBJECT
public:
    TableBuilder(LimeReport::HorizontalLayout* layout, DataSourceManager* dataManager);
    ~TableBuilder(){delete m_patternLayout;}
    Q_INVOKABLE QObject* addRow();
    Q_INVOKABLE QObject* currentRow();
    Q_INVOKABLE void fillInRowData(QObject* row);
    Q_INVOKABLE void buildTable(const QString& datasourceName);
private:
    void checkBaseLayout();
private:
    LimeReport::HorizontalLayout* m_horizontalLayout;
    LimeReport::HorizontalLayout* m_patternLayout;
    LimeReport::VerticalLayout* m_baseLayout;
    DataSourceManager* m_dataManager;
};

class DatasourceFunctions : public QObject{
    Q_OBJECT
public:
    explicit DatasourceFunctions(IDataSourceManager* dataManager)
        : m_dataManager(dynamic_cast<DataSourceManager*>(dataManager)){}
    Q_INVOKABLE bool first(const QString& datasourceName);
    Q_INVOKABLE bool next(const QString& datasourceName);
    Q_INVOKABLE bool prior(const QString& datasourceName);
    Q_INVOKABLE bool isEOF(const QString& datasourceName);
    Q_INVOKABLE int rowCount(const QString& datasourceName);
    Q_INVOKABLE bool invalidate(const QString& datasourceName);
    Q_INVOKABLE QObject *createTableBuilder(QObject *horizontalLayout);
private:
    DataSourceManager* m_dataManager;
};

class ScriptFunctionsManager : public QObject{
    Q_OBJECT
public:
    explicit ScriptFunctionsManager(QObject* parent = 0):QObject(parent){
        m_wrappersFactory.insert("QComboBox",new  ComboBoxWrapperCreator());

    }
    ~ScriptFunctionsManager(){
        foreach(IWrapperCreator* wrapper, m_wrappersFactory.values()){ delete wrapper;} m_wrappersFactory.clear();
    }
    Q_INVOKABLE QVariant calcGroupFunction(const QString& name, const QString& expressionID, const QString& bandName, QObject* currentPage);
    Q_INVOKABLE QVariant calcGroupFunction(const QString& name, const QString& expressionID, const QString& bandName);
    Q_INVOKABLE QVariant line(const QString& bandName);
    Q_INVOKABLE QVariant numberFormat(QVariant value, const char &format, int precision, const QString& locale);
    Q_INVOKABLE QVariant dateFormat(QVariant value, const QString& format, const QString& locale);
    Q_INVOKABLE QVariant timeFormat(QVariant value, const QString& format);
    Q_INVOKABLE QVariant dateTimeFormat(QVariant value, const QString& format, const QString& locale);
    Q_INVOKABLE QVariant sectotimeFormat(QVariant value, const QString& format);
    Q_INVOKABLE QVariant date();
    Q_INVOKABLE QVariant now();
    Q_INVOKABLE QVariant currencyFormat(QVariant value, const QString& locale);
    Q_INVOKABLE QVariant currencyUSBasedFormat(QVariant value, const QString& currencySymbol);
    Q_INVOKABLE void     setVariable(const QString& name, QVariant value);
    Q_INVOKABLE QVariant getVariable(const QString& name);
    Q_INVOKABLE QVariant getField(const QString& field);
    Q_INVOKABLE QVariant getFieldByKeyField(const QString& datasourceName, const QString& valueFieldName, const QString& keyFieldName, QVariant keyValue);
    Q_INVOKABLE QVariant getFieldByRowIndex(const QString& fieldName, int rowIndex);
    Q_INVOKABLE void     reopenDatasource(const QString& datasourceName);
    Q_INVOKABLE QVariant color(const QString& color){ return  QColor(color);}
    Q_INVOKABLE void     addBookmark(const QString& uniqKey, const QString& content);
    Q_INVOKABLE int      findPageIndexByBookmark(const QString &uniqKey);
    Q_INVOKABLE void     addTableOfContentsItem(const QString& uniqKey, const QString& content, int indent = 0);
    Q_INVOKABLE void     clearTableOfContents();
    Q_INVOKABLE QFont    font(const QString& family, int pointSize = -1, bool bold = false, bool italic = false, bool underLine = false);
#ifdef USE_QJSENGINE
    Q_INVOKABLE void addItemsToComboBox(QJSValue object, const QStringList& values);
    Q_INVOKABLE void addItemToComboBox(QJSValue object, const QString& value);
    Q_INVOKABLE QJSValue createComboBoxWrapper(QJSValue comboBox);
    Q_INVOKABLE QJSValue createWrapper(QJSValue item);
#else
    Q_INVOKABLE void addItemsToComboBox(QScriptValue object, const QStringList& values);
    Q_INVOKABLE void addItemToComboBox(QScriptValue object, const QString& value);
    Q_INVOKABLE QScriptValue createComboBoxWrapper(QScriptValue comboBox);
    Q_INVOKABLE QScriptValue createWrapper(QScriptValue item);
#endif
    Q_INVOKABLE QFont font(QVariantMap params);
    Q_INVOKABLE int getPageFreeSpace(QObject *page);
    ScriptEngineManager *scriptEngineManager() const;
    void setScriptEngineManager(ScriptEngineManager *scriptEngineManager);
    static QColor createQColor(const QString& color){ return QColor(color);}
private:
    ScriptEngineManager* m_scriptEngineManager;
    QMap<QString, IWrapperCreator*> m_wrappersFactory;
};

class ScriptNode{
public:
    typedef QSharedPointer<ScriptNode> Ptr;
    QString body(){
        if (m_body.isEmpty() && m_children.count() > 0)
          return m_children.at(0)->body();
        return m_body;
    }
    void setBody(const QString& body){ m_body = body;}
    void setStartLex(const QString startLex){ m_startLex = startLex;}
    QString script(){return m_startLex + m_body + '}';}
    Ptr createChildNode(){
        Ptr result = Ptr(new ScriptNode());
        m_children.append(result);
        return result;
    }
    QVector<Ptr> children() const {return m_children;}
private:
    QVector<Ptr> m_children;
    QString m_body;
    QString m_startLex;
};

class ScriptExtractor
{
public:
    enum State{None,BuksFound,SFound,StartScriptFound,OpenBracketFound,CloseBracketFound,DFound,VFound, SignFound};
    explicit ScriptExtractor(const QString& value):
        m_context(value), m_scriptTree(ScriptNode::Ptr(new ScriptNode())){}
    bool parse();
    ScriptNode::Ptr scriptTree(){return m_scriptTree;}
private:
    bool isStartLexem(int &curPos, QChar value);
    bool parse(int& curPos, const State &state, ScriptNode::Ptr scriptNode);
    void skipField(int &curPos);
    void extractScript(int& curPos, const QString &startStr, ScriptNode::Ptr scriptNode);
    bool extractBracket(int& curPos, ScriptNode::Ptr scriptNode);
    bool isStartScriptLexem(int &curPos);
    bool isStartFieldLexem(int &curPos);
    bool isStartVariableLexem(int &curPos);
    QString substring(const QString& value, int start, int end);
private:
    QString m_context;
    ScriptNode::Ptr m_scriptTree;
};

class ScriptEngineManager : public QObject, public Singleton<ScriptEngineManager>, public IScriptEngineManager
{
    Q_OBJECT
public:
    friend class Singleton<ScriptEngineManager>;
    ScriptEngineType* scriptEngine(){return m_scriptEngine;}
    ~ScriptEngineManager();
    bool isFunctionExists(const QString& functionName) const;
    void deleteFunction(const QString& functionsName);

    bool addFunction(const JSFunctionDesc& functionsDescriber);
#ifdef USE_QTSCRIPTENGINE
    bool addFunction(const QString &name, QScriptEngine::FunctionSignature function, const QString &category, const QString &description);
#endif
    bool addFunction(const QString &name, const QString& script,
                             const QString &category="", const QString &description="");
    const QString& lastError() const {return m_lastError;}
    QStringList functionsNames();
    const QHash<QString,ScriptFunctionDesc>& functionsDescribers(){return m_functions;}
    ScriptEngineModel* model(){return m_model;}
    void setContext(ScriptEngineContext* context){m_context=context;}
    DataSourceManager* dataManager() const {return m_dataManager;}
    void setDataManager(DataSourceManager* dataManager);

    QString expandUserVariables(QString context, RenderPass pass, ExpandType expandType, QVariant &varValue);
    QString expandDataFields(QString context, ExpandType expandType, QVariant &varValue, QObject* reportItem);
    QString expandScripts(QString context, QVariant &varValue, QObject* reportItem);

    QString replaceScripts(QString context, QVariant& varValue, QObject *reportItem, ScriptEngineType *se, ScriptNode::Ptr scriptTree);

    QVariant evaluateScript(const QString &script);
    void    addBookMark(const QString &uniqKey, const QString &content);
    int     findPageIndexByBookmark(const QString& uniqKey);
    void    addTableOfContentsItem(const QString& uniqKey, const QString& content, int indent);
    void    clearTableOfContents();
    int     getPageFreeSpace(PageItemDesignIntf *page);
    ScriptValueType moveQObjectToScript(QObject* object, const QString objectName);
protected:
    void updateModel();
private:
    Q_DISABLE_COPY(ScriptEngineManager)
    bool createLineFunction();
    bool createNumberFomatFunction();
    bool createDateFormatFunction();
    bool createTimeFormatFunction();
    bool createDateTimeFormatFunction();
    bool createSectotimeFormatFunction();
    bool createDateFunction();
    bool createNowFunction();
    bool createCurrencyFormatFunction();
    bool createCurrencyUSBasedFormatFunction();
    bool createSetVariableFunction();
    bool createGetVariableFunction();
    bool createGetFieldFunction();
    bool createGetFieldByKeyFunction();
    bool createGetFieldByRowIndex();
    bool createAddBookmarkFunction();
    bool createFindPageIndexByBookmark();
    bool createAddTableOfContentsItemFunction();
    bool createClearTableOfContentsFunction();
    bool createReopenDatasourceFunction();
private:
    ScriptEngineManager();
    ScriptEngineType*  m_scriptEngine;
    QString m_lastError;
    QHash<QString,ScriptFunctionDesc> m_functions;
    ScriptEngineModel* m_model;
    ScriptEngineContext* m_context;
    DataSourceManager* m_dataManager;
    ScriptFunctionsManager* m_functionManager;
};


#ifdef USE_QTSCRIPTENGINE
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
#endif

}
#ifdef USE_QTSCRIPTENGINE
Q_DECLARE_METATYPE(LimeReport::ComboBoxPrototype*)
Q_DECLARE_METATYPE(QComboBox*)
#endif

#endif // LRSCRIPTENGINEMANAGER_H
