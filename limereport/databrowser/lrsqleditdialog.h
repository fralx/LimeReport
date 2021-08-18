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
#ifndef LRSQLEDITDIALOG_H
#define LRSQLEDITDIALOG_H

#include <stdexcept>
#include <QDialog>
#include <QCompleter>
#include <QSettings>
#include "lrreportengine_p.h"
#include "lrdatasourcemanager.h"
#include "lrdatadesignintf.h"

namespace LimeReport{

namespace Ui {
    class SQLEditDialog;
}

struct SQLEditResult;

class SQLEditDialog : public QDialog
{
    Q_OBJECT

public:
    enum SQLDialogMode {AddMode,EditMode};
    explicit SQLEditDialog(QWidget *parent, LimeReport::DataSourceManager* dataSources,SQLDialogMode dialogMode);
    void setDataSources(LimeReport::DataSourceManager *dataSources,QString datasourceName="");
    void setDefaultConnection(QString defaultConnection);
    ~SQLEditDialog();
    QSettings* settings();
    void setSettings(QSettings* value, bool owned = false);
signals:
    void signalSqlEditingFinished(SQLEditResult result);
protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent*);
    void hideEvent(QHideEvent*);
    void check();
    void initConnections();
private slots:
    void accept();
    void slotDataSourceNameEditing();
    void on_cbSubdetail_clicked(bool checked);
    void on_rbProxy_clicked(bool checked);
    void on_rbSubQuery_clicked(bool checked);
    void on_pbAddField_clicked();
    void on_pbDelField_clicked();
    void initQueryMode();
    void initSubQueryMode();
    void initProxyMode();
    void initCSVMode();
    void slotPreviewData();
    void slotHidePreview();
private:
    void writeSetting();
    void readSettings();
    void hidePreview();
private:
    Ui::SQLEditDialog *ui;
    LimeReport::DataSourceManager* m_datasources;
    SQLDialogMode m_dialogMode;
    QString m_oldDatasourceName;
    QCompleter* m_masterDatasources;
    QString m_defaultConnection;
    QSettings* m_settings;
    bool m_ownedSettings;
    QSharedPointer<QAbstractItemModel> m_previewModel;
};

struct SQLEditResult{
    enum ResultMode{Query, SubQuery, SubProxy, CSVText, Undefined};
    QString connectionName;
    QString datasourceName;
    QString oldDatasourceName;
    QString sql;
    QString csv;
    bool subdetail;
    ResultMode resultMode;
    QString masterDatasource;
    QString childDataSource;
    SQLEditDialog::SQLDialogMode dialogMode;
    QList<LimeReport::FieldsCorrelation> fieldMap;
    QString separator;
    bool firstRowIsHeader;
};

} // namespace LimeReport

#endif // LRSQLEDITDIALOG_H
