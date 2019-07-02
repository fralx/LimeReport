#ifndef LRDESIGNERPLUGININTERFACE_H
#define LRDESIGNERPLUGININTERFACE_H

#include <QString>
#include <QVariant>

#include <QtPlugin>
#include "lrreportdesignwindowintrerface.h"

QT_BEGIN_NAMESPACE
class QSettings;
class QMainWindow;
QT_END_NAMESPACE

namespace LimeReport {
    class ReportDesignWindow;
    class ReportEnginePrivateInterface;
}

class LimeReportDesignerPluginInterface {
public:
    virtual ~LimeReportDesignerPluginInterface() { }
    virtual LimeReport::ReportDesignWindowInterface* getDesignerWindow(LimeReport::ReportEnginePrivateInterface* report, QWidget *parent = 0, QSettings* settings=0) = 0;
};

Q_DECLARE_INTERFACE( LimeReportDesignerPluginInterface, "ru.limereport.LimeReport.DesignerPluginInterface/1.0" )

#endif // LRDESIGNERPLUGININTERFACE_H
