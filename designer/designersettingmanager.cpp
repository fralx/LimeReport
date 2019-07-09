#include "designersettingmanager.h"
#include <QMessageBox>

DesignerSettingManager::DesignerSettingManager(QObject *parent) : QObject(parent)
{
    m_setting = new QSettings("LimeReport",QCoreApplication::applicationName());
}

DesignerSettingManager::~DesignerSettingManager()
{
    delete m_setting;
}

void DesignerSettingManager::getAvailableLanguages(QList<QLocale::Language>* languages)
{
    languages->append(QLocale::Russian);
    languages->append(QLocale::English);
//    languages->append(QLocale::Arabic);
    languages->append(QLocale::French);
    languages->append(QLocale::Chinese);
    languages->append(QLocale::Spanish);
    languages->append(QLocale::Polish);
}

QLocale::Language DesignerSettingManager::getCurrentDefaultLanguage()
{
    m_setting->beginGroup("ReportDesigner");
    QVariant v = m_setting->value("DesignerLanguage");
    m_setting->endGroup();
    if (v.isValid()){
        return static_cast<QLocale::Language>(v.toInt()) ;
    } else {
        return QLocale::system().language();
    }
}

void DesignerSettingManager::currentDefaultLanguageChanged(QLocale::Language language)
{
    QMessageBox::information(0, tr("Warning") , tr("The language will change after the application is restarted"));
    m_setting->beginGroup("ReportDesigner");
    m_setting->setValue("DesignerLanguage", (int)language);
    m_setting->endGroup();
}
