#ifndef LANGUAGESELECTDIALOG_H
#define LANGUAGESELECTDIALOG_H

#include <QDialog>
#include <QLocale>

namespace Ui {
class LanguageSelectDialog;
}

class LanguageSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LanguageSelectDialog(QWidget *parent = 0);
    ~LanguageSelectDialog();
    QLocale::Language getSelectedLanguage();
private:
    Ui::LanguageSelectDialog *ui;
};

Q_DECLARE_METATYPE(QLocale::Language)

#endif // LANGUAGESELECTDIALOG_H
