#include <QApplication>
#include <LimeReport>
#include <QTranslator>
#include <QDebug>
#include "designersettingmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DesignerSettingManager manager;

    QTranslator limeReportTranslator;
    QString translationPath = QApplication::applicationDirPath();
    translationPath.append("/languages");

    QString designerTranslation = QLocale(manager.getCurrentDefaultLanguage()).name();

    limeReportTranslator.load("limereport_"+designerTranslation, translationPath);
    a.installTranslator(&limeReportTranslator);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + designerTranslation, translationPath);
    a.installTranslator(&qtTranslator);

    Qt::LayoutDirection layoutDirection = QLocale(manager.getCurrentDefaultLanguage()).textDirection();

    LimeReport::ReportEngine report;
    a.setLayoutDirection(layoutDirection);
    report.setPreviewLayoutDirection(layoutDirection);

    if (a.arguments().count()>1){
        report.loadFromFile(a.arguments().at(1));
    }
    QObject::connect(&report, SIGNAL(getAviableLanguages(QList<QLocale::Language>*)),
                     &manager, SLOT(getAviableLanguages(QList<QLocale::Language>*)));

    QObject::connect(&report, SIGNAL(getCurrentDefaultLanguage()),
                     &manager, SLOT(getCurrentDefaultLanguage()));

    QObject::connect(&report, SIGNAL(currentDefaulLanguageChanged(QLocale::Language)),
                     &manager, SLOT(currentDefaulLanguageChanged(QLocale::Language)));

    report.designReport();
    return a.exec();
}

