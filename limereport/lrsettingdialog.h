#ifndef LRSETTINGDIALOG_H
#define LRSETTINGDIALOG_H

#include <QDialog>

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
    bool suppressAbsentFieldsAndVarsWarnings();
    void setSuppressAbsentFieldsAndVarsWarnings(bool value);
    void setHorizontalGridStep(int value);
    void setVerticalGridStep(int value);
    void setDefaultFont(const QFont& value);
private:
    Ui::SettingDialog *ui;
};
} // namespace LimeReport

#endif // LRSETTINGDIALOG_H
