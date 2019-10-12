#include <QPrinter>

#include "lrpdfexporter.h"
#include "lrexportersfactory.h"
#include "lrreportengine_p.h"

namespace{

LimeReport::ReportExporterInterface* createPDFExporter(LimeReport::ReportEnginePrivate* parent){
    return new LimeReport::PDFExporter(parent);
}

bool VARIABLE_IS_NOT_USED registred = LimeReport::ExportersFactory::instance().registerCreator("PDF", LimeReport::ExporterAttribs(QObject::tr("Export to PDF"), "PDFExporter"), createPDFExporter);

}

namespace LimeReport{

PDFExporter::PDFExporter(ReportEnginePrivate *parent) : QObject(parent), m_reportEngine(parent)
{}

bool PDFExporter::exportPages(ReportPages pages, const QString &fileName, const QMap<QString, QVariant> &params)
{
    Q_UNUSED(params);
    if (!fileName.isEmpty()){
        QPrinter printer;
        printer.setOutputFileName(fileName);
        printer.setOutputFormat(QPrinter::PdfFormat);
        if (!pages.isEmpty()){
            m_reportEngine->printPages(pages, &printer);
        }
        m_reportEngine->emitPrintedToPDF(fileName);
        return true;
    }
    return false;
}

}
