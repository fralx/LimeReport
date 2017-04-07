#include "lrdialogdesigner.h"

#include <QPluginLoader>

#include <QDesignerComponents>
#include <QDesignerIntegration>
#include <abstractobjectinspector.h>

#include <QDesignerFormEditorInterface>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormWindowManagerInterface>
#include <QDesignerFormEditorPluginInterface>

#include <QDesignerWidgetBoxInterface>
#include <QDesignerActionEditorInterface>
#include <QDesignerPropertyEditorInterface>
#include <QDesignerObjectInspectorInterface>
#include <QDesignerFormEditorInterface>
#include "pluginmanager_p.h"
//#include <QExtensionManager>

#include "widgethost.h"

namespace LimeReport{

DialogDesigner::DialogDesigner(QObject *parent) : QObject(parent)
{
    QDesignerComponents::initializeResources();
    m_formEditor = QDesignerComponents::createFormEditor(this);
    QDesignerComponents::initializePlugins(m_formEditor);
    QDesignerComponents::createTaskMenu(m_formEditor, this);

    foreach ( QObject* o, QPluginLoader::staticInstances() << m_formEditor->pluginManager()->instances() )
    {
        if (  QDesignerFormEditorPluginInterface* fep = qobject_cast<QDesignerFormEditorPluginInterface*>( o ) )
        {
            // initialize plugin if needed
            if ( !fep->isInitialized() )
                fep->initialize( m_formEditor );

            // set action chackable
//            fep->action()->setCheckable( true );

//            // add action mode to group
//            aModes->addAction( fep->action() );
        }
    }


    m_widgetBox = QDesignerComponents::createWidgetBox(m_formEditor, 0);
    m_widgetBox->setWindowTitle(tr("Widget Box"));
    m_widgetBox->setObjectName(QLatin1String("WidgetBox"));
    m_formEditor->setWidgetBox(m_widgetBox);
    m_formEditor->setTopLevel(m_widgetBox);
    m_designerToolWindows.append(m_widgetBox);
    connect(m_widgetBox, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)) );

    m_objectInspector = QDesignerComponents::createObjectInspector(m_formEditor, 0);
    m_objectInspector->setWindowTitle(tr("Object Inspector"));
    m_objectInspector->setObjectName(QLatin1String("ObjectInspector"));
    m_formEditor->setObjectInspector(m_objectInspector);
    m_designerToolWindows.append(m_objectInspector);
    connect(m_objectInspector, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)) );

    m_propertyEditor = QDesignerComponents::createPropertyEditor(m_formEditor, 0);
    m_propertyEditor->setWindowTitle(tr("Property Editor"));
    m_propertyEditor->setObjectName(QLatin1String("PropertyEditor"));
    m_formEditor->setPropertyEditor(m_propertyEditor);
    m_designerToolWindows.append(m_propertyEditor);
    connect(m_propertyEditor, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)) );

    m_signalSlotEditor = QDesignerComponents::createSignalSlotEditor(m_formEditor, 0);
    m_signalSlotEditor->setWindowTitle(tr("Signals && Slots Editor"));
    m_signalSlotEditor->setObjectName(QLatin1String("SignalsAndSlotsEditor"));

    m_designerToolWindows.append(m_signalSlotEditor);
    connect(m_signalSlotEditor, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)) );

    m_resourcesEditor = QDesignerComponents::createResourceEditor(m_formEditor, 0);
    m_resourcesEditor->setWindowTitle(tr("Resource Editor"));
    m_resourcesEditor->setObjectName(QLatin1String("ResourceEditor"));
    m_designerToolWindows.append(m_resourcesEditor);
    connect(m_resourcesEditor, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)) );

    m_actionEditor = QDesignerComponents::createActionEditor(m_formEditor, 0);
    m_actionEditor->setWindowTitle(tr("Action Editor"));
    m_actionEditor->setObjectName("ActionEditor");
    m_formEditor->setActionEditor(m_actionEditor);
    m_designerToolWindows.append(m_actionEditor);
    connect(m_formEditor, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)) );

    m_designerIntegration = new QDesignerIntegration(m_formEditor,this);
    m_formEditor->setIntegration(m_designerIntegration);

}

DialogDesigner::~DialogDesigner()
{
    for (int i = 0; i<m_designerToolWindows.size();++i){
        if (m_designerToolWindows[i])
            delete m_designerToolWindows[i];
    }

    delete m_designerIntegration;
    delete m_formEditor;
}

QWidget *DialogDesigner::createFormEditor(const QString &content)
{
    QDesignerFormWindowInterface* wnd = m_formEditor->formWindowManager()->createFormWindow(0, Qt::Window);
    wnd->setContents(content);
    m_formEditor->formWindowManager()->setActiveFormWindow(wnd);
    m_formEditor->objectInspector()->setFormWindow(wnd);
    wnd->editWidgets();

    SharedTools::WidgetHost *placeholder = new SharedTools::WidgetHost(0,wnd);
    placeholder->setFrameStyle( QFrame::NoFrame | QFrame::Plain );
    placeholder->setFocusProxy( wnd );

    return placeholder;

}

void DialogDesigner::setActiveEditor(QWidget *widget)
{
    SharedTools::WidgetHost* wh = dynamic_cast<SharedTools::WidgetHost*>(widget);
    if (wh){
        m_formEditor->formWindowManager()->setActiveFormWindow(wh->formWindow());
    }

}

QWidget* DialogDesigner::widgetBox() const
{
    return m_widgetBox;
}

QWidget* DialogDesigner::actionEditor() const
{
    return m_actionEditor;
}

QWidget* DialogDesigner::propertyEditor() const
{
    return m_propertyEditor;
}

QWidget* DialogDesigner::objectInspector() const
{
    return m_objectInspector;
}

QWidget *DialogDesigner::signalSlotEditor() const
{
    return m_signalSlotEditor;
}

QWidget *DialogDesigner::resourcesEditor() const
{
    return m_resourcesEditor;
}

void DialogDesigner::objectDestroyed(QObject *object)
{
    for ( int i = 0; i<m_designerToolWindows.size();++i){
        m_designerToolWindows[i] = m_designerToolWindows[i] == object ? 0 : m_designerToolWindows[i];
    }

}

}
