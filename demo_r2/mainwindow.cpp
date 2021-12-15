#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QDir>
#include <QDebug>
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
#include <QDesktopWidget>
#else
#include <QScreen>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_preview = m_report.createPreviewWidget();
    m_scalePercent = new QComboBox(this);
    m_pageNavigator = new QSpinBox(this);

    m_pageNavigator->setPrefix(tr("Page :"));
    ui->toolBar->insertWidget(ui->actionZoom_Out,m_scalePercent);
    ui->toolBar->insertWidget(ui->actionNext_Page,m_pageNavigator);
    connect(m_scalePercent, SIGNAL(currentIndexChanged(QString)), this, SLOT(scaleComboboxChanged(QString)));
    connect(m_pageNavigator, SIGNAL(valueChanged(int)), this, SLOT(slotPageNavigatorChanged(int)));
    ui->groupBox_2->layout()->addWidget(m_preview);
    buildReportsTree(QApplication::applicationDirPath()+"/demo_reports/", ui->treeWidget);
    connect(ui->actionZoomIn, SIGNAL(triggered()), m_preview, SLOT(zoomIn()));
    connect(ui->actionZoom_Out, SIGNAL(triggered()), m_preview, SLOT(zoomOut()));
    connect(ui->actionFirst_Page, SIGNAL(triggered()), m_preview, SLOT(firstPage()));
    connect(ui->actionPrior_Page, SIGNAL(triggered()), m_preview, SLOT(priorPage()));
    connect(ui->actionNext_Page, SIGNAL(triggered()), m_preview, SLOT(nextPage()));
    connect(ui->actionLast_Page, SIGNAL(triggered()), m_preview, SLOT(lastPage()));
    connect(m_preview,SIGNAL(scalePercentChanged(int)), this, SLOT(slotScalePercentChanged(int)));
    //connect(ui->cbScalePercent, SIGNAL(currentIndexChanged(QString)), this, SLOT(scaleComboboxChanged(QString)));
    connect(ui->actionFit_Width, SIGNAL(triggered()), m_preview, SLOT(fitWidth()));
    connect(ui->actionFit_Page, SIGNAL(triggered()), m_preview, SLOT(fitPage()));
    connect(m_preview, SIGNAL(pagesSet(int)), this, SLOT(slotPagesSet(int)));
    connect(m_preview, SIGNAL(pageChanged(int)), this, SLOT(slotPageChanged(int)));
    connect(ui->actionExport_to_PDF, SIGNAL(triggered()), m_preview, SLOT(printToPDF()));
    connect(ui->actionPrint_Report, SIGNAL(triggered()), m_preview, SLOT(print()));
    connect(ui->actionDesign_Report, SIGNAL(triggered()), this, SLOT(slotDesignReport()));
    connect(ui->actionOne_to_One, SIGNAL(triggered()), this, SLOT(slotOneToOne()));
    initPercentCombobox();
    enableUI(false);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    QDesktopWidget *desktop = QApplication::desktop();

    int screenWidth = desktop->screenGeometry().width();
    int screenHeight = desktop->screenGeometry().height();
#else
    QScreen *screen = QGuiApplication::primaryScreen();

    int screenWidth = screen->geometry().width();
    int screenHeight = screen->geometry().height();
#endif

    int x = screenWidth*0.1;
    int y = screenHeight*0.1;

    resize(screenWidth*0.8, screenHeight*0.8);
    move(x, y);

    if (ui->treeWidget->topLevelItemCount()>0){
        int index = 0;
        while (index<ui->treeWidget->topLevelItemCount()){
            if (ui->treeWidget->topLevelItem(index)->childCount()>0)
                ++index;
            else {
                m_report.loadFromFile(ui->treeWidget->topLevelItem(index)->data(0,Qt::UserRole).toString());
                ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(index));
                break;
            }
        }

    }
        m_preview->refreshPages();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::enableUI(bool value)
{
    ui->actionDesign_Report->setEnabled(value);
    ui->actionPrint_Report->setEnabled(value);
    ui->actionExport_to_PDF->setEnabled(value);
    ui->actionFirst_Page->setEnabled(value);
    ui->actionPrior_Page->setEnabled(value);
    ui->actionNext_Page->setEnabled(value);
    ui->actionLast_Page->setEnabled(value);
    ui->actionZoomIn->setEnabled(value);
    ui->actionZoom_Out->setEnabled(value);
    ui->actionFit_Width->setEnabled(value);
    ui->actionFit_Page->setEnabled(value);
    ui->actionOne_to_One->setEnabled(value);
    m_pageNavigator->setEnabled(value);
    m_scalePercent->setEnabled(value);
}

void MainWindow::slotScalePercentChanged(int percent)
{
    //ui->cbScalePercent->setEditText(QString("%1%").arg(percent));
    m_scalePercent->setEditText(QString("%1%").arg(percent));
}

void MainWindow::scaleComboboxChanged(QString text)
{
    m_preview->setScalePercent(text.remove(text.count()-1,1).toInt());
}

void MainWindow::slotPagesSet(int pagesCount)
{
//    ui->sbPageNavigator->setSuffix(tr(" of %1").arg(pagesCount));
//    ui->sbPageNavigator->setMinimum(1);
//    ui->sbPageNavigator->setMaximum(pagesCount);
//    ui->sbPageNavigator->setValue(1);

    m_pageNavigator->setSuffix(tr(" of %1").arg(pagesCount));
    m_pageNavigator->setMinimum(1);
    m_pageNavigator->setMaximum(pagesCount);
    m_pageNavigator->setValue(1);
    enableUI(true);
}

void MainWindow::slotPageChanged(int page)
{
    m_pageNavigator->setValue(page);
}

void MainWindow::slotPageNavigatorChanged(int page)
{
    m_preview->pageNavigatorChanged(page);
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int )
{
    if (!m_report.isBusy()){
        m_report.loadFromFile(item->data(0,Qt::UserRole).toString());
        m_preview->refreshPages();
    }
}

void MainWindow::initPercentCombobox()
{
    for (int i = 10; i<310; i+=10){
//        ui->cbScalePercent->addItem(QString("%1%").arg(i));
        m_scalePercent->addItem(QString("%1%").arg(i));
    }
//    ui->cbScalePercent->setCurrentIndex(4);
    m_scalePercent->setCurrentIndex(4);
}

//void MainWindow::on_sbPageNavigator_valueChanged(int arg1)
//{
//    m_preview->pageNavigatorChanged(arg1);
//}

void MainWindow::slotDesignReport()
{
    m_report.designReport();
    m_preview->refreshPages();
}

void MainWindow::slotOneToOne()
{
    m_preview->setScalePercent(100);
}
