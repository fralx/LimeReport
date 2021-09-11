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
#ifndef LRBUTTONLINEEDIT_H
#define LRBUTTONLINEEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>
#include <QDebug>
#include "lrtextitempropertyeditor.h"

namespace LimeReport{

class ButtonLineEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ButtonLineEditor(const QString& propertyName,QWidget *parent = 0);
    ~ButtonLineEditor();
    void setText(const QString &value);
    QString text();
signals:
    void editingFinished();
public slots:
    virtual void editButtonClicked();
    void editingByEditorFinished();
protected:
    QString propertyName(){return m_propertyName;}
private:
    QLineEdit* m_lineEdit;
    QToolButton* m_buttonEdit;
    bool m_overButton;
    QString m_propertyName;
private:
    bool eventFilter(QObject *, QEvent *);
    //QHBoxLayout* m_layout;
};

} //namespace LimeReport

#endif // LRBUTTONLINEEDIT_H
