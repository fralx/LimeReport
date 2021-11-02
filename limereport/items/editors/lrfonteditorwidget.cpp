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
#include "lrfonteditorwidget.h"

namespace LimeReport{

FontEditorWidget::FontEditorWidget(const QString& title, QWidget* parent)
    :ItemEditorWidget(title, parent), m_ignoreSlots(false)
{
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 3)
    connect(m_fontSizeEditor,SIGNAL(currentTextChanged(QString)),this,SLOT(slotFontSizeChanged(QString)));
#else
    connect(m_fontSizeEditor,SIGNAL(currentIndexChanged(QString)),this,SLOT(slotFontSizeChanged(QString)));
#endif
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

bool FontEditorWidget::ignoreSlots() const
{
    return m_ignoreSlots;
}


void FontEditorWidget::slotFontChanged(const QFont& /*font*/)
{
     //if (page()) page()->setFont(font);
}

void FontEditorWidget::slotFontSizeChanged(const QString &value)
{
    if (m_ignoreSlots) return;
    m_resFont = fontNameEditor()->currentFont();
    m_resFont.setPointSize(value.toInt());
}

void FontEditorWidget::slotFontAttribsChanged(bool)
{
    if (m_ignoreSlots) return;
    m_resFont = m_fontNameEditor->currentFont();
    m_resFont.setPointSize(m_fontSizeEditor->currentText().toInt());
    m_resFont.setBold(m_fontBold->isChecked());
    m_resFont.setItalic(m_fontItalic->isChecked());
    m_resFont.setUnderline(m_fontUnderline->isChecked());
}

void FontEditorWidget::slotPropertyChanged(const QString &objectName, const QString &property, const QVariant& oldValue, const QVariant& newValue)
{
    Q_UNUSED(oldValue)
    Q_UNUSED(newValue)
    if (item()&&(item()->objectName()==objectName)&&(property=="font")){
        updateValues(item()->property("font").value<QFont>());
    }
}


void FontEditorWidgetForPage::slotFontChanged(const QFont& font)
{
    if (!ignoreSlots())
        m_page->setFont(font);
}

void FontEditorWidgetForPage::slotFontSizeChanged(const QString& value)
{
    if (!ignoreSlots()){
        FontEditorWidget::slotFontSizeChanged(value);
        m_page->setFont(resFont());
    }
}

void FontEditorWidgetForPage::slotFontAttribsChanged(bool value)
{
    if (!ignoreSlots()){
        FontEditorWidget::slotFontAttribsChanged(value);
        m_page->setFont(resFont());
    }
}

#ifdef HAVE_REPORT_DESIGNER
void FontEditorWidgetForDesigner::initEditor()
{
    connect(m_reportEditor,SIGNAL(itemPropertyChanged(QString,QString,QVariant,QVariant)),
            this,SLOT(slotPropertyChanged(QString,QString,QVariant,QVariant)));
}

void FontEditorWidgetForDesigner::slotFontChanged(const QFont& font)
{
    if (!ignoreSlots()) m_reportEditor->setFont(font);
}

void FontEditorWidgetForDesigner::slotFontSizeChanged(const QString& value)
{
    if (!ignoreSlots()){
        FontEditorWidget::slotFontSizeChanged(value);
        m_reportEditor->setFont(resFont());
    }
}

void FontEditorWidgetForDesigner::slotFontAttribsChanged(bool value)
{
    if (!ignoreSlots()){
        FontEditorWidget::slotFontAttribsChanged(value);
        m_reportEditor->setFont(resFont());
    }
}

#endif


} //namespace LimeReport
