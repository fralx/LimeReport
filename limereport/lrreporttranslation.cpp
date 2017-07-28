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
    pageTranslation->pageName = page->objectName();
    foreach(BaseDesignIntf* item, page->pageItem()->allChildBaseItems()){
        QMap<QString,QString> stringsForTranslation = item->getStringForTranslation();
        if (!stringsForTranslation.isEmpty()){
            ItemTranslation itemTranslation;
            itemTranslation.itemName = item->objectName();
            foreach(QString propertyName, stringsForTranslation.keys()){
                PropertyTranslation propertyTranslation;
                propertyTranslation.propertyName = propertyName;
                propertyTranslation.value = stringsForTranslation.value(propertyName);
                itemTranslation.propertyesTranslation.append(propertyTranslation);
            }
            pageTranslation->itemsTranslation.append(itemTranslation);
        }
    }
    return pageTranslation;
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

} //namespace LimeReport
