#ifndef LRDESIGNERPLUGIN_H
#define LRDESIGNERPLUGIN_H

#include <QMainWindow>
#include <lrdesignerplugininterface.h>

class DesignerFactoryPlugin : public QObject, public LimeReportPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ru.limereport.DersignerFactoryInterface")
    Q_INTERFACES( LimeReportPluginInterface )

public:
    ~DesignerFactoryPlugin();

    QString getString() const;
    QVariant getVar() const;
    QMainWindow* getDesignerWindow(LimeReport::ReportEnginePrivateInterface* report, QWidget* parent, QSettings* settings);
};

#endif
