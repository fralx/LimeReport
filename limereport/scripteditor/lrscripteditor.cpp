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
    ui(new Ui::ScriptEditor), m_reportEngine(0), m_page(0)
{
    ui->setupUi(this);
    setFocusProxy(ui->textEdit);
    m_completer = new ReportStructureCompleater(this);
    ui->textEdit->setCompleter(m_completer);
    connect(ui->splitter, SIGNAL(splitterMoved(int,int)), this, SIGNAL(splitterMoved(int,int)));
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
    initEditor(dm);
}

void ScriptEditor::setReportPage(PageDesignIntf* page)
{
    m_page = page;
    DataSourceManager* dm = page->datasourceManager();
    initEditor(dm);
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

void ScriptEditor::initCompleter()
{
//    QStringList dataWords;

//    DataSourceManager* dm = 0;
//    if (m_reportEngine)
//        dm = m_reportEngine->dataManager();
//    if (m_page)
//        dm = m_page->datasourceManager();

//#ifdef USE_QJSENGINE
//    ScriptEngineManager& se = LimeReport::ScriptEngineManager::instance();
//    QJSValue globalObject = se.scriptEngine()->globalObject();
//    QJSValueIterator it(globalObject);
//    while (it.hasNext()){
//        it.next();
//        if (it.value().isCallable() ){
//            dataWords << it.name();
//        }
//    }
//#endif
//    foreach(const QString &dsName,dm->dataSourceNames()){
//        dataWords << dsName;
//        foreach(const QString &field, dm->fieldNames(dsName)){
//            dataWords<<dsName+"."+field;
//        }
//    }

//    foreach (QString varName, dm->variableNames()) {
//        dataWords << varName.remove("#");
//    }

//    if (m_reportEngine){
//        for ( int i = 0; i < m_reportEngine->pageCount(); ++i){
//            PageDesignIntf* page = m_reportEngine->pageAt(i);
//            dataWords << page->pageItem()->objectName();
//            QMetaObject const * mo = page->pageItem()->metaObject();
//            for(int i = mo->methodOffset(); i < mo->methodCount(); ++i)
//            {
//                if (mo->method(i).methodType() == QMetaMethod::Signal) {
//                    dataWords << page->pageItem()->objectName() +"."+QString::fromLatin1(mo->method(i).name());
//                }
//            }
//            dataWords << page->pageItem()->objectName()+".beforeRender";
//            dataWords << page->pageItem()->objectName()+".afterRender";
//            foreach (BaseDesignIntf* item, page->pageItem()->childBaseItems()){
//                addItemToCompleater(page->pageItem()->objectName(), item, dataWords);
//            }
//        }
//    }

//    dataWords.sort();
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

void ScriptEditor::addItemToCompleater(const QString& pageName, BaseDesignIntf* item, QStringList& dataWords)
{
    BandDesignIntf* band = dynamic_cast<BandDesignIntf*>(item);
    if (band){
        dataWords << band->objectName();
        dataWords << pageName+"_"+band->objectName();
        dataWords << pageName+"_"+band->objectName()+".beforeRender";
        dataWords << pageName+"_"+item->objectName()+".afterData";
        dataWords << pageName+"_"+band->objectName()+".afterRender";
        foreach (BaseDesignIntf* child, band->childBaseItems()){
            addItemToCompleater(pageName, child, dataWords);
        }
    } else {
        dataWords << item->objectName();
        dataWords << pageName+"_"+item->objectName();
        dataWords << pageName+"_"+item->objectName()+".beforeRender";
        dataWords << pageName+"_"+item->objectName()+".afterData";
        dataWords << pageName+"_"+item->objectName()+".afterRender";
    }
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

void ReportStructureCompleater::addAdditionalDatawords(DataSourceManager* dataManager){

    foreach(const QString &dsName,dataManager->dataSourceNames()){
        QStandardItem* dsNode = new QStandardItem;
        dsNode->setText(dsName);
        foreach(const QString &field, dataManager->fieldNames(dsName)){
            QStandardItem* fieldNode = new QStandardItem;
            fieldNode->setText(field);
            dsNode->appendRow(fieldNode);
        }
        m_model.invisibleRootItem()->appendRow(dsNode);
    }

    foreach (QString varName, dataManager->variableNames()) {
        QStandardItem* varNode = new QStandardItem;
        varNode->setText(varName.remove("#"));
        m_model.invisibleRootItem()->appendRow(varNode);
    }

#ifdef USE_QJSENGINE
    ScriptEngineManager& se = LimeReport::ScriptEngineManager::instance();
    QJSValue globalObject = se.scriptEngine()->globalObject();
    QJSValueIterator it(globalObject);
    while (it.hasNext()){
        it.next();
        if (it.value().isCallable() ){
            QStandardItem* itemNode = new QStandardItem;
            itemNode->setText(it.name());
            m_model.invisibleRootItem()->appendRow(itemNode);
        }
    }
#endif

}

void ReportStructureCompleater::updateCompleaterModel(ReportEnginePrivateInterface* report)
{
    if (report){
        m_model.clear();
        QIcon signalIcon(":/report/images/signal");
        QIcon propertyIcon(":/report/images/property");
        addAdditionalDatawords(report->dataManager());

        for ( int i = 0; i < report->pageCount(); ++i){
            PageDesignIntf* page = report->pageAt(i);

            QStandardItem* itemNode = new QStandardItem;
            itemNode->setText(page->pageItem()->objectName());
            itemNode->setIcon(QIcon(":/report/images/object"));
            m_model.invisibleRootItem()->appendRow(itemNode);

            QStringList items = extractSignalNames(page->pageItem());
            foreach(QString slotName, items){
                QStandardItem* slotItem = new QStandardItem;
                slotItem->setText(slotName);
                slotItem->setIcon(signalIcon);
                itemNode->appendRow(slotItem);
            }
            items = extractProperties(page->pageItem());
            foreach(QString propertyName, items){
                QStandardItem* properyItem = new QStandardItem;
                properyItem->setText(propertyName);
                properyItem->setIcon(propertyIcon);
                itemNode->appendRow(properyItem);
            }
            foreach (BaseDesignIntf* item, page->pageItem()->childBaseItems()){
                addChildItem(item, itemNode->text(), m_model.invisibleRootItem());
            }

        }
    }
}

void ReportStructureCompleater::updateCompleaterModel(DataSourceManager *dataManager)
{
    m_model.clear();
    addAdditionalDatawords(dataManager);
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

void ReportStructureCompleater::addChildItem(BaseDesignIntf *item, const QString &pageName, QStandardItem *parent)
{
    if (!item) return;

    QIcon signalIcon(":/report/images/signal");
    QIcon propertyIcon(":/report/images/property");

    QStandardItem* itemNode = new QStandardItem;
    itemNode->setText(pageName+"_"+item->objectName());
    itemNode->setIcon(QIcon(":/report/images/object"));
    parent->appendRow(itemNode);
    QStringList items;

    if (!m_signals.contains(item->metaObject()->className())){
        items = extractSignalNames(item);
        m_signals.insert(item->metaObject()->className(),items);
    } else {
        items = m_signals.value(item->metaObject()->className());
    }

    foreach(QString slotName, items){
        QStandardItem* slotItem = new QStandardItem;
        slotItem->setText(slotName);
        slotItem->setIcon(signalIcon);
        itemNode->appendRow(slotItem);
    }

    if (!m_properties.contains(item->metaObject()->className())){
        items = extractProperties(item);
        m_properties.insert(item->metaObject()->className(),items);
    } else {
        items = m_properties.value(item->metaObject()->className());
    }

    foreach(QString propertyName, items){
        QStandardItem* properyItem = new QStandardItem;
        properyItem->setText(propertyName);
        properyItem->setIcon(propertyIcon);
        itemNode->appendRow(properyItem);
    }

    foreach (BaseDesignIntf* child, item->childBaseItems()){
        addChildItem(child, pageName, parent);
    }
}

} // namespace LimeReport




