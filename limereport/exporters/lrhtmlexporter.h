#ifndef LREXCELEXPORTER_H
#define LREXCELEXPORTER_H

#include <QSet>
#include <QtXml>
#include "lrexporterintf.h"

namespace LimeReport{

struct GridItem{
    BaseDesignIntf* item;
    long    endRow;
    long    endColumn;
public:
    GridItem(BaseDesignIntf* item, long endRow, long endColumn);
};

class HTMLDocument{
public:
    HTMLDocument();
    bool saveAs(const QString& fileName);
    QDomElement* header(){return &m_header;}
    QDomElement* body(){return &m_body;}
    QDomNode* table(){return &m_table;}
    QDomElement createElement(const QString& name);
    void initGrid(QSet<qreal>columns, QSet<qreal>rows);
    void write(int startRow, int startColumn, int endRow, int endColumn, BaseDesignIntf *item);
private:
    void preapareHTML();
private:
    QDomDocument m_htmlDocument;
    QDomElement m_header;
    QDomElement m_body;
    QDomNode m_table;
    QList<GridItem> m_items;
    QVector<QVector<GridItem*>> m_grid;
};

struct Grid{
    QSet<qreal> x;
    QSet<qreal> y;
};

class HTMLExporter : public QObject, public ReportExporterInterface{
    Q_OBJECT
public:
    HTMLExporter(QObject *parent = 0);
    // IReportExporter interface
    QString exporterName();
    QString exporterFileExt();
    QString hint();
    bool exportPages(ReportPages pages, const QString& fileName, const QMap<QString, QVariant>& params = QMap<QString, QVariant>());
private:
    void buildGrid(Grid* grid, BaseDesignIntf* item);
    void exportItem(BaseDesignIntf* item);
    void createItemCell(BaseDesignIntf* item);
    int cellStartColumnIndex(BaseDesignIntf* item);
    int cellEndColumnIndex(BaseDesignIntf* item);
    int cellStartRowIndex(BaseDesignIntf* item);
    int cellEndRowIndex(BaseDesignIntf* item);
//    QXlsx::Format createTextItemFormat(BaseDesignIntf *item);
private:

    QVector<qreal>x;
    QVector<qreal>y;
    QVector<QString> exportedTypes;

    int m_xOffset;
    int m_yOffset;
    HTMLDocument* m_document;

};

} //namespace LimeReport

#endif // LREXCELEXPORTER_H
