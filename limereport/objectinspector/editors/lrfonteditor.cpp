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
#include "lrfonteditor.h"
#include <QHBoxLayout>
#include <QFontDialog>
#include <QDebug>

namespace LimeReport{

FontEditor::FontEditor(QWidget *parent) :
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
    setLayout(layout);
    setAutoFillBackground(true);
    connect(m_button,SIGNAL(clicked()),this,SLOT(slotButtonCliked()));
}

FontEditor::~FontEditor()
{}

void FontEditor::setFontValue(const QFont &font)
{
    m_font=font;
    m_button->setText(toString(font));
}

QFont FontEditor::fontValue()
{
    return m_font;
}

void FontEditor::slotButtonCliked()
{
    QFontDialog* dialog = new QFontDialog(this);
    dialog->setCurrentFont(m_font);
    if (dialog->exec()) m_font=dialog->currentFont();
    delete dialog;
    emit(editingFinished());
}

QString FontEditor::toString(const QFont &value) const
{
    QString attribs="[";
    if (value.bold()) (attribs=="[") ? attribs+="b":attribs+=",b";
    if (value.italic()) (attribs=="[") ? attribs+="i":attribs+=",i";
    attribs+="]";
    return value.family()+" "+QString::number(value.pointSize())+" "+attribs;
}

} // namespace LimeReport
