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

PageTranslation* ReportTranslation::findPageTranslation(const QString& page_name)
{
    foreach(PageTranslation* page, m_pagesTranslation){
        if (page->pageName.compare(page_name) == 0){
            return page;
        }
    }
    return 0;
}

void ReportTranslation::updatePageTranslation(PageDesignIntf* page)
{
    PageTranslation* pageTranslation = findPageTranslation(page->pageItem()->objectName());
    if (!pageTranslation){
       pageTranslation = createPageTranslation(page);
       m_pagesTranslation.append(pageTranslation);
    }
    if (pageTranslation){
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
                } else {
                   createItemTranslation(item, pageTranslation);
                }
            }
        }
    }
}

QList<PageTranslation*> ReportTranslation::pagesTranslation() const
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

} //namespace LimeReport
