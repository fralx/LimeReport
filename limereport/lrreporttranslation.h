#ifndef REPORTTRANSLATION_H
#define REPORTTRANSLATION_H

#include <QString>
#include <QList>
#include <QLocale>
#include <QMetaType>

#include "lrpagedesignintf.h"


class ATranslationProperty{
public:
    ATranslationProperty(){}
    ATranslationProperty(const ACollectionProperty& ){}
    virtual ~ATranslationProperty(){}
};

Q_DECLARE_METATYPE(ATranslationProperty)
const int TRANSLATION_TYPE_ID = qMetaTypeId<ATranslationProperty>();

namespace LimeReport{

struct PropertyTranslation{
    QString propertyName;
    QString value;
};

struct ItemTranslation{
    QString itemName;
    QList<PropertyTranslation> propertyesTranslation;
};

struct PageTranslation{
    QString pageName;
    QList<ItemTranslation> itemsTranslation;
};

class ReportTranslation{
public:
    ReportTranslation(QLocale::Language language) :m_language(language){}
    ReportTranslation(QLocale::Language language, QList<PageDesignIntf*> pages);
    ReportTranslation(const ReportTranslation& reportTranslation);
    ~ReportTranslation();
    QLocale::Language language() const;
    QList<PageTranslation*> pagesTranslation() const;
    PageTranslation* createEmptyPageTranslation();
private:
    PageTranslation* createPageTranslation(PageDesignIntf* page);
private:
    QLocale::Language m_language;
    QList<PageTranslation*> m_pagesTranslation;
};


typedef QMap<QLocale::Language, ReportTranslation*> Translations;

class ITranslationContainer{
public:
    virtual Translations* translations() = 0;
};

} // namespace LimeReport

//Q_DECLARE_METATYPE(ReportTranslation)

#endif // REPORTTRANSLATION_H
