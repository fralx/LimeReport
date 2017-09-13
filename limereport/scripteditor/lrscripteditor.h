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

class TextEditorWithCompleater :public QTextEdit
{
    Q_OBJECT
public:
    TextEditorWithCompleater(QWidget* parent=0);
    void setCompleter(QCompleter* value);
    QCompleter* compleater() const{ return m_compleater;}
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
private:
    QString textUnderCursor() const;
private slots:
    void insertCompletion(const QString& completion);
private:
    QCompleter* m_compleater;
};

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
