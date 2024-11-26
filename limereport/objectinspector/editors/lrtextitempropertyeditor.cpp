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
#include "lrtextitempropertyeditor.h"
#include "ui_ltextitempropertyeditor.h"
#include <QCompleter>
#include <QListWidget>

namespace LimeReport{

TextItemPropertyEditor::TextItemPropertyEditor(QWidget *parent, const QStringList &availVars) :
    QDialog(parent),
    ui(new Ui::TextItemPropertyEditor)
{
    ui->setupUi(this);
    ui->textEdit->setAcceptRichText(false);
    ui->listVars->setVisible(!availVars.isEmpty());
    ui->listVars->addItems(availVars);
    connect(ui->listVars, &QListWidget::itemClicked, this,
            [this](QListWidgetItem *item) {
                ui->textEdit->setPlainText(item->text());
    });
}

TextItemPropertyEditor::~TextItemPropertyEditor()
{
    delete ui;
}

void TextItemPropertyEditor::setText(const QString &value)
{
    ui->textEdit->setPlainText(value);
}

QString TextItemPropertyEditor::text()
{
    return ui->textEdit->toPlainText();
}

} //namespace LimeReport
