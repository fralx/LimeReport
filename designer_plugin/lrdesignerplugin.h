#ifndef LRDESIGNERPLUGIN_H
#define LRDESIGNERPLUGIN_H

#include <QMainWindow>
#include <lrdesignerplugininterface.h>

class ReportDesignerFactoryPlugin : public QObject, public LimeReportDesignerPluginInterface {
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "ru.limereport.DersignerFactoryInterface")
#endif
    Q_INTERFACES( LimeReportDesignerPluginInterface )

public:
    ~ReportDesignerFactoryPlugin();
    LimeReport::ReportDesignWindowInterface* getDesignerWindow(LimeReport::ReportEnginePrivateInterface* report, QWidget* parent, QSettings* settings);
};

#endif
