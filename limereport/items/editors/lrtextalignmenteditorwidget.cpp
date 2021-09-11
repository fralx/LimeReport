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
#include "lrtextalignmenteditorwidget.h"

namespace LimeReport{
TextAlignmentEditorWidget::TextAlignmentEditorWidget(const QString& title, QWidget* parent)
    :ItemEditorWidget(title, parent), m_textAttibutesIsChanging(false), m_flag(0)
{
    initEditor();
}

void TextAlignmentEditorWidget::setItemEvent(BaseDesignIntf *item)
{
    QVariant align=item->property("alignment");
    if (align.isValid()){
        updateValues(Qt::Alignment(align.value<int>()));
        setEnabled(true);
    }
}

void TextAlignmentEditorWidget::initEditor()
{
    m_textAliginLeft = new QAction(tr("Text align left"),this);
    m_textAliginLeft->setIcon(QIcon(":/report/images/textAlignHLeft"));
    m_textAliginLeft->setCheckable(true);
    connect(m_textAliginLeft,SIGNAL(toggled(bool)),this,SLOT(slotTextHAttribsChanged(bool)));
    addAction(m_textAliginLeft);

    m_textAliginHCenter = new QAction(tr("Text align center"),this);
    m_textAliginHCenter->setIcon(QIcon(":/report/images/textAlignHCenter"));
    m_textAliginHCenter->setCheckable(true);
    connect(m_textAliginHCenter,SIGNAL(toggled(bool)),this,SLOT(slotTextHAttribsChanged(bool)));
    addAction(m_textAliginHCenter);

    m_textAliginRight = new QAction(tr("Text align right"),this);
    m_textAliginRight->setIcon(QIcon(":/report/images/textAlignHRight"));
    m_textAliginRight->setCheckable(true);
    connect(m_textAliginRight,SIGNAL(toggled(bool)),this,SLOT(slotTextHAttribsChanged(bool)));
    addAction(m_textAliginRight);

    m_textAliginJustify = new QAction(tr("Text align justify"),this);
    m_textAliginJustify->setIcon(QIcon(":/report/images/textAlignHJustify"));
    m_textAliginJustify->setCheckable(true);
    connect(m_textAliginJustify,SIGNAL(toggled(bool)),this,SLOT(slotTextHAttribsChanged(bool)));
    addAction(m_textAliginJustify);

    addSeparator();

    m_textAliginTop = new QAction(tr("Text align top"),this);
    m_textAliginTop->setIcon(QIcon(":/report/images/textAlignVTop"));
    m_textAliginTop->setCheckable(true);
    connect(m_textAliginTop,SIGNAL(toggled(bool)),this,SLOT(slotTextVAttribsChanged(bool)));
    addAction(m_textAliginTop);

    m_textAliginVCenter = new QAction(tr("Text align center"),this);
    m_textAliginVCenter->setIcon(QIcon(":/report/images/textAlignVCenter"));
    m_textAliginVCenter->setCheckable(true);
    connect(m_textAliginVCenter,SIGNAL(toggled(bool)),this,SLOT(slotTextVAttribsChanged(bool)));
    addAction(m_textAliginVCenter);

    m_textAliginBottom = new QAction(tr("Text align bottom"),this);
    m_textAliginBottom->setIcon(QIcon(":/report/images/textAlignVBottom"));
    m_textAliginBottom->setCheckable(true);
    connect(m_textAliginBottom,SIGNAL(toggled(bool)),this,SLOT(slotTextVAttribsChanged(bool)));
    addAction(m_textAliginBottom);
    setEnabled(false);
}

void TextAlignmentEditorWidget::updateValues(const Qt::Alignment &align)
{
    m_textAttibutesIsChanging=true;
    m_textAliginLeft->setChecked((align & Qt::AlignLeft)==Qt::AlignLeft);
    m_textAliginRight->setChecked((align & Qt::AlignRight)==Qt::AlignRight);
    m_textAliginHCenter->setChecked((align & Qt::AlignHCenter)==Qt::AlignHCenter);
    m_textAliginJustify->setChecked((align & Qt::AlignJustify)==Qt::AlignJustify);
    m_textAliginTop->setChecked((align & Qt::AlignTop)==Qt::AlignTop);
    m_textAliginVCenter->setChecked((align & Qt::AlignVCenter)==Qt::AlignVCenter);
    m_textAliginBottom->setChecked((align & Qt::AlignBottom)==Qt::AlignBottom);
    m_textAttibutesIsChanging=false;
}

Qt::Alignment TextAlignmentEditorWidget::createAlignment()
{
    Qt::Alignment align = Qt::Alignment();
    if (m_textAliginLeft->isChecked()) align |= Qt::AlignLeft;
    if (m_textAliginHCenter->isChecked()) align |= Qt::AlignHCenter;
    if (m_textAliginRight->isChecked()) align |= Qt::AlignRight;
    if (m_textAliginJustify->isChecked()) align |= Qt::AlignJustify;
    if (m_textAliginTop->isChecked()) align |= Qt::AlignTop;
    if (m_textAliginVCenter->isChecked()) align |= Qt::AlignVCenter;
    if (m_textAliginBottom->isChecked()) align |= Qt::AlignBottom;
    return align;
}

void TextAlignmentEditorWidget::slotTextHAttribsChanged(bool)
{
    if (m_textAttibutesIsChanging) return;
    m_textAttibutesIsChanging = true;

    m_textAliginLeft->setChecked(sender()==m_textAliginLeft);
    m_textAliginHCenter->setChecked(sender()==m_textAliginHCenter);
    m_textAliginRight->setChecked(sender()==m_textAliginRight);
    m_textAliginJustify->setChecked(sender()==m_textAliginJustify);

    m_flag = 0;
    if (sender()==m_textAliginLeft) m_flag |= Qt::AlignLeft;
    if (sender()==m_textAliginHCenter) m_flag |= Qt::AlignHCenter;
    if (sender()==m_textAliginRight) m_flag |= Qt::AlignRight;
    if (sender()==m_textAliginJustify) m_flag |= Qt::AlignJustify;
    m_textAttibutesIsChanging = false;
}

void TextAlignmentEditorWidget::slotTextVAttribsChanged(bool)
{
    if (m_textAttibutesIsChanging) return;
    m_textAttibutesIsChanging = true;

    m_textAliginTop->setChecked(sender()==m_textAliginTop);
    m_textAliginVCenter->setChecked(sender()==m_textAliginVCenter);
    m_textAliginBottom->setChecked(sender()==m_textAliginBottom);

    m_flag = 0;
    if (sender()==m_textAliginTop) m_flag |= Qt::AlignTop;
    if (sender()==m_textAliginVCenter) m_flag |= Qt::AlignVCenter;
    if (sender()==m_textAliginBottom) m_flag |= Qt::AlignBottom;
    m_textAttibutesIsChanging = false;
}

void TextAlignmentEditorWidget::slotPropertyChanged(const QString &objectName, const QString &property, const QVariant &oldValue, const QVariant &newValue)
{
    Q_UNUSED(oldValue)
    Q_UNUSED(newValue)

    if (item()&&(item()->objectName()==objectName)&&(property=="alignment")){
        updateValues(Qt::Alignment(item()->property("alignment").value<int>()));
    }
}

int TextAlignmentEditorWidget::flag() const
{
    return m_flag;
}

void TextAlignmentEditorWidgetForPage::initEditor()
{
    TextAlignmentEditorWidget::initEditor();
    connect(m_page,SIGNAL(itemPropertyChanged(QString,QString,QVariant,QVariant)),
            this,SLOT(slotPropertyChanged(QString,QString,QVariant,QVariant)));
}

void TextAlignmentEditorWidgetForPage::slotTextHAttribsChanged(bool value)
{

    TextAlignmentEditorWidget::slotTextHAttribsChanged(value);
    if (m_textAttibutesIsChanging) return;
    m_textAttibutesIsChanging = true;
    m_page->changeSelectedGrpoupTextAlignPropperty(true,Qt::AlignmentFlag(flag()));
    m_textAttibutesIsChanging = false;
}

void TextAlignmentEditorWidgetForPage::slotTextVAttribsChanged(bool value)
{
    TextAlignmentEditorWidget::slotTextVAttribsChanged(value);
    if (m_textAttibutesIsChanging) return;
    m_textAttibutesIsChanging = true;
    m_page->changeSelectedGrpoupTextAlignPropperty(false,Qt::AlignmentFlag(flag()) );
    m_textAttibutesIsChanging = false;
}

#ifdef HAVE_REPORT_DESIGNER
void TextAlignmentEditorWidgetForDesigner::initEditor()
{
    connect(m_reportEditor,SIGNAL(itemPropertyChanged(QString,QString,QVariant,QVariant)),
            this,SLOT(slotPropertyChanged(QString,QString,QVariant,QVariant)));

}

void TextAlignmentEditorWidgetForDesigner::slotTextHAttribsChanged(bool value)
{
    TextAlignmentEditorWidget::slotTextHAttribsChanged(value);
    if (m_textAttibutesIsChanging) return;
    m_textAttibutesIsChanging = true;
    m_reportEditor->setTextAlign(true,Qt::AlignmentFlag(flag()));
    m_textAttibutesIsChanging = false;
}

void TextAlignmentEditorWidgetForDesigner::slotTextVAttribsChanged(bool value)
{
    TextAlignmentEditorWidget::slotTextVAttribsChanged(value);
    if (m_textAttibutesIsChanging) return;
    m_textAttibutesIsChanging = true;
    m_reportEditor->setTextAlign(false,Qt::AlignmentFlag(flag()));
    m_textAttibutesIsChanging = false;
}
#endif

} //namespace LimeReport
