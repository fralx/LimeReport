#ifndef DESIGNERSETTINGMANAGER_H
#define DESIGNERSETTINGMANAGER_H

#include <QObject>
#include <QLocale>
#include <QApplication>
#include <QSettings>

class DesignerSettingManager : public QObject
{
    Q_OBJECT
public:
    explicit DesignerSettingManager(QObject *parent = 0);
    ~DesignerSettingManager();
    void setApplicationInstance(QApplication* application);
public slots:
    void getAviableLanguages(QList<QLocale::Language>* languages);
    QLocale::Language getCurrentDefaultLanguage();
    void currentDefaulLanguageChanged(QLocale::Language language);
private:
    QApplication* m_app;
    QSettings* m_setting;
};

#endif // DESIGNERSETTINGMANAGER_H
