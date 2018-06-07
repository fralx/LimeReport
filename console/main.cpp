#include <QApplication>
#include <QUuid>
#include <LimeReport>
#include <iostream>
#include <QDebug>
#include <QDir>
#include <QFile>

#ifdef _WIN32
  #include <io.h>
  #include <fcntl.h>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList vars;
    LimeReport::ReportEngine report;
    if (a.arguments().count() < 2 ){
        std::cerr<<"Error! Report file is not specified !!";
        return 1;
    }

    if (a.arguments().count()>2){
        vars = a.arguments().at(2).split(";");
        qDebug()<<vars;
        foreach(QString var, vars){
            QStringList varItem = var.split("=");
            report.dataManager()->setReportVariable(varItem.at(0),varItem.at(1));
        }
    }

    QString reportFile = a.arguments().at(1);

    if (!report.loadFromFile(reportFile)){
        qDebug()<<QDir::currentPath();
        QFile::exists(reportFile);
        std::cerr<<"Error! Report file \""+reportFile.toStdString()+"\" not found";
        return 1;
    }

    QUuid uid = QUuid::createUuid();
    QString uidStr = uid.toString()+".pdf";
    report.printToPDF(uidStr);
    QFile in(uidStr);
    QFile out;
    out.open(stdout, QFile::WriteOnly);
    in.open(QIODevice::ReadOnly);
#ifdef _WIN32
    _setmode(fileno(stdout),O_BINARY);
    //qDebug()<<result;
#endif
    QByteArray buffer = in.readAll();
    fwrite(buffer,1,buffer.size(),stdout);
//    out.write(in.readAll());
    in.close();
    in.remove();
//    std::cout << uidStr.toStdString();
//    std::cout << in.readAll().data();
//    in.close();
//    in.remove();

    return 0;
    //return a.exec();
}
