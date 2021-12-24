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
    QString sourceValue;
    bool    checked;
    bool    sourceHasBeenChanged;
};

struct ItemTranslation{
    QString itemName;
    bool checked;
    PropertyTranslation* findProperty(const QString& propertyName);
    ~ItemTranslation();
    QList<PropertyTranslation*> propertyesTranslation;
};

struct PageTranslation{
    QString pageName;
    bool checked;
    ~PageTranslation();
    void renameItem(const QString& oldName, const QString& newName);
    QHash<QString, ItemTranslation*> itemsTranslation;
};

class ReportTranslation{
public:
    ReportTranslation(QLocale::Language language) :m_language(language){}
    ReportTranslation(QLocale::Language language, QList<PageDesignIntf*> pages);
    ReportTranslation(const ReportTranslation& reportTranslation);
    ~ReportTranslation();
    QLocale::Language language() const;
    QList<PageTranslation *> &pagesTranslation();
    PageTranslation* createEmptyPageTranslation();
    void updatePageTranslation(PageDesignIntf* page);
    PageTranslation* findPageTranslation(const QString& pageName);
    void renamePage(const QString& oldName, const QString& newName);
    void invalidatePages();
    void clearInvalidPages();
private:
    void createItemTranslation(BaseDesignIntf* item, PageTranslation* pageTranslation);
    PageTranslation* createPageTranslation(PageDesignIntf* page);
private:
    QLocale::Language m_language;
    QLocale::Script m_script;
    QList<PageTranslation*> m_pagesTranslation;
};


typedef QMap<QLocale::Language, ReportTranslation*> Translations;

class ITranslationContainer{
public:
    virtual ~ITranslationContainer(){}
    virtual Translations* translations() = 0;
    virtual void updateTranslations() = 0;
    virtual bool addTranslationLanguage(QLocale::Language language) = 0;
    virtual bool removeTranslationLanguage(QLocale::Language language) = 0;
    virtual QList<QLocale::Language> aviableLanguages() = 0;
};

} // namespace LimeReport

//Q_DECLARE_METATYPE(ReportTranslation)

#endif // REPORTTRANSLATION_H
