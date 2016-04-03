#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QDir>
#include <QDebug>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_preview = m_report.createPreviewWidget();
    connect(m_preview,SIGNAL(scalePercentChanged(int)), this, SLOT(slotScalePercentChanged(int)));
    ui->groupBox_2->layout()->addWidget(m_preview);
    buildReportsTree(QApplication::applicationDirPath()+"/demo_reports/", ui->treeWidget);
    connect(ui->tbZoomIn, SIGNAL(clicked(bool)), m_preview, SLOT(zoomIn()));
    connect(ui->tbZoomOut, SIGNAL(clicked(bool)), m_preview, SLOT(zoomOut()));
    connect(ui->tbFirstPage, SIGNAL(clicked(bool)), m_preview, SLOT(firstPage()));
    connect(ui->tbPrevPage, SIGNAL(clicked(bool)), m_preview, SLOT(priorPage()));
    connect(ui->tbNextPage, SIGNAL(clicked(bool)), m_preview, SLOT(nextPage()));
    connect(ui->tbLastPage, SIGNAL(clicked(bool)), m_preview, SLOT(lastPage()));
    connect(ui->cbScalePercent, SIGNAL(currentIndexChanged(QString)), this, SLOT(scaleComboboxChanged(QString)));
    connect(ui->tbFitWidth, SIGNAL(clicked(bool)), m_preview, SLOT(fitWidth()));
    connect(ui->tbFitPage, SIGNAL(clicked(bool)), m_preview, SLOT(fitPage()));
    connect(m_preview, SIGNAL(pagesSet(int)), this, SLOT(slotPagesSet(int)));
    connect(m_preview, SIGNAL(pageChanged(int)), this, SLOT(slotPageChanged(int)));
    connect(ui->tbPDFExport, SIGNAL(clicked(bool)), m_preview, SLOT(printToPDF()));
    connect(ui->tbPrint, SIGNAL(clicked(bool)), m_preview, SLOT(print()));
    initPercentCombobox();
    enableUI(false);
    QDesktopWidget *desktop = QApplication::desktop();

    int screenWidth = desktop->screenGeometry().width();
    int screenHeight = desktop->screenGeometry().height();

    int x = screenWidth*0.1;
    int y = screenHeight*0.1;

    resize(screenWidth*0.8, screenHeight*0.8);
    move(x, y);
    if (QFile::exists(QApplication::applicationDirPath()+"/demo_reports/categories.lrxml")){
        m_report.loadFromFile(QApplication::applicationDirPath()+"/demo_reports/categories.lrxml");
        m_preview->refreshPages();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::enableUI(bool value)
{
    ui->tbDesign->setEnabled(value);
    ui->tbPrint->setEnabled(value);
    ui->tbPDFExport->setEnabled(value);
    ui->tbFirstPage->setEnabled(value);
    ui->tbPrevPage->setEnabled(value);
    ui->tbNextPage->setEnabled(value);
    ui->tbLastPage->setEnabled(value);
    ui->tbZoomIn->setEnabled(value);
    ui->tbZoomOut->setEnabled(value);
    ui->tbFitWidth->setEnabled(value);
    ui->tbFitPage->setEnabled(value);
    ui->tbOneToOne->setEnabled(value);
    ui->sbPageNavigator->setEnabled(value);
    ui->cbScalePercent->setEnabled(value);
}

void MainWindow::slotScalePercentChanged(int percent)
{
    ui->cbScalePercent->setCurrentText(QString("%1%").arg(percent));
}

void MainWindow::scaleComboboxChanged(QString text)
{
    m_preview->setScalePercent(text.remove(text.count()-1,1).toInt());
}

void MainWindow::slotPagesSet(int pagesCount)
{
    ui->sbPageNavigator->setSuffix(tr(" of %1").arg(pagesCount));
    ui->sbPageNavigator->setMinimum(1);
    ui->sbPageNavigator->setMaximum(pagesCount);
    ui->sbPageNavigator->setValue(1);
    enableUI(true);
}

void MainWindow::slotPageChanged(int page)
{
    ui->sbPageNavigator->setValue(page);
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int )
{
    m_report.loadFromFile(item->data(0,Qt::UserRole).toString());
    m_preview->refreshPages();
}

void MainWindow::initPercentCombobox()
{
    for (int i = 10; i<310; i+=10){
        ui->cbScalePercent->addItem(QString("%1%").arg(i));
    }
    ui->cbScalePercent->setCurrentIndex(4);
}

void MainWindow::on_sbPageNavigator_valueChanged(int arg1)
{
    m_preview->pageNavigatorChanged(arg1);
}

void MainWindow::on_tbDesign_clicked()
{
    m_report.designReport();
    m_preview->refreshPages();
}

void MainWindow::on_tbOneToOne_clicked()
{
    m_preview->setScalePercent(100);
}
