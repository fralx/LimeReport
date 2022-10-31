#ifndef LRPAGEEDITOR_H
#define LRPAGEEDITOR_H

#include <QDialog>
#include "lrpageitemdesignintf.h"
#include <QPushButton>
namespace Ui {
class lrpageeditor;
}

class PageEditor : public QDialog
{
    Q_OBJECT

public:
    explicit PageEditor(QWidget *parent = nullptr,LimeReport::PageItemDesignIntf *page = nullptr);
    ~PageEditor();

private slots:
//    void on_buttonBox_accepted();
    void on_format_currentIndexChanged(int index);
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::lrpageeditor *ui;
    LimeReport::PageItemDesignIntf* m_page;

    void applyChanges();
    QSizeF getRectByPageSize(const LimeReport::PageItemDesignIntf::PageSize& size);
};

#endif // LRPAGEEDITOR_H
