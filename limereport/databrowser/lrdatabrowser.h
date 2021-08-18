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
#ifndef LRDATABROWSER_H
#define LRDATABROWSER_H

#include <QWidget>
#include <QTreeWidget>
#include <QToolButton>
#include <QDockWidget>

#include "lrreportdesignwidget.h"
#include "lrsqleditdialog.h"
#include "lrdatabrowsertree.h"

namespace LimeReport{

namespace Ui {
    class DataBrowser;
}

class DataBrowser : public QWidget, public IConnectionController
{
    Q_OBJECT
public:

    explicit DataBrowser(QWidget *parent = 0);
    ~DataBrowser();
    QSize sizeHint() const;
    void setReportEditor(LimeReport::ReportDesignWidget* report);
    void setMainWindow(QMainWindow* mainWindow);
    void initConnections();
    void updateDataTree();
    void updateVariablesTree();
    void closeAllDataWindows();
    void setSettings(QSettings* value, bool owned = false);
    QSettings* settings();
    QString lastError() const;
    void setLastError(const QString &lastError);

private slots:
    void slotDatasourcesChanged();
    void slotAddConnection();
    void slotDeleteConnection();
    void slotAddDataSource();
    void slotViewDatasource();
    void slotEditDatasource();
    void slotDeleteDatasource();
    void slotSQLEditingFinished(SQLEditResult result);
    void slotClear();
    void slotDataWindowClosed();
    void slotChangeConnection();
    void slotChangeConnectionState();
    void slotVariableEditorAccept(const QString &variable);
    void on_dataTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_editVariable_clicked();
    void on_deleteVariable_clicked();
    void on_addVariable_clicked();
    void on_variablesTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_errorMessage_clicked();
    void on_varToReport_clicked();
    void on_variablesTree_itemDoubleClicked(QTreeWidgetItem *item, int);

private:
    enum NameType{NameForUser, NameForReport};
    QString getDatasourceName();
    QString getConnectionName(NameType nameType);
    QString getVariable();
    bool isClosingWindows() const {return m_closingWindows;}
    QTreeWidgetItem * findByNameAndType(QString name, int itemType);
    void fillFields(QTreeWidgetItem *parentItem, LimeReport::IDataSource *dataSource);
    QDockWidget* createDataWindow(QString datasourceName);
    void closeDataWindow(QString datasourceName);
    QDockWidget* dataWindow(QString datasourceName);
    void showDataWindow(QString datasourceName);
    void removeDatasource(QString datasourceName);
    void addQuery(SQLEditResult result);
    void changeQuery(SQLEditResult result);
    void addSubQuery(SQLEditResult result);
    void changeSubQuery(SQLEditResult result);
    void addProxy(SQLEditResult result);
    void changeProxy(SQLEditResult result);
    void addCSV(SQLEditResult result);
    void changeCSV(SQLEditResult result);

    SQLEditResult::ResultMode currentDatasourceType(const QString& datasourceName);
    void applyChanges(SQLEditResult result);
    void addDatasource(SQLEditResult result);

    void addConnectionDesc(ConnectionDesc *connection);
    void changeConnectionDesc(ConnectionDesc *connection);
    bool checkConnectionDesc(ConnectionDesc *connection);
    bool containsDefaultConnection();
    void activateItem(const QString &name, DataBrowserTree::NodeType type);

private:
    Ui::DataBrowser*           ui;
    ReportDesignWidget*         m_report;
    QMap<QString,QDockWidget*>  m_dataWindows;
    QMainWindow*                m_mainWindow;
    bool                        m_closingWindows;
    QSettings*                  m_settings;
    bool                        m_ownedSettings;
    QString                     m_lastError;
};

}
#endif // LRDATABROWSER_H
