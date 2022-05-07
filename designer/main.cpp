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
    QTranslator qtBaseTranslator;
    QTranslator qtDesignerTranslator;
    QTranslator qtLinguistTranslator;

    QString translationPath = QApplication::applicationDirPath();
    translationPath.append("/translations");
    Qt::LayoutDirection layoutDirection = QLocale::system().textDirection();

    QString designerTranslation = QLocale(manager.getCurrentDefaultLanguage()).name();

    if (limeReportTranslator.load("limereport_"+designerTranslation, translationPath)){
        qtBaseTranslator.load("qtbase_" + designerTranslation, translationPath);
        qtDesignerTranslator.load("designer_"+designerTranslation,translationPath);

        a.installTranslator(&qtBaseTranslator);
        a.installTranslator(&qtDesignerTranslator);
        a.installTranslator(&limeReportTranslator);

        Qt::LayoutDirection layoutDirection = QLocale(manager.getCurrentDefaultLanguage()).textDirection();
        a.setLayoutDirection(layoutDirection);
    }

    LimeReport::ReportEngine report;
    report.setPreviewLayoutDirection(layoutDirection);

    if (a.arguments().count()>1){
        report.loadFromFile(a.arguments().at(1));
    }
    QObject::connect(&report, SIGNAL(getAvailableDesignerLanguages(QList<QLocale::Language>*)),
                     &manager, SLOT(getAvailableLanguages(QList<QLocale::Language>*)));

    QObject::connect(&report, SIGNAL(getCurrentDefaultDesignerLanguage()),
                     &manager, SLOT(getCurrentDefaultLanguage()));

    QObject::connect(&report, SIGNAL(currentDefaultDesignerLanguageChanged(QLocale::Language)),
                     &manager, SLOT(currentDefaultLanguageChanged(QLocale::Language)));

    report.setShowDesignerModal(false);
    report.designReport();
    return a.exec();
}

