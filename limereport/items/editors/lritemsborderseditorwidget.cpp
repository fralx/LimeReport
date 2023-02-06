/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2021 by Alexander Arin                                  *
 *   arin_a@bk.ru                                                          *
 *                                                                         *
 **                   GNU General Public License Usage                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 **                  GNU Lesser General Public License                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library.                                      *
 *   If not, see <http://www.gnu.org/licenses/>.                           *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ****************************************************************************/
#include "lritemsborderseditorwidget.h"
#include <QAction>
#include "lrbordereditor.h"
namespace LimeReport{

void ItemsBordersEditorWidget::setItemEvent(BaseDesignIntf* item)
{
    if(QString(item->metaObject()->className()) == "LimeReport::ShapeItem")
    {
        setDisabled(true);
        return;
    }
    QVariant borders=item->property("borders");
    if (borders.isValid()){
        updateValues((BaseDesignIntf::BorderLines)borders.toInt());
        setEnabled(true);
    }
    m_item = item;
}

void ItemsBordersEditorWidget::properyChangedEvent(const QString& property, const QVariant& oldValue, const QVariant& newValue)
{
    Q_UNUSED(oldValue)
    if (property == "borders"){
        m_changing = true;
        updateValues((BaseDesignIntf::BorderLines)newValue.toInt());
        m_changing = false;
    }
}

void ItemsBordersEditorWidget::noBordesClicked()
{
    updateValues({});
}

void ItemsBordersEditorWidget::allBordesClicked()
{
    int borders = BaseDesignIntf::LeftLine |
            BaseDesignIntf::RightLine |
            BaseDesignIntf::TopLine |
            BaseDesignIntf::BottomLine;

    updateValues((BaseDesignIntf::BorderLines)borders);
}

void ItemsBordersEditorWidget::buttonClicked(bool){}

void ItemsBordersEditorWidget::editBorderClicked()
{
    BorderEditor be;
    be.loadItem(m_item);
    if ( be.exec() == QDialog::Rejected ) return;
    updateValues(be.borderSides());
    m_item->setBorderLinesFlags(be.borderSides());
    m_item->setBorderLineSize(be.borderWidth());
    m_item->setBorderStyle((LimeReport::BaseDesignIntf::BorderStyle)be.borderStyle());
    m_item->setBorderColor(be.borderColor());
}

void ItemsBordersEditorWidget::initEditor()
{

    m_topLine = new QAction(tr("Top line"),this);
    m_topLine->setIcon(QIcon(":/report/images/topLine"));
    m_topLine->setCheckable(true);
    connect(m_topLine,SIGNAL(toggled(bool)),this,SLOT(buttonClicked(bool)));
    addAction(m_topLine);

    m_bottomLine = new QAction(tr("Bottom line"),this);
    m_bottomLine->setIcon(QIcon(":/report/images/bottomLine"));
    m_bottomLine->setCheckable(true);
    connect(m_bottomLine,SIGNAL(toggled(bool)),this,SLOT(buttonClicked(bool)));
    addAction(m_bottomLine);

    m_leftLine = new QAction(tr("Left line"),this);
    m_leftLine->setIcon(QIcon(":/report/images/leftLine"));
    m_leftLine->setCheckable(true);
    connect(m_leftLine,SIGNAL(toggled(bool)),this,SLOT(buttonClicked(bool)));
    addAction(m_leftLine);

    m_rightLine = new QAction(tr("Right line"),this);
    m_rightLine->setIcon(QIcon(":/report/images/rightLine"));
    m_rightLine->setCheckable(true);
    connect(m_rightLine,SIGNAL(toggled(bool)),this,SLOT(buttonClicked(bool)));
    addAction(m_rightLine);

    addSeparator();

    m_noLines = new QAction(tr("No borders"),this);
    m_noLines->setIcon(QIcon(":/report/images/noLines"));
    connect(m_noLines,SIGNAL(triggered()),this,SLOT(noBordesClicked()));
    addAction(m_noLines);

    m_allLines = new QAction(tr("All borders"),this);
    m_allLines->setIcon(QIcon(":/report/images/allLines"));
    connect(m_allLines,SIGNAL(triggered()),this,SLOT(allBordesClicked()));
    addAction(m_allLines);
    addSeparator();
    m_BorderEditor = new QAction(tr("Edit border"),this);
    m_BorderEditor->setIcon(QIcon(":/report/images/borderEditor"));
    connect(m_BorderEditor,SIGNAL(triggered()),this,SLOT(editBorderClicked()));
    addAction(m_BorderEditor);

    setEnabled(false);

}

void ItemsBordersEditorWidget::updateValues(BaseDesignIntf::BorderLines borders)
{
    m_changing = true;
    m_topLine->setChecked(borders & BaseDesignIntf::TopLine);
    m_bottomLine->setChecked(borders & BaseDesignIntf::BottomLine);
    m_leftLine->setChecked(borders & BaseDesignIntf::LeftLine);
    m_rightLine->setChecked(borders & BaseDesignIntf::RightLine);
    m_changing = false;
}

BaseDesignIntf::BorderLines ItemsBordersEditorWidget::createBorders()
{
    int borders = 0;
    borders += (m_topLine->isChecked()) ? BaseDesignIntf::TopLine:0;
    borders += (m_bottomLine->isChecked()) ? BaseDesignIntf::BottomLine:0;
    borders += (m_leftLine->isChecked()) ? BaseDesignIntf::LeftLine:0;
    borders += (m_rightLine->isChecked()) ? BaseDesignIntf::RightLine:0;
    return (BaseDesignIntf::BorderLines)borders;
}

bool ItemsBordersEditorWidget::changing() const
{
    return m_changing;
}

#ifdef HAVE_REPORT_DESIGNER
void ItemsBordersEditorWidgetForDesigner::buttonClicked(bool)
{
    if (!changing())
        m_reportEditor->setBorders(createBorders());
}

void ItemsBordersEditorWidgetForDesigner::noBordesClicked()
{
    m_reportEditor->setBorders({});
    ItemsBordersEditorWidget::noBordesClicked();
}

void ItemsBordersEditorWidgetForDesigner::allBordesClicked()
{
    ItemsBordersEditorWidget::allBordesClicked();
    m_reportEditor->setBorders(createBorders());
}

void ItemsBordersEditorWidgetForDesigner::editBorderClicked()
{
    BorderEditor be;
    be.loadItem(m_item);
    if ( be.exec() == QDialog::Rejected ) return;

    m_reportEditor->setBordersExt(
                be.borderSides(),
                be.borderWidth(),
                (LimeReport::BaseDesignIntf::BorderStyle)be.borderStyle(),
                be.borderColor()
    );
}

#endif

} //namespace LimeReport
