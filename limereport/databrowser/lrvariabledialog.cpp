/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2015 by Alexander Arin                                  *
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
#include "lrvariabledialog.h"
#include "ui_lrvariabledialog.h"
#include "lrglobal.h"
#include <stdexcept>
#include <QMessageBox>

LRVariableDialog::LRVariableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LRVariableDialog),
    m_variableName(""),
    m_variablesContainer(0),
    m_changeMode(false),
    m_oldVariableName("")
{
    ui->setupUi(this);
    ui->cbbType->setVisible(false);
    ui->lblType->setVisible(false);
}

LRVariableDialog::~LRVariableDialog()
{
    delete ui;
}

void LRVariableDialog::setVariableContainer(LimeReport::IVariablesContainer *value)
{
    m_variablesContainer=value;
}

void LRVariableDialog::setVariableName(const QString &value)
{
    m_variableName=value;
    m_changeMode=true;
    m_oldVariableName=value;
}

void LRVariableDialog::showEvent(QShowEvent *)
{
    ui->leName->setText(m_variableName);
    if (!m_variableName.isEmpty()&&m_variablesContainer&&m_variablesContainer->containsVariable(m_variableName)){
        ui->leValue->setText(m_variablesContainer->variable(m_variableName).toString());
    }
}

void LRVariableDialog::accept()
{
    try{
    if (m_variablesContainer&&!ui->leName->text().isEmpty()){
        if (m_changeMode){
            if (m_oldVariableName==ui->leName->text()){
                m_variablesContainer->changeVariable(m_oldVariableName,value());
            } else {
                m_variablesContainer->deleteVariable(m_oldVariableName);
                m_variablesContainer->addVariable(ui->leName->text(),value(), LimeReport::VarDesc::Report);
            }
        } else {
            m_variablesContainer->addVariable(ui->leName->text(),value(), LimeReport::VarDesc::Report);
        }
        emit signalVariableAccepted(ui->leName->text());
        QDialog::accept();
    }
    } catch (LimeReport::ReportError &exception){
        QMessageBox::critical(this,tr("Attention"),exception.what());
    }
}

QVariant LRVariableDialog::value()
{
    return ui->leValue->text();
}
