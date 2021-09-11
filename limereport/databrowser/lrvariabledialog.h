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
#ifndef LRVARIABLEDIALOG_H
#define LRVARIABLEDIALOG_H

#include <QDialog>
#include "lrvariablesholder.h"

namespace Ui {
class LRVariableDialog;
}

class LRVariableDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LRVariableDialog(QWidget *parent = 0);
    ~LRVariableDialog();
    void setVariableContainer(LimeReport::IVariablesContainer *value);
    void setVariableName(const QString &value);
protected:
    void showEvent(QShowEvent *);
private slots:
    void accept();
signals:
    void signalVariableAccepted(const QString &variable);
private:
    QVariant value();
private:
    Ui::LRVariableDialog *ui;
    QString m_variableName;
    LimeReport::IVariablesContainer* m_variablesContainer;
    bool m_changeMode;
    QString m_oldVariableName;
};

#endif // LRVARIABLEDIALOG_H
