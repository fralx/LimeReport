#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <LimeReport>
#include <QTreeWidget>
#include <QDir>
#include <QDebug>
#include <QComboBox>
#include <QSpinBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    void enableUI(bool value);
private slots:
    void slotScalePercentChanged(int percent);
    void scaleComboboxChanged(QString text);
    void slotPagesSet(int pagesCount);
    void slotPageChanged(int page);
    void slotPageNavigatorChanged(int page);
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int);
    //void on_sbPageNavigator_valueChanged(int arg1);
    void slotDesignReport();
    void slotOneToOne();
private:
    template< typename T >
    void buildReportsTree(const QString &path, T* parentItem)
    {
        QDir reportsDir(path);
        QStringList items = reportsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach( QString dir, items){
            QTreeWidgetItem* listItem = new QTreeWidgetItem(parentItem);
            listItem->setText(0,dir);
            listItem->setIcon(0,QIcon(":/images/images/folder.png"));
            buildReportsTree(reportsDir.path()+"/"+dir, listItem);
        }
        QStringList nameFilters;
        nameFilters <<"*.lrxml";
        items = reportsDir.entryList(nameFilters, QDir::Files);
        foreach( QString file, items){
            QTreeWidgetItem* listItem = new QTreeWidgetItem(parentItem);
            listItem->setIcon(0,QIcon(":/images/images/report.png"));
            listItem->setText(0,file);
            listItem->setData(0,Qt::UserRole,reportsDir.path()+"/"+file);
        }
    }
    void initPercentCombobox();
private:
    Ui::MainWindow *ui;
    LimeReport::ReportEngine m_report;
    LimeReport::PreviewReportWidget* m_preview;
    QComboBox* m_scalePercent;
    QSpinBox* m_pageNavigator;
};

#endif // MAINWINDOW_H
