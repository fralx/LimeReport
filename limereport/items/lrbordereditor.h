#ifndef LRBORDEREDITOR_H
#define LRBORDEREDITOR_H

#include <QDialog>
#include "lrbasedesignintf.h"
namespace Ui {
class lrbordereditor;
}

class lrbordereditor : public QDialog
{
    Q_OBJECT

public:
    explicit lrbordereditor(QWidget *parent = nullptr);
    void loadItem(LimeReport::BaseDesignIntf *i);
    LimeReport::BaseDesignIntf::BorderLines borderSides();
    LimeReport::BaseDesignIntf::BorderStyle border_style();
    QString borderColor();
    double border_width();


    ~lrbordereditor();

private slots:
    void on_listWidget_currentRowChanged(int currentRow);

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_toolButton_4_clicked();

    void on_noLines_clicked();

    void on_topLine_clicked();
    void checkToolButtons(int side, bool check);

    void on_bottomLine_clicked();

    void on_leftLine_clicked();

    void on_toolButton_3_clicked();

private:
    Ui::lrbordereditor *ui;
    LimeReport::BaseDesignIntf *item;
    QString border_color;
    int borderStyle = 1;
    double borderWidth = 1;


};


#endif // LRBORDEREDITOR_H
