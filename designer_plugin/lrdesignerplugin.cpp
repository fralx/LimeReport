#include "lrdesignerplugin.h"

#include "lrreportdesignwindow.h"

#include <QRect>

ReportDesignerFactoryPlugin::~ReportDesignerFactoryPlugin() { }

LimeReport::ReportDesignWindowInterface*
ReportDesignerFactoryPlugin::getDesignerWindow(LimeReport::ReportEnginePrivateInterface* report,
                                               QWidget* parent, QSettings* settings)
{
    return new LimeReport::ReportDesignWindow(report, parent, settings);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(LimeReportPluginInterface, ReportDesignerFactoryPlugin)
#endif
