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
#ifndef LRFONTEDITOR_H
#define LRFONTEDITOR_H

#include <QWidget>
#include <QPushButton>
#include <QToolButton>

namespace LimeReport{

class FontEditor : public QWidget
{
    Q_OBJECT
public:
    explicit FontEditor(QWidget *parent = 0);
    ~FontEditor();
    void setFontValue(const QFont &font);
    QFont fontValue();
signals:
    void editingFinished();
public slots:
    void slotButtonCliked();
private:
    QString toString(const QFont& value) const;
private:
    //QPushButton* m_button;
    QToolButton* m_button;
    QFont m_font;
};

} // namespace LimeReport

#endif // LRFONTEDITOR_H
