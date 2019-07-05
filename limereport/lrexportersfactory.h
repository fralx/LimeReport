#ifndef LREXPORTERSFACTORY_H
#define LREXPORTERSFACTORY_H

#include "lrattribsabstractfactory.h"
#include "lrexporterintf.h"

namespace LimeReport{

typedef ReportExporterInterface* (*CreateExporter)(ReportEnginePrivate* parent);

struct ExporterAttribs{
    QString m_alias;
    QString m_tag;
    ExporterAttribs(){}
    ExporterAttribs(const QString& alias, const QString& tag):m_alias(alias),m_tag(tag){}
    bool operator==( const ExporterAttribs &right) const {
        return (m_alias==right.m_alias) && (m_tag==right.m_tag);
    }
};

class ExportersFactory : public AttribsAbstractFactory<LimeReport::ReportExporterInterface, QString, CreateExporter, ExporterAttribs>
{
private:
    friend class Singleton<ExportersFactory>;
private:
    ExportersFactory(){}
    ~ExportersFactory(){}
    ExportersFactory(const ExportersFactory&){}
    ExportersFactory& operator = (const ExportersFactory&){return *this;}
};

} // namespace LimeReport

#endif // LREXPORTERSFACTORY_H
