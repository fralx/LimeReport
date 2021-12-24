#ifndef LRSCRIPTEDITOR_H
#define LRSCRIPTEDITOR_H

#include <QWidget>
#include <QCompleter>
#include <QTextEdit>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStandardItemModel>
#include "lrcompletermodel.h"

namespace LimeReport{

class ReportEnginePrivateInterface;
class BaseDesignIntf;
class PageDesignIntf;
class BandDesignIntf;
class DataSourceManager;

namespace Ui {
class ScriptEditor;
}

struct CacheItem {
    QList<QSharedPointer<CompleterItem>> propsItems;
    QList<QSharedPointer<CompleterItem>> slotsItems;
};

class ReportStructureCompleater : public QCompleter{
    Q_OBJECT
public:
    explicit ReportStructureCompleater(QObject* parent = 0): QCompleter(parent){ setModel(&m_newModel);}
    explicit ReportStructureCompleater(QAbstractItemModel* model, QObject* parent = 0)
        :QCompleter(model, parent){ setModel(&m_newModel);}
public:
    // QCompleter interface
    QString pathFromIndex(const QModelIndex& index) const;
    QStringList splitPath(const QString& path) const;
    void updateCompleaterModel(ReportEnginePrivateInterface* report);
    void updateCompleaterModel(DataSourceManager* dataManager);
protected:
    QStringList extractSignalNames(BaseDesignIntf* item);
    QStringList extractProperties(BaseDesignIntf* item);
    void addChildItem(BaseDesignIntf *item, const QString &pageName, CompleterItem *parent);
    void addAdditionalDatawords(CompleterModel *model, DataSourceManager *dataManager);
private:
    CompleterModel m_newModel;
    QMap<QString, QStringList> m_properties;
    QMap<QString, QStringList> m_signals;
};

class ScriptEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptEditor(QWidget *parent = 0);
    ~ScriptEditor();
    void setReportEngine(LimeReport::ReportEnginePrivateInterface* reportEngine);
    void setReportPage(PageDesignIntf* page);
    void setPageBand(BandDesignIntf* band);
    void setTabIndention(int charCount);
    void initCompleter();
    QByteArray saveState();
    void restoreState(QByteArray state);
    void setPlainText(const QString &text);
    void setEditorFont(QFont font);
    QFont editorFont();
    QString toPlainText();
    bool hasChanges() const;
    void setHasChanges(bool hasChanges);
signals:
    void splitterMoved(int, int);
    void textChanged();
protected:
    void initEditor(DataSourceManager* dm);

private slots:
    void on_twData_doubleClicked(const QModelIndex &index);
    void on_twScriptEngine_doubleClicked(const QModelIndex &index);
    void slotOnCurrentChanged(const QModelIndex& to, const QModelIndex&);

private:
    Ui::ScriptEditor *ui;
    ReportEnginePrivateInterface* m_reportEngine;
    PageDesignIntf* m_page;
    ReportStructureCompleater* m_completer;
    int m_tabIndention;
};

} // namespace LimeReport

#endif // LRSCRIPTEDITOR_H
