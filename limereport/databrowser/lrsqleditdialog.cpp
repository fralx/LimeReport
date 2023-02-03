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
#include "lrsqleditdialog.h"
#include "lrreportengine_p.h"
#include "ui_lrsqleditdialog.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>

namespace LimeReport {

SQLEditDialog::SQLEditDialog(QWidget *parent, LimeReport::DataSourceManager *dataSources, SQLDialogMode dialogMode)
    : QDialog(parent), ui(new Ui::SQLEditDialog), m_datasources(dataSources), m_dialogMode(dialogMode),
      m_oldDatasourceName(""), m_settings(0), m_ownedSettings(false) {
    ui->setupUi(this);
    m_masterDatasources = new QCompleter(this);
    ui->leMaster->setCompleter(m_masterDatasources);
    ui->leChild->setCompleter(m_masterDatasources);
    ui->gbFieldsMap->setVisible(false);
    ui->gbDataPreview->setVisible(false);
    ui->pbHidePreview->setVisible(false);
    ui->rbSubQuery->setChecked(true);
    ui->rbProxy->setVisible(false);
    ui->rbSubQuery->setVisible(false);
    ui->leMaster->setVisible(false);
    ui->lbMaster->setVisible(false);
    ui->fieldsMap->setHorizontalHeaderItem(0, new QTableWidgetItem("master"));
    ui->fieldsMap->setHorizontalHeaderItem(1, new QTableWidgetItem("detail"));

    ui->fieldsMap->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->pnlChildDatasource->setVisible(false);
    connect(ui->pbPreview, SIGNAL(pressed()), this, SLOT(slotPreviewData()));
    connect(ui->pbHidePreview, SIGNAL(pressed()), this, SLOT(slotHidePreview()));
}

SQLEditDialog::~SQLEditDialog() {
    delete ui;
    if (m_settings && m_ownedSettings)
        delete m_settings;
}

QSettings *SQLEditDialog::settings() {
    if (m_settings) {
        return m_settings;
    } else {
        m_settings = new QSettings("LimeReport", QCoreApplication::applicationName());
        m_ownedSettings = true;
        return m_settings;
    }
}

void SQLEditDialog::setSettings(QSettings *value, bool owned) {
    if (m_settings && m_ownedSettings)
        delete m_settings;
    m_settings = value;
    m_ownedSettings = owned;
}

void SQLEditDialog::accept() {
    SQLEditResult result;

    if (ui->tabWidget->currentIndex() == 1) {
        result.resultMode = SQLEditResult::CSVText;
    } else if (!ui->cbSubdetail->isChecked()) {
        result.resultMode = SQLEditResult::Query;
    } else {
        if (ui->rbSubQuery->isChecked())
            result.resultMode = SQLEditResult::SubQuery;
        else
            result.resultMode = SQLEditResult::SubProxy;
    }

    result.connectionName = ConnectionDesc::connectionNameForReport(ui->cbbConnection->currentText());
    result.datasourceName = ui->leDatasourceName->text();
    result.sql = ui->sqlText->toPlainText();
    result.csv = ui->csvText->toPlainText();
    result.dialogMode = m_dialogMode;
    result.oldDatasourceName = m_oldDatasourceName;
    result.subdetail = ui->cbSubdetail->isChecked();
    result.masterDatasource = ui->leMaster->text();
    result.childDataSource = ui->leChild->text();
    result.separator = ui->leSeparator->text();
    result.firstRowIsHeader = ui->cbUseFirstRowAsHeader->isChecked();

    if (ui->fieldsMap->rowCount() > 0) {
        for (int i = 0; i < ui->fieldsMap->rowCount(); ++i) {
            LimeReport::FieldsCorrelation fieldsCorrelation;
            fieldsCorrelation.master =
                ui->fieldsMap->item(i, 0) ? ui->fieldsMap->item(i, 0)->data(Qt::DisplayRole).toString() : "";
            fieldsCorrelation.detail =
                ui->fieldsMap->item(i, 1) ? ui->fieldsMap->item(i, 1)->data(Qt::DisplayRole).toString() : "";
            result.fieldMap.append(fieldsCorrelation);
        }
    }

    try {
        check();
        emit signalSqlEditingFinished(result);
        QDialog::accept();
    } catch (LimeReport::ReportError &exception) {
        QMessageBox::critical(this, tr("Error"), exception.what());
    }
}

void SQLEditDialog::showEvent(QShowEvent *) {
    ui->lblInfo->setVisible(false);
    initConnections();
    readSettings();
}

void SQLEditDialog::closeEvent(QCloseEvent *) { writeSetting(); }

void SQLEditDialog::hideEvent(QHideEvent *) { writeSetting(); }

void SQLEditDialog::check() {
    if (ui->leDatasourceName->text().isEmpty())
        throw LimeReport::ReportError(tr("Datasource Name is empty!"));
    if (ui->sqlText->toPlainText().isEmpty() && (!ui->rbProxy))
        throw LimeReport::ReportError(tr("SQL is empty!"));
    if (m_dialogMode == AddMode) {
        if (m_datasources->containsDatasource(ui->leDatasourceName->text())) {
            throw LimeReport::ReportError(
                QString(tr("Datasource with name: \"%1\" already exists!")).arg(ui->leDatasourceName->text()));
        }
    }
}

void SQLEditDialog::initConnections() {
    foreach (QString connectionName, QSqlDatabase::connectionNames()) {
        ui->cbbConnection->addItem(QIcon(":/databrowser/images/plug-connect.png"),
                                   ConnectionDesc::connectionNameForUser(connectionName));
    }

    foreach (QString connectionName, m_datasources->connectionNames()) {
        connectionName =
            (connectionName.compare(QSqlDatabase::defaultConnection) == 0) ? tr("defaultConnection") : connectionName;
        if (ui->cbbConnection->findText(connectionName, Qt::MatchExactly) == -1)
            ui->cbbConnection->addItem(QIcon(":/databrowser/images/plug-disconnect.png"),
                                       ConnectionDesc::connectionNameForUser(connectionName));
    }

    ui->cbbConnection->setCurrentIndex(ui->cbbConnection->findText(m_defaultConnection));
    if (!m_oldDatasourceName.isEmpty()) {
        ui->cbbConnection->setCurrentIndex(ui->cbbConnection->findText(
            ConnectionDesc::connectionNameForUser(m_datasources->connectionName(m_oldDatasourceName))));
    }
}

void SQLEditDialog::setDataSources(LimeReport::DataSourceManager *dataSources, QString datasourceName) {
    m_datasources = dataSources;
    if (!datasourceName.isEmpty()) {
        ui->cbSubdetail->setEnabled(true);
        m_oldDatasourceName = datasourceName;
        ui->leDatasourceName->setText(datasourceName);
        ui->sqlText->setText(dataSources->queryText(datasourceName));
        if (dataSources->isQuery(datasourceName)) {
            initQueryMode();
        }
        if (dataSources->isSubQuery(datasourceName)) {
            initSubQueryMode();
            ui->leMaster->setText(dataSources->subQueryByName(datasourceName)->master());
        }
        if (dataSources->isProxy(datasourceName)) {
            initProxyMode();
            LimeReport::ProxyDesc *proxyDesc = dataSources->proxyByName(datasourceName);
            ui->leChild->setText(proxyDesc->child());
            ui->leMaster->setText(proxyDesc->master());
            int curIndex = 0;
            foreach (LimeReport::FieldMapDesc *fields, *proxyDesc->fieldsMap()) {
                ui->fieldsMap->setRowCount(curIndex + 1);
                ui->fieldsMap->setItem(curIndex, 0, new QTableWidgetItem(fields->master()));
                ui->fieldsMap->setItem(curIndex, 1, new QTableWidgetItem(fields->detail()));
                curIndex++;
            }
        }
        if (dataSources->isCSV(datasourceName)) {
            ui->csvText->setPlainText(dataSources->csvByName(datasourceName)->csvText());
            ui->leSeparator->setText(dataSources->csvByName(datasourceName)->separator());
            ui->cbUseFirstRowAsHeader->setChecked(dataSources->csvByName(datasourceName)->firstRowIsHeader());
            initCSVMode();
        }
    }
}

void SQLEditDialog::setDefaultConnection(QString defaultConnection) {
    m_defaultConnection = ConnectionDesc::connectionNameForUser(defaultConnection);
}

void SQLEditDialog::slotDataSourceNameEditing() {
    if (m_dialogMode == AddMode) {
        QPalette palette = ui->leDatasourceName->palette();
        if (m_datasources->containsDatasource(ui->leDatasourceName->text())) {
            palette.setColor(QPalette::Text, Qt::red);
            ui->leDatasourceName->setPalette(palette);
            ui->lblInfo->setText(
                QString(tr("Datasource with name %1 already exist")).arg(ui->leDatasourceName->text()));
            ui->lblInfo->setVisible(true);
        } else {
            palette.setColor(QPalette::Text, QApplication::palette().text().color());
            ui->leDatasourceName->setPalette(palette);
            ui->lblInfo->setVisible(false);
        }
    }
}

void SQLEditDialog::on_cbSubdetail_clicked(bool checked) {
    if (checked) {
        m_masterDatasources->setModel(new QStringListModel(m_datasources->dataSourceNames(), m_datasources));
    }
    ui->leMaster->setEnabled(checked);
    ui->rbProxy->setEnabled(checked);
    ui->rbSubQuery->setEnabled(checked);
    if ((checked) && (ui->rbProxy->isChecked()))
        initProxyMode();
    if ((checked) && (ui->rbSubQuery->isChecked()))
        initSubQueryMode();
    if (!checked)
        initQueryMode();
}

void SQLEditDialog::on_rbProxy_clicked(bool checked) {
    if (checked)
        initProxyMode();
}

void SQLEditDialog::on_rbSubQuery_clicked(bool checked) {
    if (checked)
        initSubQueryMode();
}

void SQLEditDialog::on_pbAddField_clicked() { ui->fieldsMap->setRowCount(ui->fieldsMap->rowCount() + 1); }

void SQLEditDialog::initQueryMode() {
    ui->gbSQL->setVisible(true);
    ui->gbFieldsMap->setVisible(false);
    ui->pnlChildDatasource->setVisible(false);
    ui->rbSubQuery->setVisible(false);
    ui->rbProxy->setVisible(false);
    ui->cbSubdetail->setChecked(false);
    ui->leMaster->setVisible(false);
    ui->lbMaster->setVisible(false);
    // ui->tabWidget->removeTab(1);
    ui->tabWidget->addTab(ui->csvTab, tr("CSV"));
}

void SQLEditDialog::initSubQueryMode() {
    ui->gbSQL->setVisible(true);
    ui->gbFieldsMap->setVisible(false);
    ui->pnlChildDatasource->setVisible(false);
    ui->rbSubQuery->setChecked(true);
    ui->cbSubdetail->setChecked(true);
    ui->rbSubQuery->setVisible(true);
    ui->rbProxy->setVisible(true);
    ui->leMaster->setVisible(true);
    ui->leMaster->setEnabled(true);
    ui->lbMaster->setVisible(true);
    ui->tabWidget->removeTab(1);
}

void SQLEditDialog::initProxyMode() {
    ui->gbSQL->setVisible(false);
    ui->gbFieldsMap->setVisible(true);
    ui->pnlChildDatasource->setVisible(true);
    ui->rbProxy->setChecked(true);
    ui->cbSubdetail->setChecked(true);
    ui->rbSubQuery->setVisible(true);
    ui->rbProxy->setVisible(true);
    ui->leMaster->setVisible(true);
    ui->leMaster->setEnabled(true);
    ui->lbMaster->setVisible(true);
    ui->cbSubdetail->setEnabled(false);
    ui->tabWidget->removeTab(1);
}

void SQLEditDialog::initCSVMode() { ui->tabWidget->setCurrentWidget(ui->csvTab); }

void SQLEditDialog::slotPreviewData() {
    if (ui->cbbConnection->currentText().isEmpty()) {
        QMessageBox::critical(this, tr("Attention"), tr("Connection is not specified"));
        return;
    }
    m_previewModel =
        m_datasources->previewSQL(ConnectionDesc::connectionNameForReport(ui->cbbConnection->currentText()),
                                  ui->sqlText->toPlainText(), ui->leMaster->text());
    if (m_previewModel) {
        ui->tvPreview->setModel(m_previewModel.data());
        ui->gbDataPreview->setVisible(true);
        ui->pbPreview->setText(tr("Refresh"));
        ui->pbHidePreview->setVisible(true);
    } else {
        if (ui->gbDataPreview->isVisible())
            hidePreview();
        QMessageBox::critical(this, tr("Attention"), m_datasources->lastError());
    }
}

void SQLEditDialog::slotHidePreview() { hidePreview(); }

void SQLEditDialog::writeSetting() {
    if (settings() != 0) {
        settings()->beginGroup("SQLEditor");
        settings()->setValue("Geometry", saveGeometry());
        settings()->endGroup();
    }
}

void SQLEditDialog::readSettings() {
    if (settings() == 0)
        return;
    settings()->beginGroup("SQLEditor");
    QVariant v = settings()->value("Geometry");
    if (v.isValid()) {
        restoreGeometry(v.toByteArray());
    }
    settings()->endGroup();
}

void SQLEditDialog::hidePreview() {
    ui->gbDataPreview->setVisible(false);
    ui->pbPreview->setText(tr("Preview"));
    ui->pbHidePreview->setVisible(false);
}

void SQLEditDialog::on_pbDelField_clicked() { ui->fieldsMap->removeRow(ui->fieldsMap->currentRow()); }

} // namespace LimeReport
