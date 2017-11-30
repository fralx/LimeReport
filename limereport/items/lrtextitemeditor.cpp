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
#include "lrtextitemeditor.h"
#include "ui_lrtextitemeditor.h"

#include "lrdatasourcemanager.h"
#include "lrscriptenginemanager.h"
#include "lrdatadesignintf.h"
#include "lrscripteditor.h"

#include <QMenu>
#include <QScrollBar>

namespace LimeReport{

TextItemEditor::TextItemEditor(LimeReport::TextItem *item, LimeReport::PageDesignIntf *page, QSettings* settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextItemEditor), m_textItem(item), m_page(page), m_settings(settings), m_ownedSettings(false), m_isReadingSetting(false)
{
    ui->setupUi(this);
    initUI();
    setWindowIcon(QIcon(":/items/images/TextItem"));
    readSetting();
    connect(ui->codeEditor, SIGNAL(splitterMoved(int,int)), this, SLOT(slotSplitterMoved(int,int)) );
}

TextItemEditor::~TextItemEditor()
{
#ifdef Q_OS_WIN
    writeSetting();
#endif
#ifdef Q_OS_MAC
    writeSetting();
#endif
    delete ui;
}

void TextItemEditor::setSettings(QSettings* value)
{
    if (m_ownedSettings)
        delete m_settings;
    m_settings=value;
    m_ownedSettings=false;
    readSetting();
}

QSettings*TextItemEditor::settings()
{
    if (m_settings){
        return m_settings;
    } else {
        m_settings = new QSettings("LimeReport",QCoreApplication::applicationName());
        m_ownedSettings = true;
        return m_settings;
    }
}

void TextItemEditor::resizeEvent(QResizeEvent*)
{
#ifdef Q_OS_UNIX
    writeSetting();
#endif
}

void TextItemEditor::moveEvent(QMoveEvent*)
{
#ifdef Q_OS_UNIX
    writeSetting();
#endif
}

void TextItemEditor::closeEvent(QCloseEvent* event)
{
    if (settings()!=0){
        settings()->beginGroup("TextItemEditor");
        settings()->setValue("CodeEditorState",ui->codeEditor->saveState());
        settings()->endGroup();
    }
    QWidget::closeEvent(event);
}

void TextItemEditor::on_pbOk_clicked()
{
    if (m_textItem->content()!= ui->codeEditor->toPlainText()){
        m_textItem->setContent(ui->codeEditor->toPlainText());
    }
    close();
}

void TextItemEditor::initUI()
{
    QStringList dataWords;

    ui->toolButton->setChecked(false);
    ui->gbSettings->setVisible(false);
    LimeReport::DataSourceManager* dm =  m_page->datasourceManager();
    LimeReport::ScriptEngineManager& se = LimeReport::ScriptEngineManager::instance();
    se.setDataManager(dm);

    ScriptEditor* scriptEditor = dynamic_cast<ScriptEditor*>(ui->codeEditor);
    if (scriptEditor){
        scriptEditor->setReportPage(m_page);
        scriptEditor->setPageBand(findParentBand());
        scriptEditor->setPlainText(m_textItem->content());
    }
}

void TextItemEditor::on_pbCancel_clicked()
{
    close();
}

void TextItemEditor::readSetting()
{
    if (settings()==0) return;

    m_isReadingSetting = true;

    settings()->beginGroup("TextItemEditor");
    QVariant v = settings()->value("Geometry");
    if (v.isValid()){
        restoreGeometry(v.toByteArray());
    }
    v = settings()->value("CodeEditorState");
    if (v.isValid()){
        ui->codeEditor->restoreState(v.toByteArray());
    }

    QVariant fontName = settings()->value("FontName");
    if (fontName.isValid()){
        QVariant fontSize = settings()->value("FontSize");
        ui->codeEditor->setEditorFont(QFont(fontName.toString(),fontSize.toInt()));
        ui->editorFont->setCurrentFont(ui->codeEditor->editorFont());
        ui->editorFontSize->setValue(fontSize.toInt());
    }
    settings()->endGroup();

    m_isReadingSetting = false;
}

void TextItemEditor::writeSetting()
{
    if (settings()!=0){
        settings()->beginGroup("TextItemEditor");
        settings()->setValue("Geometry",saveGeometry());
        settings()->setValue("CodeEditorState",ui->codeEditor->saveState());
        settings()->endGroup();
    }
}

void TextItemEditor::on_editorFont_currentFontChanged(const QFont &f)
{
    if (m_isReadingSetting) return;
    QFont tmp = f;
    tmp.setPointSize(ui->editorFontSize->value());
    ui->codeEditor->setEditorFont(tmp);
    settings()->beginGroup("TextItemEditor");
    settings()->setValue("FontName",ui->codeEditor->editorFont().family());
    settings()->setValue("FontSize",ui->editorFontSize->value());
    settings()->endGroup();
}

void TextItemEditor::on_editorFontSize_valueChanged(int arg1)
{
    if (m_isReadingSetting) return;
    ui->codeEditor->setEditorFont(QFont(ui->codeEditor->editorFont().family(),arg1));
    settings()->beginGroup("TextItemEditor");
    settings()->setValue("FontName",ui->codeEditor->editorFont().family());
    settings()->setValue("FontSize",ui->editorFontSize->value());
    settings()->endGroup();
}

void TextItemEditor::on_toolButton_clicked(bool checked)
{
    ui->gbSettings->setVisible(checked);
}

void TextItemEditor::slotSplitterMoved(int, int)
{
    writeSetting();
}

BandDesignIntf *TextItemEditor::findParentBand()
{
    BandDesignIntf* result = 0;
    BaseDesignIntf* item = m_textItem;
    while (true){
        item = dynamic_cast<BaseDesignIntf*>(item->parentItem());
        if (item){
            result = dynamic_cast<BandDesignIntf*>(item);
            if (result) break;
        } else break;
    }
    return result;
}

} // namespace LimeReport
