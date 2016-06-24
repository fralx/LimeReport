#include "lrsettingdialog.h"
#include "ui_lrsettingdialog.h"

namespace LimeReport{

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
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

bool SettingDialog::suppressAbsentFieldsAndVarsWarnings()
{
    return ui->cbSuppressWarnings->isChecked();
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

} // namespace LimeReport
