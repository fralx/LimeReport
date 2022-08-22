#include "lrchartitemeditor.h"
#include "ui_lrchartitemeditor.h"
#include "lrchartitem.h"
#include "lrpagedesignintf.h"
#include <QColorDialog>

ChartItemEditor::ChartItemEditor(LimeReport::ChartItem *item, LimeReport::PageDesignIntf *page, QSettings *settings, QWidget *parent):
    QWidget(parent), ui(new Ui::ChartItemEditor), m_charItem(item), m_page(page),
    m_settings(settings), m_ownedSettings(false), m_isReadingSetting(false)
{
    ui->setupUi(this);
    QHBoxLayout* colorLayout = new QHBoxLayout();
    colorLayout->setContentsMargins(0, 0, 0, 0);
    m_colorButton = new QToolButton();
    m_colorButton->setText("...");
    m_colorButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    m_colorIndicator = new ColorIndicator();
    m_colorIndicator->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    ui->colorWidget->setLayout(colorLayout);
    colorLayout->addWidget(m_colorIndicator);
    colorLayout->addWidget(m_colorButton);
    colorLayout->insertStretch(0);
    readSetting();
    init();
    connect(m_colorButton, SIGNAL(clicked(bool)), this, SLOT(slotChangeSeriesColor()));
}

ChartItemEditor::~ChartItemEditor()
{
#ifdef Q_OS_WIN
    writeSetting();
#endif
#ifdef Q_OS_MAC
    writeSetting();
#endif
    delete ui;
}

QSettings* ChartItemEditor::settings()
{
    if (m_settings){
        return m_settings;
    } else {
        m_settings = new QSettings("LimeReport",QCoreApplication::applicationName());
        m_ownedSettings = true;
        return m_settings;
    }
}

void ChartItemEditor::readSetting()
{
    if (settings()==0) return;

    m_isReadingSetting = true;

    settings()->beginGroup("ChartItemEditor");
    QVariant v = settings()->value("Geometry");
    if (v.isValid()){
        restoreGeometry(v.toByteArray());
    }
    v = settings()->value("State");
    if (v.isValid()){
        ui->splitter->restoreState(v.toByteArray());
    }

    settings()->endGroup();

    m_isReadingSetting = false;
}

void ChartItemEditor::writeSetting()
{
    if (settings()!=0){
        settings()->beginGroup("ChartItemEditor");
        settings()->setValue("Geometry",saveGeometry());
        settings()->setValue("State",ui->splitter->saveState());
        settings()->endGroup();
    }
}

void ChartItemEditor::rebuildTable()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(m_charItem->series().count());
    for( int i=0;i<m_charItem->series().count();++i){
        QTableWidgetItem* newRow = new QTableWidgetItem(m_charItem->series().at(i)->name());
        ui->tableWidget->setItem(i,0,newRow);
    }
}

void ChartItemEditor::init()
{
    m_initing = true;

    ui->tableWidget->setColumnCount(1);
    ui->tableWidget->setRowCount(m_charItem->series().count());
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem(tr("Series name")));

    rebuildTable();

    if (!m_charItem->datasource().isEmpty()){
        if (m_page && m_page->datasourceManager()){
            LimeReport::IDataSource* ds = m_page->datasourceManager()->dataSource(m_charItem->datasource());
            if (ds){
                for (int i=0;i<ds->columnCount();++i){
                   ui->valuesFieldComboBox->addItem(ds->columnNameByIndex(i));
                   ui->labelsFieldComboBox->addItem(ds->columnNameByIndex(i));
                   ui->xAxisFieldComboBox->addItem(ds->columnNameByIndex(i));
                }
            }
        }

    }

    static int enumIndex = LimeReport::SeriesItem::staticMetaObject.indexOfEnumerator("SeriesItemPreferredType");
    QMetaEnum enumerator = LimeReport::SeriesItem::staticMetaObject.enumerator(enumIndex);
    for (int i = 0; i<enumerator.keyCount(); ++i){
        ui->seriesTypeComboBox->addItem(enumerator.key(i));
    }

#if QT_VERSION < 0x050000
    ui->labelsFieldComboBox->setCurrentIndex(ui->labelsFieldComboBox->findText( m_charItem->labelsField()));
    ui->xAxisFieldComboBox->setCurrentIndex(ui->xAxisFieldComboBox->findText( m_charItem->xAxisField()));
#else
    ui->labelsFieldComboBox->setCurrentText(m_charItem->labelsField());
    ui->xAxisFieldComboBox->setCurrentText(m_charItem->xAxisField());
#endif
    if (!m_charItem->series().isEmpty()){
        enableSeriesEditor();
        ui->tableWidget->selectRow(0);
    } else {
        disableSeriesEditor();
    }

    m_initing = false;
}

void ChartItemEditor::enableSeriesEditor()
{
    ui->seriesNameLineEdit->setEnabled(true);
    ui->valuesFieldComboBox->setEnabled(true);
    ui->seriesTypeComboBox->setEnabled(true);
    m_colorButton->setEnabled(true);
    m_colorIndicator->setEnabled(true);
}

void ChartItemEditor::disableSeriesEditor()
{
    ui->seriesNameLineEdit->setText("");
    ui->seriesNameLineEdit->setDisabled(true);
    ui->valuesFieldComboBox->setDisabled(true);
    m_colorButton->setDisabled(true);
    m_colorIndicator->setDisabled(true);
#if QT_VERSION < 0x050000
    ui->valuesFieldComboBox->setEditText("");
#else
    ui->valuesFieldComboBox->setCurrentText("");
#endif
    ui->seriesTypeComboBox->setDisabled(true);
}

LimeReport::SeriesItem *ChartItemEditor::currentSeries()
{
    int curRow = ui->tableWidget->currentRow();
    if ((curRow>-1) && !m_charItem->series().isEmpty() && m_charItem->series().count()>curRow){
        return m_charItem->series().at(curRow);
    }
    return 0;
}

void ChartItemEditor::resizeEvent(QResizeEvent *)
{
#ifdef Q_OS_UNIX
    writeSetting();
#endif
}

void ChartItemEditor::moveEvent(QMoveEvent *)
{
#ifdef Q_OS_UNIX
    writeSetting();
#endif
}

void ChartItemEditor::on_splitter_splitterMoved(int , int )
{
#ifdef Q_OS_UNIX
    writeSetting();
#endif
}

void ChartItemEditor::on_pbOk_clicked()
{
    emit editingFinished();
    close();
}

void ChartItemEditor::slotAddSeries()
{
    LimeReport::SeriesItem* series = new LimeReport::SeriesItem();
    int curSeriesNumber = m_charItem->series().count();
    while (m_charItem->isSeriesExists("Series"+QString::number(curSeriesNumber))) curSeriesNumber++;
    series->setName("Series"+QString::number(curSeriesNumber));
    series->setValuesColumn("");
    series->setLabelsColumn("");
    series->setColor((m_charItem->series().count()<32)?LimeReport::color_map[m_charItem->series().count()]:LimeReport::generateColor());
    m_charItem->series().append(series);
    ui->tableWidget->setRowCount(m_charItem->series().count());
    ui->tableWidget->setItem(m_charItem->series().count()-1, 0, new QTableWidgetItem(series->name()));
    ui->tableWidget->selectRow(m_charItem->series().count()-1);
#if QT_VERSION < 0x050000
    ui->valuesFieldComboBox->setEditText("");
#else
    ui->valuesFieldComboBox->setCurrentText("");
#endif
}

void ChartItemEditor::slotDeleteSeries()
{
    QList<LimeReport::SeriesItem*> itemsToRemove;
    foreach(QModelIndex index,ui->tableWidget->selectionModel()->selectedRows()){
      itemsToRemove.append(m_charItem->series().at(index.row()));
    };
    foreach (LimeReport::SeriesItem* series, itemsToRemove){
        m_charItem->series().removeOne(series);
        delete series;
    }
    rebuildTable();
    disableSeriesEditor();
}

void ChartItemEditor::on_tableWidget_itemSelectionChanged()
{
    if (ui->tableWidget->selectionModel()->hasSelection()){
        LimeReport::SeriesItem* series = m_charItem->series().at(ui->tableWidget->selectionModel()->currentIndex().row());
        ui->seriesNameLineEdit->setText(series->name());
#if QT_VERSION < 0x050000
        ui->valuesFieldComboBox->setCurrentIndex(ui->valuesFieldComboBox->findText(series->valuesColumn()));
#else
        ui->valuesFieldComboBox->setCurrentText(series->valuesColumn());
#endif
        m_colorIndicator->setColor(series->color());
        static int enumIndex = LimeReport::SeriesItem::staticMetaObject.indexOfEnumerator("SeriesItemPreferredType");
        QMetaEnum enumerator = LimeReport::SeriesItem::staticMetaObject.enumerator(enumIndex);
#if QT_VERSION < 0x050000
        ui->seriesTypeComboBox->setCurrentIndex(ui->seriesTypeComboBox->findText(enumerator.valueToKey(series->preferredType())));
#else
        ui->seriesTypeComboBox->setCurrentText(enumerator.valueToKey(series->preferredType()));
#endif
        enableSeriesEditor();
    }
}

void ChartItemEditor::on_seriesNameLineEdit_textChanged(const QString &arg1)
{
    if (currentSeries()){
        currentSeries()->setName(arg1);
        ui->tableWidget->currentItem()->setText(arg1);
    }
}

void ChartItemEditor::on_valuesFieldComboBox_currentTextChanged(const QString &arg1)
{
    if (currentSeries()){
        currentSeries()->setValuesColumn(arg1);
    }
}

void ChartItemEditor::on_labelsFieldComboBox_currentTextChanged(const QString &arg1)
{
    if (!m_initing)
        m_charItem->setLabelsField(arg1);
}

void ChartItemEditor::slotChangeSeriesColor()
{
    QColorDialog colorDialog;
    if (colorDialog.exec()){
        currentSeries()->setColor(colorDialog.selectedColor());
        m_colorIndicator->setColor(colorDialog.selectedColor());
    }
}

void ChartItemEditor::on_seriesTypeComboBox_currentIndexChanged(const QString &arg1)
{
    static int enumIndex = LimeReport::SeriesItem::staticMetaObject.indexOfEnumerator("SeriesItemPreferredType");
    QMetaEnum enumerator = LimeReport::SeriesItem::staticMetaObject.enumerator(enumIndex);
    if (currentSeries()){
        currentSeries()->setPreferredType(static_cast<LimeReport::SeriesItem::SeriesItemPreferredType>(enumerator.keysToValue(arg1.toLatin1())));
    }
}

void ChartItemEditor::on_xAxisFieldComboBox_currentTextChanged(const QString &arg1)
{
    if (!m_initing)
        m_charItem->setXAxisField(arg1);
}
void ChartItemEditor::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    if (ui->seriesNameLineEdit->hasFocus())
        return;

    const QString dataStr = item->data(Qt::DisplayRole).toString();
    if (dataStr == ui->seriesNameLineEdit->text())
        return;

    ui->seriesNameLineEdit->setText(dataStr);
}
