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
#include <QAction>
#include <QDebug>
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

    m_editWidgetsAction = new QAction(tr("Edit Widgets"));
    m_editWidgetsAction->setIcon(QIcon(":/images/images/widgettool.png"));
    m_editWidgetsAction->setEnabled(false);
    connect(m_editWidgetsAction, SIGNAL(triggered()), this, SLOT(slotEditWidgets()));
    connect(m_formEditor->formWindowManager(), SIGNAL(activeFormWindowChanged(QDesignerFormWindowInterface*)),
            this, SLOT(slotActiveFormWindowChanged(QDesignerFormWindowInterface*)) );

    m_modes = new QActionGroup(this);
    m_modes->setExclusive(true);
    m_modes->addAction(m_editWidgetsAction);

    foreach ( QObject* o, QPluginLoader::staticInstances() << m_formEditor->pluginManager()->instances() )
    {
        if (  QDesignerFormEditorPluginInterface* fep = qobject_cast<QDesignerFormEditorPluginInterface*>( o ) )
        {
            if ( !fep->isInitialized() )
                fep->initialize( m_formEditor );
            fep->action()->setCheckable( true );
            fep->action()->setIcon(QIcon(iconPathByName(fep->action()->objectName())));
            m_modes->addAction(fep->action());
        }
    }

    m_widgetBox = QDesignerComponents::createWidgetBox(m_formEditor, 0);
    m_widgetBox->setWindowTitle(tr("Widget Box"));
    m_widgetBox->setObjectName(QLatin1String("WidgetBox"));
    m_formEditor->setWidgetBox(m_widgetBox);
    m_formEditor->setTopLevel(m_widgetBox);
    m_designerToolWindows.append(m_widgetBox);
    connect(m_widgetBox, SIGNAL(destroyed(QObject*)), this, SLOT(slotObjectDestroyed(QObject*)) );

    m_objectInspector = QDesignerComponents::createObjectInspector(m_formEditor, 0);
    m_objectInspector->setWindowTitle(tr("Object Inspector"));
    m_objectInspector->setObjectName(QLatin1String("ObjectInspector"));
    m_formEditor->setObjectInspector(m_objectInspector);
    m_designerToolWindows.append(m_objectInspector);
    connect(m_objectInspector, SIGNAL(destroyed(QObject*)), this, SLOT(slotObjectDestroyed(QObject*)) );

    m_propertyEditor = QDesignerComponents::createPropertyEditor(m_formEditor, 0);
    m_propertyEditor->setWindowTitle(tr("Property Editor"));
    m_propertyEditor->setObjectName(QLatin1String("PropertyEditor"));
    m_formEditor->setPropertyEditor(m_propertyEditor);
    m_designerToolWindows.append(m_propertyEditor);
    connect(m_propertyEditor, SIGNAL(destroyed(QObject*)), this, SLOT(slotObjectDestroyed(QObject*)) );

    m_signalSlotEditor = QDesignerComponents::createSignalSlotEditor(m_formEditor, 0);
    m_signalSlotEditor->setWindowTitle(tr("Signals && Slots Editor"));
    m_signalSlotEditor->setObjectName(QLatin1String("SignalsAndSlotsEditor"));

    m_designerToolWindows.append(m_signalSlotEditor);
    connect(m_signalSlotEditor, SIGNAL(destroyed(QObject*)), this, SLOT(slotObjectDestroyed(QObject*)) );

    m_resourcesEditor = QDesignerComponents::createResourceEditor(m_formEditor, 0);
    m_resourcesEditor->setWindowTitle(tr("Resource Editor"));
    m_resourcesEditor->setObjectName(QLatin1String("ResourceEditor"));
    m_designerToolWindows.append(m_resourcesEditor);
    connect(m_resourcesEditor, SIGNAL(destroyed(QObject*)), this, SLOT(slotObjectDestroyed(QObject*)) );

    m_actionEditor = QDesignerComponents::createActionEditor(m_formEditor, 0);
    m_actionEditor->setWindowTitle(tr("Action Editor"));
    m_actionEditor->setObjectName("ActionEditor");
    m_formEditor->setActionEditor(m_actionEditor);
    m_designerToolWindows.append(m_actionEditor);
    connect(m_formEditor, SIGNAL(destroyed(QObject*)), this, SLOT(slotObjectDestroyed(QObject*)) );

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

void DialogDesigner::initToolBar(QToolBar *tb)
{
    tb->setIconSize(QSize(16,16));
    tb->addAction(m_formEditor->formWindowManager()->actionCopy());
    tb->addAction(m_formEditor->formWindowManager()->actionPaste());
    tb->addAction(m_formEditor->formWindowManager()->actionCut());
    tb->addAction(m_formEditor->formWindowManager()->actionUndo());
    tb->addAction(m_formEditor->formWindowManager()->actionRedo());

    tb->addActions(m_modes->actions());

    tb->addAction(m_formEditor->formWindowManager()->actionHorizontalLayout());
    tb->addAction(m_formEditor->formWindowManager()->actionVerticalLayout());
    tb->addAction(m_formEditor->formWindowManager()->actionSplitHorizontal());
    tb->addAction(m_formEditor->formWindowManager()->actionSplitVertical());
    tb->addAction(m_formEditor->formWindowManager()->actionGridLayout());
    m_formEditor->formWindowManager()->actionFormLayout()->setIcon(QIcon(":/images/images/editform.png"));
    tb->addAction(m_formEditor->formWindowManager()->actionFormLayout());
    tb->addAction(m_formEditor->formWindowManager()->actionBreakLayout());
    tb->addAction(m_formEditor->formWindowManager()->actionAdjustSize());
}

QWidget *DialogDesigner::createFormEditor(const QString &content)
{
    QDesignerFormWindowInterface* wnd = m_formEditor->formWindowManager()->createFormWindow(0, Qt::Window);
    wnd->setContents(content);
    m_formEditor->formWindowManager()->setActiveFormWindow(wnd);
    m_formEditor->objectInspector()->setFormWindow(wnd);
    wnd->editWidgets();

    connect(wnd, SIGNAL(changed()), this, SIGNAL(dialogChanged()));

    SharedTools::WidgetHost *placeholder = new SharedTools::WidgetHost(0,wnd);
    placeholder->setFrameStyle( QFrame::NoFrame | QFrame::Plain );
    placeholder->setFocusProxy( wnd );

    return placeholder;    
}

QByteArray DialogDesigner::getDialogDescription(QWidget *form)
{
    SharedTools::WidgetHost* wh = dynamic_cast<SharedTools::WidgetHost*>(form);
    if (wh){
        return wh->formWindow()->contents().toUtf8();
    }
    return QByteArray();
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

void DialogDesigner::slotObjectDestroyed(QObject *object)
{
    for ( int i = 0; i<m_designerToolWindows.size();++i){
        m_designerToolWindows[i] = m_designerToolWindows[i] == object ? 0 : m_designerToolWindows[i];
    }

}

void DialogDesigner::slotEditWidgets()
{
   for (int i = 0; i<m_formEditor->formWindowManager()->formWindowCount(); ++i){
       m_formEditor->formWindowManager()->formWindow(i)->editWidgets();
   }
}

void DialogDesigner::slotActiveFormWindowChanged(QDesignerFormWindowInterface *formWindow)
{
    if (formWindow){
        m_editWidgetsAction->setEnabled(true);
        m_activeWindowName = formWindow->objectName();
    }
}

QString DialogDesigner::iconPathByName(const QString &name)
{
    if (name.compare("__qt_edit_signals_slots_action") == 0)
        return ":/images/images/signalslottool.png";
    if (name.compare("__qt_edit_buddies_action") == 0)
        return ":/images/images/buddytool.png";
    if (name.compare("_qt_edit_tab_order_action") == 0)
        return ":/images/images/tabordertool.png";
    return "";
}

}
