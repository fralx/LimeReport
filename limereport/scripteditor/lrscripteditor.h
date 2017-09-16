#ifndef LRSCRIPTEDITOR_H
#define LRSCRIPTEDITOR_H

#include <QWidget>
#include <QCompleter>
#include <QTextEdit>
#include <QKeyEvent>
#include <QScrollBar>

namespace LimeReport{

class ReportEnginePrivate;
class BaseDesignIntf;

namespace Ui {
class ScriptEditor;
}

class ScriptEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptEditor(QWidget *parent = 0);
    ~ScriptEditor();
    void setReportEngine(ReportEnginePrivate* reportEngine);
    void initCompleter();
private:
    void addItemToCompleater(const QString& pageName, BaseDesignIntf* item, QStringList& dataWords);
private:
    Ui::ScriptEditor *ui;
    ReportEnginePrivate* m_reportEngine;
    QCompleter* m_completer;

};

} // namespace LimeReport

#endif // LRSCRIPTEDITOR_H
