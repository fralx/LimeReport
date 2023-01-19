#ifndef LANGUAGESELECTDIALOG_H
#define LANGUAGESELECTDIALOG_H

#include <QDialog>
#include <QLocale>

namespace Ui {
class LanguageSelectDialog;
}

class LanguageSelectDialog : public QDialog {
    Q_OBJECT

  public:
    explicit LanguageSelectDialog(QWidget *parent = 0);
    ~LanguageSelectDialog();
    QLocale::Language getSelectedLanguage();

  private:
    Ui::LanguageSelectDialog *ui;
};

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
Q_DECLARE_METATYPE(QLocale::Language)
#endif

#endif // LANGUAGESELECTDIALOG_H
