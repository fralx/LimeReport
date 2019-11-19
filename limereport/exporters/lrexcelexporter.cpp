#include "lrexcelexporter.h"
#include "lrtextitem.h"
#include "lrimageitem.h"
#include "lrshapeitem.h"
#include "lrexportersfactory.h"
#include "lrreportengine_p.h"

namespace{

LimeReport::ReportExporterInterface* createExcelExporter(LimeReport::ReportEnginePrivate* parent){
    return new LimeReport::ExcelExporter(parent);
}

bool VARIABLE_IS_NOT_USED registred = LimeReport::ExportersFactory::instance().registerCreator("XLSX", LimeReport::ExporterAttribs(QObject::tr("Export to xlsx"), "XLSXExporter"), createExcelExporter);

}

namespace  LimeReport{

ExcelExporter::ExcelExporter(ReportEnginePrivate *parent)
    :QObject(parent)
{
    exportedTypes << "ImageItem" << "TextItem";
}

QString ExcelExporter::exporterName()
{
    return QObject::tr("XLSX");
}

QString ExcelExporter::exporterFileExt()
{
    return "xlsx";
}

QString ExcelExporter::hint()
{
    return QObject::tr("Export report to xlsx");
}

bool ExcelExporter::exportPages(ReportPages pages, const QString &fileName, const QMap<QString, QVariant> &params)
{

    double xSizeFactor = params["xSizeFactor"].isNull() ? 2 : params["xSizeFactor"].toDouble();
    double ySizeFactor = params["ySizeFactor"].isNull() ? 0.37 : params["ySizeFactor"].toDouble();

    if (pages.isEmpty()) return false;

    m_document = new QXlsx::Document();

    x.clear();
    y.clear();

    Grid grid;

    if ((pages.size() > 1) && (pages[0]->pos() == pages[1]->pos()) ){
        int curHeight = 0;
        foreach(PageItemDesignIntf::Ptr pageItem, pages){
            pageItem->setPos(0,curHeight);
            curHeight+=pageItem->height();
        }
    }

    foreach(PageItemDesignIntf::Ptr page, pages){
        buildGrid(&grid,page.data());
    }

    foreach (qreal value, grid.x.values()) {
        x.append(value);
    }

    foreach (qreal value, grid.y.values()) {
        y.append(value);
    }

    std::sort(x.begin(), x.end());
    std::sort(y.begin(), y.end());

    m_yOffset = 0;
    m_xOffset = 0;

    m_document->setTopPageMargin(pages.first()->topMargin()/25.4);
    m_document->setLeftPageMargin(pages.first()->leftMargin()/25.4);
    m_document->setRightPageMargin(pages.first()->rightMargin()/25.4);
    m_document->setBottomPageMargin(pages.first()->bottomMargin()/25.4);

    for(int i = 0; i<x.size(); ++i){
        if ( i == 0 ){
            if (x[i] > 0){
                qreal columnWidth = x[i] - pages.at(0)->leftMargin();
                if (columnWidth>0){
                    m_document->setColumnWidth(i+1, columnWidth / xSizeFactor);
                    m_xOffset = 1;
                }
            }
        } else {
            m_document->setColumnWidth(i+1, (x[i]-x[i-1]) / xSizeFactor);
        }
    }

    for(int i = 0; i<y.size(); ++i){
        if ( i == 0 ) {
            if (y[i] > 0){
                qreal rowHeight = y[i] - pages.at(0)->topMargin();
                if (rowHeight > 0){
                    m_document->setRowHeight(i+1, rowHeight / ySizeFactor);
                    m_yOffset = 1;
                }
            }
        } else {
            m_document->setRowHeight(i+1, (y[i]-y[i-1]) / ySizeFactor);
        }
    }

    foreach(PageItemDesignIntf::Ptr page, pages){
        exportItem(page.data());
    }


    return m_document->saveAs(fileName);
}

void ExcelExporter::buildGrid(Grid *grid, BaseDesignIntf *item)
{
    foreach (BaseDesignIntf* child, item->childBaseItems()) {
        if (child->canContainChildren())
            buildGrid(grid, child);
        else{
            if (exportedTypes.contains(child->storageTypeName())){
                grid->x.insert(child->getAbsolutePosX());
                grid->x.insert(child->getAbsolutePosX()+child->getItemWidth());
                grid->y.insert(child->getAbsolutePosY());
                grid->y.insert(child->getAbsolutePosY()+child->getItemHeight());
            }
        }
    }

}

void ExcelExporter::exportItem(BaseDesignIntf *item)
{
    if (item->canContainChildren()){
        foreach (BaseDesignIntf* child, item->childBaseItems()){
            exportItem(child);
        }
    } else {
       createItemCell(item);
    }
}

QXlsx::Format ExcelExporter::createTextItemFormat(BaseDesignIntf* item){
    TextItem* textItem = dynamic_cast<TextItem*>(item);
    QXlsx::Format format;
    format.setTextWarp(true);
//    format.setFontName(item->font().family());
//    format.setFontBold(item->font().bold());
//    format.setFontItalic(item->font().italic());
//    format.setFontUnderline(item->font().underline() ? QXlsx::Format::FontUnderlineSingle : QXlsx::Format::FontUnderlineNone);
//    format.setFontSize(item->font().pointSize());
    format.setFont(item->font());
//    format.setFontSize(item->font().pointSize()-1);
    format.setFontColor(item->fontColor());
    format.setFillPattern(QXlsx::Format::PatternSolid);
    format.setPatternBackgroundColor(item->backgroundColor());
    format.setBorderColor(item->borderColor());

    if (item->borderLines() & BaseDesignIntf::TopLine){
        format.setTopBorderStyle(QXlsx::Format::BorderThin);
    }
    if (item->borderLines() & BaseDesignIntf::RightLine){
        format.setRightBorderStyle(QXlsx::Format::BorderThin);
    }
    if (item->borderLines() & BaseDesignIntf::LeftLine){
        format.setLeftBorderStyle(QXlsx::Format::BorderThin);
    }
    if (item->borderLines() & BaseDesignIntf::BottomLine){
        format.setBottomBorderStyle(QXlsx::Format::BorderThin);
    }

    if (textItem->alignment() & Qt::AlignLeft){
       format.setHorizontalAlignment(QXlsx::Format::AlignLeft);
    }
    if (textItem->alignment() & Qt::AlignRight){
       format.setHorizontalAlignment(QXlsx::Format::AlignRight);
    }
    if (textItem->alignment() & Qt::AlignHCenter){
       format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    }
    if (textItem->alignment() & Qt::AlignTop){
        format.setVerticalAlignment(QXlsx::Format::AlignTop);
    }
    if (textItem->alignment() & Qt::AlignBottom){
        format.setVerticalAlignment(QXlsx::Format::AlignBottom);
    }
    if (textItem->alignment() & Qt::AlignVCenter){
        format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    }

    return format;
}


void ExcelExporter::createItemCell(BaseDesignIntf *item)
{
    int startColumnIndex = cellStartColumnIndex(item);
    int endColumnIndex = cellEndColumnIndex(item);
    int startRowIndex = cellStartRowIndex(item);
    int endRowIndex = cellEndRowIndex(item);
    bool needMerge = (endColumnIndex - startColumnIndex > 1) || (endRowIndex - startRowIndex > 1);

    QXlsx::Format format;

    if (item->storageTypeName().compare("TextItem") == 0){
        TextItem* ci = dynamic_cast<TextItem*>(item);
        format = createTextItemFormat(item);
        m_document->write(startRowIndex, startColumnIndex, ci->content(), format);
        if (needMerge)
            m_document->mergeCells(QXlsx::CellRange(startRowIndex, startColumnIndex, endRowIndex-1, endColumnIndex-1), format);
    } else if (item->storageTypeName().compare("ImageItem") == 0){
        LimeReport::ImageItem* ii = dynamic_cast<ImageItem*>(item);
        QImage img = ii->image().scaled(item->width()/3,item->height()/3, Qt::KeepAspectRatio);
        m_document->insertImage(startRowIndex-1, startColumnIndex, img);
        if (needMerge)
            m_document->mergeCells(QXlsx::CellRange(startRowIndex, startColumnIndex, endRowIndex-1, endColumnIndex-1), format);
    } else {
        //m_document->write(startRowIndex, startColumnIndex, item->objectName());
    }


}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"

int ExcelExporter::cellStartColumnIndex(BaseDesignIntf *item)
{
    for(int i = 1; i <= x.size(); ++i){
        if (x[i-1] == item->getAbsolutePosX()) return i + m_xOffset;
    }
    return -1;
}

int ExcelExporter::cellEndColumnIndex(BaseDesignIntf *item)
{
    for(int i = 1; i <= x.size(); ++i){
        if (x[i-1] == (item->getAbsolutePosX() + item->getItemWidth()) ) return i + m_xOffset;
    }
    return -1;
}

int ExcelExporter::cellStartRowIndex(BaseDesignIntf *item)
{
    for(int i = 1; i <= y.size(); ++i){
        if (y[i-1] == item->getAbsolutePosY()) return i + m_yOffset;
    }
    return -1;
}

int ExcelExporter::cellEndRowIndex(BaseDesignIntf *item)
{
    for(int i = 1; i <= y.size(); ++i){
        if (y[i-1] == (item->getAbsolutePosY() + item->getItemHeight()) ) return i + m_yOffset;
    }
    return -1;
}

#pragma clang diagnostic pop
} //namespace LimeReport
