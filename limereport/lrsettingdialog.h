#ifndef LRSETTINGDIALOG_H
#define LRSETTINGDIALOG_H

#include <QDialog>
#include <QLocale>
#include <QSettings>

namespace LimeReport{

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();
    int verticalGridStep();
    int horizontalGridStep();
    QFont defaultFont();
    QFont scriptFont();
    int tabIndention();
    QString theme();
    bool suppressAbsentFieldsAndVarsWarnings();
    QLocale::Language designerLanguage();
    QString reportUnits();
    void setSuppressAbsentFieldsAndVarsWarnings(bool value);
    void setHorizontalGridStep(int value);
    void setVerticalGridStep(int value);
    void setDefaultFont(const QFont& value);
    void setScriptFont(const QFont& value);
    void setScritpTabIndention(int size);
    void setTheme(const QString& theme);
    void setDesignerLanguages(QList<QLocale::Language> languages, QLocale::Language currentLanguage);
    void setDesignerThemes(QList<QString> themes, const QString& currentTheme);
    void setDesignerUnites(QList<QString> unitTypes, const QString currentUnitType);
    void setSettings(QSettings* settings);
private slots:
    void on_bbOkCancel_accepted();
private:
    bool isFileExists(const QString& path);
private:
    Ui::SettingDialog *ui;
    QList<QLocale::Language> m_aviableLanguages;
    QLocale::Language m_currentLanguage;
    QSettings* m_settings;
};
} // namespace LimeReport

#endif // LRSETTINGDIALOG_H
