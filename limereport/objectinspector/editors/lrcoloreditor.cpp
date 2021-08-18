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
#include "lrcoloreditor.h"
#include "lrglobal.h"

#include <QHBoxLayout>
#include <QColorDialog>
#include <QPaintEvent>
#include <QPainter>
#include <QApplication>
#include <QStyle>

namespace LimeReport{

ColorEditor::ColorEditor(QWidget *parent) :
    QWidget(parent), m_buttonPressed(false)
{
    m_colorIndicator = new ColorIndicator(this);
    m_colorIndicator->setColor(m_color);
    m_button = new QToolButton(this);
    m_button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_button->setText("...");
    m_button->installEventFilter(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(m_colorIndicator);
    layout->addWidget(m_button);
    layout->setSpacing(0);
    layout->setContentsMargins(1,1,1,1);
    setFocusProxy(m_button);
    setAutoFillBackground(true);
    setLayout(layout);
    setAutoFillBackground(true);
    connect(m_button,SIGNAL(clicked()),this,SLOT(slotClicked()));
}

void ColorEditor::setColor(const QColor &value)
{
    m_color=value;
    m_colorIndicator->setColor(m_color);
}

bool ColorEditor::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_button){
        if (event->type() == QEvent::FocusOut && !m_buttonPressed){
            QFocusEvent* focusEvent = dynamic_cast<QFocusEvent*>(event);
            if (focusEvent && focusEvent->reason()!=Qt::MouseFocusReason){
                setFocusToParent();
                emit(editingFinished());
            }
            return false;
        }
    }
    return false;
}

void ColorEditor::setFocusToParent(){
    if (parentWidget())
        parentWidget()->setFocus();
}

void ColorEditor::slotClicked()
{
    m_buttonPressed = true;
    QColorDialog* dialog = new QColorDialog(this);
    dialog->setCurrentColor(m_color);
    if (dialog->exec()) m_color=dialog->currentColor();
    delete dialog;
    setFocusToParent();
    emit(editingFinished());
}

void ColorIndicator::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    QStyle* style = QApplication::style();
    painter.save();
    painter.setBrush(m_color);
    QColor penColor = isColorDark(m_color) ? Qt::transparent : Qt::darkGray;

    painter.setPen(penColor);
    int border = (event->rect().height() - style->pixelMetric(QStyle::PM_IndicatorWidth))/2;

    QRect rect(event->rect().x()+border, event->rect().y()+border,
               style->pixelMetric(QStyle::PM_IndicatorWidth),
               style->pixelMetric(QStyle::PM_IndicatorWidth));// = option.rect.adjusted(4,4,-4,-6);

    painter.drawRect(rect);
    painter.restore();
}

ColorIndicator::ColorIndicator(QWidget *parent)
    :QWidget(parent), m_color(Qt::white){
    setAttribute(Qt::WA_StaticContents);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    setFocusPolicy(Qt::NoFocus);
}

QColor ColorIndicator::color() const
{
    return m_color;
}

void ColorIndicator::setColor(const QColor &color)
{
    m_color = color;
}

QSize ColorIndicator::sizeHint() const
{
    return QSize(20,20);
}

} // namespace LimeReport
