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
#include <QHBoxLayout>
#include <QToolButton>
#include <QComboBox>
#include <QLineEdit>
#include <QDebug>
#include <QEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include "lrcomboboxeditor.h"

namespace LimeReport{

ComboBoxEditor::ComboBoxEditor(QWidget *parent, bool clearable) :
    QWidget(parent),
    m_comboBox(new InternalComboBox(this)),
    m_buttonClear(0),
    m_settingValues(false)
{
    setFocusProxy(m_comboBox);

    if (clearable) {
        m_buttonClear = new QToolButton(this);
        m_buttonClear->setIcon(QIcon(":/items/clear.png"));
        m_buttonClear->installEventFilter(this);
        m_buttonClear->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
        m_buttonClear->setMaximumHeight(QWIDGETSIZE_MAX);
        connect(m_buttonClear,SIGNAL(clicked()),this,SLOT(slotClearButtonClicked()));
    }

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    connect(m_comboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(slotCurrentIndexChanged(QString)));
#else
    connect(m_comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotCurrentIndexChanged(QString)));
#endif

    m_comboBox->installEventFilter(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_comboBox);
    if (clearable)
        layout->addWidget(m_buttonClear);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(2);
    setLayout(layout);
    setAutoFillBackground(true);
}

void ComboBoxEditor::addItems(const QStringList &values){
    m_settingValues = true;
    m_comboBox->addItems(values);
    m_settingValues = false;
}

void ComboBoxEditor::setTextValue(const QString &value){
    m_settingValues = true;
    if (m_comboBox->findText(value)>0){
        m_comboBox->setCurrentIndex(m_comboBox->findText(value));
    } else {
        m_comboBox->setEditText(value);
    }
    m_settingValues = false;
}

void ComboBoxEditor::slotClearButtonClicked(){
    m_comboBox->setCurrentIndex(-1);
}

void ComboBoxEditor::slotCurrentIndexChanged(const QString& value)
{
    if (!m_settingValues){
        emit currentIndexChanged(value);
    }
}

QString ComboBoxEditor::text(){
    return m_comboBox->currentText();
}

void ComboBoxEditor::setEditable(bool value)
{
    if (m_comboBox) {
        m_comboBox->setEditable(value);
    }
}

bool ComboBoxEditor::eventFilter(QObject *target, QEvent *event){
    if (target == m_buttonClear){
        if (event->type()==QEvent::FocusOut){
            if (static_cast<QFocusEvent*>(event)->reason()!=Qt::MouseFocusReason){
                m_comboBox->setFocus();
            }
        }
        QSet<int> enterKeys;
        enterKeys.insert(Qt::Key_Enter);
        enterKeys.insert(Qt::Key_Return);

        if (event->type()==QEvent::KeyPress){
            if (enterKeys.contains(static_cast<QKeyEvent*>(event)->key())){
                m_buttonClear->click();
                return true;
            }
        }
    }

    if (target == m_comboBox){
        if (event->type() == QEvent::FocusOut){
            if (!m_comboBox->isPopup() || (m_buttonClear && m_buttonClear->hasFocus()))
                emit editingFinished();
        }
    }
    return QWidget::eventFilter(target,event);
}


void ComboBoxEditor::resizeEvent(QResizeEvent *e)
{
    if (m_buttonClear)
        m_buttonClear->setMinimumHeight(e->size().height()-4);
}

} // namespace LimeReport
