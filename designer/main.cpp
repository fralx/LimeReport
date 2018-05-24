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
    QTranslator qtTranslator;
    QString translationPath = QApplication::applicationDirPath();
    translationPath.append("/languages");
    Qt::LayoutDirection layoutDirection = QLocale::system().textDirection();

    QString designerTranslation = QLocale(manager.getCurrentDefaultLanguage()).name();

    if (limeReportTranslator.load("limereport_"+designerTranslation, translationPath)){
        qtTranslator.load("qt_" + designerTranslation, translationPath);
        a.installTranslator(&qtTranslator);
        a.installTranslator(&limeReportTranslator);
        Qt::LayoutDirection layoutDirection = QLocale(manager.getCurrentDefaultLanguage()).textDirection();
        a.setLayoutDirection(layoutDirection);
    }

    LimeReport::ReportEngine report;
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

