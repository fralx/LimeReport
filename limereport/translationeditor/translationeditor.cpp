#include "translationeditor.h"
#include "ui_translationeditor.h"
#include "lrreportengine.h"
#include "lrreportengine_p.h"
#include "lrreporttranslation.h"
#include "languageselectdialog.h"
#include <QShortcut>

namespace LimeReport {

TranslationEditor::TranslationEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TranslationEditor), m_translationContainer(0),
    m_currentReportTranslation(0), m_currentPageTranslation(0),
    m_currentPropertyTranslation(0), m_translationChanging(false)
{
    ui->setupUi(this);
    ui->splitter_3->setStretchFactor(1,10);
    ui->splitter_3->setStretchFactor(0,2);
    ui->splitter_2->setStretchFactor(1,2);
    ui->splitter->setStretchFactor(0,2);
    QTableWidgetItem* item = new QTableWidgetItem();
    item->setIcon(QIcon(":/translationeditor/checked"));
    ui->tbStrings->setColumnCount(4);
    ui->tbStrings->setColumnWidth(0,30);
    ui->tbStrings->setColumnWidth(1,100);
    ui->tbStrings->setColumnWidth(2,100);
    ui->tbStrings->setHorizontalHeaderItem(0,item);
    ui->tbStrings->setHorizontalHeaderItem(1,new QTableWidgetItem(tr("Report Item")));
    ui->tbStrings->setHorizontalHeaderItem(2,new QTableWidgetItem(tr("Property")));
    ui->tbStrings->setHorizontalHeaderItem(3,new QTableWidgetItem(tr("Source text")));
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    m_clrReturn = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), this, SLOT(slotItemChecked()));
#else
    m_clrReturn = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this, SLOT(slotItemChecked()));
#endif
    //ui->tbStrings->setSortingEnabled(true);

}

void TranslationEditor::setReportEngine(ITranslationContainer* translationContainer)
{
    m_translationContainer = translationContainer;
    m_currentReportTranslation = 0;
    m_currentPageTranslation = 0;
    m_currentPropertyTranslation = 0;

    if (m_translationContainer){
        m_translationContainer->updateTranslations();
        updateUi();
    }
}

TranslationEditor::~TranslationEditor()
{
    delete ui;
    delete m_clrReturn;
}

QLocale::Language TranslationEditor::getLanguageByName(const QString& languageName){
   foreach(QLocale::Language language, m_translationContainer->translations()->keys()){
       if (QLocale::languageToString(language).compare(languageName) == 0){
           return language;
       }
   }
   return QLocale::AnyLanguage;
}

void TranslationEditor::updateUi()
{
    ui->lvLanguages->clear();
    ui->twPages->clear();
    ui->tbStrings->clearContents();
    ui->teTranslation->setPlainText("");
    ui->cbChecked->setEnabled(false);

    Q_ASSERT(m_translationContainer != 0);
    if (m_translationContainer){
        Translations* translations = m_translationContainer->translations();
        Q_ASSERT(translations != 0);
        if (translations){
            foreach(QLocale::Language language, translations->keys()){
                if (language != QLocale::AnyLanguage)
                    ui->lvLanguages->addItem(QLocale::languageToString(language));
            }
            if (!translations->keys().isEmpty()){
                if (ui->lvLanguages->count() != 0){
                    ui->lvLanguages->item(0)->setSelected(true);
                    activateLanguage(getLanguageByName(ui->lvLanguages->item(0)->text()));
                } else {
                    ui->twPages->clear();
                    ui->tbStrings->setRowCount(0);
                }
            }
        }
    }
}

void TranslationEditor::activateLanguage(QLocale::Language language)
{
    m_translationChanging = true;
    ui->teTranslation->setEnabled(false);
    ui->teTranslation->setPlainText("");
    ui->cbChecked->setEnabled(false);
    ui->twPages->clear();
    Translations* translations = m_translationContainer->translations();
    Q_ASSERT(translations != 0);
    if (translations){
        m_currentReportTranslation = translations->value(language);
        Q_ASSERT(m_currentReportTranslation != 0);
        if (m_currentReportTranslation){
           foreach(PageTranslation* pageTranslation, m_currentReportTranslation->pagesTranslation()){
               QTreeWidgetItem* pageItem = new QTreeWidgetItem();
               pageItem->setText(0,pageTranslation->pageName);
               ui->twPages->addTopLevelItem(pageItem);
           }
        }
        if (ui->twPages->topLevelItem(0)){
            ui->twPages->topLevelItem(0)->setSelected(true);
            activatePage(m_currentReportTranslation->findPageTranslation(ui->twPages->topLevelItem(0)->text(0)));
        }
    }
    m_translationChanging = false;
}

void TranslationEditor::activatePage(PageTranslation* pageTranslation)
{
    ui->teTranslation->setEnabled(false);
    ui->cbChecked->setEnabled(false);
    Q_ASSERT(pageTranslation != 0);
    if(pageTranslation){
        ui->tbStrings->clearContents();
        ui->tbStrings->setRowCount(0);
        m_currentPageTranslation = pageTranslation;
        QStringList items = pageTranslation->itemsTranslation.keys();
        items.sort();
        foreach(QString itemName, items){
            ItemTranslation* itemTranslation = pageTranslation->itemsTranslation.value(itemName);
            int rowIndex = ui->tbStrings->rowCount();
            ui->tbStrings->setRowCount(rowIndex+1);
            foreach(PropertyTranslation* propertyTranslation, itemTranslation->propertyesTranslation){
                QTableWidgetItem* checkItem = new QTableWidgetItem();

                if (propertyTranslation->sourceHasBeenChanged){
                    checkItem->setIcon(QIcon(":/translationeditor/question"));
                }
                if (propertyTranslation->checked){
                    checkItem->setIcon(QIcon(":/translationeditor/checked"));
                }

                ui->tbStrings->setItem(rowIndex,0,checkItem);
                ui->tbStrings->setItem(rowIndex,1,new QTableWidgetItem(itemTranslation->itemName));
                ui->tbStrings->setItem(rowIndex,2,new QTableWidgetItem(propertyTranslation->propertyName));
                ui->tbStrings->setItem(rowIndex,3,new QTableWidgetItem(propertyTranslation->sourceValue));
            }
        }
    }

}

void TranslationEditor::activateTranslation(const QString& itemName, const QString& propertyName)
{
    Q_ASSERT(m_currentPageTranslation != 0);
    if (m_currentPageTranslation){
        ItemTranslation* itemTranslation = m_currentPageTranslation->itemsTranslation.value(itemName);
        Q_ASSERT(itemTranslation !=0 );
        if (itemTranslation){
            m_currentPropertyTranslation = m_currentPageTranslation->itemsTranslation.value(itemName)->findProperty(propertyName);
            Q_ASSERT(m_currentPropertyTranslation != 0);
            if (m_currentPropertyTranslation){
                ui->teTranslation->setEnabled(true);
                ui->cbChecked->setEnabled(true);
                ui->teTranslation->setText(m_currentPropertyTranslation->value);
                ui->cbChecked->setChecked(m_currentPropertyTranslation->checked);
            }
        }
    }
}

void TranslationEditor::on_tbStrings_itemSelectionChanged()
{
    if (m_currentPageTranslation && !m_translationChanging)
        activateTranslation(ui->tbStrings->item(ui->tbStrings->currentRow(),1)->text(), ui->tbStrings->item(ui->tbStrings->currentRow(),2)->text());
}

void TranslationEditor::on_teTranslation_textChanged()
{
    if (m_currentPropertyTranslation && !m_translationChanging)
        m_currentPropertyTranslation->value = ui->teTranslation->toPlainText();
}

void TranslationEditor::on_cbChecked_toggled(bool checked)
{
    if (m_currentPropertyTranslation){
        m_currentPropertyTranslation->checked = checked;
        ui->tbStrings->item(ui->tbStrings->currentRow(),0)->setIcon(checked ? QIcon(":/translationeditor/checked"):QIcon());
    }
}

void TranslationEditor::on_twPages_itemSelectionChanged()
{
    if (!ui->twPages->selectedItems().isEmpty()){
        activatePage(m_currentReportTranslation->findPageTranslation(ui->twPages->selectedItems().at(0)->text(0)));
    }
}

void TranslationEditor::on_tbAddLanguage_clicked()
{
    LanguageSelectDialog dialog;
    if (dialog.exec()){
        m_translationChanging = true;
        m_translationContainer->addTranslationLanguage(dialog.getSelectedLanguage());
        updateUi();
        activateLanguage(dialog.getSelectedLanguage());
        foreach (QListWidgetItem* item, ui->lvLanguages->findItems(QLocale::languageToString(dialog.getSelectedLanguage()), Qt::MatchExactly)){
            item->setSelected(true);
        }
        m_translationChanging = false;
    }
}

void TranslationEditor::on_tbDeleteLanguage_clicked()
{
    if (!m_currentPageTranslation) return;
    m_translationChanging = true;
    m_translationContainer->removeTranslationLanguage(m_currentReportTranslation->language());
    updateUi();
    m_translationChanging = false;
}

void TranslationEditor::slotItemChecked()
{
    if (ui->tbStrings->currentRow()<ui->tbStrings->rowCount()){
        ui->cbChecked->setChecked(true);
        ui->tbStrings->selectRow(ui->tbStrings->currentRow()+1);
        ui->teTranslation->setFocus();
    }
}

void TranslationEditor::on_lvLanguages_itemSelectionChanged()
{
    if (ui->lvLanguages->currentItem() && m_currentReportTranslation){
        activateLanguage(getLanguageByName(ui->lvLanguages->currentItem()->text()));
    }
}

} //namespace LimeReport






