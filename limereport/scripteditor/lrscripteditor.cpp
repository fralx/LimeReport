#include "lrscripteditor.h"
#include "ui_lrscripteditor.h"

#ifdef USE_QJSENGINE
#include <QJSValueIterator>
#endif

#include "lrdatasourcemanager.h"
#include "lrscriptenginemanager.h"
#include "lrdatadesignintf.h"
#include "lrreportengine_p.h"

namespace LimeReport{

ScriptEditor::ScriptEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptEditor), m_reportEngine(0), m_page(0), m_tabIndention(4)
{
    ui->setupUi(this);
    setFocusProxy(ui->textEdit);
    m_completer = new ReportStructureCompleater(this);
    ui->textEdit->setCompleter(m_completer);
#if QT_VERSION < QT_VERSION_CHECK(5,12,3)
    ui->textEdit->setTabStopWidth(ui->textEdit->fontMetrics().boundingRect("0").width()*m_tabIndention);
#else
    ui->textEdit->setTabStopDistance(ui->textEdit->fontMetrics().boundingRect("0").width()*m_tabIndention);
#endif
    connect(ui->splitter, SIGNAL(splitterMoved(int,int)), this, SIGNAL(splitterMoved(int,int)));
    connect(ui->textEdit, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
}

ScriptEditor::~ScriptEditor()
{
    delete ui;
}

void ScriptEditor::initEditor(DataSourceManager* dm)
{
    ScriptEngineManager& se = LimeReport::ScriptEngineManager::instance();
    se.setDataManager(dm);

    initCompleter();

    if (dm){
        if (dm->isNeedUpdateDatasourceModel())
           dm->updateDatasourceModel();
        ui->twData->setModel(dm->datasourcesModel());
        ui->twScriptEngine->setModel(se.model());
    }

    if (ui->twScriptEngine->selectionModel()){
        connect(ui->twScriptEngine->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                this, SLOT(slotOnCurrentChanged(QModelIndex,QModelIndex)));
    }
}

void ScriptEditor::setReportEngine(ReportEnginePrivateInterface* reportEngine)
{
    m_reportEngine = reportEngine;
    DataSourceManager* dm = m_reportEngine->dataManager();
    if (dm) initEditor(dm);
    else ui->tabWidget->setVisible(false);
}

void ScriptEditor::setReportPage(PageDesignIntf* page)
{
    m_page = page;
    DataSourceManager* dm = page->datasourceManager();
    if (dm) initEditor(dm);
    else ui->tabWidget->setVisible(false);
}

void ScriptEditor::setPageBand(BandDesignIntf* band)
{
    if (band && ui->twData->model() && !band->datasourceName().isEmpty()){
        QModelIndexList nodes = ui->twData->model()->match(
                    ui->twData->model()->index(0,0),
                    Qt::DisplayRole,
                    band->datasourceName(),
                    2,
                    Qt::MatchRecursive
        );
        if (!nodes.isEmpty()){
            ui->twData->expand(nodes.at(0).parent());
            ui->twData->expand(nodes.at(0));
        }
    }
}

void ScriptEditor::setTabIndention(int charCount)
{
    if (m_tabIndention != charCount){
#if QT_VERSION < QT_VERSION_CHECK(5,12,3)
        ui->textEdit->setTabStopWidth(ui->textEdit->fontMetrics().boundingRect("W").width()*charCount);
#else
        ui->textEdit->setTabStopDistance(ui->textEdit->fontMetrics().boundingRect("W").width()*charCount);
#endif
        m_tabIndention = charCount;
    }
}

void ScriptEditor::initCompleter()
{
    if (m_reportEngine)
        m_completer->updateCompleaterModel(m_reportEngine);
    else
        m_completer->updateCompleaterModel(m_page->datasourceManager());
}

QByteArray ScriptEditor::saveState()
{
    return ui->splitter->saveState();
}

void ScriptEditor::restoreState(QByteArray state)
{
    ui->splitter->restoreState(state);
}

void ScriptEditor::setPlainText(const QString& text)
{
    ui->textEdit->setPlainText(text);
}

void ScriptEditor::setEditorFont(QFont font)
{
    ui->textEdit->setFont(font);
}

QFont ScriptEditor::editorFont()
{
    return ui->textEdit->font();
}

QString ScriptEditor::toPlainText()
{
    return ui->textEdit->toPlainText();
}

void ScriptEditor::on_twData_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    LimeReport::DataNode* node = static_cast<LimeReport::DataNode*>(index.internalPointer());
    if (node->type()==LimeReport::DataNode::Field){
        ui->textEdit->insertPlainText(QString("$D{%1.%2}").arg(node->parent()->name()).arg(node->name()));
    }
    if (node->type()==LimeReport::DataNode::Variable){
        ui->textEdit->insertPlainText(QString("$V{%1}").arg(node->name()));
    }
    ui->textEdit->setFocus();
}

void ScriptEditor::on_twScriptEngine_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    LimeReport::ScriptEngineNode* node = static_cast<LimeReport::ScriptEngineNode*>(index.internalPointer());
    if (node->type()==LimeReport::ScriptEngineNode::Function){
        ui->textEdit->insertPlainText(node->name()+"()");
    }
    ui->textEdit->setFocus();
}

void ScriptEditor::slotOnCurrentChanged(const QModelIndex &to, const QModelIndex &)
{
    LimeReport::ScriptEngineNode* node = static_cast<LimeReport::ScriptEngineNode*>(to.internalPointer());
    if (node->type()==LimeReport::ScriptEngineNode::Function){
       ui->lblDescription->setText(node->description());
    }
}

QString ReportStructureCompleater::pathFromIndex(const QModelIndex &index) const
{
    QStringList dataList;
    for (QModelIndex i = index; i.isValid(); i = i.parent()) {
        dataList.prepend(model()->data(i, Qt::DisplayRole).toString());
    }
    return dataList.join(".");
}

QStringList ReportStructureCompleater::splitPath(const QString &path) const
{
    return path.split(".");
}

void ReportStructureCompleater::addAdditionalDatawords(CompleterModel* model, DataSourceManager* dataManager){

    foreach(const QString &dsName,dataManager->dataSourceNames()){
        CompleterItem* dsNode = new CompleterItem;
        dsNode->setText(dsName);
        model->invisibleRootItem()->appendRow(dsNode);
        foreach(const QString &field, dataManager->fieldNames(dsName)){
            CompleterItem* fieldNode = new CompleterItem;
            fieldNode->setText(field);
            dsNode->appendRow(fieldNode);
        }
    }

    foreach (QString varName, dataManager->variableNames()) {
        CompleterItem* varNode = new CompleterItem;
        varNode->setText(varName.remove("#"));
        model->invisibleRootItem()->appendRow(varNode);
    }

#ifdef USE_QJSENGINE
    ScriptEngineManager& se = LimeReport::ScriptEngineManager::instance();
    QJSValue globalObject = se.scriptEngine()->globalObject();
    QJSValueIterator it(globalObject);
    while (it.hasNext()){
        it.next();
        if (it.value().isCallable() ){
            CompleterItem* itemNode = new CompleterItem;
            itemNode->setText(it.name()+"()");
            model->invisibleRootItem()->appendRow(itemNode);
        }
        if (it.value().isQObject()){
            if (it.value().toQObject()){
                if (model->findItems(it.name()).isEmpty()){
                    CompleterItem* objectNode = new CompleterItem;
                    objectNode->setText(it.name());
                    objectNode->setIcon(QIcon(":/report/images/object"));

                    for (int i = 0; i< it.value().toQObject()->metaObject()->methodCount();++i){
                        if (it.value().toQObject()->metaObject()->method(i).methodType() == QMetaMethod::Method){
                            CompleterItem* methodNode = new CompleterItem;
                            QMetaMethod m = it.value().toQObject()->metaObject()->method(i);
                            QString methodSignature = m.name() + "(";
                            bool isFirst = true;
                            for (int j = 0; j <  m.parameterCount(); ++j){
                                    methodSignature += (isFirst ? "" : ",") + m.parameterTypes()[j]+" "+m.parameterNames()[j];
                                if (isFirst) isFirst = false;
                            }
                            methodSignature += ")";
                            methodNode->setText(methodSignature);
                            objectNode->appendRow(methodNode);
                        }
                    }
                    model->invisibleRootItem()->appendRow(objectNode);
                }
            }
        }
    }
#endif

}

void ReportStructureCompleater::updateCompleaterModel(ReportEnginePrivateInterface* report)
{
    if (report){
        m_newModel.clear();

        QIcon signalIcon(":/report/images/signal");
        QIcon propertyIcon(":/report/images/property");

        for ( int i = 0; i < report->pageCount(); ++i){
            PageDesignIntf* page = report->pageAt(i);

            CompleterItem* itemNode = new CompleterItem;
            itemNode->setText(page->pageItem()->objectName());
            itemNode->setIcon(QIcon(":/report/images/object"));

            QStringList items = extractSignalNames(page->pageItem());
            foreach(QString slotName, items){
                CompleterItem* slotItem = new CompleterItem;
                slotItem->setText(slotName);
                slotItem->setIcon(signalIcon);
                itemNode->appendRow(slotItem);
            }
            items = extractProperties(page->pageItem());
            foreach(QString propertyName, items){
                CompleterItem* properyItem = new CompleterItem;
                properyItem->setText(propertyName);
                properyItem->setIcon(propertyIcon);
                itemNode->appendRow(properyItem);
            }

            foreach (BaseDesignIntf* item, page->pageItem()->childBaseItems()){
                addChildItem(item, itemNode->text(), m_newModel.invisibleRootItem());
            }

            m_newModel.invisibleRootItem()->appendRow(itemNode);
        }

        addAdditionalDatawords(&m_newModel, report->dataManager());
        m_newModel.sort(0);
    }
}

void ReportStructureCompleater::updateCompleaterModel(DataSourceManager *dataManager)
{
    m_newModel.clear();
    addAdditionalDatawords(&m_newModel, dataManager);
}

QStringList ReportStructureCompleater::extractSignalNames(BaseDesignIntf *item)
{
    QStringList result;
    if (!item) return result;
    QMetaObject const * mo = item->metaObject();
    while (mo){
        for(int i = mo->methodOffset(); i < mo->methodCount(); ++i)
        {
            if (mo->method(i).methodType() == QMetaMethod::Signal) {
#ifndef HAVE_QT4
                result.append(QString::fromLatin1(mo->method(i).name()));
#else
                result.append(QString::fromLatin1(mo->method(i).signature()));
#endif
            }
        }
        mo = mo->superClass();
    }
    result.sort();
    return result;
}

QStringList ReportStructureCompleater::extractProperties(BaseDesignIntf *item)
{
    QStringList result;
    if (!item) return result;
    QMetaObject const * mo = item->metaObject();
    while (mo){
        for(int i = mo->propertyOffset(); i < mo->propertyCount(); ++i)
        {
                result.append(QString::fromLatin1(mo->property(i).name()));
        }
        mo = mo->superClass();
    }
    result.sort();
    return result;
}

void ReportStructureCompleater::addChildItem(BaseDesignIntf *item, const QString &pageName, CompleterItem *parent)
{
    if (!item) return;

    QIcon signalIcon(":/report/images/signal");
    QIcon propertyIcon(":/report/images/property");

    CompleterItem* itemNode = new CompleterItem;
    itemNode->setText(pageName+"_"+item->objectName());
    itemNode->setIcon(QIcon(":/report/images/object"));
    parent->appendRow(itemNode);

//    if (m_cache.contains(item->metaObject()->className())){

//        QSharedPointer<CacheItem> cacheItem = m_cache.value(item->metaObject()->className());
//        itemNode->appendRows(cacheItem->slotsItems);
//        itemNode->appendRows(cacheItem->propsItems);

//    } else {

//        QSharedPointer<CacheItem> cacheItem = QSharedPointer<CacheItem>(new CacheItem);

        QStringList items;
        if (!m_signals.contains(item->metaObject()->className())){
            items = extractSignalNames(item);
            m_signals.insert(item->metaObject()->className(),items);
        } else {
            items = m_signals.value(item->metaObject()->className());
        }

        foreach(QString slotName, items){
            CompleterItem* slotItem = new CompleterItem;
            slotItem->setText(slotName);
            slotItem->setIcon(signalIcon);
            //cacheItem->slotsItems.append(QSharedPointer<CompleterItem>(slotItem));
            itemNode->appendRow(slotItem);

        }

        if (!m_properties.contains(item->metaObject()->className())){
            items = extractProperties(item);
            m_properties.insert(item->metaObject()->className(),items);
        } else {
            items = m_properties.value(item->metaObject()->className());
        }

        foreach(QString propertyName, items){
            CompleterItem* properyItem = new CompleterItem;
            properyItem->setText(propertyName);
            properyItem->setIcon(propertyIcon);
            itemNode->appendRow(properyItem);
            //cacheItem->propsItems.append(QSharedPointer<CompleterItem>(properyItem));
        }

        //m_cache.insert(item->metaObject()->className(), cacheItem);
        //itemNode->appendRows(cacheItem->slotsItems);
        //itemNode->appendRows(cacheItem->propsItems);
    //}

    foreach (BaseDesignIntf* child, item->childBaseItems()){
        addChildItem(child, pageName, parent);
    }
}

} // namespace LimeReport






