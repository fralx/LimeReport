#include "lrdialogdesigner.h"

#include <QPluginLoader>

#include <QDesignerComponents>
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
#include <QVBoxLayout>

#if HAVE_QT5
#include <QDesignerIntegration>
#endif
#if HAVE_QT4
#include "qdesigner_integration_p.h"
#include <QtDesigner/QDesignerIntegrationInterface>
#endif
#include "pluginmanager_p.h"
#include "widgethost.h"
#include <abstractobjectinspector.h>

namespace LimeReport{

DialogDesignerManager::DialogDesignerManager(QObject *parent) : QObject(parent)
{
    QDesignerComponents::initializeResources();
    m_formEditor = QDesignerComponents::createFormEditor(this);
    QDesignerComponents::initializePlugins(m_formEditor);
    QDesignerComponents::createTaskMenu(m_formEditor, this);

    m_editWidgetsAction = new QAction(tr("Edit Widgets"), this);
    m_editWidgetsAction->setIcon(QIcon(":/images/images/widgettool.png"));
    m_editWidgetsAction->setEnabled(false);
    m_editWidgetsAction->setCheckable(true);
    m_editWidgetsAction->setChecked(true);
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
    connect(m_actionEditor, SIGNAL(destroyed(QObject*)), this, SLOT(slotObjectDestroyed(QObject*)) );

#if QT_VERSION < 0x050000
    m_designerIntegration = new qdesigner_internal::QDesignerIntegration(m_formEditor,this);
#else
    m_designerIntegration = new QDesignerIntegration(m_formEditor,this);
#endif
    m_formEditor->setIntegration(m_designerIntegration);

}

DialogDesignerManager::~DialogDesignerManager()
{
    for (int i = 0; i<m_designerToolWindows.size();++i){
        if (m_designerToolWindows[i])
            delete m_designerToolWindows[i];
    }

    delete m_designerIntegration;
    delete m_formEditor;
}

void DialogDesignerManager::initToolBar(QToolBar *tb)
{
    tb->setIconSize(QSize(16,16));
    m_formEditor->formWindowManager()->actionCopy()->setIcon(QIcon(":/report/images/copy"));
    tb->addAction(m_formEditor->formWindowManager()->actionCopy());
    m_formEditor->formWindowManager()->actionPaste()->setIcon(QIcon(":/report/images/paste"));
    tb->addAction(m_formEditor->formWindowManager()->actionPaste());
    m_formEditor->formWindowManager()->actionCut()->setIcon(QIcon(":/report/images/cut"));
    tb->addAction(m_formEditor->formWindowManager()->actionCut());
    m_formEditor->formWindowManager()->actionUndo()->setIcon(QIcon(":/report/images/undo"));
    tb->addAction(m_formEditor->formWindowManager()->actionUndo());
    m_formEditor->formWindowManager()->actionRedo()->setIcon(QIcon(":/report/images/redo"));
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

QWidget *DialogDesignerManager::createFormEditor(const QString &content)
{
    QDesignerFormWindowInterface* wnd = m_formEditor->formWindowManager()->createFormWindow(0, Qt::Window);
    wnd->setContents(content);
    m_formEditor->formWindowManager()->setActiveFormWindow(wnd);
    m_formEditor->objectInspector()->setFormWindow(wnd);
    wnd->editWidgets();

    DialogDesigner* dialogDesigner = new DialogDesigner(wnd, m_formEditor);

    connect(dialogDesigner, SIGNAL(dialogChanged(QString)), this, SIGNAL(dialogChanged(QString)));
    connect(dialogDesigner, SIGNAL(dialogNameChanged(QString,QString)), this, SIGNAL(dialogNameChanged(QString,QString)));
    connect(dialogDesigner, SIGNAL(destroyed(QObject*)), this, SLOT(slotObjectDestroyed(QObject*)));

    m_dialogDesigners.append(dialogDesigner);

    return dialogDesigner;

}

QByteArray DialogDesignerManager::getDialogDescription(QWidget *form)
{
    QByteArray result;
    DialogDesigner* dialogDesigner = dynamic_cast<DialogDesigner*>(form);
    Q_ASSERT(dialogDesigner != NULL);
    //SharedTools::WidgetHost* wh = dynamic_cast<SharedTools::WidgetHost*>(form);
    if (dialogDesigner){
        result = dialogDesigner->dialogContent();
        //wh->formWindow()->setDirty(false);
    }
    return result;
}

void DialogDesignerManager::setActiveEditor(QWidget *widget)
{
    SharedTools::WidgetHost* wh = dynamic_cast<SharedTools::WidgetHost*>(widget);
    if (wh){
        m_formEditor->formWindowManager()->setActiveFormWindow(wh->formWindow());
    }
}

void DialogDesignerManager::setDirty(bool value)
{
    foreach(DialogDesigner* dialogDesigner, m_dialogDesigners){
        dialogDesigner->setChanged(value);
    }
}

QWidget* DialogDesignerManager::widgetBox() const
{
    return m_widgetBox;
}

QWidget* DialogDesignerManager::actionEditor() const
{
    return m_actionEditor;
}

QWidget* DialogDesignerManager::propertyEditor() const
{
    return m_propertyEditor;
}

QWidget* DialogDesignerManager::objectInspector() const
{
    return m_objectInspector;
}

QWidget *DialogDesignerManager::signalSlotEditor() const
{
    return m_signalSlotEditor;
}

QWidget *DialogDesignerManager::resourcesEditor() const
{
    return m_resourcesEditor;
}

void DialogDesignerManager::slotObjectDestroyed(QObject* object)
{

    QList<DialogDesigner*>::Iterator it = m_dialogDesigners.begin();
    while(it!=m_dialogDesigners.end()){
        if (*it == object){
            it = m_dialogDesigners.erase(it);
            return;
        } else {
            ++it;
        }
    }

    for ( int i = 0; i<m_designerToolWindows.size();++i){
        m_designerToolWindows[i] = m_designerToolWindows[i] == object ? 0 : m_designerToolWindows[i];
    }

}

void DialogDesignerManager::slotEditWidgets()
{
   for (int i = 0; i<m_formEditor->formWindowManager()->formWindowCount(); ++i){
       m_formEditor->formWindowManager()->formWindow(i)->editWidgets();
   }
}

void DialogDesignerManager::slotActiveFormWindowChanged(QDesignerFormWindowInterface *formWindow)
{
    if (formWindow){
        m_editWidgetsAction->setEnabled(true);
        m_editWidgetsAction->trigger();
        m_editWidgetsAction->setChecked(true);
        m_activeWindowName = formWindow->objectName();
    }
}

QString DialogDesignerManager::iconPathByName(const QString &name)
{
    if (name.compare("__qt_edit_signals_slots_action") == 0)
        return ":/images/images/signalslottool.png";
    if (name.compare("__qt_edit_buddies_action") == 0)
        return ":/images/images/buddytool.png";
    if (name.compare("_qt_edit_tab_order_action") == 0)
        return ":/images/images/tabordertool.png";
    return "";
}

DialogDesigner::DialogDesigner(QDesignerFormWindowInterface* wnd, QDesignerFormEditorInterface* formEditor, QWidget *parent, Qt::WindowFlags flags)
    :QWidget(parent, flags), m_formEditor(formEditor)
{
    m_dialogName = wnd->mainContainer()->objectName();
    connect(wnd, SIGNAL(changed()), this, SLOT(slotDialogChanged()));

    m_designerHolder = new SharedTools::WidgetHost(this,wnd);
    m_designerHolder->setFrameStyle( QFrame::NoFrame | QFrame::Plain );
    m_designerHolder->setFocusProxy( wnd );

    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(m_designerHolder);
    setLayout(l);

}

DialogDesigner::~DialogDesigner(){}

QString DialogDesigner::dialogName() const
{
    return m_dialogName;
}

void DialogDesigner::setDialogName(const QString &dialogName)
{
    m_dialogName = dialogName;
}

bool DialogDesigner::isChanged()
{
    return m_designerHolder->formWindow()->isDirty();
}

void DialogDesigner::setChanged(bool value)
{
    m_designerHolder->formWindow()->setDirty(value);
}

QByteArray DialogDesigner::dialogContent()
{
    if (m_designerHolder && m_designerHolder->formWindow())
        return m_designerHolder->formWindow()->contents().toUtf8();
    return QByteArray();
}

void DialogDesigner::undo()
{
    Q_ASSERT(m_formEditor != NULL);
    if (m_formEditor){
        m_formEditor->formWindowManager()->actionUndo()->trigger();
    }
}

void DialogDesigner::redo()
{
    Q_ASSERT(m_formEditor != NULL);
    if (m_formEditor){
        m_formEditor->formWindowManager()->actionRedo()->trigger();
    }
}

void DialogDesigner::slotMainContainerNameChanged(QString newName)
{
    if (m_dialogName.compare(newName) != 0){
        emit dialogNameChanged(m_dialogName, newName);
        m_dialogName = newName;
    }
}

void DialogDesigner::slotDialogChanged()
{
    Q_ASSERT(m_designerHolder != NULL);
    if (m_designerHolder && m_designerHolder->formWindow()){
        if ( m_designerHolder->formWindow()->mainContainer()->objectName().compare(m_dialogName) !=0 ){
            emit dialogNameChanged(m_dialogName, m_designerHolder->formWindow()->mainContainer()->objectName());
            m_dialogName = m_designerHolder->formWindow()->mainContainer()->objectName();
        }
        emit dialogChanged(m_dialogName);
        m_designerHolder->formWindow()->setDirty(false);
    }
}

}
