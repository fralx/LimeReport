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
#include "lrcheckboxeditor.h"
#include <QDebug>
#include <QPainter>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QApplication>
#include <QStyle>

namespace LimeReport{

CheckBoxEditor::CheckBoxEditor(QWidget *parent)
    :QWidget(parent), m_editing(false)
{   
    m_checkBox = new QCheckBox(this);
    init();
}
CheckBoxEditor::CheckBoxEditor(const QString &text, QWidget *parent)
    :QWidget(parent), m_editing(false)
{
    m_checkBox = new QCheckBox(text,this);
    init();
}

CheckBoxEditor::~CheckBoxEditor(){}

void CheckBoxEditor::init()
{    
    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->addStretch();
    layout->addWidget(m_checkBox);
#ifdef HAVE_QT5
    m_checkBox->setFocusPolicy(Qt::NoFocus);
#endif
    connect(m_checkBox, SIGNAL(stateChanged(int)), this, SLOT(slotStateChanged(int)));
    layout->addStretch();
    layout->setContentsMargins(2,1,1,1);
    layout->setSpacing(0);
    setLayout(layout);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
}

void CheckBoxEditor::setEditing(bool value)
{
    m_editing=value;
}

void CheckBoxEditor::setChecked(bool value)
{
    m_checkBox->setChecked(value);
}

bool CheckBoxEditor::isChecked()
{
    return m_checkBox->isChecked();
}

void CheckBoxEditor::mousePressEvent(QMouseEvent *)
{
    m_checkBox->setChecked(!m_checkBox->isChecked());
    emit editingFinished();
}

void CheckBoxEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_Space) m_checkBox->setChecked(!m_checkBox->isChecked());
    if ((event->key() == Qt::Key_Up) || (event->key() == Qt::Key_Down)){
        emit editingFinished();
    }
    QWidget::keyPressEvent(event);
}

void CheckBoxEditor::showEvent(QShowEvent *)
{
    int border = (height() - QApplication::style()->pixelMetric(QStyle::PM_IndicatorWidth))/2
#ifdef Q_OS_MAC
            +QApplication::style()->pixelMetric(QStyle::PM_FocusFrameVMargin)
#endif
    ;
    layout()->setContentsMargins(border,0,0,0);
}

void CheckBoxEditor::slotStateChanged(int)
{
    emit editingFinished();
}

}
