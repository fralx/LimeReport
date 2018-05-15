#ifndef LRSETTINGDIALOG_H
#define LRSETTINGDIALOG_H

#include <QDialog>
#include <QLocale>

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
    bool userDarkTheme();
    bool suppressAbsentFieldsAndVarsWarnings();
    QLocale::Language designerLanguage();
    void setSuppressAbsentFieldsAndVarsWarnings(bool value);
    void setHorizontalGridStep(int value);
    void setVerticalGridStep(int value);
    void setDefaultFont(const QFont& value);
    void setUseDarkTheme(bool value);
    void setDesignerLanguages(QList<QLocale::Language> languages, QLocale::Language currentLanguage);
private:
    Ui::SettingDialog *ui;
    QList<QLocale::Language> m_aviableLanguages;
    QLocale::Language m_currentLanguage;
};
} // namespace LimeReport

#endif // LRSETTINGDIALOG_H
