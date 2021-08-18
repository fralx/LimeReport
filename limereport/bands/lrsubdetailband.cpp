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
#include "lrsubdetailband.h"
#include "lrdesignelementsfactory.h"
#include "lrglobal.h"


const QString xmlTagBand = QLatin1String("SubDetail");
const QString xmlTagHeader = QLatin1String("SubDetailHeader");
const QString xmlTagFooter = QLatin1String("SubDetailFooter");
const QColor BAND_COLOR = Qt::red;

namespace{

LimeReport::BaseDesignIntf * createBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::SubDetailBand(owner,parent);
}

bool VARIABLE_IS_NOT_USED registred = LimeReport::DesignElementsFactory::instance().registerCreator(
        xmlTagBand,
        LimeReport::ItemAttribs(QObject::tr("SubDetail"),LimeReport::Const::bandTAG),
        createBand
    );

LimeReport::BaseDesignIntf * createHeader(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::SubDetailHeaderBand(owner,parent);
}

bool VARIABLE_IS_NOT_USED registredHeader = LimeReport::DesignElementsFactory::instance().registerCreator(
       xmlTagHeader,
        LimeReport::ItemAttribs(QObject::tr("SubDetailHeader"),LimeReport::Const::bandTAG),
        createHeader
    );

LimeReport::BaseDesignIntf * createFooter(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::SubDetailFooterBand(owner,parent);
}

bool VARIABLE_IS_NOT_USED registredFooter = LimeReport::DesignElementsFactory::instance().registerCreator(
        xmlTagFooter,
        LimeReport::ItemAttribs(QObject::tr("SubDetailFooter"),LimeReport::Const::bandTAG),
        createFooter
    );

} // namespace

namespace LimeReport{

//SubDetailBand

SubDetailBand::SubDetailBand(QObject *owner, QGraphicsItem *parent)
    : DataBandDesignIntf(BandDesignIntf::SubDetailBand, xmlTagBand, owner,parent)
{
    setBandTypeText(tr("SubDetail"));
    setFixedPos(false);
    setMarkerColor(bandColor());
}

bool SubDetailBand::isHasHeader() const
{
    return isConnectedToBand(BandDesignIntf::SubDetailHeader);
}

bool SubDetailBand::isHasFooter() const
{
    return isConnectedToBand(BandDesignIntf::SubDetailFooter);
}

BaseDesignIntf *SubDetailBand::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new SubDetailBand(owner,parent);
}

QColor SubDetailBand::bandColor() const
{
    return BAND_COLOR;
}

//SubDetailHeaderBand

SubDetailHeaderBand::SubDetailHeaderBand(QObject *owner, QGraphicsItem *parent)
    :BandDesignIntf(BandDesignIntf::SubDetailHeader,xmlTagHeader,owner,parent)
{
    setBandTypeText(tr("SubDetailHeader"));
    setMarkerColor(bandColor());
}

bool SubDetailHeaderBand::isUnique() const
{
    return false;
}

QColor SubDetailHeaderBand::bandColor() const
{
    return BAND_COLOR;
}

BaseDesignIntf *SubDetailHeaderBand::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new SubDetailHeaderBand(owner,parent);
}

//SubDetailFooterBand

SubDetailFooterBand::SubDetailFooterBand(QObject *owner, QGraphicsItem *parent)
    : BandDesignIntf(BandDesignIntf::SubDetailFooter,xmlTagFooter,owner,parent)
{
    setMarkerColor(bandColor());
}

bool SubDetailFooterBand::isUnique() const
{
    return false;
}

QColor SubDetailFooterBand::bandColor() const
{
    return BAND_COLOR;
}

BaseDesignIntf *SubDetailFooterBand::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new SubDetailFooterBand(owner,parent);
}


}
