#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();
    void setCertPrinterName(QString printerName);
    void setOtherPrinterName(QString printerName);
    QString certPrinterName();
    QString othenPrinterName();
    bool deleteLastFile();
    void setDeleteLastFile(bool value);
private:
    void initPrinters();

private:
    Ui::SettingDialog *ui;
};

#endif // SETTINGDIALOG_H
