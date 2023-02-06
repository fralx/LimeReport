#ifndef LRPAGEEDITOR_H
#define LRPAGEEDITOR_H

#include <QDialog>
#include "lrpageitemdesignintf.h"
#include <QPushButton>
namespace LimeReport{

namespace Ui {
    class PageEditor;
}

class LIMEREPORT_EXPORT PageEditor : public QDialog
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
    Ui::PageEditor *ui;
    LimeReport::PageItemDesignIntf* m_page;

    void applyChanges();
    QSizeF getRectByPageSize(const LimeReport::PageItemDesignIntf::PageSize& size);
};

} // namespace LimeReport

#endif // LRPAGEEDITOR_H
