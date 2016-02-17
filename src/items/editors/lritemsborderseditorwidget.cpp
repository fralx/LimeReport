/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2015 by Alexander Arin                                  *
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

namespace LimeReport{

ItemsBordersEditorWidget::ItemsBordersEditorWidget(ReportDesignWidget* reportEditor, const QString& title, QWidget* parent)
    : ItemEditorWidget(reportEditor,title,parent), m_changing(false)
{
    initEditor();
}

ItemsBordersEditorWidget::ItemsBordersEditorWidget(ReportDesignWidget* reportEditor, QWidget* parent)
    : ItemEditorWidget(reportEditor,parent), m_changing(false)
{
    initEditor();
}

void ItemsBordersEditorWidget::setItemEvent(BaseDesignIntf* item)
{
    QVariant borders=item->property("borders");
    if (borders.isValid()){
        updateValues((BaseDesignIntf::BorderLines)borders.toInt());
        setEnabled(true);
    }
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
    if (reportEditor())
        reportEditor()->setBorders(0);
    updateValues(0);
}

void ItemsBordersEditorWidget::allBordesClicked()
{
    int borders = BaseDesignIntf::LeftLine |
            BaseDesignIntf::RightLine |
            BaseDesignIntf::TopLine |
            BaseDesignIntf::BottomLine;

    updateValues((BaseDesignIntf::BorderLines)borders);
    if (reportEditor())
        reportEditor()->setBorders((BaseDesignIntf::BorderLines)borders);
}

void ItemsBordersEditorWidget::buttonClicked(bool)
{
    if (!m_changing&&reportEditor())
        reportEditor()->setBorders(createBorders());
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

    setEnabled(false);

}

void ItemsBordersEditorWidget::updateValues(BaseDesignIntf::BorderLines borders)
{
    m_changing = true;
    m_topLine->setChecked(borders&BaseDesignIntf::TopLine);
    m_bottomLine->setChecked(borders&BaseDesignIntf::BottomLine);
    m_leftLine->setChecked(borders&BaseDesignIntf::LeftLine);
    m_rightLine->setChecked(borders&BaseDesignIntf::RightLine);
    m_changing = false;
}

BaseDesignIntf::BorderLines ItemsBordersEditorWidget::createBorders()
{
    int borders = 0;
    borders += (m_topLine->isChecked())?BaseDesignIntf::TopLine:0;
    borders += (m_bottomLine->isChecked())?BaseDesignIntf::BottomLine:0;
    borders += (m_leftLine->isChecked())?BaseDesignIntf::LeftLine:0;
    borders += (m_rightLine->isChecked())?BaseDesignIntf::RightLine:0;
    return (BaseDesignIntf::BorderLines)borders;
}

} //namespace LimeReport
