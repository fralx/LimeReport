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

DataFooterBand::DataFooterBand(QObject *owner, QGraphicsItem *parent)
    :BandDesignIntf(BandDesignIntf::DataFooter,xmlTagFooter,owner,parent)
{
    setBandTypeText(tr("DataFooter"));
    setMarkerColor(bandColor());
}

}

