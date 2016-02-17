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

const QString xmlTag ="Data";

namespace{

LimeReport::BaseDesignIntf * createBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::DataBand(owner,parent);
}
bool registred = LimeReport::DesignElementsFactory::instance().registerCreator(
        xmlTag,
        LimeReport::ItemAttribs(QObject::tr("Data"),LimeReport::bandTAG),
        createBand
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

}

