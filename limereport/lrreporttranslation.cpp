#include "lrreporttranslation.h"

#include "lrbasedesignintf.h"
#include "lrpagedesignintf.h"

namespace LimeReport{

ReportTranslation::ReportTranslation(QLocale::Language language, QList<PageDesignIntf*> pages)
 : m_language(language)
{
    foreach (PageDesignIntf* page, pages){
        m_pagesTranslation.append(createPageTranslation(page));
    }
}

ReportTranslation::ReportTranslation(const ReportTranslation& reportTranslation)
    :m_language(reportTranslation.m_language)
{
    foreach(PageTranslation* pageTranslation, reportTranslation.m_pagesTranslation){
        m_pagesTranslation.append(pageTranslation);
    }
}

ReportTranslation::~ReportTranslation()
{
    foreach(PageTranslation* page, m_pagesTranslation){
        delete page;
    }
    m_pagesTranslation.clear();
}

PageTranslation* ReportTranslation::createPageTranslation(PageDesignIntf* page)
{
    PageTranslation* pageTranslation = new PageTranslation;
    pageTranslation->checked = true;
    pageTranslation->pageName = page->pageItem()->objectName();
    foreach(BaseDesignIntf* item, page->pageItem()->allChildBaseItems()){
        createItemTranslation(item, pageTranslation);
    }
    return pageTranslation;
}

void ReportTranslation::createItemTranslation(BaseDesignIntf* item, PageTranslation* pageTranslation){
    QMap<QString,QString> stringsForTranslation = item->getStringForTranslation();
    if (!stringsForTranslation.isEmpty()){
        ItemTranslation* itemTranslation = new ItemTranslation;
        itemTranslation->checked = true;
        itemTranslation->itemName = item->objectName();
        foreach(QString propertyName, stringsForTranslation.keys()){
            PropertyTranslation* propertyTranslation = new PropertyTranslation;
            propertyTranslation->propertyName = propertyName;
            propertyTranslation->value = stringsForTranslation.value(propertyName);
            propertyTranslation->sourceValue = stringsForTranslation.value(propertyName);
            propertyTranslation->checked = false;
            propertyTranslation->sourceHasBeenChanged = false;
            itemTranslation->propertyesTranslation.append(propertyTranslation);
        }
        pageTranslation->itemsTranslation.insert(itemTranslation->itemName, itemTranslation);
    }
}

PageTranslation* ReportTranslation::findPageTranslation(const QString& pageName)
{
    foreach(PageTranslation* page, m_pagesTranslation){
        if (page->pageName.compare(pageName) == 0){
            return page;
        }
    }
    return 0;
}

void ReportTranslation::renamePage(const QString &oldName, const QString &newName)
{
    PageTranslation* page = findPageTranslation(oldName);
    if (page){
        page->pageName = newName;
    }
}

void ReportTranslation::invalidatePages()
{
    foreach(PageTranslation* page, m_pagesTranslation){
        page->checked = false;
    }
}

void ReportTranslation::clearInvalidPages()
{
    QList<PageTranslation*>::Iterator it = m_pagesTranslation.begin();
    while (it != m_pagesTranslation.end()){
        if (!(*it)->checked){
            delete *it;
            it = m_pagesTranslation.erase(it);
        } else ++it;
    }
}

void ReportTranslation::updatePageTranslation(PageDesignIntf* page)
{
    PageTranslation* pageTranslation = findPageTranslation(page->pageItem()->objectName());
    if (!pageTranslation){
       pageTranslation = createPageTranslation(page);
       m_pagesTranslation.append(pageTranslation);
    }
    if (pageTranslation){
        pageTranslation->checked = true;
        foreach(ItemTranslation* item, pageTranslation->itemsTranslation){
            item->checked = false;
        }
        foreach(BaseDesignIntf* item, page->pageItem()->allChildBaseItems()){
            QMap<QString,QString> stringsForTranslation = item->getStringForTranslation();
            if (!stringsForTranslation.isEmpty()){
                ItemTranslation* itemTranslation = pageTranslation->itemsTranslation.value(item->objectName());
                if (itemTranslation){
                    foreach(QString propertyName, stringsForTranslation.keys()){
                        PropertyTranslation* propertyTranslation = itemTranslation->findProperty(propertyName);
                        bool translated = propertyTranslation->sourceValue != propertyTranslation->value;
                        if (propertyTranslation->checked)
                            propertyTranslation->sourceHasBeenChanged = propertyTranslation->sourceValue != stringsForTranslation.value(propertyName);
                        if (propertyTranslation->sourceHasBeenChanged)
                            propertyTranslation->checked = false;
                        propertyTranslation->sourceValue = stringsForTranslation.value(propertyName);
                        if (!translated) propertyTranslation->value = propertyTranslation->sourceValue;
                    }
                    itemTranslation->checked = true;
                } else {
                   createItemTranslation(item, pageTranslation);
                }
            }
        }
        QHash<QString, ItemTranslation*>::Iterator it = pageTranslation->itemsTranslation.begin();
        while( it != pageTranslation->itemsTranslation.end()){
            if (!it.value()->checked) {
                delete it.value();
                it = pageTranslation->itemsTranslation.erase(it);
            } else ++it;
        }
    }
}

QList<PageTranslation*>& ReportTranslation::pagesTranslation()
{
    return m_pagesTranslation;
}

PageTranslation*ReportTranslation::createEmptyPageTranslation()
{
    PageTranslation* pageTranslation = new PageTranslation;
    m_pagesTranslation.append(pageTranslation);
    return pageTranslation;
}

QLocale::Language ReportTranslation::language() const
{
    return m_language;
}

PropertyTranslation* ItemTranslation::findProperty(const QString& propertyName)
{
    foreach(PropertyTranslation* propertyTranslation, propertyesTranslation){
        if (propertyTranslation->propertyName.compare(propertyName) == 0){
            return propertyTranslation;
        }
    }
    return 0;
}

ItemTranslation::~ItemTranslation()
{
    foreach(PropertyTranslation* property, propertyesTranslation){
        delete property;
    }
}

PageTranslation::~PageTranslation()
{
    foreach(ItemTranslation* item, itemsTranslation){
        delete item;
    }
}

void PageTranslation::renameItem(const QString &oldName, const QString &newName)
{
    ItemTranslation* item = itemsTranslation.value(oldName);
    if (item){
        itemsTranslation.remove(oldName);
        item->itemName = newName;
        itemsTranslation[newName] = item;
    }
}

} //namespace LimeReport
