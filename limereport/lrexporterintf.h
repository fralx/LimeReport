#ifndef LREXPORTERINTF_H
#define LREXPORTERINTF_H

#include <QString>
#include <QSharedPointer>
#include <QtPlugin>
#include "lrpageitemdesignintf.h"

namespace LimeReport {

class ReportExporterInterface {
public:
    virtual ~ReportExporterInterface(){}
    virtual bool exportPages(LimeReport::ReportPages pages, const QString& fileName, const QMap<QString, QVariant>& params = QMap<QString, QVariant>()) = 0;
    virtual QString exporterName() = 0;
    virtual QString exporterFileExt() = 0;
    virtual QString hint() = 0;
};

} // namespace LimeReport
#endif // LREXPORTERINTF_H
