#include "lrchartaxiseditor.h"

#include "ui_lrchartaxiseditor.h"
#include "lraxisdata.h"

#include "lrbasedesignintf.h"

ChartAxisEditor::ChartAxisEditor(LimeReport::ChartItem *item, LimeReport::PageDesignIntf *page, bool isXAxis, QSettings *settings, QWidget *parent):
      QWidget(parent), ui(new Ui::ChartAxisEditor), m_chartItem(item), m_page(page),
      m_settings(settings), m_isXAxis(isXAxis)
{
    ui->setupUi(this);
    readSetting();
    init();
}

ChartAxisEditor::~ChartAxisEditor()
{
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    writeSetting();
#endif
    delete ui;
}

QSettings* ChartAxisEditor::settings()
{
    if (m_settings){
        return m_settings;
    }
    m_settings = new QSettings("LimeReport",QCoreApplication::applicationName());
    return m_settings;
}

void ChartAxisEditor::readSetting()
{
    if (settings() == 0) return;

    settings()->beginGroup("ChartAxisEditor");
    QVariant v = settings()->value("Geometry");
    if (v.isValid()) {
        restoreGeometry(v.toByteArray());
    }

    settings()->endGroup();
}

void ChartAxisEditor::writeSetting()
{
    if (settings() == 0) {
        return;
    }
    settings()->beginGroup("ChartAxisEditor");
    settings()->setValue("Geometry",saveGeometry());
    settings()->endGroup();
}

void ChartAxisEditor::init()
{
    ui->gbAxis->setTitle(m_isXAxis ? QObject::tr("X Axis") : QObject::tr("Y Axis"));
    ui->direction_checkbox->setVisible(!m_isXAxis);

    LimeReport::AxisData *axisData = m_isXAxis ? m_chartItem->xAxisData() : m_chartItem->yAxisData();

    ui->minimumSpinBox->setValue(axisData->manualMinimum());
    ui->maximumSpinBox->setValue(axisData->manualMaximum());
    ui->stepSpinBox->setValue(axisData->manualStep());

    ui->minimumCheckBox->setChecked(axisData->isMinimumAutomatic());
    ui->maximumCheckBox->setChecked(axisData->isMaximumAutomatic());
    ui->stepCheckBox->setChecked(axisData->isStepAutomatic());

    ui->direction_checkbox->setChecked(axisData->reverseDirection());

    const bool isScaleCalcEnabled = axisData->calculateAxisScale();
    ui->enableScaleCalculation_checkbox->setChecked(isScaleCalcEnabled);
    on_enableScaleCalculation_checkbox_stateChanged(isScaleCalcEnabled);
}

void ChartAxisEditor::on_minimumCheckBox_stateChanged(int arg1)
{
    const bool isAutomatic = (bool)arg1;
    ui->minimumSpinBox->setEnabled(!isAutomatic);
}

void ChartAxisEditor::on_maximumCheckBox_stateChanged(int arg1)
{
    const bool isAutomatic = (bool)arg1;
    ui->maximumSpinBox->setEnabled(!isAutomatic);
}

void ChartAxisEditor::on_stepCheckBox_stateChanged(int arg1)
{
    const bool isAutomatic = (bool)arg1;
    ui->stepSpinBox->setEnabled(!isAutomatic);
}

void ChartAxisEditor::on_pushButtonOk_clicked()
{
    LimeReport::AxisData *axisData = m_isXAxis ? m_chartItem->xAxisData() : m_chartItem->yAxisData();
    if (!m_isXAxis) {
        axisData->setReverseDirection(ui->direction_checkbox->isChecked());
    }

    axisData->setIsStepAutomatic(ui->stepCheckBox->isChecked());
    axisData->setManualStep(ui->stepSpinBox->value());
    axisData->setIsMinimumAutomatic(ui->minimumCheckBox->isChecked());
    axisData->setManualMinimum(ui->minimumSpinBox->value());
    axisData->setIsMaximumAutomatic(ui->maximumCheckBox->isChecked());
    axisData->setManualMaximum(ui->maximumSpinBox->value());

    axisData->setCalculateAxisScale(ui->enableScaleCalculation_checkbox->isChecked());

    if (m_chartItem->itemMode() == LimeReport::DesignMode) {
        axisData->updateForDesignMode();
    } else {
        axisData->update();
    }
    m_chartItem->update();
    close();
}

void ChartAxisEditor::on_enableScaleCalculation_checkbox_stateChanged(int arg1)
{
    const bool isEnabled = (bool)arg1;
    ui->minimumCheckBox->setEnabled(isEnabled);
    ui->maximumCheckBox->setEnabled(isEnabled);
    ui->stepCheckBox->setEnabled(isEnabled);

    ui->minimumSpinBox->setEnabled(!ui->minimumCheckBox->isChecked() && isEnabled);
    ui->maximumSpinBox->setEnabled(!ui->maximumCheckBox->isChecked() && isEnabled);
    ui->stepSpinBox->setEnabled(!ui->stepCheckBox->isChecked() && isEnabled);

    ui->minimumCheckBox->setEnabled(isEnabled);
    ui->maximumCheckBox->setEnabled(isEnabled);
    ui->stepCheckBox->setEnabled(isEnabled);
}

void ChartAxisEditor::on_cancelButton_clicked()
{
    close();
}

