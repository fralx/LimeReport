#include "lrsettingdialog.h"
#include "ui_lrsettingdialog.h"
#include "lrglobal.h"
#include <QFile>

namespace LimeReport{

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent), m_settings(0),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    QFile theme(":/qdarkstyle/style.qss");
    if (!theme.exists()){
        ui->cbbUseDarkTheme->setVisible(false);
    }
    ui->indentSize->setRange(0,10);
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

QFont SettingDialog::scriptFont()
{
    QFont result = ui->scriptFont->currentFont();
    result.setPointSize(ui->scriptFontSize->value());
    return result;
}

int SettingDialog::tabIndention()
{
    return ui->indentSize->value();
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

void SettingDialog::setScriptFont(const QFont& value)
{
    ui->scriptFont->setCurrentFont(value);
    ui->scriptFontSize->setValue(value.pointSize());
}

void SettingDialog::setScritpTabIndention(int size)
{
    ui->indentSize->setValue(size);
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
#ifdef HAVE_QT4
    ui->designerLanguage->setCurrentIndex(ui->designerLanguage->findText(QLocale::languageToString(currentLanguage)));
#else
    ui->designerLanguage->setCurrentText(QLocale::languageToString(currentLanguage));
#endif
}

void SettingDialog::setSettings(QSettings* settings){
    m_settings = settings;
    if (m_settings){
        m_settings->beginGroup("ScriptEditor");
        QVariant fontName = m_settings->value("DefaultFontName");
        if (fontName.isValid()){
            QVariant fontSize = m_settings->value("DefaultFontSize");
            ui->scriptFont->setCurrentFont(QFont(fontName.toString(),fontSize.toInt()));
            ui->scriptFontSize->setValue(fontSize.toInt());
        }
        QVariant indentSize = m_settings->value("TabIndention");
        if (indentSize.isValid()){
            ui->indentSize->setValue(indentSize.toInt());
        } else {
            ui->indentSize->setValue(LimeReport::Const::DEFAULT_TAB_INDENTION);
        }
        m_settings->endGroup();
    }
}

void SettingDialog::on_bbOkCancel_accepted()
{
    if (m_settings){
        m_settings->beginGroup("ScriptEditor");
        m_settings->setValue("DefaultFontName", ui->scriptFont->currentFont().family());
        m_settings->setValue("DefaultFontSize", ui->scriptFontSize->value());
        m_settings->setValue("TabIndention", ui->indentSize->value());
        m_settings->endGroup();
    }
}

} // namespace LimeReport


