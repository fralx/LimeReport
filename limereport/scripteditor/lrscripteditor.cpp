#include "lrscripteditor.h"
#include "ui_lrscripteditor.h"
#include <QJSValueIterator>

#include "lrdatasourcemanager.h"
#include "lrscriptenginemanager.h"
#include "lrdatadesignintf.h"
#include "lrreportengine_p.h"

namespace LimeReport{

ScriptEditor::ScriptEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptEditor)
{
    ui->setupUi(this);
    m_completer = new QCompleter(this);
    ui->textEdit->setCompleter(m_completer);
}

ScriptEditor::~ScriptEditor()
{
    delete ui;
}

void ScriptEditor::setReportEngine(ReportEnginePrivate* reportEngine)
{
    m_reportEngine = reportEngine;
    DataSourceManager* dm = m_reportEngine->dataManager();
    ScriptEngineManager& se = LimeReport::ScriptEngineManager::instance();
    se.setDataManager(dm);

    initCompleter();

    if (dm){
        if (dm->isNeedUpdateDatasourceModel())
           dm->updateDatasourceModel();
        ui->twData->setModel(dm->datasourcesModel());
        ui->twScriptEngine->setModel(se.model());
    }

}

void ScriptEditor::initCompleter()
{
    QStringList dataWords;
    DataSourceManager* dm = m_reportEngine->dataManager();
    ScriptEngineManager& se = LimeReport::ScriptEngineManager::instance();

    QJSValue globalObject = se.scriptEngine()->globalObject();
    QJSValueIterator it(globalObject);
    while (it.hasNext()){
        it.next();
        if (it.value().isCallable() ){
            dataWords << it.name();
        }
    }

    foreach(const QString &dsName,dm->dataSourceNames()){
        dataWords << dsName;
        foreach(const QString &field, dm->fieldNames(dsName)){
            dataWords<<dsName+"."+field;
        }
    }

    foreach (QString varName, dm->variableNames()) {
        dataWords << varName.remove("#");
    }

    for ( int i = 0; i < m_reportEngine->pageCount(); ++i){
        PageDesignIntf* page = m_reportEngine->pageAt(i);
        dataWords << page->pageItem()->objectName();
        foreach (BaseDesignIntf* item, page->pageItem()->childBaseItems()){
            addItemToCompleater(page->pageItem()->objectName(), item, dataWords);
        }
    }

    m_completer->setModel(new QStringListModel(dataWords,m_completer));
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

} // namespace LimeReport
