#ifndef DIALOGDESIGNER_H
#define DIALOGDESIGNER_H

#include <QObject>
#include <QVector>
#include <QToolBar>
#include <QActionGroup>

class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;
class QDesignerIntegrationInterface;
class QDesignerWidgetBoxInterface;
class QDesignerActionEditorInterface;
class QDesignerPropertyEditorInterface;
class QDesignerObjectInspectorInterface;
class QDesignerFormWindowManagerInterface;

namespace SharedTools{
    class WidgetHost;
}

namespace LimeReport{

class DialogDesigner : public QWidget{
    Q_OBJECT
public:
    DialogDesigner(QDesignerFormWindowInterface *wnd, QDesignerFormEditorInterface* formEditor, QWidget *parent = NULL, Qt::WindowFlags flags = Qt::WindowFlags());
    ~DialogDesigner();
    QString dialogName() const;
    void    setDialogName(const QString &dialogName);
    bool    isChanged();
    void    setChanged(bool value);
    QByteArray dialogContent();
public slots:
    void undo();
    void redo();
signals:
    void dialogChanged(QString dialogName);
    void dialogNameChanged(QString oldName, QString newName);
private slots:
    void slotMainContainerNameChanged(QString newName);
    void slotDialogChanged();
private:
    QString m_dialogName;
    SharedTools::WidgetHost* m_designerHolder;
    QDesignerFormEditorInterface* m_formEditor;
};

class DialogDesignerManager : public QObject
{
    Q_OBJECT
public:
    explicit DialogDesignerManager(QObject *parent = 0);
    ~DialogDesignerManager();
    void initToolBar(QToolBar* tb);
    QWidget* createFormEditor(const QString& content);
    QByteArray getDialogDescription(QWidget* form);
    void setActiveEditor(QWidget* widget);
    void setDirty(bool value);
    QWidget* widgetBox() const;
    QWidget* actionEditor() const;
    QWidget* propertyEditor() const;
    QWidget* objectInspector() const;
    QWidget* signalSlotEditor() const;
    QWidget* resourcesEditor() const;
signals:
    void dialogChanged(QString dialogName);
    void dialogNameChanged(QString oldName, QString newName);
private slots:
    void slotObjectDestroyed(QObject* object);
    void slotEditWidgets();
    void slotActiveFormWindowChanged(QDesignerFormWindowInterface *formWindow);
private:
    QString iconPathByName(const QString& name);
private:
    QDesignerFormEditorInterface* m_formEditor;
    QDesignerIntegrationInterface* m_designerIntegration;
    QDesignerWidgetBoxInterface* m_widgetBox;
    QDesignerActionEditorInterface* m_actionEditor;
    QDesignerPropertyEditorInterface* m_propertyEditor;
    QDesignerObjectInspectorInterface* m_objectInspector;
    QWidget* m_signalSlotEditor;
    QWidget* m_resourcesEditor;
    QVector<QWidget*> m_designerToolWindows;
    QAction* m_editWidgetsAction;
    QActionGroup* m_modes;
    QString m_activeWindowName;
    QList<DialogDesigner*> m_dialogDesigners;
};

} // namespace LimeReport

#endif // DIALOGDESIGNER_H
