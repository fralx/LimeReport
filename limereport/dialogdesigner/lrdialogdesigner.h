#ifndef DIALOGDESIGNER_H
#define DIALOGDESIGNER_H

#include <QObject>
#include <QVector>

class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;
class QDesignerIntegrationInterface;
class QDesignerWidgetBoxInterface;
class QDesignerActionEditorInterface;
class QDesignerPropertyEditorInterface;
class QDesignerObjectInspectorInterface;
class QDesignerFormWindowManagerInterface;

namespace LimeReport{

class DialogDesigner : public QObject
{
    Q_OBJECT
public:
    explicit DialogDesigner(QObject *parent = 0);
    ~DialogDesigner();
    QWidget* createFormEditor(const QString& content);
    void     setActiveEditor(QWidget* widget);
    QWidget* widgetBox() const;
    QWidget* actionEditor() const;
    QWidget* propertyEditor() const;
    QWidget* objectInspector() const;
    QWidget* signalSlotEditor() const;
    QWidget* resourcesEditor() const;
private slots:
    void objectDestroyed(QObject* object);
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
};

} // namespace LimeReport

#endif // DIALOGDESIGNER_H
