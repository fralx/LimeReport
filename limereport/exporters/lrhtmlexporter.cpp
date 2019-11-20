#include "lrhtmlexporter.h"
#include "lrtextitem.h"
#include "lrimageitem.h"
#include "lrshapeitem.h"
#include "lrexportersfactory.h"
#include "lrreportengine_p.h"

namespace{

LimeReport::ReportExporterInterface* createExcelExporter(LimeReport::ReportEnginePrivate* parent){
    return new LimeReport::HTMLExporter(parent);
}

bool VARIABLE_IS_NOT_USED registred = LimeReport::ExportersFactory::instance().registerCreator("HTML", LimeReport::ExporterAttribs(QObject::tr("Export to HTML"), "HTMLExporter"), createExcelExporter);

}

namespace  LimeReport{

HTMLExporter::HTMLExporter(QObject* parent)
    :QObject(parent)
{
    exportedTypes << "ImageItem" << "TextItem";
}

QString HTMLExporter::exporterName()
{
    return QObject::tr("HTML");
}

QString HTMLExporter::exporterFileExt()
{
    return "html";
}

QString HTMLExporter::hint()
{
    return QObject::tr("Export report to html");
}

bool HTMLExporter::exportPages(ReportPages pages, const QString &fileName, const QMap<QString, QVariant> &params)
{

    double xSizeFactor = params["xSizeFactor"].isNull() ? 2 : params["xSizeFactor"].toDouble();
    double ySizeFactor = params["ySizeFactor"].isNull() ? 0.37 : params["ySizeFactor"].toDouble();

    if (pages.isEmpty()) return false;

    m_document = new HTMLDocument();

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

    m_document->initGrid(grid.x, grid.y);

//    m_document->setTopPageMargin(pages.first()->topMargin()/25.4);
//    m_document->setLeftPageMargin(pages.first()->leftMargin()/25.4);
//    m_document->setRightPageMargin(pages.first()->rightMargin()/25.4);
//    m_document->setBottomPageMargin(pages.first()->bottomMargin()/25.4);

    for(int i = 0; i<x.size(); ++i){
        if ( i == 0 ){
            if (x[i] > 0){
                qreal columnWidth = x[i] - pages.at(0)->leftMargin();
                if (columnWidth>0){
//                    m_document->setColumnWidth(i+1, columnWidth / xSizeFactor);
                    m_xOffset = 1;
                }
            }
        } else {
//            m_document->setColumnWidth(i+1, (x[i]-x[i-1]) / xSizeFactor);
        }
    }

    for(int i = 0; i<y.size(); ++i){
        if ( i == 0 ) {
            if (y[i] > 0){
                qreal rowHeight = y[i] - pages.at(0)->topMargin();
                if (rowHeight > 0){
//                    m_document->setRowHeight(i+1, rowHeight / ySizeFactor);
                    m_yOffset = 1;
                }
            }
        } else {
//            m_document->setRowHeight(i+1, (y[i]-y[i-1]) / ySizeFactor);
        }
    }

    foreach(PageItemDesignIntf::Ptr page, pages){
        exportItem(page.data());
    }


    return m_document->saveAs(fileName); //m_document->saveAs(fileName);
}

void HTMLExporter::buildGrid(Grid *grid, BaseDesignIntf *item)
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

void HTMLExporter::exportItem(BaseDesignIntf *item)
{
    if (item->canContainChildren()){
        foreach (BaseDesignIntf* child, item->childBaseItems()){
            exportItem(child);
        }
    } else {
       createItemCell(item);
    }
}

//QXlsx::Format HTMLExporter::createTextItemFormat(BaseDesignIntf* item){
//    TextItem* textItem = dynamic_cast<TextItem*>(item);
//    QXlsx::Format format;
//    format.setTextWarp(true);
////    format.setFontName(item->font().family());
////    format.setFontBold(item->font().bold());
////    format.setFontItalic(item->font().italic());
////    format.setFontUnderline(item->font().underline() ? QXlsx::Format::FontUnderlineSingle : QXlsx::Format::FontUnderlineNone);
////    format.setFontSize(item->font().pointSize());
//    format.setFont(item->font());
////    format.setFontSize(item->font().pointSize()-1);
//    format.setFontColor(item->fontColor());
//    format.setFillPattern(QXlsx::Format::PatternSolid);
//    format.setPatternBackgroundColor(item->backgroundColor());
//    format.setBorderColor(item->borderColor());

//    if (item->borderLines() & BaseDesignIntf::TopLine){
//        format.setTopBorderStyle(QXlsx::Format::BorderThin);
//    }
//    if (item->borderLines() & BaseDesignIntf::RightLine){
//        format.setRightBorderStyle(QXlsx::Format::BorderThin);
//    }
//    if (item->borderLines() & BaseDesignIntf::LeftLine){
//        format.setLeftBorderStyle(QXlsx::Format::BorderThin);
//    }
//    if (item->borderLines() & BaseDesignIntf::BottomLine){
//        format.setBottomBorderStyle(QXlsx::Format::BorderThin);
//    }

//    if (textItem->alignment() & Qt::AlignLeft){
//       format.setHorizontalAlignment(QXlsx::Format::AlignLeft);
//    }
//    if (textItem->alignment() & Qt::AlignRight){
//       format.setHorizontalAlignment(QXlsx::Format::AlignRight);
//    }
//    if (textItem->alignment() & Qt::AlignHCenter){
//       format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
//    }
//    if (textItem->alignment() & Qt::AlignTop){
//        format.setVerticalAlignment(QXlsx::Format::AlignTop);
//    }
//    if (textItem->alignment() & Qt::AlignBottom){
//        format.setVerticalAlignment(QXlsx::Format::AlignBottom);
//    }
//    if (textItem->alignment() & Qt::AlignVCenter){
//        format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
//    }

//    return format;
//}


void HTMLExporter::createItemCell(BaseDesignIntf *item)
{
    int startColumnIndex = cellStartColumnIndex(item);
    int endColumnIndex = cellEndColumnIndex(item);
    int startRowIndex = cellStartRowIndex(item);
    int endRowIndex = cellEndRowIndex(item);
    bool needMerge = (endColumnIndex - startColumnIndex > 1) || (endRowIndex - startRowIndex > 1);

//    QXlsx::Format format;

    if (item->storageTypeName().compare("TextItem") == 0 || item->storageTypeName().compare("ImageItem") == 0){
//        TextItem* ci = dynamic_cast<TextItem*>(item);
//        format = createTextItemFormat(item);
        m_document->write(startRowIndex-1, startColumnIndex-1, endRowIndex-1, endColumnIndex-1, item/*, format*/);
//        if (needMerge)
//            m_document->mergeCells(QXlsx::CellRange(startRowIndex, startColumnIndex, endRowIndex-1, endColumnIndex-1), format);
//    } else if (item->storageTypeName().compare("ImageItem") == 0){
//        m_document->write(startRowIndex, startColumnIndex, endRowIndex, endColumnIndex, item/*, format*/);
//        LimeReport::ImageItem* ii = dynamic_cast<ImageItem*>(item);
//        QImage img = ii->image().scaled(item->width()/3,item->height()/3, Qt::KeepAspectRatio);
//        m_document->insertImage(startRowIndex-1, startColumnIndex, img);
//        if (needMerge)
//            m_document->mergeCells(QXlsx::CellRange(startRowIndex, startColumnIndex, endRowIndex-1, endColumnIndex-1), format);
    } else {
        //m_document->write(startRowIndex, startColumnIndex, item->objectName());
    }

}

int HTMLExporter::cellStartColumnIndex(BaseDesignIntf *item)
{
    for(int i = 1; i <= x.size(); ++i){
        if (x[i-1] == item->getAbsolutePosX()) return i + m_xOffset;
    }
    return -1;
}

int HTMLExporter::cellEndColumnIndex(BaseDesignIntf *item)
{
    for(int i = 1; i <= x.size(); ++i){
        if (x[i-1] == (item->getAbsolutePosX() + item->getItemWidth()) ) return i + m_xOffset;
    }
    return -1;
}

int HTMLExporter::cellStartRowIndex(BaseDesignIntf *item)
{
    for(int i = 1; i <= y.size(); ++i){
        if (y[i-1] == item->getAbsolutePosY()) return i + m_yOffset;
    }
    return -1;
}

int HTMLExporter::cellEndRowIndex(BaseDesignIntf *item)
{
    for(int i = 1; i <= y.size(); ++i){
        if (y[i-1] == (item->getAbsolutePosY() + item->getItemHeight()) ) return i + m_yOffset;
    }
    return -1;
}

HTMLDocument::HTMLDocument()
{
    m_header = m_htmlDocument.createElement("head");
    m_htmlDocument.appendChild(m_header);
    m_body = m_htmlDocument.createElement("body");
    m_htmlDocument.appendChild(m_body);
    m_table = m_body.appendChild(m_htmlDocument.createElement("table")).appendChild(m_htmlDocument.createElement("tbody"));
    m_body.setAttribute("style","background-color: #fff");

}

bool HTMLDocument::saveAs(const QString &fileName)
{
    QFile xmlFile(fileName);
    if (xmlFile.open(QFile::WriteOnly)) {
        QTextStream buffer(&xmlFile);
        m_htmlDocument.save(buffer,2);
        xmlFile.close();
        return true;
    }
    return false;
}

QDomElement HTMLDocument::createElement(const QString &name){
    return  m_htmlDocument.createElement(name);
}

void HTMLDocument::initGrid(QSet<qreal> columns, QSet<qreal> rows)
{
    m_grid.clear();
    m_items.clear();

    QDomNode columnsRow = table()->appendChild(createElement("tr"));
    QDomElement firstEmpltyColumn = createElement("td");
    firstEmpltyColumn.setAttribute("style","width:0px;height:0px;");
    columnsRow.appendChild(firstEmpltyColumn);
    foreach(qreal value, columns.values()){
        QDomElement curColumn = createElement("td");
        curColumn.setAttribute("style",QString("width:%1px").arg(static_cast<int>(value)));
        columnsRow.appendChild(curColumn);
        QVector<GridItem*> row;
        for(int i = 0; i<rows.values().size(); ++i){
            row.append(0);
        }
        m_grid.append(row);
    }

}

void HTMLDocument::write(int startRow, int startColumn, int endRow, int endColumn, BaseDesignIntf* item)
{
    m_items.append(GridItem(item, endRow, endColumn));
    for (int i = startRow; i < endRow; ++i) {
        for (int j = startColumn; j < endColumn; ++j) {
            m_grid[i][j] = &m_items.last();
        }
    }
}

void HTMLDocument::preapareHTML()
{
    for (int i = 0; i<m_grid.size(); ++i) {
        for (int j = 0; j<m_grid[i].size(); ++j) {

        }
    }
};

GridItem::GridItem(BaseDesignIntf *item, long endRow, long endColumn)
{
    this->item = item;
    this->endRow = endRow;
    this->endColumn = endColumn;
}

} //namespace LimeReport
