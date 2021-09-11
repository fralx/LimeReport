#include "SettingDialog.h"
#include "ui_SettingDialog.h"
#include <QPrinterInfo>

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    initPrinters();
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::initPrinters()
{
    foreach (QPrinterInfo pi, QPrinterInfo::availablePrinters()) {
        ui->certPrinter->addItem(pi.printerName());
        ui->otherPrinter->addItem(pi.printerName());
    }
}

void SettingDialog::setCertPrinterName(QString printerName){

    ui->certPrinter->setCurrentIndex(ui->certPrinter->findText(printerName));
}

void SettingDialog::setOtherPrinterName(QString printerName)
{
    ui->otherPrinter->setCurrentIndex(ui->otherPrinter->findText(printerName));
}

QString SettingDialog::certPrinterName(){
    return ui->certPrinter->currentText();
}

QString SettingDialog::othenPrinterName()
{
    return ui->otherPrinter->currentText();
}

bool SettingDialog::deleteLastFile(){
    return ui->checkBox->isChecked();
}

void SettingDialog::setDeleteLastFile(bool value)
{
    ui->checkBox->setChecked(value);
}

