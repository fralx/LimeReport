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
    QByteArray saveState();
    void restoreState(QByteArray state);
    void setPlainText(const QString &text);
    QString toPlainText();
private slots:
    void on_twData_doubleClicked(const QModelIndex &index);

    void on_twScriptEngine_doubleClicked(const QModelIndex &index);

private:
    void addItemToCompleater(const QString& pageName, BaseDesignIntf* item, QStringList& dataWords);
private:
    Ui::ScriptEditor *ui;
    ReportEnginePrivate* m_reportEngine;
    QCompleter* m_completer;

};

} // namespace LimeReport

#endif // LRSCRIPTEDITOR_H
