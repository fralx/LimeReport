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
#include "lrdataband.h"
#include "lrdesignelementsfactory.h"
#include "lrglobal.h"

const QString xmlTag = "Data";
const QString xmlTagHeader = "DataHeader";
const QString xmlTagFooter = "DataFooter";

namespace{

LimeReport::BaseDesignIntf * createBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::DataBand(owner,parent);
}
LimeReport::BaseDesignIntf * createHeader(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::DataHeaderBand(owner,parent);
}
LimeReport::BaseDesignIntf * createFooter(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::DataFooterBand(owner,parent);
}

bool VARIABLE_IS_NOT_USED registred = LimeReport::DesignElementsFactory::instance().registerCreator(
        xmlTag,
        LimeReport::ItemAttribs(QObject::tr("Data"),LimeReport::Const::bandTAG),
        createBand
    );
bool VARIABLE_IS_NOT_USED registredHeader = LimeReport::DesignElementsFactory::instance().registerCreator(
            xmlTagHeader,
            LimeReport::ItemAttribs(QObject::tr("DataHeader"),LimeReport::Const::bandTAG),
            createHeader
        );
bool VARIABLE_IS_NOT_USED registredFooter = LimeReport::DesignElementsFactory::instance().registerCreator(
            xmlTagFooter,
            LimeReport::ItemAttribs(QObject::tr("DataFooter"),LimeReport::Const::bandTAG),
            createFooter
        );

}

namespace LimeReport {

DataBand::DataBand(QObject *owner, QGraphicsItem *parent)
    : DataBandDesignIntf(LimeReport::BandDesignIntf::Data,xmlTag,owner,parent) {
        setBandTypeText(tr("Data"));
        setFixedPos(false);
        setMarkerColor(bandColor());
}

bool DataBand::isUnique() const
{
    return false;
}

QColor DataBand::bandColor() const
{
    return QColor(Qt::darkGreen);
}

void DataBand::preparePopUpMenu(QMenu &menu)
{
    BandDesignIntf::preparePopUpMenu(menu);

    QAction* currAction = menu.addAction(tr("Use alternate background color"));
    currAction->setCheckable(true);
    currAction->setChecked(useAlternateBackgroundColor());

    currAction = menu.addAction(tr("Keep footer together"));
    currAction->setCheckable(true);
    currAction->setChecked(keepFooterTogether());

    currAction = menu.addAction(tr("Keep subdetail together"));
    currAction->setCheckable(true);
    currAction->setChecked(tryToKeepTogether());

    currAction = menu.addAction(tr("Slice last row"));
    currAction->setCheckable(true);
    currAction->setChecked(sliceLastRow());

    currAction = menu.addAction(tr("Start from new page"));
    currAction->setCheckable(true);
    currAction->setChecked(startFromNewPage());

    currAction = menu.addAction(tr("Start new page"));
    currAction->setCheckable(true);
    currAction->setChecked(startNewPage());



}

void DataBand::processPopUpAction(QAction *action)
{
    BandDesignIntf::processPopUpAction(action);
    if (action->text().compare(tr("Keep footer together")) == 0){
        setProperty("keepFooterTogether",action->isChecked());
    }

    if (action->text().compare(tr("Keep subdetail together")) == 0){
        setProperty("keepSubdetailTogether",action->isChecked());
    }

    if (action->text().compare(tr("Slice last row")) == 0){
        setProperty("sliceLastRow",action->isChecked());
    }

    if (action->text().compare(tr("Use alternate background color")) == 0){
        setProperty("useAlternateBackgroundColor",action->isChecked());
    }

    if (action->text().compare(tr("Start new page")) == 0){
        setProperty("startNewPage",action->isChecked());
    }

    if (action->text().compare(tr("Start from new page")) == 0){
        setProperty("startFromNewPage",action->isChecked());
    }
}

BaseDesignIntf *DataBand::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new DataBand(owner,parent);
}

DataHeaderBand::DataHeaderBand(QObject *owner, QGraphicsItem *parent)
    :BandDesignIntf(BandDesignIntf::DataHeader,xmlTagHeader,owner,parent)
{
    setBandTypeText(tr("DataHeader"));
    setMarkerColor(bandColor());
}

void DataHeaderBand::preparePopUpMenu(QMenu &menu)
{
    BandDesignIntf::preparePopUpMenu(menu);
    QAction* currAction = menu.addAction(tr("Reprint on each page"));
    currAction->setCheckable(true);
    currAction->setChecked(reprintOnEachPage());

    currAction = menu.addAction(tr("Repeat on each row"));
    currAction->setCheckable(true);
    currAction->setChecked(repeatOnEachRow());

    currAction = menu.addAction(tr("Print always"));
    currAction->setCheckable(true);
    currAction->setChecked(printAlways());
}

void DataHeaderBand::processPopUpAction(QAction *action)
{
    BandDesignIntf::processPopUpAction(action);
    if (action->text().compare(tr("Reprint on each page")) == 0){
        setProperty("reprintOnEachPage",action->isChecked());
    }

    if (action->text().compare(tr("Repeat on each row")) == 0){
        setProperty("repeatOnEachRow",action->isChecked());
    }

    if (action->text().compare(tr("Print always")) == 0){
        setProperty("printAlways",action->isChecked());
    }
}

DataFooterBand::DataFooterBand(QObject *owner, QGraphicsItem *parent)
    :BandDesignIntf(BandDesignIntf::DataFooter,xmlTagFooter,owner,parent)
{
    setBandTypeText(tr("DataFooter"));
    setMarkerColor(bandColor());
}

void DataFooterBand::preparePopUpMenu(QMenu &menu)
{
    BandDesignIntf::preparePopUpMenu(menu);
    QAction* currAction = menu.addAction(tr("Print always"));
    currAction->setCheckable(true);
    currAction->setChecked(printAlways());
}

void DataFooterBand::processPopUpAction(QAction *action)
{
    BandDesignIntf::processPopUpAction(action);
    if (action->text().compare(tr("Print always")) == 0){
        setProperty("printAlways",action->isChecked());
    }
}

}

