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
#include "lrconnectiondialog.h"
#include "ui_lrconnectiondialog.h"
#include "lrglobal.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <stdexcept>
#include <QFileDialog>

namespace LimeReport{

ConnectionDialog::ConnectionDialog(LimeReport::IConnectionController *conControl, LimeReport::ConnectionDesc* connectionDesc, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionDialog), m_connection(connectionDesc), m_controller(conControl), m_savedConnectionName("")
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose,true);
    m_changeMode=m_connection!=0;
}

ConnectionDialog::~ConnectionDialog()
{
    delete ui;
}

void ConnectionDialog::init()
{
   ui->cbbDrivers->addItems(QSqlDatabase::drivers());
   ui->cbbUseDefaultConnection->setEnabled(!m_controller->containsDefaultConnection());
}

void ConnectionDialog::showEvent(QShowEvent *)
{
    connectionToUI();
}

void ConnectionDialog::slotAccept()
{
    try {
        checkFieldsFill();
        if (ui->cbAutoConnect->isChecked()) checkConnection();
        if (!m_connection){
            m_controller->addConnectionDesc(uiToConnection());
        } else {
            m_controller->changeConnectionDesc(uiToConnection(m_connection));
        }
        close();
    }
    catch(LimeReport::ReportError &exception){
        QMessageBox::critical(this,tr("Error"),exception.what());
    }
}

void ConnectionDialog::slotCheckConnection()
{
    try{
        checkConnection();
        QMessageBox::information(this,tr("Connection"),tr("Connection succsesfully established!"));
    } catch(LimeReport::ReportError &exception) {
        QMessageBox::critical(this,tr("Error"),exception.what());
    }
}

void ConnectionDialog::checkFieldsFill()
{
    if (ui->leConnectionName->text().isEmpty()){throw LimeReport::ReportError(tr("Connection Name is empty"));}
    if (!m_changeMode&&QSqlDatabase::connectionNames().contains(ui->leConnectionName->text())) {
        throw LimeReport::ReportError(tr("Connection with name ")+ui->leConnectionName->text()+tr(" already exists! "));
    }
}

bool ConnectionDialog::checkConnection()
{
    QScopedPointer<LimeReport::ConnectionDesc> con(uiToConnection());
    if (!m_controller->checkConnectionDesc(con.data())){
        throw LimeReport::ReportError(m_controller->lastError());
    }
    return true;
}

ConnectionDesc *ConnectionDialog::uiToConnection(LimeReport::ConnectionDesc* conDesc)
{
    LimeReport::ConnectionDesc* result;
    if (conDesc)
        result = conDesc;
    else
        result = new LimeReport::ConnectionDesc();
    result ->setName(ConnectionDesc::connectionNameForReport(ui->leConnectionName->text()));
    result ->setHost(ui->leServerName->text());
    if (!ui->lePort->text().isEmpty())
        result->setPort(ui->lePort->text());
    result ->setDriver(ui->cbbDrivers->currentText());
    result ->setUserName(ui->leUserName->text());
    result ->setPassword(ui->lePassword->text());
    result ->setDatabaseName(ui->leDataBase->text());
    result ->setAutoconnect(ui->cbAutoConnect->isChecked());
    result->setKeepDBCredentials(!ui->cbbKeepCredentials->isChecked());
    return result ;
}

void ConnectionDialog::connectionToUI()
{
    init();
    if (!m_connection) return;
    ui->leConnectionName->setText(ConnectionDesc::connectionNameForUser(m_connection->name()));
    ui->cbbUseDefaultConnection->setChecked(m_connection->name().compare(QSqlDatabase::defaultConnection) == 0);
    ui->leDataBase->setText(m_connection->databaseName());
    ui->leServerName->setText(m_connection->host());
    ui->leUserName->setText(m_connection->userName());
    ui->lePassword->setText(m_connection->password());
    ui->cbbDrivers->setCurrentIndex(ui->cbbDrivers->findText(m_connection->driver()));
    ui->cbAutoConnect->setChecked(m_connection->autoconnect());
    ui->cbbKeepCredentials->setChecked(!m_connection->keepDBCredentials());
    ui->lePort->setText(m_connection->port());
}

void ConnectionDialog::on_toolButton_clicked()
{
    ui->leDataBase->setText(QFileDialog::getOpenFileName());
}

void ConnectionDialog::on_cbbUseDefaultConnection_toggled(bool checked)
{
    if (checked){
        m_savedConnectionName = ui->leConnectionName->text();
        ui->leConnectionName->setText(tr("defaultConnection"));
        ui->leConnectionName->setEnabled(false);
    } else {
        ui->leConnectionName->setText(m_savedConnectionName);
        ui->leConnectionName->setEnabled(true);
    }
}
void ConnectionDialog::on_toolButton_2_toggled(bool checked)
{
    ui->lePassword->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

} // namespace LimeReport







