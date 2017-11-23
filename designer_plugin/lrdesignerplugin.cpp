#include "lrdesignerplugin.h"

#include <QRect>
#include "lrreportdesignwindow.h"

DesignerFactoryPlugin::~DesignerFactoryPlugin() {
}

QString DesignerFactoryPlugin::getString() const {
    return "Hello, Plugin!";
}

QVariant DesignerFactoryPlugin::getVar() const {
    return QRect( 10, 10, 500, 500 );
}

QMainWindow* DesignerFactoryPlugin::getDesignerWindow(LimeReport::ReportEnginePrivateInterface* report, QWidget* parent, QSettings* settings)
{
    LimeReport::ReportDesignWindow* designerWindow = new LimeReport::ReportDesignWindow(report, parent, settings);

    settings->beginGroup("DesignerWindow");
    designerWindow->setAttribute(Qt::WA_DeleteOnClose,true);
    designerWindow->setWindowIcon(QIcon(":report/images/logo32"));
    designerWindow->setShowProgressDialog(settings->value("showProgressDialog").toBool());
    settings->endGroup();

    return designerWindow;
}

//Q_EXPORT_PLUGIN2( LimeReportPluginInterface, DesignerFactoryPlugin )

