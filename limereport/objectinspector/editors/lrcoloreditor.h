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
#ifndef LRCOLOREDITOR_H
#define LRCOLOREDITOR_H

#include <QWidget>
#include <QPushButton>
#include <QToolButton>

namespace LimeReport{

class ColorIndicator : public QWidget{
    Q_OBJECT
public:
    ColorIndicator(QWidget* parent = 0);
    QColor color() const;
    void setColor(const QColor &color);
    QSize sizeHint() const;
protected:
    void paintEvent(QPaintEvent *event);
private:
    QColor m_color;
};

class ColorEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ColorEditor(QWidget *parent = 0);
    QColor color(){return m_color;}
    void setColor(const QColor& value);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
private:
    void setFocusToParent();
signals:
   void editingFinished();
private slots:
   void slotClicked();
private:
   QColor m_color;
   QToolButton* m_button;
   ColorIndicator* m_colorIndicator;
   bool m_buttonPressed;
};

} // namespace LimeReport

#endif // LRCOLOREDITOR_H
