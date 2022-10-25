#include "lrbordereditor.h"
#include "ui_lrbordereditor.h"
#include <QColorDialog>
#include "lrbasedesignintf.h"
lrbordereditor::lrbordereditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lrbordereditor)
{
    ui->setupUi(this);

    connect(ui->borderFrame,SIGNAL(borderSideClicked(int, bool)), this, SLOT(checkToolButtons(int, bool)));
}

void lrbordereditor::loadItem(LimeReport::BaseDesignIntf *i)
{
    item = i;
    if(item->borderLines() & LimeReport::BaseDesignIntf::TopLine)
    {
        emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::TopLine,true);

    }
    if(item->borderLines() & LimeReport::BaseDesignIntf::LeftLine)
    {
        emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::LeftLine,true);

    }
    if(item->borderLines() & LimeReport::BaseDesignIntf::RightLine)
    {
        emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::RightLine,true);

    }
    if(item->borderLines() & LimeReport::BaseDesignIntf::BottomLine)
    {
        emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::BottomLine,true);
    }
    QPen pen;
    pen.setWidthF(item->borderLineSize());
    pen.setColor(item->borderColor());
    pen.setStyle((Qt::PenStyle)item->borderStyle());
    ui->borderFrame->setPen(pen);
    border_color = item->borderColor().name();
    ui->listWidget->setCurrentRow((Qt::PenStyle)item->borderStyle());
    ui->comboBox->setCurrentText(QString::number(item->borderLineSize()));
            borderWidth = ui->comboBox->currentText().toDouble();
    borderStyle =ui->listWidget->currentRow();
    ui->pushButton->setStyleSheet(QString("#pushButton{background-color:%1;}").arg(border_color));
}

LimeReport::BaseDesignIntf::BorderLines lrbordereditor::borderSides()
{
    int borders = 0;
    borders += (ui->topLine->isChecked())?LimeReport::BaseDesignIntf::TopLine:0;
    borders += (ui->bottomLine->isChecked())?LimeReport::BaseDesignIntf::BottomLine:0;
    borders += (ui->leftLine->isChecked())?LimeReport::BaseDesignIntf::LeftLine:0;
    borders += (ui->rightLine->isChecked())?LimeReport::BaseDesignIntf::RightLine:0;
    return (LimeReport::BaseDesignIntf::BorderLines)borders;
}

LimeReport::BaseDesignIntf::BorderStyle lrbordereditor::border_style()
{
    return (LimeReport::BaseDesignIntf::BorderStyle)borderStyle;
}

QString lrbordereditor::borderColor()
{
    return border_color;
}

double lrbordereditor::border_width()
{
    return borderWidth;
}

lrbordereditor::~lrbordereditor()
{
    delete ui;
}

void lrbordereditor::on_listWidget_currentRowChanged(int currentRow)
{
    QPen pen = ui->borderFrame->pen();
    pen.setStyle((Qt::PenStyle)currentRow);
    borderStyle = currentRow;
    ui->borderFrame->setPen(pen);


}


void lrbordereditor::on_comboBox_currentTextChanged(const QString &arg1)
{
    QPen pen = ui->borderFrame->pen();
    pen.setWidthF(arg1.toDouble());
    ui->borderFrame->setPen(pen);
    borderWidth = arg1.toDouble();
}


void lrbordereditor::on_pushButton_clicked()
{
    QColorDialog cd(this);
    if(cd.exec() == QDialog::Rejected)return;
    QPen pen = ui->borderFrame->pen();
    pen.setColor(cd.selectedColor().name());
    border_color = pen.color().name();

    ui->pushButton->setStyleSheet(QString("#pushButton{background-color:%1;}").arg(border_color));
    ui->borderFrame->setPen(pen);
}


void lrbordereditor::on_toolButton_4_clicked()
{
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::TopLine,true);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::BottomLine,true);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::LeftLine,true);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::RightLine,true);
    QPen pen = ui->borderFrame->pen();

    ui->borderFrame->setPen(pen);
}


void lrbordereditor::on_noLines_clicked()
{
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::TopLine,false);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::BottomLine,false);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::LeftLine,false);
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::RightLine,false);

}


void lrbordereditor::on_topLine_clicked()
{

    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::TopLine,ui->topLine->isChecked());

}

void lrbordereditor::checkToolButtons(int side, bool check)
{

    switch(side)
    {
    case LimeReport::BaseDesignIntf::BorderSide::TopLine:
    {
        ui->topLine->setChecked(check);
    }break;
    case LimeReport::BaseDesignIntf::BorderSide::BottomLine:
    {
        ui->bottomLine->setChecked(check);
    }break;
    case LimeReport::BaseDesignIntf::BorderSide::LeftLine:
    {
        ui->leftLine->setChecked(check);
    }break;
    case LimeReport::BaseDesignIntf::BorderSide::RightLine:
    {
        ui->rightLine->setChecked(check);
    }break;
    }
}


void lrbordereditor::on_bottomLine_clicked()
{
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::BottomLine,ui->bottomLine->isChecked());
}


void lrbordereditor::on_leftLine_clicked()
{
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::LeftLine,ui->leftLine->isChecked());
}


void lrbordereditor::on_toolButton_3_clicked()
{
    emit ui->borderFrame->borderSideClicked(LimeReport::BaseDesignIntf::BorderSide::RightLine,ui->rightLine->isChecked());
}

