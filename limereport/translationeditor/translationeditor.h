#ifndef TRANSLATIONEDITOR_H
#define TRANSLATIONEDITOR_H

#include <QWidget>
#include <QLocale>
#include <QTreeWidgetItem>
#include <QShortcut>
#include "lrreporttranslation.h"

namespace LimeReport {

namespace Ui {
class TranslationEditor;
}


class TranslationEditor : public QWidget
{
    Q_OBJECT

public:
    explicit TranslationEditor(QWidget *parent = 0);
    void setReportEngine(ITranslationContainer* translationContainer);
    ~TranslationEditor();
    void updateUi();
    void activateLanguage(QLocale::Language language);
    void activatePage(PageTranslation* pageTranslation);
    void activateTranslation(const QString& itemName, const QString& propertyName);
private slots:
    void on_tbStrings_itemSelectionChanged();
    void on_teTranslation_textChanged();
    void on_cbChecked_toggled(bool checked);
    void on_twPages_itemSelectionChanged();   
    void on_tbAddLanguage_clicked();
    void on_tbDeleteLanguage_clicked();
    void slotItemChecked();
    void on_lvLanguages_itemSelectionChanged();

private:
    QLocale::Language getLanguageByName(const QString& languageName);
private:
    Ui::TranslationEditor *ui;
    ITranslationContainer* m_translationContainer;
    QMap<QString, ReportTranslation*> m_reportTranslations;
    QMap<QString, PageTranslation> m_pageTranslations;
    ReportTranslation* m_currentReportTranslation;
    PageTranslation* m_currentPageTranslation;
    PropertyTranslation* m_currentPropertyTranslation;
    bool m_translationChanging;
    QShortcut* m_clrReturn;
};

} //namespace LimeReport

#endif // TRANSLATIONEDITOR_H
