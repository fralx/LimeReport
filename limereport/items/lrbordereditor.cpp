#include "lrbordereditor.h"
#include "ui_lrbordereditor.h"
#include <QColorDialog>
#include "lrbasedesignintf.h"

namespace LimeReport{

BorderEditor::BorderEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BorderEditor),
    m_borderStyle(1),
    m_borderWidth(1)
{
    ui->setupUi(this);
    connect(
        ui->borderFrame, SIGNAL(borderSideClicked(LimeReport::BaseDesignIntf::BorderSide, bool)),
        this, SLOT(checkToolButtons(LimeReport::BaseDesignIntf::BorderSide, bool))
    );
}

void BorderEditor::loadItem(LimeReport::BaseDesignIntf *item)
{
    m_item = item;
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::TopLine,
                                            item->borderLines() & LimeReport::BaseDesignIntf::TopLine);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::LeftLine,
                                            item->borderLines() & LimeReport::BaseDesignIntf::LeftLine);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::RightLine,
                                            item->borderLines() & LimeReport::BaseDesignIntf::RightLine);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::BottomLine,
                                            item->borderLines() & LimeReport::BaseDesignIntf::BottomLine);

    QPen pen;
    pen.setWidthF(item->borderLineSize());
    pen.setColor(item->borderColor());
    pen.setStyle((Qt::PenStyle)item->borderStyle());
    ui->borderFrame->setPen(pen);
    m_borderColor = item->borderColor().name();
    ui->listWidget->setCurrentRow((Qt::PenStyle)item->borderStyle());
    ui->comboBox->setCurrentText(QString::number(item->borderLineSize()));
    m_borderWidth = ui->comboBox->currentText().toDouble();
    m_borderStyle =ui->listWidget->currentRow();
    ui->colorIndicator->setStyleSheet(QString("background-color:%1;").arg(m_borderColor));
}

LimeReport::BaseDesignIntf::BorderLines BorderEditor::borderSides()
{
    int borders = 0;
    borders += (ui->topLine->isChecked()) ? LimeReport::BaseDesignIntf::TopLine : 0;
    borders += (ui->bottomLine->isChecked()) ? LimeReport::BaseDesignIntf::BottomLine : 0;
    borders += (ui->leftLine->isChecked()) ? LimeReport::BaseDesignIntf::LeftLine : 0;
    borders += (ui->rightLine->isChecked()) ? LimeReport::BaseDesignIntf::RightLine : 0;
    return (LimeReport::BaseDesignIntf::BorderLines) borders;
}

LimeReport::BaseDesignIntf::BorderStyle BorderEditor::borderStyle()
{
    return (LimeReport::BaseDesignIntf::BorderStyle) m_borderStyle;
}

QString BorderEditor::borderColor()
{
    return m_borderColor;
}

double BorderEditor::borderWidth()
{
    return m_borderWidth;
}

BorderEditor::~BorderEditor()
{
    delete ui;
}

void BorderEditor::on_listWidget_currentRowChanged(int currentRow)
{
    QPen pen = ui->borderFrame->pen();
    pen.setStyle((Qt::PenStyle)currentRow);
    m_borderStyle = currentRow;
    ui->borderFrame->setPen(pen);
}

void BorderEditor::on_comboBox_currentTextChanged(const QString &arg1)
{
    QPen pen = ui->borderFrame->pen();
    pen.setWidthF(arg1.toDouble());
    ui->borderFrame->setPen(pen);
    m_borderWidth = arg1.toDouble();
}

void BorderEditor::checkToolButtons(LimeReport::BaseDesignIntf::BorderSide side, bool check)
{
    switch(side)
    {
        case BaseDesignIntf::BorderSide::TopLine:
            ui->topLine->setChecked(check);
            break;
        case BaseDesignIntf::BorderSide::BottomLine:
            ui->bottomLine->setChecked(check);
            break;
        case BaseDesignIntf::BorderSide::LeftLine:
            ui->leftLine->setChecked(check);
            break;
        case BaseDesignIntf::BorderSide::RightLine:
            ui->rightLine->setChecked(check);
            break;
    }
}

void BorderEditor::on_topLine_clicked(bool checked){
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::TopLine, checked);
}

void BorderEditor::on_bottomLine_clicked(bool checked){
    emit ui->borderFrame->borderSideClicked(BaseDesignIntf::BorderSide::BottomLine, checked);
}

void BorderEditor::on_leftLine_clicked(bool checked){
    emit ui->borderFrame->borderSideClicked(BaseDesignIntf::BorderSide::LeftLine, checked);
}

void BorderEditor::on_rightLine_clicked(bool checked){
    emit ui->borderFrame->borderSideClicked(BaseDesignIntf::BorderSide::RightLine, checked);
}


void BorderEditor::on_allLines_clicked()
{
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::TopLine, true);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::BottomLine, true);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::LeftLine, true);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::RightLine, true);
}

void BorderEditor::on_noLines_clicked()
{
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::TopLine, false);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::BottomLine, false);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::LeftLine, false);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::RightLine, false);
}


void BorderEditor::on_selectColor_clicked()
{
    QColorDialog cd(this);
    if(cd.exec() == QDialog::Rejected) return;
    QPen pen = ui->borderFrame->pen();
    pen.setColor(cd.selectedColor().name());
    m_borderColor = pen.color().name();
    ui->colorIndicator->setStyleSheet(QString("background-color:%1;").arg(m_borderColor));
    ui->borderFrame->setPen(pen);
}

} // namespace LimeReport
