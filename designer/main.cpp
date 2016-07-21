#include <QApplication>
#include <LimeReport>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LimeReport::ReportEngine report;
    report.designReport();
    return a.exec();
}
