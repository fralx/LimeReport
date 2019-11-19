#ifndef LREXCELEXPORTER_H
#define LREXCELEXPORTER_H

#include <QSet>
#include <xlsxdocument.h>
#include "lrexporterintf.h"


namespace LimeReport{

struct Grid{
    QSet<qreal> x;
    QSet<qreal> y;
};

class ExcelExporter : public QObject, public ReportExporterInterface{
    Q_OBJECT
public:
    ExcelExporter(ReportEnginePrivate *parent = 0);
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
    QXlsx::Format createTextItemFormat(BaseDesignIntf *item);
private:

    QVector<qreal>x;
    QVector<qreal>y;
    QVector<QString> exportedTypes;

    int m_xOffset;
    int m_yOffset;

    QXlsx::Document* m_document;

};

} //namespace LimeReport

#endif // LREXCELEXPORTER_H
