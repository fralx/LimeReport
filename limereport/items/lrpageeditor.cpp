#include "lrpageeditor.h"
#include "ui_lrpageeditor.h"
#include "lrpagedesignintf.h"
#include "lrpageitemdesignintf.h"
#include <QPushButton>
#include <QPageSize>

namespace LimeReport{

PageEditor::PageEditor(QWidget *parent, LimeReport::PageItemDesignIntf *page) :
    QDialog(parent),
    ui(new Ui::PageEditor)
{
    ui->setupUi(this);
    m_page = page;
    //Paper
    QMetaEnum pageSizes = page->metaObject()->property(page->metaObject()->indexOfProperty("pageSize")).enumerator();

    for (int i=0;i<pageSizes.keyCount();i++){
        ui->format->addItem(pageSizes.key(i));
    }
    ui->format->setCurrentIndex(m_page->pageSize());
    ui->width->setValue(m_page->width() / m_page->unitFactor());
    ui->height->setValue(m_page->height() / m_page->unitFactor());
    ui->portrait->setChecked(m_page->pageOrientation() == LimeReport::PageItemDesignIntf::Portrait);
    ui->landscape->setChecked(m_page->pageOrientation() == LimeReport::PageItemDesignIntf::Landscape);
    //Margins
    ui->marginTop->setValue(m_page->topMargin());
    ui->marginRight->setValue(m_page->rightMargin());
    ui->marginLeft->setValue(m_page->leftMargin());
    ui->marginBottom->setValue(m_page->bottomMargin());
    ui->dropPrinterMargins->setChecked(m_page->dropPrinterMargins());

    //Other
    ui->endlessHeight->setChecked(m_page->endlessHeight());
    ui->extendedHeight->setValue(m_page->extendedHeight());
    ui->fullPage->setChecked(m_page->fullPage());
}

PageEditor::~PageEditor()
{
    delete ui;
}

void PageEditor::applyChanges()
{
    m_page->setPageSize(static_cast<LimeReport::PageItemDesignIntf::PageSize>(ui->format->currentIndex()));
    m_page->setWidth(ui->width->value() * LimeReport::Const::mmFACTOR);
    m_page->setHeight(ui->height->value() *  LimeReport::Const::mmFACTOR);
    m_page->setPageOrientation(ui->portrait->isChecked()? LimeReport::PageItemDesignIntf::Portrait : LimeReport::PageItemDesignIntf::Landscape);
    m_page->setTopMargin(ui->marginTop->value());
    m_page->setBottomMargin(ui->marginBottom->value());
    m_page->setRightMargin(ui->marginRight->value());
    m_page->setLeftMargin(ui->marginLeft->value());
    m_page->setDropPrinterMargins(ui->dropPrinterMargins->isChecked());
    ui->endlessHeight->setChecked(ui->endlessHeight->isChecked());
    m_page->setExtendedHeight(ui->extendedHeight->value());
    ui->width->setValue(m_page->getItemWidth());
    ui->height->setValue(m_page->getItemHeight());
}

QSizeF PageEditor::getRectByPageSize(const LimeReport::PageItemDesignIntf::PageSize& size)
{
    if (size != PageItemDesignIntf::Custom) {
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
        printer.setOrientation(ui->portrait->isChecked() ? QPrinter::Portrait : QPrinter::Landscape);
        printer.setPaperSize((QPrinter::PageSize)size);
        return QSizeF(printer.paperSize(QPrinter::Millimeter).width() * m_page->unitFactor(),
                      printer.paperSize(QPrinter::Millimeter).height() * m_page->unitFactor());

#else
        printer.setPageOrientation((QPageLayout::Orientation)m_page->pageOrientation());
        printer.setPageSize(QPageSize((QPageSize::PageSizeId)size));
        return QSizeF(printer.pageLayout().pageSize().size(QPageSize::Millimeter).width() * m_page->unitFactor(),
                      printer.pageLayout().pageSize().size(QPageSize::Millimeter).height() * m_page->unitFactor());
#endif
    } else {
        return QSizeF(m_page->getItemWidth(), m_page->getItemHeight());
    }
}

void PageEditor::on_format_currentIndexChanged(int index)
{
    QPageSize ps = *new QPageSize();
    if(ui->format->currentText() != "Custom")
    {
        QSizeF pageSize = getRectByPageSize(static_cast<LimeReport::PageItemDesignIntf::PageSize>(index));
        ui->width->setValue(pageSize.width() / m_page->unitFactor());
        ui->height->setValue(pageSize.height() / m_page->unitFactor());
    }

}


void PageEditor::on_buttonBox_clicked(QAbstractButton *button)
{
    switch(ui->buttonBox->buttonRole(button)){
        case QDialogButtonBox::ApplyRole:
            applyChanges();
            break;
        case QDialogButtonBox::AcceptRole:
            applyChanges();
            accept();
    }

}

} // namespace
