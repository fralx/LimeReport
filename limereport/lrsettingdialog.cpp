#include "lrsettingdialog.h"
#include "ui_lrsettingdialog.h"
#include <QFile>

namespace LimeReport{

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    QFile theme(":/qdarkstyle/style.qss");
    if (!theme.exists()){
        ui->cbbUseDarkTheme->setVisible(false);
    }
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

int SettingDialog::verticalGridStep()
{
    return ui->verticalGridStep->value();
}

int SettingDialog::horizontalGridStep()
{
    return ui->horizontalGridStep->value();
}

QFont SettingDialog::defaultFont()
{
    QFont result = ui->defaultFont->currentFont();
    result.setPointSize(ui->defaultFontSize->value());
    return result;
}

bool SettingDialog::userDarkTheme()
{
    return ui->cbbUseDarkTheme->isChecked();
}

bool SettingDialog::suppressAbsentFieldsAndVarsWarnings()
{
    return ui->cbSuppressWarnings->isChecked();
}

QLocale::Language SettingDialog::designerLanguage()
{
    foreach (QLocale::Language language, m_aviableLanguages) {
        if (ui->designerLanguage->currentText().compare(QLocale::languageToString(language)) == 0)
            return language;
    }
    return QLocale().language();
}

void SettingDialog::setSuppressAbsentFieldsAndVarsWarnings(bool value){
    ui->cbSuppressWarnings->setChecked(value);
}

void SettingDialog::setHorizontalGridStep(int value)
{
    ui->horizontalGridStep->setValue(value);
}

void SettingDialog::setVerticalGridStep(int value)
{
    ui->verticalGridStep->setValue(value);
}

void SettingDialog::setDefaultFont(const QFont &value)
{
    ui->defaultFont->setCurrentFont(value);
    ui->defaultFontSize->setValue(value.pointSize());
}

void SettingDialog::setUseDarkTheme(bool value)
{
    ui->cbbUseDarkTheme->setChecked(value);
}

void SettingDialog::setDesignerLanguages(QList<QLocale::Language> languages, QLocale::Language currentLanguage)
{
    m_aviableLanguages = languages;
    m_currentLanguage = currentLanguage;

    if (languages.isEmpty()) {
        ui->designerLanguage->setVisible(false);
        ui->lblLanguage->setVisible(false);
        return;
    }
    ui->designerLanguage->addItem(QLocale::languageToString(currentLanguage));
    foreach (QLocale::Language language, languages) {
        if (language != currentLanguage)
            ui->designerLanguage->addItem(QLocale::languageToString(language));
    }
    ui->designerLanguage->setCurrentText(QLocale::languageToString(currentLanguage));
}

} // namespace LimeReport
