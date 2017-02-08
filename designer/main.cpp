#include <QApplication>
#include <LimeReport>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LimeReport::ReportEngine report;
    if (a.arguments().count()>1){
        report.loadFromFile(a.arguments().at(1));
    }
    report.designReport();
    return a.exec();
}
