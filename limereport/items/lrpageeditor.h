#ifndef LRPAGEEDITOR_H
#define LRPAGEEDITOR_H

#include <QDialog>
#include "lrpageitemdesignintf.h"
#include <QPushButton>
namespace Ui {
class lrpageeditor;
}

class lrpageeditor : public QDialog
{
    Q_OBJECT

public:
    explicit lrpageeditor(QWidget *parent = nullptr,LimeReport::PageItemDesignIntf *page = nullptr);
    ~lrpageeditor();

private slots:
    void on_buttonBox_accepted();
    void on_format_currentIndexChanged(int index);

private:
    Ui::lrpageeditor *ui;
    LimeReport::PageItemDesignIntf* m_page;

    void applyChanges();
    QSizeF getRectByPageSize(const LimeReport::PageItemDesignIntf::PageSize& size);
};

#endif // LRPAGEEDITOR_H
