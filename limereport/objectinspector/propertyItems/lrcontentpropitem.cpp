#include "lrcontentpropitem.h"
#include "lrtextitem.h"
#include "editors/lrbuttonlineeditor.h"
#include "items/lrtextitemeditor.h"
#include <QApplication>

namespace{
    LimeReport::ObjectPropItem * createContentPropItem(
        QObject *object, LimeReport::ObjectPropItem::ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& data, LimeReport::ObjectPropItem* parent, bool readonly)
    {
        return new LimeReport::ContentPropItem(object, objects, name, displayName, data, parent, readonly);
    }
    bool VARIABLE_IS_NOT_USED registredContentProp = LimeReport::ObjectPropFactory::instance().registerCreator(LimeReport::APropIdent("content","LimeReport::TextItem"),QObject::tr("content"),createContentPropItem);
} // namespace

namespace LimeReport {

QWidget *ContentPropItem::createProperyEditor(QWidget *parent) const
{
    return new ContentEditor(object(), object()->objectName()+"."+displayName(), parent);
}

void ContentEditor::editButtonClicked()
{
    QDialog* dialog = new QDialog(QApplication::activeWindow());
    dialog->setLayout(new QVBoxLayout());
    dialog->layout()->setContentsMargins(1,1,1,1);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(propertyName());
    QWidget* editor = dynamic_cast<BaseDesignIntf*>(m_object)->defaultEditor();
    dialog->layout()->addWidget(editor);
    dialog->resize(editor->size());
    connect(editor,SIGNAL(destroyed()),dialog,SLOT(close()));
    connect(editor,SIGNAL(destroyed()),this,SIGNAL(editingFinished()));
    dialog->exec();
}

} //namespace LimeReport
