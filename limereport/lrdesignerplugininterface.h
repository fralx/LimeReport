#ifndef LRDESIGNERPLUGININTERFACE_H
#define LRDESIGNERPLUGININTERFACE_H

#include <QString>
#include <QVariant>

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QSettings;
class QMainWindow;
QT_END_NAMESPACE

namespace LimeReport {
    class ReportDesignWindow;
    class ReportEnginePrivateInterface;
}

class LimeReportPluginInterface {
public:
    virtual ~LimeReportPluginInterface() { }

    virtual QString getString() const = 0;
    virtual QVariant getVar() const = 0;
    virtual QMainWindow* getDesignerWindow(LimeReport::ReportEnginePrivateInterface* report, QWidget *parent = 0, QSettings* settings=0) = 0;
};

Q_DECLARE_INTERFACE( LimeReportPluginInterface, "ru.limereport.LimeReport.DesignerPluginInterface/1.0" )

#endif // LRDESIGNERPLUGININTERFACE_H
