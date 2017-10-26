#include "languageselectdialog.h"
#include "ui_languageselectdialog.h"
#include <QLocale>
#include <QSet>
#include <QDebug>

LanguageSelectDialog::LanguageSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LanguageSelectDialog)
{
    ui->setupUi(this);
    for (int i = 2; i<QLocale::LastLanguage; ++i){
        ui->comboBox->addItem(QLocale::languageToString(static_cast<QLocale::Language>(i)),static_cast<QLocale::Language>(i));
    }
#ifdef HAVE_QT5
    ui->comboBox->setCurrentText("");
#endif
#ifdef HAVE_QT4
    ui->comboBox->setEditText("");
#endif
}

LanguageSelectDialog::~LanguageSelectDialog()
{
    delete ui;
}

QLocale::Language LanguageSelectDialog::getSelectedLanguage()
{
    return ui->comboBox->itemData(ui->comboBox->currentIndex()).value<QLocale::Language>();
}
