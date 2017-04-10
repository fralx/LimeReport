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
#include "lrdatasourcemanager.h"

#include <QMenu>
#include <QScrollBar>

namespace LimeReport{

TextItemEditor::TextItemEditor(LimeReport::TextItem *item, LimeReport::PageDesignIntf *page, QSettings* settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextItemEditor), m_textItem(item), m_page(page), m_settings(settings), m_ownedSettings(false), m_isReadingSetting(false)
{
    ui->setupUi(this);
    initUI();
    m_teContent->setPlainText(item->content());
    m_teContent->setFocus();
    setWindowIcon(QIcon(":/items/images/TextItem"));
    readSetting();
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
        m_settings = new QSettings("LimeReport",QApplication::applicationName());
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

void TextItemEditor::on_pbOk_clicked()
{
    if (m_textItem->content()!=m_teContent->toPlainText()){
        m_textItem->setContent(m_teContent->toPlainText());
    }
    close();
}

void TextItemEditor::initUI()
{
    QStringList dataWords;
    m_teContent = ui->textEdit;
    m_completer = new QCompleter(this);
    m_teContent->setCompleter(m_completer);

    m_datasourcesMenu = new QMenu(this);

    LimeReport::DataSourceManager* dm =  m_page->datasourceManager();
    LimeReport::ScriptEngineManager& se = LimeReport::ScriptEngineManager::instance();
    se.setDataManager(dm);

    if (dm){
        if (dm->isNeedUpdateDatasourceModel())
           dm->updateDatasourceModel();
        ui->twData->setModel(dm->datasourcesModel());
        ui->twScriptEngine->setModel(se.model());

        foreach(const QString &dsName,dm->dataSourceNames()){
            foreach(const QString &field, dm->fieldNames(dsName)){
                dataWords<<dsName+"."+field;
            }
        }
    } else {
        ui->tabWidget->setVisible(false);
    }

    foreach (LimeReport::ScriptFunctionDesc functionDesc, se.functionsDescriber()) {
        dataWords<<functionDesc.name;
    }

    m_completer->setModel(new QStringListModel(dataWords,m_completer));
    ui->gbSettings->setVisible(false);

    if (ui->twScriptEngine->selectionModel()){
        connect(ui->twScriptEngine->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotScriptItemsSelectionChanged(QModelIndex,QModelIndex)));
    }

    BandDesignIntf* band = findParentBand();
    if (band && ui->twData->model() && !band->datasourceName().isEmpty()){
        QModelIndexList nodes = ui->twData->model()->match(
                    ui->twData->model()->index(0,0),
                    Qt::DisplayRole,
                    band->datasourceName(),
                    2,
                    Qt::MatchRecursive
        );
        if (!nodes.isEmpty()){
            ui->twData->expand(nodes.at(0).parent());
            ui->twData->expand(nodes.at(0));
        }
    }
}

QStringListModel *TextItemEditor::getDataSources()
{
    LimeReport::DataSourceManager* dm = m_page->datasourceManager();
    QStringList dataSources;
    foreach(QString dsName,dm->dataSourceNames()){
        dataSources<<dsName;
    }
    return new QStringListModel(dataSources, m_completer);
}

QStringListModel *TextItemEditor::getPrefixes()
{
    QStringList prefixes;
    prefixes<<"D{"<<"S{";
    return new QStringListModel(prefixes, m_completer);
}

QStringListModel *TextItemEditor::getColumns(QString datasource)
{
    QStringList fields;
    LimeReport::DataSourceManager* dm = m_page->datasourceManager();
    foreach(QString field, dm->fieldNames(datasource)){
        fields<<field;
    }
    return new QStringListModel(fields, m_completer);
}

void TextItemEditor::on_pbCancel_clicked()
{
    close();
}

void TextItemEditor::slotFieldSelected()
{
    QAction* action = dynamic_cast<QAction*>(sender());
    m_teContent->insertPlainText(action->whatsThis());
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
    v = settings()->value("State");
    if (v.isValid()){
        ui->splitter->restoreState(v.toByteArray());
    }

    QVariant fontName = settings()->value("FontName");
    if (fontName.isValid()){
        QVariant fontSize = settings()->value("FontSize");
        ui->textEdit->setFont(QFont(fontName.toString(),fontSize.toInt()));
        ui->editorFont->setCurrentFont(ui->textEdit->font());
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
        settings()->setValue("State",ui->splitter->saveState());
        settings()->endGroup();
    }
}


CompleaterTextEditor::CompleaterTextEditor(QWidget *parent)
    : QTextEdit(parent),m_compleater(0)
{
}

void CompleaterTextEditor::setCompleter(QCompleter *value)
{
    if (value) disconnect(value,0,this,0);
    m_compleater = value;
    if (!m_compleater) return;
    m_compleater->setWidget(this);
    m_compleater->setCompletionMode(QCompleter::PopupCompletion);
    m_compleater->setCaseSensitivity(Qt::CaseInsensitive);
    connect(m_compleater,SIGNAL(activated(QString)),this,SLOT(insertCompletion(QString)));
}

void CompleaterTextEditor::keyPressEvent(QKeyEvent *e)
{
    if (m_compleater && m_compleater->popup()->isVisible()) {
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E);
    if (!m_compleater || !isShortcut) QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!m_compleater || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;

    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                        || eow.contains(e->text().right(1)))) {
        m_compleater->popup()->hide();
        return;
    }

    if (completionPrefix != m_compleater->completionPrefix()) {
        m_compleater->setCompletionPrefix(completionPrefix);
        m_compleater->popup()->setCurrentIndex(m_compleater->completionModel()->index(0, 0));
    }

    QRect cr = cursorRect();
    cr.setWidth(m_compleater->popup()->sizeHintForColumn(0)
                + m_compleater->popup()->verticalScrollBar()->sizeHint().width());
    m_compleater->complete(cr);

}

void CompleaterTextEditor::focusInEvent(QFocusEvent *e)
{
    if (m_compleater) m_compleater->setWidget(this);
    QTextEdit::focusInEvent(e);
}

QString CompleaterTextEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}


void CompleaterTextEditor::insertCompletion(const QString &completion)
{
    if (m_compleater->widget() != this)
             return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - m_compleater->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

void TextItemEditor::on_twData_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    LimeReport::DataNode* node = static_cast<LimeReport::DataNode*>(index.internalPointer());
    if (node->type()==LimeReport::DataNode::Field){
        m_teContent->insertPlainText(QString("$D{%1.%2}").arg(node->parent()->name()).arg(node->name()));
    }
    if (node->type()==LimeReport::DataNode::Variable){
        m_teContent->insertPlainText(QString("$V{%1}").arg(node->name()));
    }
}

void TextItemEditor::on_twScriptEngine_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    LimeReport::ScriptEngineNode* node = static_cast<LimeReport::ScriptEngineNode*>(index.internalPointer());
    if (node->type()==LimeReport::ScriptEngineNode::Function){
        m_teContent->insertPlainText(node->name()+"()");
    }
}

void TextItemEditor::on_splitter_splitterMoved(int , int )
{
#ifdef unix
    writeSetting();
#endif
}

void TextItemEditor::on_editorFont_currentFontChanged(const QFont &f)
{
    if (m_isReadingSetting) return;
    QFont tmp = f;
    tmp.setPointSize(ui->editorFontSize->value());
    ui->textEdit->setFont(tmp);
    settings()->beginGroup("TextItemEditor");
    settings()->setValue("FontName",ui->textEdit->font().family());
    settings()->setValue("FontSize",ui->editorFontSize->value());
    settings()->endGroup();
}

void TextItemEditor::on_editorFontSize_valueChanged(int arg1)
{
    if (m_isReadingSetting) return;
    ui->textEdit->setFont(QFont(ui->textEdit->font().family(),arg1));
    settings()->beginGroup("TextItemEditor");
    settings()->setValue("FontName",ui->textEdit->font().family());
    settings()->setValue("FontSize",ui->editorFontSize->value());
    settings()->endGroup();
}

void TextItemEditor::on_toolButton_clicked(bool checked)
{
    ui->gbSettings->setVisible(checked);
}


void TextItemEditor::on_twScriptEngine_activated(const QModelIndex &index)
{
    LimeReport::ScriptEngineNode* node = static_cast<LimeReport::ScriptEngineNode*>(index.internalPointer());
    if (node->type()==LimeReport::ScriptEngineNode::Function){
       ui->lblDescription->setText(node->name());
    }
}

void TextItemEditor::slotScriptItemsSelectionChanged(const QModelIndex &to, const QModelIndex)
{
    LimeReport::ScriptEngineNode* node = static_cast<LimeReport::ScriptEngineNode*>(to.internalPointer());
    if (node->type()==LimeReport::ScriptEngineNode::Function){
       ui->lblDescription->setText(node->description());
    }
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
