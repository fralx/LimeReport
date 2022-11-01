#ifndef LRBORDEREDITOR_H
#define LRBORDEREDITOR_H

#include <QDialog>
#include "lrbasedesignintf.h"

namespace LimeReport{

namespace Ui {
    class BorderEditor;
}


class LIMEREPORT_EXPORT BorderEditor : public QDialog
{
    Q_OBJECT

public:
    explicit BorderEditor(QWidget *parent = nullptr);
    void loadItem(LimeReport::BaseDesignIntf *item);
    LimeReport::BaseDesignIntf::BorderLines borderSides();
    LimeReport::BaseDesignIntf::BorderStyle borderStyle();
    QString borderColor();
    double borderWidth();
    ~BorderEditor();

private slots:
    void on_listWidget_currentRowChanged(int currentRow);
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_noLines_clicked();
    void on_topLine_clicked(bool checked);
    void on_bottomLine_clicked(bool checked);
    void on_leftLine_clicked(bool checked);
    void on_rightLine_clicked(bool checked);
    void on_allLines_clicked();
    void checkToolButtons(LimeReport::BaseDesignIntf::BorderSide side, bool check);
    void on_selectColor_clicked();

private:
    Ui::BorderEditor *ui;
    LimeReport::BaseDesignIntf *m_item;
    QString m_borderColor;
    int m_borderStyle;
    double m_borderWidth;
};

} // namespace LimeReport

#endif // LRBORDEREDITOR_H
