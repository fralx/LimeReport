#include <QApplication>
#include <LimeReport>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator limeReportTranslator;
    QString translationPath = QApplication::applicationDirPath();
    translationPath.append("/languages");
    limeReportTranslator.load("limereport_"+QLocale::system().name(),translationPath);
    a.installTranslator(&limeReportTranslator);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),translationPath);
    a.installTranslator(&qtTranslator);

    LimeReport::ReportEngine report;
    if (a.arguments().count()>1){
        report.loadFromFile(a.arguments().at(1));
    }
    report.designReport();
    return a.exec();
}
