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
#include "lrbuttonlineeditor.h"
#include <QMessageBox>
#include <QEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QApplication>
#include <QStyle>
#if QT_VERSION < QT_VERSION_CHECK(5,12,3)
#include <QDesktopWidget>
#else
#include <QScreen>
#endif
#include "lrtextitempropertyeditor.h"

namespace LimeReport{

ButtonLineEditor::ButtonLineEditor(const QString &propertyName, QWidget *parent) :
    QWidget(parent), m_overButton(false), m_propertyName(propertyName)
{
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->installEventFilter(this);
    setFocusProxy(m_lineEdit);
    m_buttonEdit = new QToolButton(this);
    m_buttonEdit->setText("...");
    m_buttonEdit->installEventFilter(this);
    m_buttonEdit->setAttribute(Qt::WA_Hover);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_buttonEdit);
    layout->setContentsMargins(1,1,1,1);
    layout->setSpacing(0);
    setAutoFillBackground(true);
    connect(m_buttonEdit,SIGNAL(clicked()),this,SLOT(editButtonClicked()));
    //connect(m_lineEdit,SIGNAL(editingFinished()),this,SLOT(lineEditEditingFinished()));
}

ButtonLineEditor::~ButtonLineEditor(){}

void ButtonLineEditor::editButtonClicked()
{
    TextItemPropertyEditor* editor = new TextItemPropertyEditor(QApplication::activeWindow());
    editor->setAttribute(Qt::WA_DeleteOnClose);
#if QT_VERSION < QT_VERSION_CHECK(5,12,3)
    editor->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, editor->size(), QApplication::desktop()->availableGeometry()));
#else
    editor->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, editor->size(), QGuiApplication::screens().first()->availableGeometry()));
#endif
    editor->setWindowTitle(m_propertyName);
    editor->setText(m_lineEdit->text());
    connect(editor,SIGNAL(accepted()),this,SLOT(editingByEditorFinished()));
    editor->exec();
}

void ButtonLineEditor::setText(const QString &value){
    m_lineEdit->setText(value);
}

QString ButtonLineEditor::text()
{
    return m_lineEdit->text();
}

bool ButtonLineEditor::eventFilter(QObject *target, QEvent *event)
{

    if (target==m_buttonEdit) {

        if (event->type()==QEvent::HoverEnter){
            m_overButton=true;
        }
        if (event->type()==QEvent::HoverLeave){
            m_overButton=false;
        }
        if (event->type()==QEvent::FocusOut){
            if (static_cast<QFocusEvent*>(event)->reason()!=Qt::MouseFocusReason){
                m_lineEdit->setFocus();
            }
        }
        QSet<int> enterKeys;
        enterKeys.insert(Qt::Key_Enter);
        enterKeys.insert(Qt::Key_Return);

        if (event->type()==QEvent::KeyPress){
            if (enterKeys.contains(static_cast<QKeyEvent*>(event)->key())){
                m_buttonEdit->click();
                return true;
            }
        }
    }
    if (target==m_lineEdit){
        if (event->type()==QEvent::FocusOut){
            switch (static_cast<QFocusEvent*>(event)->reason()){
            case Qt::TabFocusReason:
                m_overButton=true;
                break;
            case Qt::MouseFocusReason:
                break;
            default:
                m_overButton=false;
            }

        }
    }
    return QWidget::eventFilter(target,event);

}

void ButtonLineEditor::editingByEditorFinished()
{
    setText(qobject_cast<TextItemPropertyEditor*>(sender())->text());
    m_lineEdit->setFocus();
    emit editingFinished();
}

} //namespace LimeReport
