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
#ifndef LRCHECKBOXEDITOR_H
#define LRCHECKBOXEDITOR_H

#include <QCheckBox>

namespace LimeReport{

class CheckBoxEditor : public QWidget
{
    Q_OBJECT

public:
    CheckBoxEditor(QWidget * parent = 0);
    CheckBoxEditor(const QString & text, QWidget * parent = 0);
    ~CheckBoxEditor();
    void setEditing(bool value);
    void setChecked(bool value);
    bool isChecked();
protected:
    void mousePressEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *event);
    void showEvent(QShowEvent *);
signals:
    void editingFinished();
private slots:
    void slotStateChanged(int);
private:
    QCheckBox* m_checkBox;
    bool m_editing;
    void init();

};

}
#endif // LRCHECKBOXEDITOR_H
