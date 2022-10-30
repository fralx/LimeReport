#include "lrpageeditor.h"
#include "ui_lrpageeditor.h"
#include "lrpageitemdesignintf.h"
#include <QPushButton>
#include <QPageSize>

using namespace LimeReport;

lrpageeditor::lrpageeditor(QWidget *parent, PageItemDesignIntf *page) :
    QDialog(parent),
    ui(new Ui::lrpageeditor)
{
    ui->setupUi(this);
    m_page = page;
    //Paper
    QMetaEnum pageSizes = page->metaObject()->property(page->metaObject()->indexOfProperty("pageSize")).enumerator();

    for (int i=0;i<pageSizes.keyCount();i++){
        ui->format->addItem(pageSizes.key(i));
    }
    ui->format->setCurrentIndex(m_page->pageSize());
    ui->width->setValue(m_page->width() / Const::mmFACTOR);
    ui->height->setValue(m_page->height() / Const::mmFACTOR);
    ui->portrait->setChecked(m_page->pageOrientation() == PageItemDesignIntf::Portrait);
    ui->landscape->setChecked(m_page->pageOrientation() == PageItemDesignIntf::Landscape);
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

lrpageeditor::~lrpageeditor()
{
    delete ui;
}

void lrpageeditor::applyChanges()
{
    m_page->setPageSize(static_cast<PageItemDesignIntf::PageSize>(ui->format->currentIndex()));
    m_page->setWidth(ui->width->value()* Const::mmFACTOR);
    m_page->setHeight(ui->height->value()*  Const::mmFACTOR);
    m_page->setPageOrientation(ui->portrait->isChecked()?PageItemDesignIntf::Portrait : PageItemDesignIntf::Landscape);

    m_page->setTopMargin(ui->marginTop->value());
    m_page->setBottomMargin(ui->marginBottom->value());
    m_page->setRightMargin(ui->marginRight->value());
    m_page->setLeftMargin(ui->marginLeft->value());
    m_page->setDropPrinterMargins(ui->dropPrinterMargins->isChecked());
    ui->endlessHeight->setChecked(ui->endlessHeight->isChecked());
    m_page->setExtendedHeight(ui->extendedHeight->value());
}

void lrpageeditor::on_buttonBox_accepted()
{
    applyChanges();
    accept();

}

QSizeF lrpageeditor::getRectByPageSize(const PageItemDesignIntf::PageSize& size)
{
    if (size != PageItemDesignIntf::Custom) {
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
        printer.setOrientation(ui->portrait->isChecked()?QPrinter::Portrait : QPrinter::Landscape);
        printer.setPaperSize((QPrinter::PageSize)size);
        return QSizeF(printer.paperSize(QPrinter::Millimeter).width() * 10,
                      printer.paperSize(QPrinter::Millimeter).height() * 10);

#else
        QPageSize pageSize = QPageSize((QPageSize::PageSizeId)size);
        qreal width = pageSize.size(QPageSize::Millimeter).width() * 10;
        qreal height = pageSize.size(QPageSize::Millimeter).height() * 10;
        return QSizeF(m_page->pageOrientation() == PageItemDesignIntf::Portrait ? width : height,
                      m_page->pageOrientation() == PageItemDesignIntf::Orientation::Portrait ? height : width);

//        printer.setPageOrientation((QPageLayout::Orientation)pageOrientation());
//        printer.setPageSize(QPageSize((QPageSize::PageSizeId)size));
//        return QSizeF(printer.pageLayout().pageSize().size(QPageSize::Millimeter).width() * 10,
//                      printer.pageLayout().pageSize().size(QPageSize::Millimeter).height() * 10);
#endif
    }

    else {
        return QSizeF(width(),height());
    }
}
void lrpageeditor::on_format_currentIndexChanged(int index)
{
    QPageSize ps = *new QPageSize();
    if(ui->format->currentText() != "Custom")
    {
        QSizeF pageSize = getRectByPageSize(static_cast<PageItemDesignIntf::PageSize>(index));
    ui->width->setValue(pageSize.width()/10);
    ui->height->setValue(pageSize.height()/10);
    }

}

