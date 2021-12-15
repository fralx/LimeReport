#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QPrintDialog>
#include <private/qzipreader_p.h>
#include <QDebug>
#include <QtCore/qabstractanimation.h>
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
#include <QDesktopWidget>
#endif
#include "XmlModel.h"
#include "SettingDialog.h"
#include "lrreportengine.h"

//#ifndef QT_DEBUG
//Q_IMPORT_PLUGIN(adateeditorplugin)
//#endif

void centerWindow(QWidget* widget, double widthFactor, double heightFactor) {
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    QDesktopWidget desk;
    int ww = desk.width() * widthFactor;
    int wh = desk.height() * heightFactor;
    widget->move((desk.width() - ww) / 2, (desk.height() - wh) / 2);
    widget->resize(ww, wh);
#else
    QScreen* desk = QGuiApplication::primaryScreen();
    int ww = desk->geometry().width() * widthFactor;
    int wh = desk->geometry().height() * heightFactor;

    widget->resize(ww, wh);
    widget->move((desk->geometry().width() - ww) / 2, (desk->geometry().height() - wh) / 2);
#endif


}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_previewScene(0), m_isCert(false), m_deleteLastFile(false)
{
    ui->setupUi(this);
    m_report.dataManager()->addModel("reportData",&m_model,false);    
    ui->graphicsView->scale(0.5,0.5);
    centerWindow(this, 0.8, 0.8);
    m_settings = new QSettings("OCKK Ltd","LRDView");
    m_report.setSettings(m_settings);
    readSetting();
    setWindowTitle("LRDView ver 1.30");
    if (QCoreApplication::arguments().count()>1){
        loadReport(QCoreApplication::arguments().at(1));
    }
    //ui->statusBar->addWidget(new QLabel(" Designed by Arin Alexander (arin_a@bk.ru) ",this));
}

bool isFileExists(QZipReader& reader, QString fileName){
    foreach( QZipReader::FileInfo fi, reader.fileInfoList()){
        if (fi.filePath.compare(fileName)==0) return true;
    }
    return false;
}
void MainWindow::loadReport(QString fileName){
     if (!fileName.isEmpty()){
        QZipReader reader(fileName);
        QByteArray fileData = reader.fileData("report.xml");
        m_report.loadFromByteArray(&fileData);
        m_reportData = reader.fileData("data.xml");
        m_model.setXMLData(&m_reportData);
        if (m_previewScene) delete m_previewScene;
        m_previewScene = dynamic_cast<LimeReport::PageDesignIntf*>(m_report.createPreviewScene(ui->graphicsView));
        m_previewScene->setItemMode(LimeReport::PreviewMode);
        ui->graphicsView->setScene(m_previewScene);
        ui->graphicsView->centerOn(0, 0);
        ui->actionEdit->setChecked(false);
        m_isCert = isFileExists(reader,"CertFlag.sgn");
        //m_lastReport = fileName;
        ui->action_PDF->setEnabled(true);
        ui->actionPrint->setEnabled(true);
        reader.close();
        if (m_deleteLastFile){
            QFile::remove(fileName);
        }
    }
}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete ui;
}

void MainWindow::on_actionEdit_triggered()
{
    if (m_previewScene){
        if (m_previewScene->itemMode()==LimeReport::PreviewMode){
            m_previewScene->setItemMode(LimeReport::DesignMode);
        } else m_previewScene->setItemMode(LimeReport::PreviewMode);
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(0,tr("Открыть файл отчета"),QString(),"*.lrd");
    if (!fileName.isEmpty()){
        loadReport(fileName);
    }
}

void MainWindow::on_actionDesign_triggered()
{
    m_report.designReport();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSetting();
}

void MainWindow::writeSetting(){
    m_settings->setValue("geometry",saveGeometry());
}

void MainWindow::readSetting()
{
    QVariant v = m_settings->value("geometry");
    if (v.isValid()){
        restoreGeometry(v.toByteArray());
    }
    m_certPrinterName = m_settings->value("certPrinterName","").value<QString>();
    m_otherPrinterName = m_settings->value("otherPrinterName","").value<QString>();
    m_deleteLastFile = m_settings->value("deleteLastFile",false).value<bool>();
}

void MainWindow::settingAccepted()
{
    m_certPrinterName=((SettingDialog*)sender())->certPrinterName();
    m_otherPrinterName=((SettingDialog*)sender())->othenPrinterName();
    m_deleteLastFile = ((SettingDialog*)sender())->deleteLastFile();
    m_settings->setValue("certPrinterName",m_certPrinterName);
    m_settings->setValue("otherPrinterName",m_otherPrinterName);
    m_settings->setValue("deleteLastFile",m_deleteLastFile);
}

void MainWindow::on_actionZoomIn_triggered()
{
   ui->graphicsView->scale(1.2,1.2);
}

void MainWindow::on_actionZoomOut_triggered()
{
    ui->graphicsView->scale(1/1.2,1/1.2);
}

void MainWindow::on_actionPrint_triggered()
{
    QPrinterInfo pi;
    if (m_isCert)
        pi = getPrinterInfo(m_certPrinterName);
    else {
        pi = getPrinterInfo(m_otherPrinterName);
    }
    QPrinter printer(pi);
    QPrintDialog dialog(&printer,QApplication::activeWindow());


    if (dialog.exec()){
        if (m_previewScene->pageItems().isEmpty())
            m_report.printReport(&printer);
        else{
            printer.setPrintRange(QPrinter::PrintRange(dialog.printRange()));
            printer.setFromTo(dialog.fromPage(), dialog.toPage());
            m_report.printPages(
                        m_previewScene->pageItems(), &printer
            );
            foreach(LimeReport::PageItemDesignIntf::Ptr pageItem, m_previewScene->pageItems()){
                m_previewScene->reactivatePageItem(pageItem);
            }
        }
    }

}

QPrinterInfo MainWindow::getPrinterInfo(QString printerName){
    foreach (QPrinterInfo pi, QPrinterInfo::availablePrinters()) {
        if (pi.printerName().compare(printerName)==0)
            return pi;
    }
    return QPrinterInfo::defaultPrinter();
}

void MainWindow::on_actionSetting_triggered()
{
    SettingDialog w(this);
    w.setCertPrinterName(m_certPrinterName);
    w.setOtherPrinterName(m_otherPrinterName);
    w.setDeleteLastFile(m_deleteLastFile);
    connect(&w, SIGNAL(accepted()), this, SLOT(settingAccepted()));
    w.exec();
}


void MainWindow::on_actionDelete_triggered()
{
    if (m_previewScene){
        m_previewScene->deleteSelected();
    }
}

void MainWindow::on_action_PDF_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Имя файла для экспорта"),"","*.pdf" );
    if (!fileName.isEmpty()){
        QPrinter printer;
        printer.setOutputFileName(fileName);
        printer.setOutputFormat(QPrinter::PdfFormat);
        //m_report.printReport(&printer);
        if (m_previewScene->pageItems().isEmpty())
            m_report.printReport(&printer);
        else{
            m_report.printPages(
                        m_previewScene->pageItems(),
                        &printer
            );
            foreach(LimeReport::PageItemDesignIntf::Ptr pageItem, m_previewScene->pageItems()){
                m_previewScene->reactivatePageItem(pageItem);
            }
        }
    }
}
