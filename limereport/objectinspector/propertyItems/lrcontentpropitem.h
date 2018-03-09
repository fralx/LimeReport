#ifndef LRCONTENTPROPITEM_H
#define LRCONTENTPROPITEM_H

#include "lrstringpropitem.h"
#include "objectinspector/editors/lrbuttonlineeditor.h"

namespace LimeReport {

class  ContentEditor : public ButtonLineEditor{
    Q_OBJECT
public:
    explicit ContentEditor(QObject* object, const QString& propertyName,QWidget *parent = 0)
        :ButtonLineEditor(propertyName,parent), m_object(object){}
public slots:
    void editButtonClicked();
private:
    QObject* m_object;
};

class ContentPropItem : public StringPropItem{
    Q_OBJECT
public:
    ContentPropItem():StringPropItem(){}
    ContentPropItem(QObject* object, ObjectsList* objects, const QString& name, const QString& displayName, const QVariant& value, ObjectPropItem* parent, bool readonly)
        :StringPropItem(object, objects, name, displayName, value, parent, readonly){}
    QWidget* createProperyEditor(QWidget *parent) const;
};

} // namespace LimeReport

#endif // CONTENTPROPITEM_H
