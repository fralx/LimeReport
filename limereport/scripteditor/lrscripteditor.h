#ifndef LRSCRIPTEDITOR_H
#define LRSCRIPTEDITOR_H

#include <QWidget>
#include <QCompleter>
#include <QTextEdit>
#include <QKeyEvent>
#include <QScrollBar>

namespace LimeReport{

class ReportEnginePrivateInterface;
class BaseDesignIntf;
class PageDesignIntf;
class BandDesignIntf;
class DataSourceManager;

namespace Ui {
class ScriptEditor;
}

class ScriptEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptEditor(QWidget *parent = 0);
    ~ScriptEditor();
    void setReportEngine(LimeReport::ReportEnginePrivateInterface* reportEngine);
    void setReportPage(PageDesignIntf* page);
    void setPageBand(BandDesignIntf* band);
    void initCompleter();
    QByteArray saveState();
    void restoreState(QByteArray state);
    void setPlainText(const QString &text);
    void setEditorFont(QFont font);
    QFont editorFont();
    QString toPlainText();
signals:
    void splitterMoved(int, int);
protected:
    void initEditor(DataSourceManager* dm);

private slots:
    void on_twData_doubleClicked(const QModelIndex &index);
    void on_twScriptEngine_doubleClicked(const QModelIndex &index);
    void slotOnCurrentChanged(const QModelIndex& to, const QModelIndex&);
private:
    void addItemToCompleater(const QString& pageName, BaseDesignIntf* item, QStringList& dataWords);
private:
    Ui::ScriptEditor *ui;
    ReportEnginePrivateInterface* m_reportEngine;
    PageDesignIntf* m_page;
    QCompleter* m_completer;

};

} // namespace LimeReport

#endif // LRSCRIPTEDITOR_H
