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
#include "lrcoloreditor.h"

#include <QHBoxLayout>
#include <QColorDialog>

ColorEditor::ColorEditor(QWidget *parent) :
    QWidget(parent)
{
    //m_button = new QPushButton(this);
    m_button = new QToolButton(this);
    m_button->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(m_button);
    layout->setSpacing(0);
    layout->setContentsMargins(1,1,1,1);
    setFocusProxy(m_button);
    setAutoFillBackground(true);
    setLayout(layout);
    connect(m_button,SIGNAL(clicked()),this,SLOT(slotClicked()));
}

void ColorEditor::setColor(const QColor &value)
{
    m_color=value;
}

void ColorEditor::showEvent(QShowEvent *)
{
    QPixmap pixmap(m_button->width()-8,m_button->height()-8);
    pixmap.fill(m_color);
    m_button->setIcon(QIcon(pixmap));
    m_button->setIconSize(QSize(m_button->width()-8,m_button->height()-8));
}

void ColorEditor::slotClicked()
{
    QColorDialog* dialog = new QColorDialog(this);
    dialog->setCurrentColor(m_color);
    if (dialog->exec()) m_color=dialog->currentColor();
    delete dialog;
    emit(editingFinished());
}

