#include "lrscripteditor.h"
#include "ui_lrscripteditor.h"
#include <QJSValueIterator>

#include "lrdatasourcemanager.h"
#include "lrscriptenginemanager.h"
#include "lrdatadesignintf.h"
#include "lrreportengine_p.h"

namespace LimeReport{

TextEditorWithCompleater::TextEditorWithCompleater(QWidget *parent)
    : QTextEdit(parent),m_compleater(0)
{
}

void TextEditorWithCompleater::setCompleter(QCompleter *value)
{
    if (value) disconnect(value,0,this,0);
    m_compleater = value;
    if (!m_compleater) return;
    m_compleater->setWidget(this);
    m_compleater->setCompletionMode(QCompleter::PopupCompletion);
    m_compleater->setCaseSensitivity(Qt::CaseInsensitive);
    connect(m_compleater,SIGNAL(activated(QString)),this,SLOT(insertCompletion(QString)));
}

void TextEditorWithCompleater::keyPressEvent(QKeyEvent *e)
{
    if (m_compleater && m_compleater->popup()->isVisible()) {
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space);
    if (!m_compleater || !isShortcut) QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!m_compleater || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;

    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                        || eow.contains(e->text().right(1)))) {
        m_compleater->popup()->hide();
        return;
    }

    if (completionPrefix != m_compleater->completionPrefix()) {
        m_compleater->setCompletionPrefix(completionPrefix);
        m_compleater->popup()->setCurrentIndex(m_compleater->completionModel()->index(0, 0));
    }

    QRect cr = cursorRect();
    cr.setWidth(m_compleater->popup()->sizeHintForColumn(0)
                + m_compleater->popup()->verticalScrollBar()->sizeHint().width());
    m_compleater->complete(cr);

}

void TextEditorWithCompleater::focusInEvent(QFocusEvent *e)
{
    if (m_compleater) m_compleater->setWidget(this);
    QTextEdit::focusInEvent(e);
}

QString TextEditorWithCompleater::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void TextEditorWithCompleater::insertCompletion(const QString &completion)
{
    if (m_compleater->widget() != this)
             return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - m_compleater->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

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
