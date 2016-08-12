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
#include "lrfonteditorwidget.h"

namespace LimeReport{

FontEditorWidget::FontEditorWidget(ReportDesignWidget *reportEditor, const QString &title, QWidget *parent)
    : ItemEditorWidget(reportEditor,title,parent), m_ignoreSlots(false) {
    initEditor();
}

FontEditorWidget::FontEditorWidget(ReportDesignWidget *reportEditor, QWidget *parent)
    :ItemEditorWidget(reportEditor,parent), m_ignoreSlots(false) {
    initEditor();
}

FontEditorWidget::FontEditorWidget(PageDesignIntf *page, const QString &title, QWidget *parent)
    :ItemEditorWidget(page,title,parent), m_ignoreSlots(false) {
    initEditor();
}

FontEditorWidget::FontEditorWidget(LimeReport::PageDesignIntf *page, QWidget *parent)
    :ItemEditorWidget(page,parent), m_ignoreSlots(false){
    initEditor();
}

void FontEditorWidget::setItemEvent(BaseDesignIntf* item)
{

    QVariant font=item->property("font");
    if (font.isValid()){
        updateValues(font.value<QFont>());
        setEnabled(true);
    }

}

void FontEditorWidget::initEditor()
{
    setIconSize(QSize(24,24));
    setAllowedAreas(Qt::TopToolBarArea);
    setFloatable(false);

    m_fontNameEditor = new QFontComboBox(this);
    m_fontNameEditor->setFontFilters(QFontComboBox::AllFonts);
    connect(m_fontNameEditor,SIGNAL(currentFontChanged(QFont)),this,SLOT(slotFontChanged(QFont)));
    addWidget(m_fontNameEditor);

    m_fontSizeModel.setStringList(QStringList()<<"6"<<"7"<<"8"<<"9"<<"10"<<"11"<<"12"<<"14"<<"16"<<"18"<<"20"<<"24"<<"28"<<"30"<<"36"<<"48"<<"64"<<"72");
    m_fontSizeEditor = new QComboBox(this);
    m_fontSizeEditor->setModel(&m_fontSizeModel);
    m_fontSizeEditor->setEditable(true);
    connect(m_fontSizeEditor,SIGNAL(currentIndexChanged(QString)),this,SLOT(slotFontSizeChanged(QString)));
    addWidget(m_fontSizeEditor);

    addSeparator();
    setEnabled(false);

    m_fontBold = new QAction(tr("Font bold"),this);
    m_fontBold->setIcon(QIcon(":/report/images/textBold"));
    m_fontBold->setCheckable(true);
    connect(m_fontBold,SIGNAL(toggled(bool)),this,SLOT(slotFontAttribsChanged(bool)));
    addAction(m_fontBold);

    m_fontItalic = new QAction(tr("Font Italic"),this);
    m_fontItalic->setIcon(QIcon(":/report/images/textItalic"));
    m_fontItalic->setCheckable(true);
    connect(m_fontItalic,SIGNAL(toggled(bool)),this,SLOT(slotFontAttribsChanged(bool)));
    addAction(m_fontItalic);

    m_fontUnderline = new QAction(tr("Font Underline"),this);
    m_fontUnderline->setIcon(QIcon(":/report/images/textUnderline"));
    m_fontUnderline->setCheckable(true);
    connect(m_fontUnderline,SIGNAL(toggled(bool)),this,SLOT(slotFontAttribsChanged(bool)));
    addAction(m_fontUnderline);

    if (reportEditor()){
        connect(reportEditor(),SIGNAL(itemPropertyChanged(QString,QString,QVariant,QVariant)),
                this,SLOT(slotPropertyChanged(QString,QString,QVariant,QVariant)));
    }
}

void FontEditorWidget::updateValues(const QFont& font)
{
    m_ignoreSlots=true;
    m_fontNameEditor->setCurrentFont(font);
    m_fontSizeEditor->setEditText(QString::number(font.pointSize()));
    m_fontBold->setChecked(font.bold());
    m_fontItalic->setChecked(font.italic());
    m_fontUnderline->setChecked(font.underline());
    m_ignoreSlots=false;
}


void FontEditorWidget::slotFontChanged(const QFont &font)
{
    if (reportEditor() && !m_ignoreSlots) reportEditor()->setFont(font);
    if (page()) page()->setFont(font);
}

void FontEditorWidget::slotFontSizeChanged(const QString &value)
{
    if (m_ignoreSlots) return;

    QFont resFont(fontNameEditor()->currentFont());
    resFont.setPointSize(value.toInt());

    if (reportEditor()) reportEditor()->setFont(resFont);
    if (page()) page()->setFont(resFont);
}

void FontEditorWidget::slotFontAttribsChanged(bool)
{
    if (m_ignoreSlots) return;

    QFont resFont(m_fontNameEditor->currentFont());
    resFont.setBold(m_fontBold->isChecked());
    resFont.setItalic(m_fontItalic->isChecked());
    resFont.setUnderline(m_fontUnderline->isChecked());
    if (reportEditor()) reportEditor()->setFont(resFont);
    if (page()) page()->setFont(resFont);

}

void FontEditorWidget::slotPropertyChanged(const QString &objectName, const QString &property, const QVariant& oldValue, const QVariant& newValue)
{
    Q_UNUSED(oldValue)
    Q_UNUSED(newValue)
    if (item()&&(item()->objectName()==objectName)&&(property=="font")){
        updateValues(item()->property("font").value<QFont>());
    }
}


} //namespace LimeReport
