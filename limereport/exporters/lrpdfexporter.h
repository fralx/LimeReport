#ifndef LRPDFEXPORTER_H
#define LRPDFEXPORTER_H

#include <QObject>
#include "lrexporterintf.h"

namespace LimeReport{
class ReportEnginePrivate;

class PDFExporter : public QObject, public ReportExporterInterface
{
    Q_OBJECT
public:
    explicit PDFExporter(ReportEnginePrivate *parent = NULL);
    // ReportExporterInterface interface
    bool exportPages(ReportPages pages, const QString &fileName, const QMap<QString, QVariant> &params);
    QString exporterName()
    {
        return "PDF";
    }
    QString exporterFileExt()
    {
        return "pdf";
    }
    QString hint()
    {
        return tr("Export to PDF");
    }
private:
    ReportEnginePrivate* m_reportEngine;
};

} //namespace LimeReport

#endif // LRPDFEXPORTER_H
